#include "TEvolutionaryProcess.h"
#include "TParallelEvolutionaryProcess.h"
#include "service.h"
#include "settings.h"
#include "TAnalysis.h"
#include "TBehaviorAnalysis.h"
#include "TTopologyAnalysis.h"
#include "tests.h"
#include "techanalysis.h"
#include "TSharedEvolutionaryProcess.h"

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
    THypercubeEnvironment environment("C:/Test/Environment");
    settings::fillEnvironmentSettingsFromFile(environment, "C:/Test/settings.ini");
    environment.setStochasticityCoefficient(0.0);
    TAgent agent;
    settings::fillAgentSettingsFromFile(agent, "C:/Test/settings.ini");
    agent.loadGenome("C:/Test/bestagent_lastGen.txt", 1);
    agent.linearSystemogenesis();
    double reward = 0;
    for (int i=0; i<environment.getInitialStatesQuantity(); ++i){
      environment.setEnvironmentState(i);
      agent.life(environment, 250);
      reward += agent.getReward()/environment.getInitialStatesQuantity();
    }
    cout << reward << endl;
        return 0;

    agent.getPointerToAgentGenome()->printGraphNetwork("C:/Test/net.jpg");
    vector<TBehaviorAnalysis::SCycle> cycles = TBehaviorAnalysis::findAllCyclesOfAgent(agent, environment,true);
    TBehaviorAnalysis::uploadCycles(cycles, "C:/Test/aim_cycles.txt");
    TBehaviorAnalysis::drawCyclesListToDot(cycles, environment, "C:/Test/cycles.jpg");
    techanalysis::makeBehaviorConvergenceDiagram(agent, environment, "C:/Test/BehConv.jpg");
    for (int i =0; i<cycles.size(); ++i)
      cout << TBehaviorAnalysis::calculateCycleReward(cycles[i], environment) << "\t" 
        << TBehaviorAnalysis::calculateCycleLongestMemory(cycles[i], environment) << endl;
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
    srand(static_cast<unsigned int>(time(0)));
    //techanalysis::analysisSLengthVsConvSize("C:/Test/settings_LINSYS.ini", "C:/Test/En1001_linsys(1)_bestpopulation.txt", 
    //                                        "C:/Test/Environments/Environment1001.txt", "C:/Test/An_SLengthVsConvSize_En1001_linsys(1).txt");
    //techanalysis::evolutionSLengthVsConvSize("C:/Test/SANDBOX/settings_LINSYS.ini", "C:/Test/SANDBOX/En1113_linsys(2)_bestagents.txt", 5000, 
    //                                       "C:/Test/SANDBOX/Environment1113.txt", "C:/Test/SANDBOX/An_evolutionSLengthVsConvSize_En1113_linsys(2).txt");
    //techanalysis::conductBehaviorEvolutionAnalysis("D:/1/Test/SANDBOX/settings_LINSYS.ini","D:/1/Test/SANDBOX/Environment1120.txt",  "D:/1/Test/SANDBOX/En1120_63(5)_bestagents_mod_newformat.txt", 5000, 
    //                                                "D:/1/Test/SANDBOX/AimCyclesEvolution_En1120_63(5).txt", "D:/1/Test/SANDBOX/AimCyclesEvolution(CyclesData)_En1120_63(5).txt", true);
    TAgent agent;
    settings::fillAgentSettingsFromFile(agent, "C:/Test/SANDBOX/settings_PC40ANP40.ini");
    THypercubeEnvironment environment("C:/Test/SANDBOX/Environment1113.txt");
    settings::fillEnvironmentSettingsFromFile(environment, "C:/Test/SANDBOX/settings_PC40ANP40.ini");
    agent.loadGenome("C:/Test/SANDBOX/En1113_pc40anp40(18)_bestpopulation.txt", 42);
    //agent.primarySystemogenesis();
    //ofstream out;
    //out.open("C:/Test/SANDBOX/En1113_pc40anp40(18)_bestpop_179agentController.txt");
    //agent.uploadController(out);
    //out.close();
    //ifstream in;
    //in.open("C:/Test/SANDBOX/En1113_pc40anp40(18)_bestpop_179agentController_goodNoAL.txt");
    //agent.loadController(in);
    //in.close();
    //agent.setLearningMode(0);
    //techanalysis::makeBehaviorConvergenceDiagram(agent, environment, "C:/Test/SANDBOX/BehConvergence_En1113_pc40anp40(18)_bestpop_179agent_goodNoAL.jpg"); 
    //return 0;
    //TNeuralNetwork initialController = *(agent.getPointerToAgentController());
    techanalysis::conductLearningVsNonLearningAnalysis(agent, environment, "C:/Test/SANDBOX/En1113_pc40anp40(18)_bestpop_42agent_learnVSnonlearn.txt", 500);
    return 0;
    environment.setStochasticityCoefficient(0.0);
    double env[] = {1,0,1,0,1,0,1,0};
    bool _env[] = {1,0,1,0,1,0,1,0};
    double state = service::binToDec(_env, 8);
    cout << "State: " << state << endl;
    environment.setEnvironmentVector(env);
    //agent.printLife(environment, 50);
    //return 0;
    agent.life(environment, 30);
    cout << agent.getReward() << endl;
    vector<double> life;
    const vector< vector<double> >& agentLife = agent.getPointerToAgentLife(); 
    for (unsigned int i=0; i < agentLife.size(); ++i)
      life.push_back(agentLife[i][0]);
    TBehaviorAnalysis::drawActionSequenceToDot(life, environment, "C:/Test/SANDBOX/En1113_pc40anp40(18)_bestpop_179agent_poorNoAL_noL_state170.jpg", state);
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
}
