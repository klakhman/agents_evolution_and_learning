#ifndef TPOPULATION_H
#define TPOPULATION_H

#include "TAgent.h"
#include "TEnvironment.h"

/*
����� ��������� ������������ �������
*/
class TPopulation{
	// ���-�� ������� � ���������
	int populationSize;
public:
	
	// ������ ������� � ���������
	TAgent* agents;

	// ��������� �������� ���������
	void evolution(TEnvironment& environment, int evolutionTime);
};

#endif // TPOPULATION_H
