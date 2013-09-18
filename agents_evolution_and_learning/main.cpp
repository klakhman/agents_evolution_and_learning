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
#include "algorithm"
#include "functional"
#include <iterator>
#include "RestrictedHypercubeEnv.h"
#include "params.h"

#include "TEnkiEnvironment.h"
#include "TEnkiAgent.h"
#include "TEnkiEvolutionaryProcess.h"

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

void testFunc();
void currentAnalysis();

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
    srand(static_cast<unsigned int>(time(0)));
    const string homeDir = "C:/SANDBOX/";
    TAgent agent;
    settings::fillAgentSettingsFromFile(agent, homeDir + "settings_PC40REDSYS.ini");
    RestrictedHypercubeEnv env(homeDir + "Environment2004.txt");
    settings::fillEnvironmentSettingsFromFile(env, homeDir + "settings_PC40REDSYS.ini");
    //vector<double> allRewards(250, 0);
    //for (unsigned int agentN = 0; agentN < 250; ++agentN){
    //  agent.loadGenome(homeDir + "En2004_pc40redsys_mod(7)_bestpopulation.txt", agentN+1);
    //  for (unsigned int sysN = 0; sysN < 3; ++sysN){
    //    agent.systemogenesis();
    //    vector<double> agentRewards = techanalysis::totalRun(agent, env);
    //    allRewards[agentN] += techanalysis::sum(agentRewards) / (agentRewards.size() * 3);
    //    cout << " + ";
    //  }
    //  cout << agentN << " = " << allRewards[agentN] << endl;
    //}
    //ofstream of((homeDir + "En2004_pc40redsys_mod(7)_bestpopulation_rewards_noal.txt").c_str());
    //copy(allRewards.begin(), allRewards.end(), ostream_iterator<double>(of, "\n"));
    //of.close();
    //return 0;
    agent.loadGenome(homeDir + "En2004_pc40redsys_mod(7)_bestpopulation.txt", 6);
    const unsigned int initState = 127;
    env.setStochasticityCoefficient(0.0);
    env.setEnvironmentState(initState);
    agent.systemogenesis();
    agent.life(env, 250);
    cout << agent.getReward() << endl << endl;
    vector<unsigned int> aimSequence = env.aimsReachedLife();
    vector<unsigned int> complexLevel(5, 0);
    for (unsigned int aim = 0; aim < aimSequence.size(); ++aim)
      complexLevel[env.getAimReference(aimSequence[aim]).aimComplexity - 2]++;
    for (unsigned int complA = 0; complA < complexLevel.size(); ++complA)
      cout << complA + 2 << " : " << complexLevel[complA] << endl;
    cout << endl;
    for (vector<unsigned int>::iterator aim = aimSequence.begin(); aim != aimSequence.end(); ++aim){
      cout << *aim << endl;
      env.getAimReference(*aim).print(cout);
      cout << endl;
    }
    //copy(aimSequence.begin(), aimSequence.end(), ostream_iterator<unsigned int>(cout, "\t"));
    cout << endl;
    env.setEnvironmentState(initState);
    TBehaviorAnalysis::drawAgentLife(agent, env, 50, homeDir + "En2004_pc40redsys_mod(7)_A6_S0.jpg"); 
    return 0;




    //const string root = "C:/AlternativeSystemogenesis/";
    //const string resultsDirectory = root + "/Results";  
    //const string runSign = "pc40alsys";
    //TAnalysis analysis;
    //cout << analysis.startBestPopulationAnalysis(service::bestPopPath(resultsDirectory, 1113, 14, runSign), root + "/Environments/Environment1113.txt",
    //                                              root + "/settings_PC40ALSYS.ini");

    //testFunc();
    //currentAnalysis();
    return 0;

    srand(static_cast<unsigned int>(time(0)));
    for (unsigned int envN = 1; envN <= 20; ++envN){
      RestrictedHypercubeEnv* env = RestrictedHypercubeEnv::generateEnvironment(8, 1);
      cout << env->calculateOccupancyCoefficient() << endl;
      stringstream tmp;
      tmp << "C:/CurrentTest/Environment" << 10000 + envN << ".txt";
      env->uploadEnvironment(tmp.str());
      delete env;
    }
    //env->printEnvironmentsGoalsHierarchy("C:/CurrentTest/generated_env.jpg");
    //RestrictedHypercubeEnv envi("C:/CurrentTest/test_env.txt", 22);
    //envi.setEnvironmentState(0);
    //const double actions[] = {5, 4, 1, 3, -5, -4, -1, -3, 5, 4, 1, 3, -5, -4, -1, -3, 5, 4, 1, 3, -5, -4, -1, -3, 5, 4, 1, 3};
    //for (unsigned int action = 0; action < sizeof(actions) / sizeof(*actions); ++action){
    //  vector<double> _action(1, actions[action]);
    //  int success = envi.forceEnvironment(_action);
    //  cout << actions[action] << "\t" << success << "\t" << envi.calculateReward(vector< vector<double> >(1, vector<double>(1, 0)), 1) << "\t" << envi.getEnvironmentState() << endl;
    //}
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
	  std::fstream inputFile("agent1.txt");
	  std::ofstream outputFile("out_file1.txt");
	  cout << "we are here" << endl;
	  testAgent.loadGenome(inputFile, true);
	  testAgent.uploadGenome(outputFile, true);
	  (testAgent.getPointerToAgentGenome())->printGraphNetworkGraal("grl1");
	
	  TTopologyAnalysis *topologyAnalysis = new TTopologyAnalysis;
	  topologyAnalysis->initializeIdArray(testAgent.getPointerToAgentGenome());

	  std::fstream inputFile1("agent2.txt");
	  std::ofstream outputFile1("out_file2.txt");
	  testAgent.loadGenome(inputFile1, true);
	  testAgent.uploadGenome(outputFile1, true);
	  (testAgent.getPointerToAgentGenome())->printGraphNetworkGraal("grl2");

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
  else if (programMode == "AVANALYSIS"){
    settings::PromtParams params;
    unsigned int firstEnv = 0;
    unsigned int lastEnv = 0;
    unsigned int firstTry = 0;
    unsigned int lastTry = 0;
    string settingsFilename;
    string runSign;
    params << settings::DoubleParam("env", firstEnv, lastEnv) 
            << settings::DoubleParam("try", firstTry , lastTry)
            << settings::SingleParam("settings", settingsFilename)
            << settings::SingleParam("sign", runSign);
    params.fillSettings(argc, argv);
    techanalysis::AnalysisRange range;
    range.setEnvRange(firstEnv, lastEnv);
    range.setTrialRange(firstTry, lastTry);
    vector<double> results = techanalysis::analyseBestPopStruct(range, techanalysis::agentPredCon(), settingsFilename, runSign);
    stringstream outFilename;
    outFilename << "AveragePredConQuantity_En" << firstEnv << "-" << lastEnv << "_" << runSign << ".txt";
    ofstream ofs(outFilename.str().c_str());
    copy(results.begin(), results.end(), ostream_iterator<double>(ofs, "\n"));
    ofs.close();
  }
  else if (programMode == "DIFFEV"){
    const string dir = "C:/SANDBOX/";
    techanalysis::difEvolutionAnalysis(dir + "En2004_pc40redsys_mod(7)_bestagents.txt", 5000, 
                                      dir + "Environment2004.txt", 1, 
                                      dir + "settings_PC40REDSYS.ini", dir + "En2004_pc40redsys_mod(7)_difevolution.txt");
  }
  else if (programMode == "TESTUNIT"){
    srand(static_cast<unsigned int>(time(0)));
    rand();
    rand();
    rand();
    THypercubeEnvironment env("C:/SANDBOX/Environment1001.txt");
    cout << env.calculateOccupancyCoefficient() << endl;
    RestrictedHypercubeEnv renv("C:/SANDBOX/Environment2001.txt");
    cout << renv.calculateOccupancyCoefficient() << endl;
    RestrictedHypercubeEnv* new_env = RestrictedHypercubeEnv::generateEnvironment(8, 0, 6, 2);
    cout << new_env->calculateOccupancyCoefficient();
    new_env->uploadEnvironment("C:/SANDBOX/Environment3010.txt");
    delete new_env;
    //srand(static_cast<unsigned int>(time(0)));
    //time_t start = clock();
    //for (unsigned int n = 0; n < 2500000u; ++n){
    //  vector<double> vec(10000u, 0);
    //  for (auto& el : vec)
    //    el = service::uniformDistribution(0, 100);
    //  sort(vec.begin(), vec.end());
    //  double tmp = vec[0];
    //  ++tmp;
    //  if (n % 1000 == 0) cout << n << endl;
    //}
    //cout << (clock() - start) / static_cast<double>(CLOCKS_PER_SEC) << endl;

    //tests::t_learn_1("C:/SANDBOX/Tests");
    //tests::t_learn_2("C:/SANDBOX/Tests");
    //tests::t_learn_3("C:/SANDBOX/Tests");
    //tests::t_learn_4("C:/SANDBOX/Tests");
    return 0;
  }
