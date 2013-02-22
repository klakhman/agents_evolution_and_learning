#ifndef TANALYSIS_H
#define TANALYSIS_H

#include <string>
#include "THypercubeEnvironment.h"
#include "TPopulation.h"

class TAgent;

// Класс основного анализа
class TAnalysis{
	
	// ----------- Методы для параллельного стандартного анализа лучших популяций ----------------
	// Расишифровка парметров командной строки
	void decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, std::string& runSign, std::string& analysisRunSign, double& stochasticityCoefficient);
  // Составление сообщения для рабочего процесса
  std::string composeMessageForWorkProcess(int currentEnvironment, int currentTry, std::string runSign, double stochasticityCoefficient);
  // Нахождение записи о параметре в строке с сообщением от процесса
  std::string findParameterNote(std::string inputMessage, std::string parameterString);
	// Расшифровка сообщения от рабочего процесса 
	void decodeFinishedWorkMessage(std::string inputMessage, int& processRankSend, int& finishedEnvironment, int& finishedTry, double& averageReward);
	// Выполнение управляющего процесса
	void rootProcess(int argc, char **argv);
	// Расшифровка сообщения от рутового процесса 
	void decodeTaskMessage(std::string inputMessage, int& currentEnvironment, int& currentTry, std::string& runSign, double& stochasticityCoefficient);
	// Выполнение рабочего процесса
	void workProcess(int argc, char **argv);

public:
	TAnalysis() {};
	~TAnalysis() {};

	// Запуск процедуры анализа путем прогона лучшей популяции (возвращает среднее значений награды по популяции после прогона всех агентов из всех состояний)
  // Если stochasticityCoefficient не равен -1, то он переписывает загруженный из файла настроек.
	double startBestPopulationAnalysis(std::string bestPopulationFilename, std::string environmentFilename, std::string settingsFilename, unsigned int randomSeed = 0, double stochasticityCoefficient = -1);
	// Запуск параллельного процесса анализа по методу лучшей популяции в каждом запуске (!!!в папке с результатами должна быть папка "/Analysis"!!!)
	void startParallelBestPopulationAnalysis(int argc, char **argv);
	// Процедура усреднение параметров анализа по лучшей популяции по глобальным сложностям сред (membersQuantity - кол-во сред в рамках одного коэффициента заполненности)
	static void makeBestPopulationAnalysisSummary(std::string analysisFilename, std::string summaryFilname, int occupancyCoeffcientsQuantity, int membersQuantity, int tryQuantity);
	
	// Процедура прогона случайного агента (на каждом такте времени действие агента определяется случайно)
	static double randomAgentLife(THypercubeEnvironment& environment, int agentLifeTime);

	// Процедура анализа базового уровня награды для среды (путем прогона случайного агента)
	static void randomAgentAnalysis(std::string environmentDirectory, int firstEnvNumber, int lastEnvNumber, std::string settingsFilename, std::string resultsFilename);
	
	// Масштабирование гистограммы (передается последовательность немасшабированной гистограммы - кол-во измерений начиная со значения параметра 1..., scale - значение масштабирования (напр. если передано 10, то масштабируется от 1 до 10, от 11 до 20 и т.д.))
	static void scaleHistogram(int* unscaledHistogram, int parametersQuantity, int scale, std::string resultFilename);

	// Парсинг файлов углубленного анализа поведения (zeroConvergenceMean - среднее кол-во состояний, из которых агенты никуда не сходятся)
	static void advancedBehaviorFilesParsing(int* cyclesConvergenceHist, int statesQuantity, double& zeroConvergenceMean, int* cyclesLengthHist, int maxCyclesLength, std::string analysisFilename);

  // Подсчет среднего количества нейронов (после развертывания пулов) в популяции
  static double calculatePopAverageNeurons(const TPopulation<TAgent>& population);

  // Подсчет среднего кол-ва нейронов для запуска на одной среде
  static void calculateOneEnvironmentAverageNeurons(int argc, char**argv);
};

#endif // TANALYSIS_H