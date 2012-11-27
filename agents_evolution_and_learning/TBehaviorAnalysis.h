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
#include "TEnvironment.h"
#include "TPopulation.h"

/* Класс анализа поведенческих циклов агента */
class TBehaviorAnalysis{
  
	//Раздичные режимы анализа
	typedef enum {
		TBAModePoulation,//Поиск циклов во всей популяции
		TBAModeSingleAgent,//Поиск циклов в одном агенте
    TBAModeEvolution,//Поиск циклов в ходе эволюции
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
		//Награда, получаемая за прохождение цикла
		double cycleReward;
	};

	//Максимально возможная длина цикла(чуть меньше половины длины жизни агента)
	static const int maxCycleLength = 70;
	//Число повторов, необоходимое для верификации нахождения цикла
	static const int sufficientRepeatNumber = 4;
	//Длина жизни агента для прогона
	static const int agentLifeTime = 350;

  //Запустить процесс анализа, какой именно - указано в argv
  void beginAnalysis(int argc, char **argv);

  //Найти все циклы в популяции путем прогона агентов через из всех начальных состояний среды
  std::vector<SCycle> findCyclesInPopulation(TPopulation &population, TEnvironment &environment);

  //Прогон агента из всех возможных состояний среды для обнаружения всех возможных циклов
  static std::vector<SCycle> findAllCyclesOfAgent(TAgent &agent, TEnvironment &environment);

	// Нахождение цикла в массиве существующих циклов (0 - цикла в массиве нет, иначе возвращает номер цикла, начиная с 1)
  static int findCycleInExistingCycles(SCycle &cycleToAdd,std::vector<SCycle> &existingCycles);

  //Находит цикл в жизни данного агента - возвращает пустой цикл, если цикл не найден 
  //(!!! состояние среды, из которого должен запускаться агент должно быть уже становлено, коэффициент стохастичности среды должен быть уже установлен!!!)
  static SCycle findCycleInAgentLife(TAgent &agent, TEnvironment &environment);

	// Поиск цикла в последовательности (возвращает пустой вектор, если цикл не найден, но !!! может вернуть цикл из нуля длиной 1 !!!)
  static std::vector<double> findCycleInSequence(double *sequence,int sequenceLength);

  //Поиск подциклов в цикле (проверка на то, состоит ли цикл из более коротких циклов) - возвращает найденный подцикл, иначе возвращает пустой цикл
  static std::vector<double> findBaseCycleInCompoundCycle(double *cycle, int cycleLength);

  //Сравнение двух последовательностей (линейное)
  static bool plainSequencesComparison(double* firstSequence, double* secondSequence, int sequenceLength);
  
  //Поиск максимальных циклов для луших агентов в эволюционном процессе, с записью в два файла
  std::vector<SCycle> findCyclesInEvolution(TEnvironment &environment);
     
	// Выгрузка одного цикла в файл
	static void uploadSingleCycleToFile(SCycle& cycle, std::ofstream& cyclesFile);

	// Выгрузка списка циклов в файл
	static void TBehaviorAnalysis::uploadCycles(std::vector<SCycle> cyclesList, std::string cyclesFilename);

	// Загрузка циклов из файла
	static std::vector<SCycle> loadCycles(std::string cyclesFilename);
};

#endif /* defined(__agents_evolution_and_learning__TBehaviorAnalysis__) */