#ifndef NOT_USE_ROBOT_LIB
  else if (programMode == "ENKITEST") {
    
    // Устанавливаем зерно для генератора случайных чисел
    srand(static_cast<unsigned int>(time(0)));
    
    // Формирование файла, заключающего информацию о объектах среды (необходим для построения графика в гнуплоте)
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects1.txt");
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects2.txt");
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects3.txt");
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects4.txt");
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotRobot.txt");
    
    // Запускаем эволюционный процесс
    TEnkiEvolutionaryProcess* evolutionaryProcess = new TEnkiEvolutionaryProcess;
		evolutionaryProcess->filenameSettings.settingsFilename = settings::getSettingsFilename(argc, argv);
		long randomSeed = 0;
		bool extraPrint = false;
		decodeCommandPromt(evolutionaryProcess->filenameSettings.environmentFilename, evolutionaryProcess->filenameSettings.resultsFilename, evolutionaryProcess->filenameSettings.bestPopulationFilename, evolutionaryProcess->filenameSettings.bestAgentsFilename, randomSeed, extraPrint, argc, argv);
		evolutionaryProcess->setExtraPrint(extraPrint);
		evolutionaryProcess->start(static_cast<unsigned int>(randomSeed));
    delete evolutionaryProcess;
    
  } else if (programMode == "ENKIDRAWBESTAGENT") {
    
    // Устанавливаем зерно для генератора случайных чисел
    srand(static_cast<unsigned int>(time(0)));
    
    // Удаление файлов, заключающих информацию о объектах среды (необходимы для построения графика в гнуплоте)
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects1.txt");
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects2.txt");
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects3.txt");
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects4.txt");
    
    // Удаление файла с логами перемещения робота
    std::remove("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotRobot.txt");
    
    // Формирование среды
    TEnkiEnvironment enkiEnvironment;
    settings::fillEnvironmentSettingsFromFile(enkiEnvironment, "/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/enkiEnvironmentSettings.txt");
    enkiEnvironment.loadEnvironment("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/testingEnvironment.txt");
    
    // Установка начальных параметров робота (положения, угла (отсчитывается против часовой стрелки от оси, направленной вправо) и начальных скоростей)
    enkiEnvironment.setRandomEnvironmentState();
    /*enkiEnvironment.ePuckBot->pos.x = 20.0;
    enkiEnvironment.ePuckBot->pos.y = 120.0;
    enkiEnvironment.ePuckBot->angle = 0.0;
    enkiEnvironment.ePuckBot->leftSpeed = 0.0;
    enkiEnvironment.ePuckBot->rightSpeed = 0.0;*/
    
    // Формирование агента
    TEnkiAgent enkiAgent;
    settings::fillAgentSettingsFromFile(enkiAgent, "/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/enkiEnvironmentSettings.txt");
    
    // Формирование нейронной сети агента
    std::fstream inputFileForGenome("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/enkiBestAgent.txt");
    enkiAgent.loadGenome(inputFileForGenome, 1158);
    enkiAgent.linearSystemogenesis();
    
    // Формирование файлов с информацией об объектах в среде для гнуплота
    enkiEnvironment.printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects1.txt", 0);
    enkiEnvironment.printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects2.txt", 1);
    enkiEnvironment.printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects3.txt", 2);
    enkiEnvironment.printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects4.txt", 3);
    
    // Запуск агента
    enkiAgent.life(enkiEnvironment, 1600);
    
  }
