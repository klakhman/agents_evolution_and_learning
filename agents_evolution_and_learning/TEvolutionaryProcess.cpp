#include "TEvolutionaryProcess.h"
#include "TPopulation.h"
#include "TEnvironment.h"
#include <ctime>

// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
void TEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/){
	// Если было передано зерно рандомизации, то инициализируем им
	if (randomSeed)
		srand(randomSeed);
	else 
		srand(static_cast<unsigned int>(time(0)));
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
	agentsPopulation->generateMinimalPopulation();

	fillAgentSettingsFromFile();

}

