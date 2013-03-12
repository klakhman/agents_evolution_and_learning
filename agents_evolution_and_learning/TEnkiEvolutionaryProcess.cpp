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

using namespace std;

void TEnkiEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/){
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
	environment = new TEnkiEnvironment(filenameSettings.environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*dynamic_cast<THypercubeEnvironment*>(environment), filenameSettings.settingsFilename);
	// Если этот процесс уже запускался (ВООБЩЕ НАДО БЫ СДЕЛАТЬ ВОЗМОЖНОСТЬ ПРОСТОГО ПРОДОЛЖЕНИЯ ЭВОЛЮЦИИ)
	if (agentsPopulation)
		delete agentsPopulation;
	agentsPopulation = new TPopulation<TEnkiAgent>;
	//settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	//settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
	// Опустошаем файл лучших агентов если он есть и создаем файл результатов
	ofstream resultsFile;
	createMainResultsFile(resultsFile, randomSeed);
	ofstream bestAgentsFile;
	bestAgentsFile.open(filenameSettings.bestAgentsFilename.c_str());
	// Настройки уже загружены в агентов, поэтому можем генерировать минимальную популяцию
	agentsPopulation->generateMinimalPopulation(environment->getEnvironmentResolution());
	// Создаем структуру лучшей популяции (если процессор достаточно быстрый, то копирование популяций будет быстрее чем каждый раз записывать популяцию в файл)
	TPopulation<TAgent>* bestPopulation = new TPopulation<TAgent>;
	for (int currentEvolutionStep = 1; currentEvolutionStep <= agentsPopulation->evolutionSettings.evolutionTime; ++currentEvolutionStep){
		agentsPopulation->evolutionaryStep(*environment, currentEvolutionStep);
		makeLogNote(resultsFile, bestAgentsFile, bestPopulation, currentEvolutionStep);
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