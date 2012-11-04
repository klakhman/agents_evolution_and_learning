#include "TEvolutionaryProcess.h"
#include "TPopulation.h"
#include "TEnvironment.h"
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>

#include <sstream>


using namespace std;

// Загрузка параметров популяции из файла
void TEvolutionaryProcess::fillPopulationSettingsFromFile(){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(filenameSettings.settingsFilename);
	while (settingsFile >> optionString){
		if (optionString == "population-size") { settingsFile >> optionString; agentsPopulation->setPopulationSize(atoi(optionString.c_str())); }
		if (optionString == "agent-lifetime") { settingsFile >> optionString; agentsPopulation->evolutionSettings.agentLifetime = atoi(optionString.c_str()); }
		if (optionString == "evolution-time") { settingsFile >> optionString; agentsPopulation->evolutionSettings.evolutionTime = atoi(optionString.c_str()); }
		// Мутационные параметры
		if (optionString == "mut-weight-probability") { settingsFile >> optionString; agentsPopulation->mutationSettings.mutWeightProbability = atof(optionString.c_str()); }
		if (optionString == "mut-weight-mean-disp") { settingsFile >> optionString; agentsPopulation->mutationSettings.mutWeightMeanDisp = atof(optionString.c_str()); }
		if (optionString == "mut-weight-disp-disp") { settingsFile >> optionString; agentsPopulation->mutationSettings.mutWeightDispDisp = atof(optionString.c_str()); }
		if (optionString == "dis-limit") { settingsFile >> optionString; agentsPopulation->mutationSettings.disLimit = atoi(optionString.c_str()); }
		if (optionString == "enable-connection-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.enableConnectionProb = atof(optionString.c_str()); }
		if (optionString == "disable-connection-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.disableConnectionProb = atof(optionString.c_str()); }
		if (optionString == "add-connection-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.addConnectionProb = atof(optionString.c_str()); }
		if (optionString == "add-predconnection-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.addPredConnectionProb = atof(optionString.c_str()); }
		if (optionString == "delete-connection-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.deleteConnectionProb = atof(optionString.c_str()); }
		if (optionString == "delete-predconnection-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.deletePredConnectionProb = atof(optionString.c_str()); }
		if (optionString == "duplicate-pool-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.duplicatePoolProb = atof(optionString.c_str()); }
		if (optionString == "pool-division-coef") { settingsFile >> optionString; agentsPopulation->mutationSettings.poolDivisionCoef = atof(optionString.c_str()); }
		if (optionString == "pool-standart-amount") { settingsFile >> optionString; agentsPopulation->mutationSettings.poolStandartAmount = atoi(optionString.c_str()); }
		if (optionString == "connection-standart-amount") { settingsFile >> optionString; agentsPopulation->mutationSettings.connectionStandartAmount = atoi(optionString.c_str()); }
		if (optionString == "mut-develop-con-prob-prob") { settingsFile >> optionString; agentsPopulation->mutationSettings.mutDevelopConProbProb = atof(optionString.c_str()); }
		if (optionString == "mut-develop-con-prob-disp") { settingsFile >> optionString; agentsPopulation->mutationSettings.mutDevelopConProbDisp = atof(optionString.c_str()); }
	}
	settingsFile.close();
}

// Загрузка параметров среды из файла
void TEvolutionaryProcess::fillEnvironmentSettingsFromFile(){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(filenameSettings.settingsFilename);
	while (settingsFile >> optionString){
		if (optionString == "reward-recovery-time") { settingsFile >> optionString; environment->setRewardRecoveryTime(atoi(optionString.c_str())); }
		if (optionString == "nonstationarity-coefficient") { settingsFile >> optionString; environment->setNonstationarityCoefficient(atof(optionString.c_str())); }
	}
	settingsFile.close();
}

// Загрузка параметров агента из файла
void TEvolutionaryProcess::fillAgentSettingsFromFile(){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(filenameSettings.settingsFilename);
	TAgent::SPrimarySystemogenesisSettings* primarySystemogenesisSettings = new TAgent::SPrimarySystemogenesisSettings;
	while (settingsFile >> optionString){
		if (optionString == "initial-pool-capacity") { settingsFile >> optionString; primarySystemogenesisSettings->initialPoolCapacity = atoi(optionString.c_str()); }
		if (optionString == "initial-develop-synapse-probability") { settingsFile >> optionString; primarySystemogenesisSettings->initialDevelopSynapseProbability = atof(optionString.c_str()); }
		if (optionString == "initial-develop-predconnection-probability") { settingsFile >> optionString; primarySystemogenesisSettings->initialDevelopPredConnectionProbability = atof(optionString.c_str()); }
		if (optionString == "primary-systemogenesis-time") { settingsFile >> optionString; primarySystemogenesisSettings->primarySystemogenesisTime = atoi(optionString.c_str()); }
		if (optionString == "spontaneous-activity-prob") { settingsFile >> optionString; primarySystemogenesisSettings->spontaneousActivityProb = atof(optionString.c_str()); }
		if (optionString == "active-neurons-percent") { settingsFile >> optionString; primarySystemogenesisSettings->activeNeuronsPercent = atof(optionString.c_str()); }
		if (optionString == "synapses-activity-treshold") { settingsFile >> optionString; primarySystemogenesisSettings->synapsesActivityTreshold = atof(optionString.c_str()); }
		if (optionString == "significance-treshold") { settingsFile >> optionString; primarySystemogenesisSettings->significanceTreshold = atof(optionString.c_str()); }
	}
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent)
	{
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialPoolCapacity = primarySystemogenesisSettings->initialPoolCapacity;
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialDevelopSynapseProbability = primarySystemogenesisSettings->initialDevelopSynapseProbability;
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialDevelopPredConnectionProbability = primarySystemogenesisSettings->initialDevelopPredConnectionProbability;
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.primarySystemogenesisTime = primarySystemogenesisSettings->primarySystemogenesisTime;
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.spontaneousActivityProb = primarySystemogenesisSettings->spontaneousActivityProb;
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.activeNeuronsPercent = primarySystemogenesisSettings->activeNeuronsPercent;
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.synapsesActivityTreshold = primarySystemogenesisSettings->synapsesActivityTreshold;
		agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesisSettings.significanceTreshold = primarySystemogenesisSettings->significanceTreshold;
	}
	delete primarySystemogenesisSettings;
	settingsFile.close();
}

// Вывод логовых сообщений (прогресса) на консоль или в файл
void TEvolutionaryProcess::makeLogNote(ostream& outputConsole, int currentEvolutionStep /*=0*/){
	// Подсчитываем средние характеристики
	double averagePoolsQuantity = 0;
	double averageConnectionsQuantity = 0;
	double averagePredConnectionsQuantity = 0;
	double averageReward = 0;
	double maxReward = 0;
	int bestAgent = 1;
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent){
		TPoolNetwork* currentGenomePointer = agentsPopulation->getPointertoAgent(currentAgent)->getPointerToAgentGenome();
		averagePoolsQuantity += currentGenomePointer->getPoolsQuantity();
		averageConnectionsQuantity += currentGenomePointer->getConnectionsQuantity();
		averagePredConnectionsQuantity += currentGenomePointer->getPredConnectionsQuantity();
		averageReward += agentsPopulation->getPointertoAgent(currentAgent)->getReward();
		if (agentsPopulation->getPointertoAgent(currentAgent)->getReward() > maxReward){
			bestAgent = currentAgent;
			maxReward = agentsPopulation->getPointertoAgent(currentAgent)->getReward();
		}
	}
	averagePoolsQuantity /= agentsPopulation->getPopulationSize();
	averageConnectionsQuantity /= agentsPopulation->getPopulationSize();
	averagePredConnectionsQuantity /= agentsPopulation->getPopulationSize();
	averageReward /= agentsPopulation->getPopulationSize();
	//outputConsole << currentEvolutionStep << ": " << averageReward << "\t" << maxReward << "\t" <<
	//	averagePoolsQuantity << "\t" << averageConnectionsQuantity << "\t" << averagePredConnectionsQuantity << endl;
	// Также записываем в файл результатов
	ofstream resultsFile;
	resultsFile.open(filenameSettings.resultsFilename, fstream::app);
	resultsFile <<  currentEvolutionStep << "\t" << averageReward << "\t" << maxReward << "\t" <<
		averagePoolsQuantity << "\t" << averageConnectionsQuantity << "\t" << averagePredConnectionsQuantity << endl;
	resultsFile.close();
	// Записываем лучшего агента и всю популяцию, если нужно
	ofstream bestAgentsFile;
	bestAgentsFile.open(filenameSettings.bestAgentsFilename, fstream::app);
	agentsPopulation->getPointertoAgent(bestAgent)->uploadGenome(bestAgentsFile);
	bestAgentsFile.close();
	if (averageReward > bestAverageReward){
		ofstream bestPopulationFile;
		bestPopulationFile.open(filenameSettings.bestPopulationFilename);
		for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent)
			agentsPopulation->getPointertoAgent(currentAgent)->uploadGenome(bestPopulationFile);
		bestPopulationFile.close();
	}
	/*stringstream tmp_stream;
	tmp_stream << currentEvolutionStep;
	string tmp_str;
	tmp_stream >> tmp_str;
	agentsPopulation->getPointertoAgent(bestAgent)->getPointerToAgentGenome()->printGraphNetwork("C:/Tests/Networks/pool_net_" + tmp_str + ".jpg");
	agentsPopulation->getPointertoAgent(bestAgent)->getPointerToAgentController()->printGraphNetwork("C:/Tests/Networks/neural_net_" + tmp_str + ".jpg");*/
}

// Создание и заполнение предварительного файла основных результатов
void TEvolutionaryProcess::createMainResultsFile(unsigned int randomSeed){
	// Опустошаем файлы если они есть
	ofstream resultsFile;
	resultsFile.open(filenameSettings.resultsFilename);
	//Записываем параметры эволюции
	resultsFile << "Evolutionary_parameters:" << "\tpopulation-size=" << agentsPopulation->getPopulationSize() << "\tagent-lifetime=" << agentsPopulation->evolutionSettings.agentLifetime
		<< "\tevolution-time=" << agentsPopulation->evolutionSettings.evolutionTime << endl << endl;
	// Записываем параметры мутаций
	resultsFile << "Mutation_parameters:" << "\tmut-weight-probability=" << agentsPopulation->mutationSettings.mutWeightProbability << "\tmut-weight-mean-disp=" << agentsPopulation->mutationSettings.mutWeightMeanDisp
		<< "\tmut-weight-disp-disp=" << agentsPopulation->mutationSettings.mutWeightDispDisp << "\tdis-limit=" << agentsPopulation->mutationSettings.disLimit << "\tenable-connection-prob=" << agentsPopulation->mutationSettings.enableConnectionProb
		<< "\tdisable-connection-prob=" << agentsPopulation->mutationSettings.disableConnectionProb << "\tadd-connection-prob=" << agentsPopulation->mutationSettings.addConnectionProb
		<< "\tadd-predconnection-prob=" << agentsPopulation->mutationSettings.addPredConnectionProb << "\tdelete-connection-prob=" << agentsPopulation->mutationSettings.deleteConnectionProb
		<< "\tdelete-predconnection-prob=" << agentsPopulation->mutationSettings.deletePredConnectionProb << "\tduplicate-pool-prob=" << agentsPopulation->mutationSettings.duplicatePoolProb
		<< "\tpool-division-coef=" << agentsPopulation->mutationSettings.poolDivisionCoef << "\tpool-standart-amount=" << agentsPopulation->mutationSettings.poolStandartAmount
		<< "\tconnection-standart-amount=" << agentsPopulation->mutationSettings.connectionStandartAmount << "\tmut-develop-con-prob-prob=" << agentsPopulation->mutationSettings.mutDevelopConProbProb
		<< "\tmut-develop-con-prob-disp=" << agentsPopulation->mutationSettings.mutDevelopConProbDisp << endl << endl;
	TAgent* pointerToAgent = agentsPopulation->getPointertoAgent(1);
	// Записываем параметры первичного системогенеза
	resultsFile << "Primary_systemogenesis_parameters:" << "\tinitial-pool-capacity=" << pointerToAgent->primarySystemogenesisSettings.initialPoolCapacity
		<< "\tinitial-develop-synapse-probability=" << pointerToAgent->primarySystemogenesisSettings.initialDevelopSynapseProbability << "\tinitial-develop-predconnection-prob=" << pointerToAgent->primarySystemogenesisSettings.initialDevelopPredConnectionProbability
		<< "\tprimary-systemogenesis-time=" << pointerToAgent->primarySystemogenesisSettings.primarySystemogenesisTime << "\tspontaneous-acivity-prob=" << pointerToAgent->primarySystemogenesisSettings.spontaneousActivityProb
		<< "\tactive-neurons-percent=" << pointerToAgent->primarySystemogenesisSettings.activeNeuronsPercent << "\tsynapses-activity-treshold=" << pointerToAgent->primarySystemogenesisSettings.synapsesActivityTreshold
		<< "\tsignificance-treshold=" << pointerToAgent->primarySystemogenesisSettings.significanceTreshold << endl << endl;
	// Записываем параметры среды
	resultsFile << "Environment_parameters:" << "\tnonstationarity-coefficient=" << environment->getNonstationarityCoefficient() << "\treward-recovery-time=" << environment->getRewardRecoveryTime() << endl << endl;
	//Записываем уникальное ядро рандомизации
	resultsFile << "Random_seed:\t" << randomSeed << endl << endl; 
	// Записываем заголовки
	resultsFile << "Step\tAverage_reward\tMax_reward\tPools\tConnections\tPredConnections" << endl;
	resultsFile.close();
}

// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
void TEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/){
	// Если не было передано зерно рандомизации
	if (!randomSeed)
		randomSeed = static_cast<unsigned int>(time(0));
	srand(randomSeed);
	// Запуски генератора случайных чисел, чтобы развести значения
	rand();
	rand();
	rand();
	// Загружаем среду
	if (environment)
		delete environment;
	environment = new TEnvironment(filenameSettings.environmentFilename);
	fillEnvironmentSettingsFromFile();
	// Если этот процесс уже запускался (ВООБЩЕ НАДО БЫ СДЕЛАТЬ ВОЗМОЖНОСТЬ ПРОСТОГО ПРОДОЛЖЕНИЯ ЭВОЛЮЦИИ)
	if (agentsPopulation)
		delete agentsPopulation;
	agentsPopulation = new TPopulation;
	fillPopulationSettingsFromFile();
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	fillAgentSettingsFromFile();
	// Опустошаем файл лучших агентов если он есть и создаем файл результатов
	createMainResultsFile(randomSeed);
	ofstream bestAgentsFile;
	bestAgentsFile.open(filenameSettings.bestAgentsFilename);
	bestAgentsFile.close();
	// Настройки уже загружены в агентов, поэтому можем генерировать минимальную популяцию
	agentsPopulation->generateMinimalPopulation(environment->getEnvironmentResolution());

	for (int currentEvolutionStep = 1; currentEvolutionStep <= agentsPopulation->evolutionSettings.evolutionTime; ++currentEvolutionStep){
		agentsPopulation->evolutionaryStep(*environment, currentEvolutionStep); 
		makeLogNote(cout, currentEvolutionStep);
	}
}

