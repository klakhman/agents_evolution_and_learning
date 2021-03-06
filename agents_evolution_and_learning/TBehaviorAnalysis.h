//
//  TBehaviorAnalysis.h
//  agents_evolution_and_learning
//
//  Created by Nikita Pestrov on 08.11.12.
//  Copyright (c) 2012 Neurointelligence and Neuromorphe lab. All rights reserved.
//

#ifndef TBehaviorAnalysis_H
#define TBehaviorAnalysis_H

#include <vector>
#include <string>
#include "THypercubeEnvironment.h"
#include "TPopulation.h"
#include "TAgent.h"

/* Класс анализа поведенческих циклов агента */
class TBehaviorAnalysis{
  
	//Раздичные режимы анализа
	typedef enum {
		TBAModePoulation,//Поиск циклов во всей популяции
		TBAModeSingleAgent,//Поиск циклов в одном агенте
    TBAModeEvolution,//Поиск циклов в ходе эволюции
    TBAModeAims
	}TBehaviorAnalysisMode;

  //Режим анализа - всей популции, одного агента, и т.д.
  TBehaviorAnalysisMode mode;
  //Определяет режим запуска и файлы с параметрами и данными
  void decodeCommandPromt(int argc, char** argv);

public:

	TBehaviorAnalysis() {};
	~TBehaviorAnalysis() {};

	// Настройки файлов процесса
	struct SFilenameSettings{
		std::string environmentFilename;
		std::string resultsFilename;
		std::string populationFilename;
		std::string agentsFilename;
		std::string settingsFilename;
		std::string cyclesFilename;
	} filenameSettings;
    
	//Структура цикла
	struct SCycle{
		//Последовательность действий в цикле
		std::vector<double> cycleSequence;
	};

	//Максимально возможная длина цикла(чуть меньше половины длины жизни агента)
	static const int maxCycleLength = 100;
	//Число повторов, необоходимое для верификации нахождения цикла
	static const int sufficientRepeatNumber = 3;
	//Длина жизни агента для прогона
	static const int agentLifeTime = 350;

  //Запустить процесс анализа, какой именно - указано в argv
  void beginAnalysis(int argc, char **argv);

  //Найти все циклы в популяции путем прогона агентов через из всех начальных состояний среды
  std::vector<SCycle> findCyclesInPopulation(TPopulation<TAgent> &population, THypercubeEnvironment &environment);

  //Прогон агента из всех возможных состояний среды для обнаружения всех возможных циклов
  static std::vector<SCycle> findAllCyclesOfAgent(TAgent &agent, THypercubeEnvironment &environment, bool aimCycles = false);

	// Нахождение цикла в массиве существующих циклов (0 - цикла в массиве нет, иначе возвращает номер цикла, начиная с 1)
  static int findCycleInExistingCycles(SCycle &cycleToAdd,std::vector<SCycle> &existingCycles);

  //Находит цикл в жизни данного агента - возвращает пустой цикл, если цикл не найден 
  //(!!! состояние среды, из которого должен запускаться агент должно быть уже становлено, коэффициент стохастичности среды должен быть уже установлен!!!)
  static SCycle findCycleInAgentLife(TAgent &agent, THypercubeEnvironment &environment, bool aimCycle = false);

	// Поиск цикла в последовательности (возвращает пустой вектор, если цикл не найден, но !!! может вернуть цикл из нуля длиной 1 !!!)
  static std::vector<double> findCycleInSequence(double *sequence,int sequenceLength);

  //Поиск подциклов в цикле (проверка на то, состоит ли цикл из более коротких циклов) - возвращает найденный подцикл, иначе возвращает пустой цикл
  static std::vector<double> findBaseCycleInCompoundCycle(double *cycle, int cycleLength);

  //Сравнение двух последовательностей (линейное)
  static bool plainSequencesComparison(double* firstSequence, double* secondSequence, int sequenceLength);
  
  //Поиск максимальных циклов для луших агентов в эволюционном процессе, с записью в два файла
  std::vector<SCycle> findCyclesInEvolution(THypercubeEnvironment &environment);

