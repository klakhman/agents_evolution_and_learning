#include "TEvolutionaryProcess.h"
#include "TParallelEvolutionaryProcess.h"
#include "service.h"
#include "settings.h"
#include "TAnalysis.h"
#include "TBehaviorAnalysis.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstdio>
#include <cstring>

#include "tests.h"
#include <ctime>
#include <new>

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

void decodeCommandPromt(string& environemtnFilename, string& resultsFilename, string& bestPopulationFilename, string& bestAgentsFilename, long& randomSeed, bool& extraPrint, int argc, char** argv){
	int currentArgNumber = 1; // Текущий номер параметра
	while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			if (!strcmp("-envfile", argv[currentArgNumber])) environemtnFilename = argv[++currentArgNumber];
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
  else if (programMode == "BCA"){//Режим анализа поведенчиских циклов
    TBehaviorAnalysis* behaviorAnalysis = new TBehaviorAnalysis;
    behaviorAnalysis->beginAnalysis(argc, argv);
    delete behaviorAnalysis;
    
  }

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
