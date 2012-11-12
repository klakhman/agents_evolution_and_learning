#include "TEvolutionaryProcess.h"
#include "TParallelEvolutionaryProcess.h"
#include "service.h"
#include "TAnalysis.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstdio>

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
void decodeProgramMode(string& programMode, int argc, char** argv){
	int currentArgNumber = 2; // Текущий номер параметра (в первом записан путь к файлу настроек)
	while (currentArgNumber < argc){
		switch (argv[currentArgNumber][1]){ // Расшифровываем параметр (в первом поле "-")
			case 'm': // Если это режим запуска программного комплекса
				programMode = argv[++currentArgNumber];
				break;
		}
		++currentArgNumber;
	}
}

void decodeCommandPromt(string& environemtnFilename, string& resultsFilename, string& bestPopulationFilename, string& bestAgentsFilename, long& randomSeed, bool& extraPrint, int argc, char** argv){
	int currentArgNumber = 2; // Текущий номер параметра (в первом записан путь к файлу настроек)
	while (currentArgNumber < argc){
	switch (argv[currentArgNumber][1]){ // Расшифровываем параметр (в первом поле "-")
			case 'e': // Если это путь к файлу среды
				environemtnFilename = argv[++currentArgNumber];
				break;
			case 'r': // Если это путь к файлу результатов
				resultsFilename = argv[++currentArgNumber];
				break;
			case 'p': // Если это путь к лучшей популяции
				bestPopulationFilename = argv[++currentArgNumber];
				break;
			case 'a': // Если это путь к файлу с лучшими агентами
				bestAgentsFilename = argv[++currentArgNumber];
				break;
			case 's': // Если это зерно рандомизации
				randomSeed = atoi(argv[++currentArgNumber]);
				break;
			case 'h': // Если это признак расширенной записи пулов
				extraPrint = !(atoi(argv[++currentArgNumber]) == 0);
				break;
		}
		++currentArgNumber;
	}
}

int main(int argc, char** argv){
	// Устанавливаем обработчик нехватки памяти
	set_new_handler(out_of_memory);

	string programMode; // Режим работы программы - "E" - эволюция, "BPA" - анализ методом прогона лучшей популяции
	decodeProgramMode(programMode, argc, argv);
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
				evolutionaryProcess->filenameSettings.settingsFilename = argv[1];
				long randomSeed = 0;
				bool extraPrint = false;
				decodeCommandPromt(evolutionaryProcess->filenameSettings.environmentFilename, evolutionaryProcess->filenameSettings.resultsFilename, evolutionaryProcess->filenameSettings.bestPopulationFilename, evolutionaryProcess->filenameSettings.bestAgentsFilename, randomSeed, extraPrint, argc, argv);
				evolutionaryProcess->setExtraPrint(extraPrint);
				evolutionaryProcess->start(randomSeed);
				delete evolutionaryProcess;
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
