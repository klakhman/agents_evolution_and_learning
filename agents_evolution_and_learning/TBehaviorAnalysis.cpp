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
  time_t start_time = time(0);
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
				if (!findCycleInExistingCycles(*singleCycle,detectedCycles))
					detectedCycles.push_back(*singleCycle);
			}
		}
    cout<<"Agent:"<<agentNumber<<" cycles:"<<detectedCycles.size()<<" time:"<<time(0)-start_time<<"sec"<<endl;
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
    //Находим цикл
    SCycle newCycle = findCycleInAgentLife(agent, environment);
    //Если цикл найден - провереяем его уникальность
    if (newCycle.cycleSequence.size())
      if (!findCycleInExistingCycles(newCycle, currentAgentCycles))//Проверяем его уникальности среди циклов данного конекретного агента
        currentAgentCycles.push_back(newCycle);
  }
  return currentAgentCycles;
}

SCycle TBehaviorAnalysis::findCycleInAgentLife(TAgent &agent, TEnvironment &environment){
  SCycle agentCycle;
  //Прогоняем жизнь агента
  agent.life(environment, agentLifeTime);
  //Находим цикл
  vector<double> cycle = findCycleInSequence(agent.getPointerToAgentLife(), agentLifeTime/*Продолжительность жизни, надо бы узнавать размер*/);
  //Если агент бездействовал - возвращаем пустой веткор
  if (cycle.size() == 1)
    return agentCycle;
  else //В противном циклв возвращаем найденный 
    agentCycle.cycleSequence = cycle;
  
	return agentCycle;
}

vector<double> TBehaviorAnalysis::findCycleInSequence(double *sequence, int sequenceLength)
{

	vector<double> theCycle;
  //Указатель на конец жизни агента, он, вообще говоря, выходит за рамки массива, не знаю, насколько это безопасно, но зато не надо с единичками возиться  
	double *endOfLifePointer = sequence+sequenceLength;
	//Начинаем с максимальной длины и идем до цикла длиной в два действия  
	for (int cycleLength = maxCycleLength; cycleLength > 0; --cycleLength) {
    
    //Проверяем, может ли уложиться цикл такой длины достаточное количество раз
    if (endOfLifePointer-cycleLength*(sufficientRepeatNumber) < sequence )
      continue;
    
		int repeatCount = 1;
    //Проходим необходимое количество раз
		for (; repeatCount < sufficientRepeatNumber; ++repeatCount) {
      //Сравниваем первую последовательность с последующей
      if (!plainSequencesComparison(endOfLifePointer-cycleLength, endOfLifePointer-cycleLength*(repeatCount+1), cycleLength))
				break;
		}
		//Проверяем, встретилась ли последовательность достаточное количество раз    
		if (repeatCount == sufficientRepeatNumber) {
      //Проверяем, не состоит ли на самом деле цикл из последовательности более коротких циклов
      vector<double> baseCycle = findBaseCycleInCompoundCycle(endOfLifePointer-cycleLength, cycleLength);
      if (baseCycle.size())
        return baseCycle;
      //Создаем вектор нужной длины
      theCycle = vector<double>(cycleLength);
      //Заполняем его последовательностью действий
      theCycle.assign(endOfLifePointer-cycleLength, endOfLifePointer);
			return theCycle;
		}
  }
	return theCycle;
}
vector<double> TBehaviorAnalysis::findBaseCycleInCompoundCycle(double *cycle, int cycleLength)
{
  vector<double> baseCycle;
  //Начинаем самого малеького возможного цикла
  for (int baseCycleLength = 1; baseCycleLength <= cycleLength/2; ++baseCycleLength) {
    //Проверяем, может ли последовательность такой длины состоять из таких подциклов
    if (!(cycleLength%baseCycleLength)) {
      int repeatCount = 1;
      //Проверяем теперь, состоит ли она из таких подциклов
      for (; repeatCount < cycleLength/baseCycleLength; ++repeatCount)
        if (!plainSequencesComparison(cycle, cycle+baseCycleLength*repeatCount, baseCycleLength))
          break;
      //Если наш цикл и правда состоит из подциклов - возвращаем его
      if (repeatCount == cycleLength/baseCycleLength) {
        baseCycle = vector<double>(baseCycleLength);
        baseCycle.assign(cycle, cycle+baseCycleLength);
        return baseCycle;
      }
    }
  }
  return baseCycle;
}
int TBehaviorAnalysis::findCycleInExistingCycles(SCycle &cycleToAdd, vector<SCycle> &existingCycles)
{
  //Проходим по списку существующих циклов
  for (int cycleNumber = 0; cycleNumber < existingCycles.size(); ++cycleNumber) {
    //Сперва сравниваем длины циклов
    if (existingCycles.at(cycleNumber).cycleSequence.size() == cycleToAdd.cycleSequence.size()) {
      //Если они равны, то, последовательно сдвигая начало, сравниваем два кусочка цикла
      for (int shift = 0; shift < cycleToAdd.cycleSequence.size(); ++shift) {
        double *cycleTail = &cycleToAdd.cycleSequence[0];
        double *shiftedCycleHead = &cycleToAdd.cycleSequence[shift];
        //Если сдвинутое начало цикла совпадает с началом цикла в массиве, и если совпадают хвосты - циклы одинаковы
        if (plainSequencesComparison(shiftedCycleHead, &existingCycles.at(cycleNumber).cycleSequence[0], (int)cycleToAdd.cycleSequence.size()-shift) &&
            plainSequencesComparison(cycleTail, &existingCycles.at(cycleNumber).cycleSequence[cycleToAdd.cycleSequence.size()-shift], shift))
          return cycleNumber+1;
      }
    }
  }
  return 0;
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
