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
#include "TEnvironment.h"
#include "TPopulation.h"
//Максимально возможная длина цикла(чуть меньше половины длины жизни агента)
static const int maxCycleLength = 70;
//Число повторов, необоходимое для верификации нахождения цикла
static const int sufficientRepeatNumber = 4;
//Длина жизни агента для прогона
static const int agentLifeTime = 350;

//Раздичные режимы анализа
typedef enum {
  TBAModePoulation,//Поиск циклов во всей популяции
  TBAModeSingleAgent,//Поиск циклов в одном агенте
}TBehaviorAnalysisMode;

//Структура цикла
struct SCycle{
  //Последовательность действий в цикле
  std::vector<double> cycleSequence;
  //Награда, получаемая за прохождение цикла
  double cycleReward;
};

/* Класс анализа поведенческих циклов агента */
class TBehaviorAnalysis{
  
  //Режим анализа - всей популции, одного агента, и т.д.
  TBehaviorAnalysisMode mode;
  //Определяет режим запуска и файлы с параметрами и данными
  void decodeCommandPromt(int argc, char** argv);
  // Настройки файлов процесса
public:
	struct SFilenameSettings{
		std::string environmentFilename;
		std::string resultsFilename;
		std::string populationFilename;
    std::string agentsFilename;
		std::string settingsFilename;
	} filenameSettings;
  
  std::vector<SCycle> detectedCycles;
  
  //Запустить процесс анализа, какой именно - указано в argv
  void beginAnalysis(int argc, char **argv);
  //Найти все циклы в популяции путем прогона агентов через из всех начальных состояний среды
  std::vector<SCycle> findCyclesInPopulation(TPopulation &population, TEnvironment &environment);
  //Прогон агента из всех возможных состояний среды для обнаружения всех возможных циклов
  std::vector<SCycle> findAllCyclesOfAgent(TAgent &agent, TEnvironment &environment);
  //Находит цикл в жизни данного агента
  SCycle findCycleInAgentLife(TAgent &agent, TEnvironment &environment);
  TBehaviorAnalysis() {};
	~TBehaviorAnalysis() {};
  //Вспомогательная процедура для поиска цикла в последовательности
  std::vector<double> findCycleInSequence(double *sequence,int sequenceLength);
  //Поиск подциклов в цикле (проверка на то, состоит ли цикл из более коротких циклов)
  std::vector<double> findBaseCycleInCompoundCycle(double *cycle,int cycleLength);
  //Проверить, нет ли уже этого цикла в массиве циклов
  int findCycleInExistingCycles(SCycle &cycleToAdd,std::vector<SCycle> &existingCycles);
  //Сравнение двух последовательностей
  bool plainSequencesComparison(double* firstSequence, double* secondSequence, int sequenceLength);
};

#endif /* defined(__agents_evolution_and_learning__TBehaviorAnalysis__) */
