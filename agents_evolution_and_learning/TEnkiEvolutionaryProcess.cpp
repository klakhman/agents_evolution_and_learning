//
//  TEnkiEvolutionaryProcess.cpp
//  Agents Evolution And Learning ENKI
//
//  Created by Сергей Муратов on 10.03.13.
//  Copyright (c) 2013 Laboratory. All rights reserved.
//

#include "TEnkiEvolutionaryProcess.h"
#include "settings.h"
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <string>

#include <windows.h>
#include "atlstr.h"
#include <tchar.h>
#include <comdef.h> 

using namespace std;

// Вывод логовых сообщений (прогресса) на консоль или в файл
void TEnkiEvolutionaryProcess::makeLogNote(std::ostream& outputConsole, std::ostream& bestAgentsConsole, TPopulation<TEnkiAgent>* bestPopulation, int currentEvolutionStep /*=0*/) {
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
	outputConsole <<  currentEvolutionStep << "\t" << averageReward << "\t" << maxReward << "\t" <<
  averagePoolsQuantity << "\t" << averageConnectionsQuantity << "\t" << averagePredConnectionsQuantity << endl;
	// Записываем лучшего агента и всю популяцию, если нужно
	agentsPopulation->getPointertoAgent(bestAgent)->uploadGenome(bestAgentsConsole, extraPrint);
  
	if (averageReward > bestAverageReward){
		//agentsPopulation->uploadPopulation(filenameSettings.bestPopulationFilename);
		*bestPopulation = *agentsPopulation;
		bestAverageReward = averageReward;
	}
}

void TEnkiEvolutionaryProcess::createMainResultsFile(std::ofstream& resultsFile, unsigned int randomSeed) {
  // Опустошаем файлы если они есть
	//ofstream resultsFile;
	resultsFile.open(filenameSettings.resultsFilename.c_str());
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
	TEnkiAgent* pointerToAgent = agentsPopulation->getPointertoAgent(1);
	// Записываем параметры первичного системогенеза
  resultsFile << "Primary_systemogenesis_parameters:" << "\tprimary-systemogenesis-mode=" << pointerToAgent->primarySystemogenesisSettings.primarySystemogenesisMode << "\tinitial-pool-capacity=" << pointerToAgent->primarySystemogenesisSettings.initialPoolCapacity
  << "\tinitial-develop-synapse-probability=" << pointerToAgent->primarySystemogenesisSettings.initialDevelopSynapseProbability << "\tinitial-develop-predconnection-prob=" << pointerToAgent->primarySystemogenesisSettings.initialDevelopPredConnectionProbability
  << "\tprimary-systemogenesis-time=" << pointerToAgent->primarySystemogenesisSettings.primarySystemogenesisTime << "\tspontaneous-acivity-prob=" << pointerToAgent->primarySystemogenesisSettings.spontaneousActivityProb
  << "\tactive-neurons-percent=" << pointerToAgent->primarySystemogenesisSettings.activeNeuronsPercent << "\tsynapses-activity-treshold=" << pointerToAgent->primarySystemogenesisSettings.synapsesActivityTreshold
  << "\tsignificance-treshold=" << pointerToAgent->primarySystemogenesisSettings.significanceTreshold << endl << endl;
  // Записываем параметры обучения
  resultsFile << "Learning_parameters:" << "\tlearning-mode=" << pointerToAgent->learningSettings.learningMode << "\tmismatch-significance-treshold=" << pointerToAgent->learningSettings.mismatchSignificanceTreshold << endl << endl;
	// Записываем параметры среды
  //environment->printSettings(resultsFile);
  //TEnkiEnvironment * enkiEnvironment = dynamic_cast<TEnkiEnvironment *>(environment);
	environment->printSettings(resultsFile);
	//Записываем уникальное ядро рандомизации
	resultsFile << "Random_seed:\t" << randomSeed << endl << endl;
	// Записываем заголовки
	resultsFile << "Step\tAverage_reward\tMax_reward\tPools\tConnections\tPredConnections" << endl;
	//resultsFile.close();
}

void TEnkiEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/, const string initPopulationFilename/*=""*/){
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
  TEnkiEnvironment * newEnvironment = new TEnkiEnvironment;
  settings::fillEnvironmentSettingsFromFile(*dynamic_cast<TEnkiEnvironment*>(newEnvironment), filenameSettings.settingsFilename);
  newEnvironment->loadEnvironment(filenameSettings.environmentFilename);
  //newEnvironment->printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects1.txt", 0);
  //newEnvironment->printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects2.txt", 1);
  //newEnvironment->printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects3.txt", 2);
  //newEnvironment->printOutObjectsForGnuplot("/Users/Sergey/Desktop/Agents-Evolution-And-Learning-ENKI/gnuplotObjects4.txt", 3);
  newEnvironment->randomlySwapCubesForCircleEnvironment();
  environment = newEnvironment;
	// environment = new TEnkiEnvironment(filenameSettings.environmentFilename);
	// Если этот процесс уже запускался (ВООБЩЕ НАДО БЫ СДЕЛАТЬ ВОЗМОЖНОСТЬ ПРОСТОГО ПРОДОЛЖЕНИЯ ЭВОЛЮЦИИ)
	if (agentsPopulation)
		delete agentsPopulation;
	agentsPopulation = new TPopulation<TEnkiAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
	// Настройки уже загружены в агентов, поэтому можем генерировать минимальную популяцию
	if (initPopulationFilename == "")
		agentsPopulation->generateMinimalPopulation(environment->getEnvironmentResolution());
	else
		agentsPopulation->loadPopulation(initPopulationFilename);
	// Опустошаем файл лучших агентов если он есть и создаем файл результатов
	ofstream resultsFile;
	createMainResultsFile(resultsFile, randomSeed);
	ofstream bestAgentsFile;
	bestAgentsFile.open(filenameSettings.bestAgentsFilename.c_str());
	// Создаем структуру лучшей популяции (если процессор достаточно быстрый, то копирование популяций будет быстрее чем каждый раз записывать популяцию в файл)
	TPopulation<TEnkiAgent>* bestPopulation = new TPopulation<TEnkiAgent>;
	for (int currentEvolutionStep = 1; currentEvolutionStep <= agentsPopulation->evolutionSettings.evolutionTime; ++currentEvolutionStep){
		agentsPopulation->evolutionaryStep(*environment, currentEvolutionStep);
		TEnkiEvolutionaryProcess::makeLogNote(resultsFile, bestAgentsFile, bestPopulation, currentEvolutionStep);
	}
	// Заспиываем лучшую популяцию
	if (!extraPrint) bestPopulation->uploadPopulation(filenameSettings.bestPopulationFilename);
	else	{
    ofstream bestPopulationFile;
    bestPopulationFile.open(filenameSettings.bestPopulationFilename.c_str());
    for (int currentAgent = 1; currentAgent <= bestPopulation->getPopulationSize(); ++ currentAgent)
      bestPopulation->getPointertoAgent(currentAgent)->uploadGenome(bestPopulationFile, extraPrint);
    bestPopulationFile.close();
  }
  
	delete bestPopulation;
  
	resultsFile.close();
	bestAgentsFile.close();
}

void TEnkiEvolutionaryProcess::makeResultsOverview(const string& dir, unsigned int evolutionDuration, const string& maxRewardFilename, const string& averageRewardFilename){
	string tmp_dir = dir + "/*.*"; 
    std::vector<TCHAR> tchar_dir(tmp_dir.size() + 1);
    std::copy(tmp_dir.begin(), tmp_dir.end(), tchar_dir.begin()); 
    tchar_dir[tchar_dir.size() - 1] = 0;
	
	vector< vector<double> > averageRewards, maxRewards;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(&tchar_dir[0], &FindFileData);
	int exit = 1;
	while (exit != 0) {
		_bstr_t tmp(FindFileData.cFileName);
		const char* c = tmp;
		string filename = c;
		if (!((filename == ".") || (filename == ".."))){ 
			filename = dir + filename;
			cout << filename << "\n";
			averageRewards.push_back(vector<double>());
			averageRewards.back().resize(evolutionDuration);
			maxRewards.push_back(vector<double>());
			maxRewards.back().resize(evolutionDuration);
			ifstream inputFile(filename.c_str());
			string tmpStr; 
			for (unsigned int i = 0; i < 13; ++i){
				getline(inputFile, tmpStr);
				//cout << tmpStr << endl;
			}
			for (unsigned int evolutionaryStep = 1; evolutionaryStep <= evolutionDuration; ++evolutionaryStep){
				inputFile >> tmpStr;
				inputFile >> averageRewards.back()[evolutionaryStep - 1] >> maxRewards.back()[evolutionaryStep - 1];
				inputFile >> tmpStr >> tmpStr >> tmpStr;
			}
		}
		exit = FindNextFile(hFind, &FindFileData);
	}
	ofstream maxRewardsFile((maxRewardFilename).c_str());
	ofstream averageRewardsFile((averageRewardFilename).c_str());
	for (unsigned int step = 1; step <= evolutionDuration; ++step){
		maxRewardsFile << step << "\t";
		averageRewardsFile << step << "\t";
		for (unsigned int run = 1; run <= averageRewards.size(); ++run){
			maxRewardsFile << maxRewards[run - 1][step - 1] << "\t";
			averageRewardsFile << averageRewards[run - 1][step - 1] << "\t";
		}
		maxRewardsFile << endl;
		averageRewardsFile << endl;
	}
	maxRewardsFile.close();
	averageRewardsFile.close();
}