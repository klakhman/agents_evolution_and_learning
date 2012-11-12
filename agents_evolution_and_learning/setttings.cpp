#include "settings.h"
#include "TEnvironment.h"
#include "TPopulation.h"
#include "TAgent.h"
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

// Заполнение параметров директорий для записи файлов
void settings::fillDirectoriesSettings(string& workDirectory, string& environmentDirectory, string& resultsDirectory, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	while (settingsFile >> optionString){
		if (optionString == "work-directory") { settingsFile >> workDirectory; }
		if (optionString == "environment-directory") { settingsFile >> environmentDirectory; }
		if (optionString == "results-directory") { settingsFile >> resultsDirectory; }
	}
	settingsFile.close();
}

// Загрузка параметров среды из файла
void settings::fillEnvironmentSettingsFromFile(TEnvironment& environment, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	while (settingsFile >> optionString){
		if (optionString == "reward-recovery-time") { settingsFile >> optionString; environment.setRewardRecoveryTime(atoi(optionString.c_str())); }
		if (optionString == "stochasticity-coefficient") { settingsFile >> optionString; environment.setStochasticityCoefficient(atof(optionString.c_str())); }
	}
	settingsFile.close();
}

// Загрузка параметров популяции из файла
void settings::fillPopulationSettingsFromFile(TPopulation& agentsPopulation, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	while (settingsFile >> optionString){
		if (optionString == "population-size") { settingsFile >> optionString; agentsPopulation.setPopulationSize(atoi(optionString.c_str())); }
		if (optionString == "agent-lifetime") { settingsFile >> optionString; agentsPopulation.evolutionSettings.agentLifetime = atoi(optionString.c_str()); }
		if (optionString == "evolution-time") { settingsFile >> optionString; agentsPopulation.evolutionSettings.evolutionTime = atoi(optionString.c_str()); }
		// Мутационные параметры
		if (optionString == "mut-weight-probability") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutWeightProbability = atof(optionString.c_str()); }
		if (optionString == "mut-weight-mean-disp") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutWeightMeanDisp = atof(optionString.c_str()); }
		if (optionString == "mut-weight-disp-disp") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutWeightDispDisp = atof(optionString.c_str()); }
		if (optionString == "dis-limit") { settingsFile >> optionString; agentsPopulation.mutationSettings.disLimit = atoi(optionString.c_str()); }
		if (optionString == "enable-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.enableConnectionProb = atof(optionString.c_str()); }
		if (optionString == "disable-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.disableConnectionProb = atof(optionString.c_str()); }
		if (optionString == "add-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.addConnectionProb = atof(optionString.c_str()); }
		if (optionString == "add-predconnection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.addPredConnectionProb = atof(optionString.c_str()); }
		if (optionString == "delete-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.deleteConnectionProb = atof(optionString.c_str()); }
		if (optionString == "delete-predconnection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.deletePredConnectionProb = atof(optionString.c_str()); }
		if (optionString == "duplicate-pool-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.duplicatePoolProb = atof(optionString.c_str()); }
		if (optionString == "pool-division-coef") { settingsFile >> optionString; agentsPopulation.mutationSettings.poolDivisionCoef = atof(optionString.c_str()); }
		if (optionString == "pool-standart-amount") { settingsFile >> optionString; agentsPopulation.mutationSettings.poolStandartAmount = atoi(optionString.c_str()); }
		if (optionString == "connection-standart-amount") { settingsFile >> optionString; agentsPopulation.mutationSettings.connectionStandartAmount = atoi(optionString.c_str()); }
		if (optionString == "mut-develop-con-prob-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutDevelopConProbProb = atof(optionString.c_str()); }
		if (optionString == "mut-develop-con-prob-disp") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutDevelopConProbDisp = atof(optionString.c_str()); }
	}
	settingsFile.close();
}

// Загрузка параметров агента из файла
void settings::fillAgentsPopulationSettingsFromFile(TPopulation& agentsPopulation, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
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
	for (int currentAgent = 1; currentAgent <= agentsPopulation.getPopulationSize(); ++currentAgent)
	{
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialPoolCapacity = primarySystemogenesisSettings->initialPoolCapacity;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialDevelopSynapseProbability = primarySystemogenesisSettings->initialDevelopSynapseProbability;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialDevelopPredConnectionProbability = primarySystemogenesisSettings->initialDevelopPredConnectionProbability;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.primarySystemogenesisTime = primarySystemogenesisSettings->primarySystemogenesisTime;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.spontaneousActivityProb = primarySystemogenesisSettings->spontaneousActivityProb;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.activeNeuronsPercent = primarySystemogenesisSettings->activeNeuronsPercent;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.synapsesActivityTreshold = primarySystemogenesisSettings->synapsesActivityTreshold;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.significanceTreshold = primarySystemogenesisSettings->significanceTreshold;
	}
	delete primarySystemogenesisSettings;
	settingsFile.close();
}

// Загрузка параметров агента из файла
void settings::fillAgentSettingsFromFile(TAgent& agent, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	while (settingsFile >> optionString){
		if (optionString == "initial-pool-capacity") { settingsFile >> optionString; agent.primarySystemogenesisSettings.initialPoolCapacity = atoi(optionString.c_str()); }
		if (optionString == "initial-develop-synapse-probability") { settingsFile >> optionString; agent.primarySystemogenesisSettings.initialDevelopSynapseProbability = atof(optionString.c_str()); }
		if (optionString == "initial-develop-predconnection-probability") { settingsFile >> optionString; agent.primarySystemogenesisSettings.initialDevelopPredConnectionProbability = atof(optionString.c_str()); }
		if (optionString == "primary-systemogenesis-time") { settingsFile >> optionString; agent.primarySystemogenesisSettings.primarySystemogenesisTime = atoi(optionString.c_str()); }
		if (optionString == "spontaneous-activity-prob") { settingsFile >> optionString; agent.primarySystemogenesisSettings.spontaneousActivityProb = atof(optionString.c_str()); }
		if (optionString == "active-neurons-percent") { settingsFile >> optionString; agent.primarySystemogenesisSettings.activeNeuronsPercent = atof(optionString.c_str()); }
		if (optionString == "synapses-activity-treshold") { settingsFile >> optionString; agent.primarySystemogenesisSettings.synapsesActivityTreshold = atof(optionString.c_str()); }
		if (optionString == "significance-treshold") { settingsFile >> optionString; agent.primarySystemogenesisSettings.significanceTreshold = atof(optionString.c_str()); }
	}
	settingsFile.close();
}