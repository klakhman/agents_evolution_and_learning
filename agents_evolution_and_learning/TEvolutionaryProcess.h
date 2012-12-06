#ifndef TEVOLUTIONARYPROCESS_H
#define TEVOLUTIONARYPROCESS_H

#include "TPopulation.h"
#include "TAgent.h"
#include "THypercubeEnvironment.h"
#include <string>
#include <iostream>
#include <fstream>

class TEvolutionaryProcess{
	// Популяция агентов
	TPopulation<TAgent>* agentsPopulation;
	// Среда для агентов
	THypercubeEnvironment* environment;
	// Лучшая средняя награда по популяции в текущем запуске
	double bestAverageReward;
	// Признак того, что надо печатать расширенную информацию про пулы, когда мы записываем агентов
	bool extraPrint;
public:
	// Настройки файлов процесса
	struct SFilenameSettings{
		std::string environmentFilename;
		std::string resultsFilename;
		std::string bestPopulationFilename;
		std::string bestAgentsFilename;
		std::string settingsFilename;
	} filenameSettings;
	// Конструктор по умолчанию
	TEvolutionaryProcess(){
		agentsPopulation = 0;
		environment = 0;
		bestAverageReward = 0.0;

		extraPrint = false;
	}
	// Деструктор
	~TEvolutionaryProcess(){
		if (agentsPopulation) delete agentsPopulation;
		if (environment) delete environment;
	}
	// Установка скрытого признака необходимости печатать расширенную информацию
	void setExtraPrint(bool _extraPrint) { extraPrint = _extraPrint; }
	
	// Вывод логовых сообщений (прогресса) на консоль или в файл
	void makeLogNote(std::ostream& outputConsole, std::ostream& bestAgentsConsole, TPopulation<TAgent>* bestPopulation, int currentEvolutionStep /*=0*/);
	
	// Создание и заполнение предварительного файла основных результатов
	void createMainResultsFile(std::ofstream& resultsFile, unsigned int randomSeed);

	// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
	void start(unsigned int randomSeed = 0);
};


#endif // TEVOLUTIONARYPROCESS_H