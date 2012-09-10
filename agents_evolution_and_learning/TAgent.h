#ifndef TAGENT_H
#define TAGENT_H

#include "TNeuralNetwork.h"
#include "TPoolNetwork.h"
#include "TEnvironment.h"

#include <iostream>

/*
����� ������������� ������
*/
class TAgent{
	// ���������� ����� ������
	double* agentLife;
	// �������, ��������� �������
	double reward;
	int parents[2];
	// ������������� �������������� ������������ ������� ��������
	double decodeAction(double outputVector[]);

public:
	TNeuralNetwork* neuralController;
	TPoolNetwork* genome;

	// ����������� �� ���������
	TAgent(){
		parents[0] = 0;
		parents[1] = 0;
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
	// ������� � �������
	double getReward() const { return reward; }
	int getMoreFitParent() const { return parents[0]; }
	int getLessFitParent() const { return parents[1]; }
	void setMoreFitParent(int moreFitParent) { parents[0] = moreFitParent; }
	void lessMoreFitParent(int lessFitParent) { parents[1] = lessFitParent; }

	// �������� ���������������� ������
	void loadController(std::istream& is);
	// �������� ������ ����������������
	void loadGenome(std::istream& is);
	// ��������� ���������� ������������ ���������� ������ ������
	void generateMinimalAgent(int inputResolution, int outputResolution, int initialPoolCapacity, int initialDevelopProbability);

	// ������������� ����� ������
	void life(TEnvironment& environment, int agentLifeTime);
};

#endif TAGENT_H // TAGENT_H