	// Выгрузка списка циклов в файл
	static void uploadCycles(const std::vector<SCycle>& cyclesList, std::string cyclesFilename);

	// Загрузка циклов из файла
	static std::vector<SCycle> loadCycles(std::string cyclesFilename);

	//Получаем вектор среды, из которого стартовал цикл
	static double * getSequenceInitialStateVector(std::vector<double> &actionsSequence, const THypercubeEnvironment &environment);

  // Преобразование последовательности действий в последовательность состояний (последовательность состояний на одно больше, чем последовательность действий)
  // Если начальное состояние не задано, то оно автоматически восстанавливается с точностью до неиспользуемых битов
  static std::vector<double> transformActionsSequenceToStatesSequence(std::vector<double> &actionsCycle, THypercubeEnvironment &environment, double startStateNumber = -1);
  
  //Переводим последоваетльность действий в последовательность environmentStates (длина последовательности состояний такая же как длина последовательности действий, так как это цикл)
  // Если начальное состояние не задано, то оно автоматически восстанавливается с точностью до неиспользуемых битов
	static SCycle transformActionsCycleToStatesCycle(SCycle &actionsCycle, THypercubeEnvironment &environment, double startStateNumber = -1);
  
  //Подсчитываем длину максимальной памяти в цикле
  static int calculateCycleLongestMemory(SCycle &cycle, THypercubeEnvironment &environment);

  	//Вычисление награды осуществляется путем вычитания награды полученой за три цикла из награды полученной за два цикла
	//Делается это с целью приближения к ситуации, когда поведение сошлось к этому циклу и награды уже были достигнуты
	//Поэтому награды восстанавливаются и агенту не дается полная награда
	static double calculateCycleReward(SCycle &actionsCycle, const THypercubeEnvironment &environment);
  // Отрисовка последовательности действий в файл(с использованием сторонней утилиты dot.exe из пакета GraphViz) 
  // Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
  static void drawActionSequenceToDot(std::vector<double> &sequence, THypercubeEnvironment &environment, std::string outputImageFilename, double initialState = -1);
  // Служебная процедура записи одной последовательности действий в dot-файл
  static void addActionSequenceToDotStream(std::vector<double> &sequence, THypercubeEnvironment &environment, std::ofstream &dotFile, int cycleNumber = 0, double initialState = -1);
  // Отрисовка одного цикла в файл(с использованием сторонней утилиты dot.exe из пакета GraphViz) 
  // Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
  static void drawCycleToDot(SCycle &cycle, THypercubeEnvironment &environment, std::string outputDotFilename);
  // Отрисовка списка циклов в файл(с использованием сторонней утилиты dot.exe из пакета GraphViz) 
  // Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
  static void drawCyclesListToDot(std::vector<SCycle> &cycles,THypercubeEnvironment &environment,std::string outputImageFilename);
  // Служебная процедура записи одного цикла действий в dot-файл
  static void addSingleCycleToDotStream(TBehaviorAnalysis::SCycle &cycle, THypercubeEnvironment &environment, std::ofstream &dotFile, int cycleNumber = 0);

  static void calculateMetricsForEvolutionaryProcess(std::string cyclesExistanceFile,std::string cyclesFile, THypercubeEnvironment &environment);

  // Преобразование последовательности действий (вероятно жизни агента) в последовательность достигнутых целей
  // Возвращает массив последовательности целей (вызывающая сторона должна обеспечить достаточный размер - одинаковый размер с массивом действий является достаточным) и длину последовательности
  static void actionsSequence2aimSequence(double* actionsSequence, int actionsQuantity, const THypercubeEnvironment& environment, double* aimsSequence, int& aimsQuantity);
  
  static void drawSequenceWithAims(std::vector<double> &sequence, THypercubeEnvironment &environment, std::string outputImageFilename, double initialState = -1);

  static void drawAgentLife(TAgent& agent, THypercubeEnvironment& env, unsigned int lifetime, const std::string& lifeFilename);
};

#endif /* defined(__agents_evolution_and_learning__TBehaviorAnalysis__) */
