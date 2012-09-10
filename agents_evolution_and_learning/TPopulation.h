#ifndef TPOPULATION_H
#define TPOPULATION_H

#include "TAgent.h"
#include "TEnvironment.h"

/*
Класс популяции нейросетевых агентов
*/
class TPopulation{
	// Кол-во агентов в популяции
	int populationSize;
public:
	
	// Список агентов в популяции
	TAgent* agents;

	// Процедура эволюции популяции
	void evolution(TEnvironment& environment, int evolutionTime);
};

#endif // TPOPULATION_H
