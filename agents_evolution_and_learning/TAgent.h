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
	// ����� ������ �� �������� ������������� ����������
	TPoolNetwork* genome;
	// ���������� �����
	TNeuralNetwork* neuralController;
	// �������, ��������� �������
	double reward;
	// �������� ������ (������ ����� ���������������, ������ - �����)
	int parents[2];
	// ���������� ����� ������
	double* agentLife;
	// ������������� �������������� ������������ ������� �������� (!!!! �� ���� ������������� ��������, ��� � ����������� ��������� ����� �� ������� ������, ��� � ����������� ����������� ����������� ��������� ������� ������ ����������� ��������� ��������� �� ������ �������, ��� ��� ��� ������������������ �������, �� ��� ���� �� ������������� ����� !!!!!)
	double decodeAction(double outputVector[]);
	// ������� ������� ��������� ������� ���������������� � ������ ������ �� ������ ������� �������� ������� �����
	int calculateOutputResoltion(int inputResolution) const;
	// ��������� ���������� ����������� (����� ���������� ������ ���� ���������� �� ��������)
	TAgent(const TAgent&);
public:
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
		if (neuralController) delete neuralController;
		if (genome) delete genome;
	}
	// ������� � �������
	double getReward() const { return reward; }
	int getMoreFitParent() const { return parents[0]; }
	int getLessFitParent() const { return parents[1]; }
	void setMoreFitParent(int moreFitParent) { parents[0] = moreFitParent; }
	void setLessFitParent(int lessFitParent) { parents[1] = lessFitParent; }
	TPoolNetwork* getPointerToAgentGenome(int agentNumber) const { return genome; }
	TNeuralNetwork* getPointerToAgentController(int agentNumber) const { return neuralController; }

	// �������� ���������������� ������
	void loadController(std::istream& is);
	// �������� ������ ����������������
	void loadGenome(std::istream& is);
	// �������� ���������������� ����� � ���� ��� �� �����
	void uploadController(std::ostream& os) const;
	// �������� ������ ������ � ���� ��� �� �����
	void uploadGenome(std::ostream& os) const;
	// ��������� ���������� ������������ ���������� ������ ������
	void generateMinimalAgent(int inputResolution, int initialPoolCapacity = 1, double initialDevelopProbability = 1);
	// �������� �������� ���������� ������������� (����� ���������� ����������� ���������� ��������) - ������������, ����� ��� �� ���������� �������������, �� ��������
	void linearSystemogenesis();
	// ������������� ����� ������
	void life(TEnvironment& environment, int agentLifeTime);
};

#endif TAGENT_H // TAGENT_H
