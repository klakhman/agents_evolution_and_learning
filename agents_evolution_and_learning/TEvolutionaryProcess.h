﻿#ifndef TEVOLUTIONARYPROCESS_H
#define TEVOLUTIONARYPROCESS_H

#include "TPopulation.h"
#include "TAgent.h"
#include "THypercubeEnvironment.h"
#include <string>
#include <iostream>
#include <fstream>

class TEvolutionaryProcess{
protected:  
	// Популяция агентов
	TPopulation<TAgent>* agentsPopulation;
	// Среда для агентов
	TEnvironment* environment;
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
    // Название файла временной популяции (используется для временной записи популяции в режиме с "общей" памятью)
    std::string tmpPopulationFilename;
	} filenameSettings;
	// Конструктор по умолчанию
	TEvolutionaryProcess(){
		agentsPopulation = 0;
		environment = 0;
		bestAverageReward = -1.0; // Чтобы записывалась хоть какая-то популяции в случае, если на всех шагах эволюции награда 0

		extraPrint = false;
	}
	// Деструктор
	virtual ~TEvolutionaryProcess(){
		if (agentsPopulation) delete agentsPopulation;
		if (environment) delete environment;
	}
	// Установка скрытого признака необходимости печатать расширенную информацию
	void setExtraPrint(bool _extraPrint) { extraPrint = _extraPrint; }
	
	// Вывод логовых сообщений (прогресса) на консоль или в файл
	void makeLogNote(std::ostream& outputConsole, std::ostream& bestAgentsConsole, TPopulation<TAgent>* bestPopulation, int currentEvolutionStep /*=0*/);
	
	// Создание и заполнение предварительного файла основных результатов
	virtual void createMainResultsFile(std::ofstream& resultsFile, unsigned int randomSeed);

	// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
	virtual void start(unsigned int randomSeed = 0);
};


#endif // TEVOLUTIONARYPROCESS_H