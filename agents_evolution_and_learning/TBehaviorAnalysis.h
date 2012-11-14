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
static const int maxCycleLength = 120;
//Минимальная длина цикла
static const int minCycleLength = 2;
//Число повторов, необоходимое для верификации нахождения цикла
static const int sufficientRepeatNumber = 4;
//Максимально возможное число циклов впринципе
static const int maxCyclesQuantity = 3500;
/*
 Класс анализа поведенческих циклов агента
*/
class TBehaviorAnalysis{
  //Структура цикла
  struct SCycle{
    //Последовательность действий в цикле
    int cycleSequence[maxCycleLength];
    //Длина цикла
    int cycleLength;
    //Награда, получаемая за прохождение цикла
    double cycleReward;
  };
//Определяет режим запуска и файлы с параметрами и данными
void decodeCommandPromt(int argc, char** argv);
public:
	// Настройки файлов процесса
	struct SFilenameSettings{
		std::string environmentFilename;
		std::string resultsFilename;
		std::string populationFilename;
    std::string agentsFilename;
		std::string settingsFilename;
	} filenameSettings;
public:
	TBehaviorAnalysis() {};
	~TBehaviorAnalysis() {};
  //Запустить процесс анализа, какой именно - указано в argv
  void beginAnalysis(int argc, char **argv);
  std::vector<double> findCyclesInPopulation();
  //Возвращает массив с последовательностью действий самого большого цикла в жизни данного агента
  std::vector<double> findCycleInAgentLife(double *agentLife,int lifeTime);


};

#endif /* defined(__agents_evolution_and_learning__TBehaviorAnalysis__) */
