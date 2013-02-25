#include "TEvolutionaryProcess.h"
#include "TParallelEvolutionaryProcess.h"
#include "service.h"
#include "settings.h"
#include "TAnalysis.h"
#include "TBehaviorAnalysis.h"
#include "TTopologyAnalysis.h"
#include "tests.h"
#include "techanalysis.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstdio>
#include <cstring>
#include <sstream>

#include "tests.h"
#include <ctime>
#include <new>

//#include "alglib/statistics.h"

//#include <boost/program_options.hpp>

using namespace std;

// Обработчик нехватки памяти
void out_of_memory(){
	cout << "\nnew() error : not enough memory\n";
	exit(999);
}

// Определение режима запуска комплекса ("E" - эволюция, "BPA" - анализ методом прогона лучшей популяции)
string decodeProgramMode(int argc, char** argv){
	int currentArgNumber = 1; // Текущий номер параметра
	while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			if (!strcmp("-mode", argv[currentArgNumber])) return argv[++currentArgNumber];
		}
		++currentArgNumber;
	}
	return "";
}

void decodeCommandPromt(string& environmentFilename, string& resultsFilename, string& bestPopulationFilename, string& bestAgentsFilename, long& randomSeed, bool& extraPrint, int argc, char** argv){
	int currentArgNumber = 1; // Текущий номер параметра
	while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			if (!strcmp("-envfile", argv[currentArgNumber])) environmentFilename = argv[++currentArgNumber];
			else if (!strcmp("-resultfile", argv[currentArgNumber])) resultsFilename = argv[++currentArgNumber];
			else if (!strcmp("-bestpopfile", argv[currentArgNumber])) bestPopulationFilename = argv[++currentArgNumber];
			else if (!strcmp("-bestagentfile", argv[currentArgNumber])) bestAgentsFilename = argv[++currentArgNumber];
			else if (!strcmp("-randomseed", argv[currentArgNumber])) randomSeed = atoi(argv[++currentArgNumber]);
			else if (!strcmp("-extraprint", argv[currentArgNumber])) extraPrint = !(atoi(argv[++currentArgNumber]) == 0);
		}
		++currentArgNumber;
	}
}

