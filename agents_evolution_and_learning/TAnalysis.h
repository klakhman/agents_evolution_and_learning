#ifndef TANALYSIS_H
#define TANALYSIS_H

#include <string>
#include "TEnvironment.h"
#include "TPopulation.h"

// Класс основного анализа
class TAnalysis{

	// Загрузка параметров среды из файла
	void fillEnvironmentSettingsFromFile(TEnvironment& environment, std::string settingsFilename);
	// Загрузка параметров популяции из файла
	void fillPopulationSettingsFromFile(TPopulation& agentsPopulation, std::string settingsFilename);
	// Загрузка параметров агента из файла
	void fillAgentSettingsFromFile(TPopulation& agentsPopulation, std::string settingsFilename);
	
	// ----------- Методы для параллельного стандартного анализа лучших популяций ----------------
	// Заполнение параметров директорий для записи файлов
	void fillDirectoriesSettings(std::string settingsFilename, std::string& workDirectory, std::string& environmentDirectory, std::string& resultsDirectory);
	// Расишифровка парметров командной строки
	void decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, std::string& runSign);
	// Расшифровка сообщения от рабочего процесса 
	void decodeFinishedWorkMessage(char inputMessage[], int& processRankSend, int& finishedEnvironment, int& finishedTry, double& averageReward);
	// Выполнение управляющего процесса (важно, чтобы количество общих заданий не было меньше кол-ва выделенных процессов!!!)
	void rootProcess(int argc, char **argv);
	// Расшифровка сообщения от рутового процесса 
	void decodeTaskMessage(char inputMessage[], int& currentEnvironment, int& currentTry, std::string& runSign);
	// Выполнение рабочего процесса
	void workProcess(int argc, char **argv);

public:
	TAnalysis() {};
	~TAnalysis() {};

	// Запуск процедуры анализа путем прогона лучшей популяции (возвращает среднее значений награды по популяции после прогона всех агентов из всех состояний)
	double startBestPopulationAnalysis(std::string bestPopulationFilename, std::string environmentFilename, std::string settingsFilename, unsigned int randomSeed = 0);
	// Запуск параллельного процесса анализа по методу лучшей популяции в каждом запуске (!!!в папке с результатами должна быть папка "/Analysis"!!!)
	void startParallelBestPopulationAnalysis(int argc, char **argv);
};

#endif // TANALYSIS_H