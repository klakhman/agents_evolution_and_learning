#ifndef TAGENT_H
#define TAGENT_H

#include "TNeuralNetwork.h"
#include "TPoolNetwork.h"
#include "TEnvironment.h"

/*
����� ������������� ������
*/
class TAgent{
	// ���������� ����� ������
	double* agentLife;
	// �������, ��������� �������
	double reward;
	// ������������� �������������� ������������ ������� ��������
	double decodeAction(double outputVector[]);

public:
	TNeuralNetwork* neuralController;
	TPoolNetwork* genome;

	// ����������� �� ���������
	TAgent(){
		agentLife = 0;
		reward = 0;
		neuralController = 0;
		genome = 0;
	}

	// ����������
	~TAgent(){
		if (agentLife) delete []agentLife;
		if (neuralController) delete []neuralController;
		if (genome) delete []genome;
	}

	// ������������� ����� ������
	void life(TEnvironment& environment, int agentLifeTime);
};

#endif TAGENT_H // TAGENT_H
