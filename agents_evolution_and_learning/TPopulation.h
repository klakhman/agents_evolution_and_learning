#ifndef TPOPULATION_H
#define TPOPULATION_H

#include "TAgent.h"
#include "TEnvironment.h"

/*
����� ��������� ������������ �������
*/
class TPopulation{
	// ������ ������� � ���������
	TAgent** agents;
	// ���-�� ������� � ���������
	int populationSize;
public:
	// ����������� �� ���������
	TPopulation(){
		agents = 0;
		populationSize = 0;
	}
	// ����������
	~TPopulation(){ 
		if (agents) {
			for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
				delete agents[currentAgent - 1];
			delete []agents;
		}
	}
	// ������� � �������
	int getPopulationSize() const { return populationSize; }
	TAgent* getPointertoAgent(int agentNumber) const { return agents[agentNumber]; }

	// ��������� �������� ���������
	void evolution(TEnvironment& environment, int evolutionTime);
};

#endif // TPOPULATION_H