int main(int argc, char** argv){
	// Устанавливаем обработчик нехватки памяти
	set_new_handler(out_of_memory);
	string programMode = decodeProgramMode(argc, argv);; // Режим работы программы - "E" - эволюция, "BPA" - анализ методом прогона лучшей популяции
	if (programMode == "E"){ // Режим эволюции
		TParallelEvolutionaryProcess* parallelProcess = new TParallelEvolutionaryProcess;
		parallelProcess->start(argc, argv);
		delete parallelProcess;	
	}
	else if (programMode == "BPA"){ // Режим анализа методом прогона лучшей популяции
		TAnalysis* bestPopulationAnalysis = new TAnalysis;
		bestPopulationAnalysis->startParallelBestPopulationAnalysis(argc, argv);
		delete bestPopulationAnalysis;
	}
	else if (programMode == "SE"){ // Режим эволюционного запуска на одном процессоре
		TEvolutionaryProcess* evolutionaryProcess = new TEvolutionaryProcess;
		evolutionaryProcess->filenameSettings.settingsFilename = settings::getSettingsFilename(argc, argv);
		long randomSeed = 0;
		bool extraPrint = false;
		decodeCommandPromt(evolutionaryProcess->filenameSettings.environmentFilename, evolutionaryProcess->filenameSettings.resultsFilename, evolutionaryProcess->filenameSettings.bestPopulationFilename, evolutionaryProcess->filenameSettings.bestAgentsFilename, randomSeed, extraPrint, argc, argv);
		evolutionaryProcess->setExtraPrint(extraPrint);
		evolutionaryProcess->start(randomSeed);
			delete evolutionaryProcess;
  }
  else if (programMode == "PROFILING"){ // Режим для различных тестов производительности
    string settingsFilename = settings::getSettingsFilename(argc, argv);
    string environmentFilename, testPopulationFilename;
    int currentArgNumber = 1; // Текущий номер параметра
	  while (currentArgNumber < argc){
		  if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			  if (!strcmp("-envfile", argv[currentArgNumber])) environmentFilename = argv[++currentArgNumber];
        else if (!strcmp("-testpopfile", argv[currentArgNumber])) testPopulationFilename = argv[++currentArgNumber];
		  }
		  ++currentArgNumber;
	  }
    TPopulation<TAgent> population;
    settings::fillPopulationSettingsFromFile(population, settingsFilename);
    population.loadPopulation(testPopulationFilename);
    settings::fillAgentsPopulationSettingsFromFile(population, settingsFilename);
    THypercubeEnvironment environment;
    settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
    environment.loadEnvironment(environmentFilename);
    // Проводим прогон для определения производительности
    int runsQuantity = 10;
    for (int run = 0; run < runsQuantity; ++run)
      for (int currentAgent = 1; currentAgent <= population.getPopulationSize(); ++currentAgent){
        population.getPointertoAgent(currentAgent)->primarySystemogenesis();
        environment.setRandomEnvironmentState();
        population.getPointertoAgent(currentAgent)->life(environment, population.evolutionSettings.agentLifetime); 
      }
    return 0;
  }
  else if(programMode == "TEST"){ // Отладочный (тестовый режим) - сюда можно писать различные тестовые запуски
    string settingsFilename = "C:/Coding/settings.ini";
    string populationFilename = "C:/Coding/En1113_pc40anp40(34)_bestpopulation.txt";
    string environmentFilename = "C:/Coding/Environment1113.txt";
    TPopulation<TAgent> population;
    THypercubeEnvironment environment;
    settings::fillPopulationSettingsFromFile(population, settingsFilename);
    settings::fillAgentsPopulationSettingsFromFile(population, settingsFilename);
    settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
    environment.loadEnvironment(environmentFilename);
    population.loadPopulation(populationFilename);
    ofstream output("C:/Coding/results_population6.txt");
    for (int currentAgent = 1; currentAgent <= population.getPopulationSize(); ++currentAgent){
      double max = 0;
      double average = 0;
      population.getPointertoAgent(currentAgent)->primarySystemogenesis();
      TNeuralNetwork initialController;
      initialController = *(population.getPointertoAgent(currentAgent)->getPointerToAgentController());
      for (int currentState = 0; currentState < environment.getInitialStatesQuantity(); ++currentState){
        environment.setEnvironmentState(currentState);
        *(population.getPointertoAgent(currentAgent)->getPointerToAgentController()) = initialController;
        population.getPointertoAgent(currentAgent)->life(environment, population.evolutionSettings.agentLifetime);
        double reward = population.getPointertoAgent(currentAgent)->getReward();
        average += reward;
        if (reward > max) max = reward;
      }
      average /= environment.getInitialStatesQuantity();
      cout << "Agent:\t" << currentAgent << "\tAverage:\t" << average << "\tMax:\t" << max << endl;
      output << "Agent:\t" << currentAgent << "\tAverage:\t" << average << "\tMax:\t" << max << endl;
    }
    return 0;
  }
  else if(programMode == "NQ"){ // Режим подсчета количества нейронов
    TAnalysis::calculateOneEnvironmentAverageNeurons(argc, argv);
    return 0;
  }
  else if (programMode == "BCA"){//Режим анализа поведенчиских циклов
    TBehaviorAnalysis* behaviorAnalysis = new TBehaviorAnalysis;
    behaviorAnalysis->beginAnalysis(argc, argv);
    delete behaviorAnalysis;
  }
  else if (programMode == "PRNT"){ // Режим визуализации лучшего агента
	  TAgent testAgent;
	  std::fstream inputFile("pop.txt");
	  std::ofstream outputFile("out_file.txt");
	  testAgent.loadGenome(inputFile);
	  testAgent.uploadGenome(outputFile);
	  (testAgent.getPointerToAgentGenome())->printGraphNetworkAlternative("1", 200, 0);
	  return 0;
  }
  else if (programMode == "EVLPRNT"){
	  TAgent testAgent;
	  std::fstream inputFile("pop.txt");
	  std::ofstream outputFile("out_file.txt");
	  testAgent.loadGenome(inputFile, true);
	  testAgent.uploadGenome(outputFile, true);
	  (testAgent.getPointerToAgentGenome())->printGraphNetworkAlternative("1", 200, 1);
	  return 0;
  }
  else if (programMode == "TLA"){
	  TAgent testAgent;
	  std::fstream inputFile("pop.txt");
	  std::ofstream outputFile("out_file.txt");
	  cout << "we are here" << endl;
	  testAgent.loadGenome(inputFile, true);
	  testAgent.uploadGenome(outputFile, true);
	
	  TTopologyAnalysis *topologyAnalysis = new TTopologyAnalysis;
	  topologyAnalysis->initializeIdArray(testAgent.getPointerToAgentGenome());
  }
  // Тестирование различных конструкций, несвязанных с основным кодом
  else if (programMode == "TESTUN"){
    //techanalysis::analysisSLengthVsConvSize("C:/Test/settings_LINSYS.ini", "C:/Test/En1001_linsys(1)_bestpopulation.txt", 
    //                                        "C:/Test/Environments/Environment1001.txt", "C:/Test/An_SLengthVsConvSize_En1001_linsys(1).txt");
    //techanalysis::evolutionSLengthVsConvSize("C:/Test/settings_LINSYS.ini", "C:/Test/En1001_linsys(3)_bestagents.txt", 5000, 
    //                                       "C:/Test/Environments/Environment1001.txt", "C:/Test/An_evolutionSLengthVsConvSize_En1001_linsys(3).txt");
    TAnalysis::makeBestPopulationAnalysisSummary("C:/Test/SANDBOX/RandomAgents_analysis_En1001-1360.txt", "C:/Test/random.txt", 18, 20, 1);

  }

  /*TAnalysis* analysis = new TAnalysis;
	analysis->randomAgentAnalysis("C:/Tests/Environments/", 1001, 1360, "C:/Tests/settings.ini", "C:/Tests/RANDOM_agent_analysis.txt");
	delete analysis;*/
	/*int coefficientsQuantity = 18;
	int environmentsQuantity = 20;
	int initialStatesQuantity = 256;
	int maxCycleLength = 100;
	string runSign = "test";
	string analysisDirectory = "C:/Advanced_Behavior/";
	string outputDirectory = "C:/Advanced_Behavior/Summary_scaled/";
	ofstream zeroConvergenceFile;
	//zeroConvergenceFile.open (("C:/Advanced_Behavior/Summary/zero_convergence" + runSign + ".txt").c_str());

	for (int coefficient = 1; coefficient <= 18; ++coefficient){
		int* cyclesConvergence = new int[initialStatesQuantity];
		memset(cyclesConvergence, 0, initialStatesQuantity * sizeof(int));
		int* cyclesLength = new int[maxCycleLength];
		memset(cyclesLength, 0, maxCycleLength * sizeof(int));
		double averageZeroConvergence = 0;
		for (int environment = 1; environment <= environmentsQuantity; ++environment){
			int* tempCyclesConvergence = new int[initialStatesQuantity];
			memset(tempCyclesConvergence, 0, initialStatesQuantity * sizeof(int));
			int* tempCyclesLength = new int[maxCycleLength];
			memset(tempCyclesLength, 0, maxCycleLength * sizeof(int));
			double tempAverageZeroConvergence = 0;
			
			stringstream analysisFilename;
			analysisFilename << analysisDirectory << "/ConvergenceAnalysisEn" << 1000 + environment + (coefficient-1) * environmentsQuantity << "_" << runSign << ".txt";
			TAnalysis::advancedBehaviorFilesParsing(tempCyclesConvergence, initialStatesQuantity, tempAverageZeroConvergence, tempCyclesLength, maxCycleLength, analysisFilename.str());

			for (int i=0; i<initialStatesQuantity; ++i)
				cyclesConvergence[i] += tempCyclesConvergence[i];
			for (int i=0; i<maxCycleLength; ++i)
				cyclesLength[i] += tempCyclesLength[i];

			averageZeroConvergence += tempAverageZeroConvergence;

			delete []tempCyclesConvergence;
			delete []tempCyclesLength;
		}
		stringstream outputFilename;
		outputFilename << outputDirectory << "/AdvancedBehaviorCyclesConvergenceCoef" << coefficient << "_" << runSign << ".txt";
		TAnalysis::scaleHistogram(cyclesConvergence, initialStatesQuantity, 10, outputFilename.str());
		outputFilename.str("");
		outputFilename << outputDirectory << "/AdvancedBehaviorCyclesLentghCoef" << coefficient << "_" << runSign << ".txt";
		TAnalysis::scaleHistogram(cyclesLength, maxCycleLength, 10, outputFilename.str());
		*/
		/*ofstream outputFile;
		outputFile.open(outputFilename.str().c_str());

		for (int i=0; i<initialStatesQuantity; ++i)
				outputFile << cyclesConvergence[i] << endl;
		
		outputFile << endl << endl << averageZeroConvergence / environmentsQuantity << endl << endl << endl;
		zeroConvergenceFile << averageZeroConvergence / environmentsQuantity << endl;
		for (int i=0; i<maxCycleLength; ++i)
				outputFile << cyclesLength[i] << endl;

		outputFile.close();*/
