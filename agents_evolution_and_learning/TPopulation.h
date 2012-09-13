#ifndef TPOPULATION_H
#define TPOPULATION_H

#include "TAgent.h"
#include "TEnvironment.h"
#include <string>

/*
Класс популяции нейросетевых агентов
*/
class TPopulation{
	// Список агентов в популяции
	TAgent** agents;
	// Кол-во агентов в популяции
	int populationSize;
public:
	// Конструктор по умолчанию
	TPopulation(){
		agents = 0;
		populationSize = 0;
	}
	// Конструктор сразу с загрузкой геномов агентов
	TPopulation(std::string populationFilename, int _populationSize){
		loadPopulation(populationFilename, _populationSize);
	}
	// Деструктор
	~TPopulation(){ 
		if (agents) {
			for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
				delete agents[currentAgent - 1];
			delete []agents;
		}
	}
	// Геттеры и сеттеры
	int getPopulationSize() const { return populationSize; }
	TAgent* getPointertoAgent(int agentNumber) const { return agents[agentNumber]; }

	// Загрузка популяции геномов из файла
	void loadPopulation(std::string populationFilename, int _populationSize);

	// Процедура эволюции популяции
	void evolution(TEnvironment& environment, int evolutionTime);
};

#endif // TPOPULATION_H
