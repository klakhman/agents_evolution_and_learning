#include "TParallelEvolutionaryProcess.h"

#include <string>
#include <fstream>
#include <cstdlib>

using namespace std;

// Заполнение параметров директорий для записи файлов
void TParallelEvolutionaryProcess::fillDirectoriesSettings(){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename);
	while (settingsFile >> optionString){
		if (optionString == "work-directory") { settingsFile >> directoriesSettings.workDirectory; }
		if (optionString == "environment-directory") { settingsFile >> directoriesSettings.environmentDirectory; }
		if (optionString == "results-directory") { settingsFile >> directoriesSettings.workDirectory; }
	}
	settingsFile.close();
}

// Расишифровка парметров командной строки
void TParallelEvolutionaryProcess::decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, string runSign){
	int currentArgNumber = 2; // Текущий номер параметра (в первом записан путь к файлу настроек)
	while (currentArgNumber < argc){
		switch (argv[currentArgNumber][1]){ // Расшифровываем параметр (в первом поле "-")
			case 'e': // Если это диапозон номеров сред
				++currentArgNumber;
				firstEnvironmentNumber = atoi(argv[currentArgNumber]);
				++currentArgNumber;
				lastEnvironmentNumber = atoi(argv[currentArgNumber]);
				break;
			case 't': // Если это диапазон попыток
				++currentArgNumber;
				firstTryNumber = atoi(argv[currentArgNumber]);
				++currentArgNumber;
				lastTryNumber = atoi(argv[currentArgNumber]);
				break;
		}
		++currentArgNumber;
	}
}

// Выполнение управляющего процесса
void TParallelEvolutionaryProcess::rootProcess(int argc, char **argv){
	int firstEnvironmentNumber; // Диапазон номеров сред
	int lastEnvironmentNumber;
	int firstTryNumber; // Диапазон попыток
	int lastTryNumber;
	string runSign; // Некоторый отличительный признак данного конкретного набора параметров или версии алгоритма
	decodeCommandPromt(argc, argv, firstEnvironmentNumber, lastEnvironmentNumber, firstTryNumber, lastTryNumber, runSign);

	// Выдача дочерним процессам всех заданий данных на выполнение программе
	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry){
			// Если не всем процессам выданы изначальные задания (простой подсчет)
		}
}