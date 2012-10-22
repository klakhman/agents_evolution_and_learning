#include "TEvolutionaryProcess.h"
#include "TPopulation.h"
#include "TEnvironment.h"
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace std;

// �������� ���������� ��������� �� �����
void TEvolutionaryProcess::fillPopulationSettingsFromFile(){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(filenameSettings.settingsFilename);
	while (settingsFile >> optionString){
		if (optionString == "population-size") { settingsFile >> optionString; agentsPopulation->setPopulationSize(atoi(optionString.c_str())); }
		if (optionString == "agent-lifetime") { settingsFile >> optionString; agentsPopulation->evolutionSettings.agentLifetime = atoi(optionString.c_str()); }
		if (optionString == "evolution-time") { settingsFile >> optionString; agentsPopulation->evolutionSettings.evolutionTime = atoi(optionString.c_str()); }
		// ����������� ���������
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

// �������� ���������� ����� �� �����
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

// �������� ���������� ������ �� �����
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

// ����� ������� ��������� (���������) �� ������� ��� � ����
void TEvolutionaryProcess::makeLogNote(ostream& outputConsole, int currentEvolutionStep /*=0*/){
	// ������������ ������� ��������������
	double averagePoolsQuantity = 0;
	double averageConnectionsQuantity = 0;
	double averagePredConnectionsQuantity = 0;
	double averageReward = 0;
	double maxReward = 0;
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent){
		TPoolNetwork* currentGenomePointer = agentsPopulation->getPointertoAgent(currentAgent)->getPointerToAgentGenome();
		averagePoolsQuantity += currentGenomePointer->getPoolsQuantity();
		averageConnectionsQuantity += currentGenomePointer->getConnectionsQuantity();
		averagePredConnectionsQuantity += currentGenomePointer->getPredConnectionsQuantity();
		averageReward += agentsPopulation->getPointertoAgent(currentAgent)->getReward();
		if (agentsPopulation->getPointertoAgent(currentAgent)->getReward() > maxReward)
			maxReward = agentsPopulation->getPointertoAgent(currentAgent)->getReward();
	}
	averagePoolsQuantity /= agentsPopulation->getPopulationSize();
	averageConnectionsQuantity /= agentsPopulation->getPopulationSize();
	averagePredConnectionsQuantity /= agentsPopulation->getPopulationSize();
	averageReward /= agentsPopulation->getPopulationSize();
	outputConsole << currentEvolutionStep << ": " << averageReward << "\t" << maxReward << "\t" <<
		averagePoolsQuantity << "\t" << averageConnectionsQuantity << "\t" << averagePredConnectionsQuantity << endl;  
}

// ������ ������������� �������� (���������� ����� ������������, ���� 0, �� ������������ ���������������� ����������)
void TEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/){
	// ���� �� ���� �������� ����� ������������
	if (!randomSeed)
		randomSeed = static_cast<unsigned int>(time(0));
	srand(randomSeed);
	// ������� ���������� ��������� �����, ����� �������� ��������
	rand();
	rand();
	rand();
	// ��������� �����
	if (environment)
		delete environment;
	environment = new TEnvironment(filenameSettings.environmentFilename);
	fillEnvironmentSettingsFromFile();
	// ���� ���� ������� ��� ���������� (������ ���� �� ������� ����������� �������� ����������� ��������)
	if (agentsPopulation)
		delete agentsPopulation;
	agentsPopulation = new TPopulation;
	fillPopulationSettingsFromFile();
	// ��������� ������ � ��������� ��� ������� (����� ����, ��� ���������� ������ ���������), ������� ����� ��������� � ��� ���������
	fillAgentSettingsFromFile();
	// ��������� ��� ��������� � �������, ������� ����� ������������ ����������� ���������
	agentsPopulation->generateMinimalPopulation(environment->getEnvironmentResolution());

	for (int currentEvolutionStep = 1; currentEvolutionStep <= agentsPopulation->evolutionSettings.evolutionTime; ++currentEvolutionStep){
		agentsPopulation->evolutionaryStep(*environment, currentEvolutionStep); 
		makeLogNote(cout, currentEvolutionStep);
	}

}