#endif //NOT_USE_ROBOT_LIB

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

void drawAgentLife(TAgent& agent, THypercubeEnvironment& environment, unsigned int lifetime, const string& imageFilename);

void testFunc(){
  //const string folder = "C:/Test/(2013-05-31)_Learning_Test/";
  //const string agentFilename = folder + "En10001_linsys(2)_bestpopulation.txt";
  //const unsigned int agentNumber = 2;
  //const string environmentFilename = folder + "Environments/Environment10001.txt";
  //const string settingsFilename = folder + "settings_LINSYS.ini";
  //const string imageFilename = folder + "agent-1_state-0.jpg";
  //const unsigned int initState = 0;
  //const unsigned int lifetime = 50;
  //RestrictedHypercubeEnv environment(environmentFilename);
  //settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
  //environment.setEnvironmentState(initState);
  //environment.setStochasticityCoefficient(0.0);
  //TAgent agent;
  //agent.loadGenome(agentFilename, agentNumber);
  //settings::fillAgentSettingsFromFile(agent, settingsFilename);
  //agent.linearSystemogenesis();
  ////drawAgentLife(agent, environment, lifetime, imageFilename);
  //auto cycle = TBehaviorAnalysis::findCycleInAgentLife(agent, environment);
  //RestrictedHypercubeEnv emptyEnv;
  //emptyEnv.setEnvResolution(environment.getEnvironmentResolution());
  //TBehaviorAnalysis::drawCycleToDot(cycle, emptyEnv, imageFilename);
  //auto cycles = TBehaviorAnalysis::findAllCyclesOfAgent(agent, environment);
  //copy(begin(cycles.at(0).cycleSequence), end(cycles.at(0).cycleSequence), ostream_iterator<double>(cout, "\t"));
  //cout << endl << endl;
  //TBehaviorAnalysis::drawCyclesListToDot(TBehaviorAnalysis::findAllCyclesOfAgent(agent, environment), environment, imageFilename);

}