//		delete []cyclesConvergence;
	//	delete []cyclesLength;
	//}

	//zeroConvergenceFile.close();
	/*TAnalysis analysis;
	analysis.makeBestPopulationAnalysisSummary("C:/Tests/RANDOM_agent_analysis.txt", "C:/Tests/summarydeterm.txt", 18, 20, 1);*/
	//return 0;

	/*TAnalysis* bestPopulationAnalysis = new TAnalysis;
	bestPopulationAnalysis->startBestPopulationAnalysis("C:/Tests/Results/En1001/En1001_test(1)_bestpopulation.txt", "C:/Tests/Environments/Environment1001.txt", "C:/Tests/test_settings.txt");
	delete bestPopulationAnalysis;*/

	/*TParallelEvolutionaryProcess* parallelProcess = new TParallelEvolutionaryProcess;
	parallelProcess->start(argc, argv);
	delete parallelProcess;*/

	/* ПРИМЕР ЗАПУСКА ОБЫЧНОГО ЭВОЛЮЦИОННОГО ПРОЦЕССА
	TEvolutionaryProcess* evolutionaryProcess = new TEvolutionaryProcess;
	evolutionaryProcess->filenameSettings.environmentFilename = "C:/Tests/test_environment.txt";
	evolutionaryProcess->filenameSettings.settingsFilename = "C:/Tests/test_settings.txt";
	evolutionaryProcess->filenameSettings.resultsFilename = "C:/Tests/test_results.txt";
	evolutionaryProcess->filenameSettings.bestPopulationFilename = "C:/Tests/test_best_population.txt";
	evolutionaryProcess->filenameSettings.bestAgentsFilename = "C:/Tests/test_best_agents.txt";
	evolutionaryProcess->start();
	delete evolutionaryProcess;*/
}
