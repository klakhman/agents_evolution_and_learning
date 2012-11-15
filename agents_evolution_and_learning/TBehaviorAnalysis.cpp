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
  //Извлекаем данные из файлов и определяем режим анализа
  decodeCommandPromt(argc, argv);
  
  //В зависимости от выбранного режима запускаем процедуры анализа
   switch (mode) {
     case TBAModePoulation: {
       TEnvironment* environment = new TEnvironment(filenameSettings.environmentFilename);
       settings::fillEnvironmentSettingsFromFile(*environment, filenameSettings.settingsFilename);
       //!!! Обнуляем степень стохастичности среды (чтобы все было детерминировано)
       environment->setStochasticityCoefficient(0.0);
       TPopulation* agentsPopulation = new TPopulation;
       settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
       // Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
       settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
       agentsPopulation->loadPopulation(filenameSettings.populationFilename);
       //Запускаем процесс поиска
       findCyclesInPopulation(*agentsPopulation, *environment);
      break;
     }
    case TBAModeSingleAgent:
      ;
    default:
      break;
  }
}
vector<SCycle> TBehaviorAnalysis::findCyclesInPopulation(TPopulation &population, TEnvironment &environment)
{  
  //Вычисляем размер популяции единожды
  int populationSize = population.getPopulationSize();
  //Создаем вектор циклов
  detectedCycles = *new vector<SCycle>;
  //Прогрняем всех агентов в популяции
  for (int agentNumber = 0; agentNumber < populationSize; ++agentNumber) {
    //Запускаем прогон агента с целью поиска циклов
    vector<SCycle> currentAgentCycles = findAllCyclesOfAgent(*population.getPointertoAgent(agentNumber), environment);
    //В случае нахождения циклов, проверяем их на уникальность и добавляем в основной вектор циклов
    if (currentAgentCycles.size()) {
      for (vector<SCycle>::iterator singleCycle = currentAgentCycles.begin(); singleCycle!=currentAgentCycles.end();++singleCycle) {
        if (checkCycleUniqueness(*singleCycle))
          detectedCycles.push_back(*singleCycle);
      }
    }
  }
  
  return detectedCycles;
}
vector<SCycle> TBehaviorAnalysis::findAllCyclesOfAgent(TAgent &agent, TEnvironment &environment)
{
  //Создаем первичный репертуар
  agent.linearSystemogenesis();
}
SCycle TBehaviorAnalysis::findCycleInAgentLife(TAgent &agent, TEnvironment &environment){
}
SCycle TBehaviorAnalysis::findCycleInSequence(double *sequence, int sequenceLength)
{
  vector<double> cycle(maxCycleLength);
  SCycle theCycle;
  //Указатель на конец жизни агента, он, вообще говоря, выходит за рамки массива, не знаю, насколько это безопасно, но зато не надо с единичками возиться
  double *endOfLifePointer = sequence+sequenceLength;
  
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
    if (repeatCount == sufficientRepeatNumber) {
      // theCycle.cycleSequence = cycle;
      return theCycle;
    }
  }
  //Вот тут нужно возвращать пустой какой-то объект - подумаю над этим завтра - просто не знаю, как это лучше оформить)
  return theCycle;

}
bool TBehaviorAnalysis::checkCycleUniqueness(SCycle &cycleToAdd)
{
  return true;
}
void TBehaviorAnalysis::decodeCommandPromt(int argc, char** argv){
  int currentArgNumber = 1; // Текущий номер параметра
  while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
      //Определяем режим
			if (!strcmp("-analysisMode", argv[currentArgNumber])) {
        if (!strcmp("population", argv[++currentArgNumber]))
          mode = TBAModePoulation;
        if (!strcmp("singleagent", argv[++currentArgNumber]))
          mode = TBAModeSingleAgent;
      }
      if (!strcmp("-settings", argv[currentArgNumber]))
        filenameSettings.settingsFilename = argv[++currentArgNumber];
      if (!strcmp("-env", argv[currentArgNumber]))
        filenameSettings.environmentFilename = argv[++currentArgNumber];
      if (!strcmp("-population", argv[currentArgNumber]))
        filenameSettings.populationFilename = argv[++currentArgNumber];
		}
		++currentArgNumber;
	}

	
}