void currentAnalysis(){
  const string folder = "C:/Test/SANDBOX/";
  const string agentFilename = folder + "En1113_pc40anp40(18)_bestpopulation.txt";
  const string bestAgentsFilename = folder + "En1113_pc40anp40(18)_bestagents.txt";
  const string environmentFilename = folder + "Environment1113.txt";
  const string settingsFilename = folder + "settings_PC40ANP40.ini";
  const unsigned int agentNumber = 179;
  const unsigned int lifetime = 100;
  TAgent agent;
  ofstream res((folder + "devPredConHist.txt").c_str());
  ifstream rew((folder + "bestPopRewards.txt").c_str());
  for (int pop = 1; pop <= 100; ++pop){
    double curRew;
    rew >> curRew;
    if (curRew < 700) continue;
    stringstream some;
    some << folder << "/bestpopulations/En1113_pc40anp40(" << pop << ")_bestpopulation.txt";
    ifstream agentsFile(some.str().c_str());
    cout << pop << endl;
    for (int agentN = 1; agentN <= 250; ++agentN){
      agent.loadGenome(agentsFile);
      TPoolNetwork* genome = agent.getPointerToAgentGenome();
      for (int pool = 16; pool <= genome->getPoolsQuantity(); ++pool)
        for (int con = 1; con <= genome->getPoolInputPredConnectionsQuantity(pool); ++con)
          res << genome->getDevelopPredConnectionProb(pool, con) << endl;
    }
    agentsFile.close();
  }
  res.close();
  return;
  //THypercubeEnvironment environment(environmentFilename);
  //settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
  //environment.setStochasticityCoefficient(0.0);
  //TAgent agent;
  //settings::fillAgentSettingsFromFile(agent, settingsFilename);
  //agent.loadGenome(agentFilename, agentNumber);
  //ofstream outRes((folder + "results.txt").c_str());
  //for (unsigned int run = 0; run < 20; ++run){
  //  agent.systemogenesis();

  //  stringstream str;
  //  str << folder << "agent" << run << ".txt";
  //  ofstream out(str.str().c_str());
  //  agent.uploadController(out);
  //  out.close();

  //  agent.setLearningMode(0);
  //  vector<double> rewardsWithoutLearning = techanalysis::totalRun(agent, environment, lifetime);
  //  double rewardWithoutLearning = techanalysis::sum(rewardsWithoutLearning) / rewardsWithoutLearning.size();
  //  agent.setLearningMode(1);
  //  vector<double> rewardsWithLearning = techanalysis::totalRun(agent, environment, lifetime);
  //  double rewardWithLearning = techanalysis::sum(rewardsWithLearning) / rewardsWithLearning.size();
  //  outRes << run << "\t" << rewardWithoutLearning << "\t" << rewardWithLearning << endl;
  //  cout << run << "\t" << rewardWithoutLearning << "\t" << rewardWithLearning << endl;
  //}
  //outRes.close();
  //techanalysis::difEvolutionAnalysis(bestAgentsFilename, 5000, environmentFilename, settingsFilename, folder + "evolutionDiffResults.txt", 25, 5, 100);
}

/**
* Отрисовка жизни агента в формате .jpg с помощью утилиты dot (путь к утилите должен находиться в переменной $PATH$).
* В среде должно быть установлено начальное состояние, из которого запускается агент.
* \param [in] agent - нейросетевой агент.
* \param [in] environment - среда с установленным начальным состоянием.
* \param [in] lifetime - время жизни агента.
* \param [in] imageFilename - путь к генерируемому файлу .jpg жизни агента.
*/
void drawAgentLife(TAgent& agent, THypercubeEnvironment& environment, unsigned int lifetime, const string& imageFilename){
  //const int initState = environment.getEnvironmentState();
  //environment.setStochasticityCoefficient(0.0);
  //agent.life(environment, lifetime, false);
  //const auto& life = agent.getPointerToAgentLife();
  //vector<double> _life;
  //_life.reserve(life.size());
  //transform(life.cbegin(), life.cend(), back_inserter(_life), [](const vector<double>& v){ return v.at(0); });  
  //TBehaviorAnalysis::drawActionSequenceToDot(_life, environment, imageFilename, initState);
