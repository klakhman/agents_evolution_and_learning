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
  
	switch (mode) {
     case TBAModePoulation: {
       TEnvironment* environment = new TEnvironment(filenameSettings.environmentFilename);
       settings::fillEnvironmentSettingsFromFile(*environment, filenameSettings.settingsFilename);
       //!!! Обнуляем степень стохастичности среды (чтобы все было детерминировано)
       environment->setStochasticityCoefficient(0.0);
       TPopulation* agentsPopulation = new TPopulation;
       settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
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
vector<SCycle> TBehaviorAnalysis::findCyclesInPopulation(TPopulation &population, TEnvironment &environment) {  
	//Вычисляем размер популяции единожды
	int populationSize = population.getPopulationSize();
	//Создаем вектор циклов
	detectedCycles = *new vector<SCycle>;
	//Прогрняем всех агентов в популяции
	for (int agentNumber = 1; agentNumber <= populationSize; ++agentNumber) {
		//Запускаем прогон агента с целью поиска циклов
		vector<SCycle> currentAgentCycles = findAllCyclesOfAgent(*population.getPointertoAgent(agentNumber), environment);
		//В случае нахождения циклов, проверяем их на уникальность и добавляем в основной вектор циклов
		if (currentAgentCycles.size()) {
			for (vector<SCycle>::iterator singleCycle = currentAgentCycles.begin(); singleCycle!=currentAgentCycles.end();++singleCycle) {
				if (checkCycleUniqueness(*singleCycle,detectedCycles))
					detectedCycles.push_back(*singleCycle);
			}
		}
    cout<<"Agent:"<<agentNumber<<" cycles:"<<detectedCycles.size()<<endl;
  }
  return detectedCycles;
}
vector<SCycle> TBehaviorAnalysis::findAllCyclesOfAgent(TAgent &agent, TEnvironment &environment)
{
  vector<SCycle> currentAgentCycles = *new vector<SCycle>;
	//Создаем первичный репертуар  
	agent.linearSystemogenesis();
  int intitalStatesQuantity = environment.getInitialStatesQuantity();
  //Идем по всем состояниям среды
  for (int currentInitialState = 0; currentInitialState < intitalStatesQuantity; ++currentInitialState){
    environment.setEnvironmentState(currentInitialState);
    //Прогоняем жизнь агента
    agent.life(environment,agentLifeTime);
    //Находим цикл
    SCycle newCycle = findCycleInAgentLife(agent, environment);
    if (newCycle.cycleLength)
      if (checkCycleUniqueness(newCycle, currentAgentCycles))//Проверяем его уникальности среди циклов данного конекретного агента
        currentAgentCycles.push_back(newCycle);
  }
  return currentAgentCycles;
}

SCycle TBehaviorAnalysis::findCycleInAgentLife(TAgent &agent, TEnvironment &environment){
  
	SCycle agentCycle = findCycleInSequence(agent.getPointerToAgentLife(), 250/*Продолжительность жизни, надо бы узнавать размер*/);
	return agentCycle;
}
SCycle TBehaviorAnalysis::findCycleInSequence(double *sequence, int sequenceLength)
{
//  for (int step = 0;step<sequenceLength;++step)
//    cout<<sequence[step];
//  cout<<endl;
	SCycle theCycle;
  theCycle.cycleLength = 0;
  //Указатель на конец жизни агента, он, вообще говоря, выходит за рамки массива, не знаю, насколько это безопасно, но зато не надо с единичками возиться  
	double *endOfLifePointer = sequence+sequenceLength;
	//Начинаем с максимальной длины и идем до цикла длиной в два действия  
	for (int cycleLength = maxCycleLength; cycleLength > 2; --cycleLength) {
    //Проверяем, не бездействовал ли агент8
    bool zeroChek = 0;
    for (int pos = 1; pos <= cycleLength; ++pos)
      if (sequence[sequenceLength-pos] != 0)
        zeroChek = true;
    if (!zeroChek)
      return theCycle;
    
		int repeatCount = 1;
    //Проходим необходимое количество раз
		for (; repeatCount < sufficientRepeatNumber && endOfLifePointer-cycleLength*repeatCount >= sequence ; ++repeatCount) {
      //Сравниваем первую последовательность с последующей
      if (!plainSequencesComparison(endOfLifePointer-cycleLength, endOfLifePointer-cycleLength*(repeatCount+1), cycleLength))
				break;
		}
		//Проверяем, встретилась ли последовательность достаточное количество раз    
		if (repeatCount == sufficientRepeatNumber) {
      //Проверяем, не состоит ли на самом деле цикл из последовательности более коротких циклов
      SCycle baseCycle = findBaseCycleInCompoundCycle(endOfLifePointer-cycleLength, cycleLength);
      if (baseCycle.cycleLength)
        return baseCycle;
      //Создаем вектор нужной длины
      theCycle.cycleSequence = vector<double>(cycleLength);
      //Заполняем его последовательностью действий
      theCycle.cycleSequence.assign(endOfLifePointer-cycleLength, endOfLifePointer);
      theCycle.cycleLength = cycleLength;
			return theCycle;
		}
  }
	return theCycle;
}
SCycle TBehaviorAnalysis::findBaseCycleInCompoundCycle(double *cycle, int cycleLength)
{
  SCycle baseCycle;
  baseCycle.cycleLength = 0;
  //Начинаем самого малеького возможного цикла
  for (int baseCycleLength = 2; baseCycleLength <= cycleLength/2; ++baseCycleLength) {
    //Проверяем, может ли последовательность такой длины состоять из таких подциклов
    if (!(cycleLength%baseCycleLength)) {
      int repeatCount = 1;
      //Проверяем теперь, состоит ли она из таких подциклов
      for (; repeatCount < cycleLength/baseCycleLength; ++repeatCount)
        if (!plainSequencesComparison(cycle, cycle+baseCycleLength*repeatCount, baseCycleLength))
          break;
      //Если наш цикл и правда состоит из подциклов - возвращаем его
      if (repeatCount == cycleLength/baseCycleLength) {
        baseCycle.cycleSequence = vector<double>(baseCycleLength);
        baseCycle.cycleSequence.assign(cycle, cycle+baseCycleLength);
        baseCycle.cycleLength = baseCycleLength;
        return baseCycle;
      }
      
    } else continue;
  }
  return baseCycle;
}
bool TBehaviorAnalysis::checkCycleUniqueness(SCycle &cycleToAdd, vector<SCycle> &existingCycles)
{
  int cycleToAddLength = cycleToAdd.cycleLength;
  //Проходим по списку существующих циклов
  for (vector<SCycle>::iterator singleCycle = existingCycles.begin(); singleCycle!=existingCycles.end();++singleCycle) {
    //Сперва сравниваем длины циклов
    if (singleCycle->cycleLength == cycleToAddLength) {
      //Если они равны, то, последовательно сдвигая начало, сравниваем два кусочка цикла
      for (int shift = 0; shift < cycleToAddLength; ++shift) {
        double *cycleTail = &cycleToAdd.cycleSequence[0];
        double *shiftedCycleHead = &cycleToAdd.cycleSequence[shift];
        //Если сдвинутое начало цикла совпадает с началом цикла в массиве, и если совпадают хвосты - циклы одинаковы
        if (plainSequencesComparison(shiftedCycleHead, &singleCycle->cycleSequence[0], cycleToAddLength-shift) &&
            plainSequencesComparison(cycleTail, &singleCycle->cycleSequence[cycleToAddLength-shift], shift))
          return false;
      }
    } else continue;
  }
  return true;
}
bool TBehaviorAnalysis::plainSequencesComparison(double* firstSequence, double* secondSequence, int sequenceLength)
{
  int currentPosition = 0;
  while ((currentPosition < sequenceLength) && (firstSequence[currentPosition] == secondSequence[currentPosition]))
    ++currentPosition;
  
  if (currentPosition == sequenceLength) //Если мы дошли до конца последовательности
    return true;
  else
    return false;
}
void TBehaviorAnalysis::decodeCommandPromt(int argc, char** argv){
  int currentArgNumber = 1; // // Текущий номер параметра
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
