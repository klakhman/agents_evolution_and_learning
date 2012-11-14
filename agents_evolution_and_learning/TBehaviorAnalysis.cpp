//
//  TBehaviorAnalysis.cpp
//  agents_evolution_and_learning
//
//  Created by Nikita Pestrov on 08.11.12.
//  Copyright (c) 2012 Neurointelligence and Neuromorphe lab. All rights reserved.
//

#include "TBehaviorAnalysis.h"
#include <string>
#include <ctime>
#include <fstream>
#include <cstring>
#include <iostream>
#include "settings.h"

using namespace std;

void TBehaviorAnalysis::beginAnalysis(int argc, char **argv)
{
  
}
std::vector<double> TBehaviorAnalysis::findCycleInAgentLife(double *agentLife,int lifeTime)
{
  vector<double> cycle(maxCycleLength);
  //Указатель на конец жизни агента, он, вообще говоря, выходит за рамки массива, не знаю, насколько это безопасно, но зато не надо с единичками возиться
  double *endOfLifePointer = agentLife+lifeTime;
  
  //Начинаем с максимальной длины и идем до цикла длиной в два действия
  for (int cycleLength = maxCycleLength; cycleLength > 2; --cycleLength) {
    //Заполняем вектор потенциальным циклом для дальнейшего сравнения
    cycle.assign(endOfLifePointer-cycleLength, endOfLifePointer);
    //TODO:zero check?
    
    int repeatCount;
    //Временный вектор для сравнения с искомым циклом
    vector<double> nextCycleOccurence(cycleLength);
    //Еще я тут опосаюсь, нет ли опасности вылететь за пределы жизни агента?
    for (repeatCount = 1; repeatCount < sufficientRepeatNumber ; ++repeatCount) {
      //Копируем следущую последовательность действий в этот вектор
      nextCycleOccurence.assign(endOfLifePointer-cycleLength*(repeatCount+1), endOfLifePointer-cycleLength*repeatCount);
      //Сравниаем искомый цикл со следущей последовательностью
      if (nextCycleOccurence != cycle)
        break;
    }
    //Проверяем, встретилась ли последовательность достаточное количество раз
    if (repeatCount == sufficientRepeatNumber)
      return cycle;
  }
  //Вот тут нужно возвращать пустой какой-то объект - подумаю над этим завтра - просто не знаю, как это лучше оформить)
  return cycle;
}
vector<double> TBehaviorAnalysis::findCyclesInPopulation(){
  
	TEnvironment* environment = new TEnvironment(filenameSettings.environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, filenameSettings.settingsFilename);
	//!!! Обнуляем степень стохастичности среды (чтобы все было детерминировано)
	environment->setStochasticityCoefficient(0.0);
	TPopulation* agentsPopulation = new TPopulation;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
	agentsPopulation->loadPopulation(filenameSettings.populationFilename);
  
  agentsPopulation->getPointertoAgent(3)->linearSystemogenesis();
  //Решил третьего проверить, например
  TAgent *agent = agentsPopulation->getPointertoAgent(47);
  agent->life(*environment, 250);
  vector<double> cycle = findCycleInAgentLife(agent->getPointerToAgentLife(),250);
 
  for (int initialState = 0;initialState<cycle.size();initialState++)
    cout<<cycle[initialState]<<" ";
  cout<<endl;
  //Пока ничего путного не возвращаем, создан только базовый метод
  return cycle;
}
void TBehaviorAnalysis::decodeCommandPromt(int argc, char** argv){
	
}

