//
//  TBehaviorAnalysis.h
//  agents_evolution_and_learning
//
//  Created by Nikita Pestrov on 08.11.12.
//  Copyright (c) 2012 Neurointelligence and Neuromorphe lab. All rights reserved.
//

#ifndef TBehaviorAnalysis_H
#define TBehaviorAnalysis_H

#include <iostream>
/*
 Класс анализа поведенческих циклов агента
*/
class TBehaviorAnalysis{
  //Максимально возможная длина цикла(чуть меньше половины длины жизни агента)
  static const int MaxCycleLength = 120;
  //Минимальная длина цикла
  static const int MinCycleLength = 2;
  //Число повторов, необоходимое для верификации нахождения цикла
  static const int SufficientRepeatNumber = 4;
  //Максимально возможное число циклов впринципе
  static const int MaxCyclesQuantity = 3500;
  
  //Структура цикла
  struct TCycle{
    //Последовательность действий в цикле
    int CycleSequence[MaxCycleLength];
    //Длина цикла
    int CycleLength;
    //Награда, получаемая за прохождение цикла
    double CycleReward;
  };
  
}

#endif /* defined(__agents_evolution_and_learning__TBehaviorAnalysis__) */
