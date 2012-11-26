#ifndef TANALYSIS_H
#define TANALYSIS_H

#include <string>
#include "TEnvironment.h"
#include "TPopulation.h"

// Класс основного анализа
class TAnalysis{
	
	// ----------- Методы для параллельного стандартного анализа лучших популяций ----------------
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
	// Процедура усреднение параметров анализа по лучшей популяции по глобальным сложностям сред (membersQuantity - кол-во сред в рамках одного коэффициента заполненности)
	void makeBestPopulationAnalysisSummary(std::string analysisFilename, std::string summaryFilname, int occupancyCoeffcientsQuantity, int membersQuantity, int tryQuantity);
	
	// Процедура прогона случайного агента (на каждом такте времени действие агента определяется случайно)
	double randomAgentLife(TEnvironment& environment, int agentLifeTime);

	// Процедура анализа базового уровня награды для среды (путем прогона случайного агента)
	void randomAgentAnalysis(std::string environmentDirectory, int firstEnvNumber, int lastEnvNumber, std::string settingsFilename, std::string resultsFilename);
	
	// Масштабирование гистограммы (передается последовательность немасшабированной гистограммы - кол-во измерений начиная со значения параметра 1..., scale - значение масштабирования (напр. если передано 10, то масштабируется от 1 до 10, от 11 до 20 и т.д.))
	static void scaleHistogram(int* unscaledHistogram, int parametersQuantity, int scale, std::string resultFilename);

	// Парсинг файлов углубленного анализа поведения (zeroConvergenceMean - среднее кол-во состояний, из которых агенты никуда не сходятся)
	static void advancedBehaviorFilesParsing(int* cyclesConvergenceHist, int statesQuantity, double& zeroConvergenceMean, int* cyclesLengthHist, int maxCyclesLength, std::string analysisFilename);

};

#endif // TANALYSIS_H