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
//Максимально возможное число циклов впринципе
static const int maxCyclesQuantity = 3500;
//Длина жизни агента для прогона
static const int agentLifeLength = 350;

//Раздичные режимы анализа
typedef enum {
  TBAModePoulation,//Поиск циклов во всей популяции
  TBAModeSingleAgent,//Поиск циклов в одном агенте
}TBehaviorAnalysisMode;

//Структура цикла
struct SCycle{
  //Последовательность действий в цикле
  double cycleSequence[maxCycleLength];
  //Длина цикла
  int cycleLength;
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
  //Вспомогательная процедура для поиска цикла в последовательности
  SCycle findCycleInSequence(double *sequence,int sequenceLength);
  //Проверить, нет ли уже этого цикла в массиве циклов
  bool checkCycleUniqueness(SCycle &cycleToAdd);
  TBehaviorAnalysis() {};
	~TBehaviorAnalysis() {};
};

#endif /* defined(__agents_evolution_and_learning__TBehaviorAnalysis__) */
