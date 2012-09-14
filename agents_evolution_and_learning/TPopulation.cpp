#include "TPopulation.h"
#include <fstream>

using namespace std;

// Загрузка популяции геномов из файла
void TPopulation::loadPopulation(std::string populationFilename, int _populationSize){
	erasePopulation();
	agents = new TAgent*[_populationSize];
	populationSize = _populationSize;
	ifstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		agents[currentAgent - 1] = new TAgent;
		agents[currentAgent - 1]->loadGenome(populationFile);
	}
	populationFile.close();
}

// Выгрузка популяции геномов в файл
void TPopulation::uploadPopulation(std::string populationFilename) const{
	ofstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->uploadGenome(populationFile);
	populationFile.close();
}

// Генерация минимальной популяции
void TPopulation::generateMinimalPopulation(int _populationSize, int inputResolution){
	erasePopulation();
	agents = new TAgent*[_populationSize];
	populationSize = _populationSize;
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		agents[currentAgent - 1] = new TAgent;
		agents[currentAgent - 1]->generateMinimalAgent(inputResolution);
	}
}

// Процедура эволюции популяции
void TPopulation::evolution(TEnvironment& environment, int evolutionTime /* = 0*/){
	for (int evolutionStep = 1; evolutionStep <= evolutionTime; ++evolutionStep){
		// Прогоняем всех агентов
		for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
			// Проводим первичный системогенез
			agents[currentAgent - 1]->linearSystemogenesis();
			environment.setRandomEnvironmentVector();
			agents[currentAgent - 1]->life(environment, 250); // !!! надо вписать жизнь агента

		}
	}
}