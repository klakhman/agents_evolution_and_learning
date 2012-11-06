#ifndef TENVIRONMENT_H
#define TENVIRONMENT_H 

#include <string>
#include <cstdlib>
#include <cmath>
#include "service.h"

/*
NB!
������ ������ ���� ������� ���� ����� ����� �����������, ����� �������� ����� ����������� �� ���� ��� ���� ������������ ���������� ���������. 
����� ������� � �������� ����� ����� ���������� ����� �����, ���� ��� ������� ��������� ��������� ����������� � ������������� �������� ������.
*/


/*
����� ���������� �����
*/
class TEnvironment{
	//����� ���� � ����� (���������)
	class TAim{	
	public:
		// ������������ ���-�� �������� � ����
		static const int MAX_AIM_COMPLEXITY = 7;
		// ��������� ������ ������������ �������� � ����
		struct SAction{
			int bitNumber; // ����� ���� � ������� ��������� �����
			bool desiredValue; // ����������� ��������
		} actionsSequence[MAX_AIM_COMPLEXITY];

		int aimComplexity; // ��������� ���� - ���-�� ��������
		double reward; // ������ ������� �� ���������� �����

		//����������� �� ���������
		TAim(){
			aimComplexity = 0;
			reward = 0;
		}
	};
	// ������������ ���-�� ����� � �����
	static const int MAX_AIMS_QUANTITY = 3000;
	// ������ ����� � ����� (������� ��� �������)
	TAim aimsSet[MAX_AIMS_QUANTITY];
	// ���-�� ����� � �����
	int aimsQuantity;
	// ����� �������������� ������� �� ���� ����� �� ����������
	int rewardRecoveryTime;
	// ������� ���������������� ����� (���������� � ���� ����������� ��������� ������ ���� �����)
	double nonstaionarityCoefficient;
	// ������� �������� ������ �����
	bool* currentEnvironmentVector;
	// ����������� ������� �����
	int environmentResolution;

	// ��������� ��������� ��������� ����� �������� ������� ����������������
	void randomizeEnvironment();
	// ��������� ���� ������ ����� ��� ��������� ��������� ����� (���������� true - ���� ���� ���� �� ���� ����������� �������)
	bool compareDifferentLengthFullAims(TAim& firstAim, int minFirstSubAimComplexity, TAim& secondAim, int minSecondSubAimComplexity);

public:
	// ����������� �� ���������
	TEnvironment(){
		aimsQuantity = 0;
		environmentResolution = 0;
		rewardRecoveryTime = 0;
		nonstaionarityCoefficient = 0;
	}
	// ����������� ����� � ��������� ����� � ��������� ���������� ���������� �����
	TEnvironment(std::string aimsFilename, int _rewardRecoveryTime = 0, double _nonstaionarityCoefficient = 0){
		aimsQuantity = 0;
		environmentResolution = 0;
		rewardRecoveryTime = _rewardRecoveryTime;
		nonstaionarityCoefficient = _nonstaionarityCoefficient;
		loadAims(aimsFilename);
	}
	// ����������
	~TEnvironment(){
		if (environmentResolution) delete []currentEnvironmentVector;
	}

	// ������� � ������� ���������� �����
	int getAimsQuantity() const { return aimsQuantity; }
	int getEnvironmentResolution() const {return environmentResolution; }
	int getRewardRecoveryTime() const {return rewardRecoveryTime; }
	void setRewardRecoveryTime(int _rewardRecoveryTime) {rewardRecoveryTime = _rewardRecoveryTime; }
	double getNonstationarityCoefficient() const { return nonstaionarityCoefficient; }
	void setNonstationarityCoefficient(double _nonstationarityCoefficient) { nonstaionarityCoefficient = _nonstationarityCoefficient; }
	void getCurrentEnvironmentVector(double environmentVector[]) const {
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			environmentVector[currentBit - 1] = static_cast<double>(currentEnvironmentVector[currentBit - 1]);
	}
	// ��������� ���-�� ��������� ��������� ����� (��� ������� �������� ����� - ���������� ����� ���-�� ���������)
	int getInitialStatesQuantity() const {
		return static_cast<int>(pow(2, environmentResolution));
	}
	// ������� ������� ����� (�� �������� ������ �������)
	void setEnvironmentVector(double environmentVector[]) {
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = (environmentVector[currentBit - 1] == 1);
	}
	// ������� ������� ����� (�� ������ - !!!������� � ����!!!!) 
	void setEnvironmentState(double stateNumber){
		bool* requiredEnvironmentVector = new bool[environmentResolution];
		service::decToBin(static_cast<int>(stateNumber), requiredEnvironmentVector, environmentResolution);
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = requiredEnvironmentVector[currentBit - 1];
		delete []requiredEnvironmentVector;
	}
	// ������� ���������� ��������� �����
	void setRandomEnvironmentState(){
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = (rand() % 2 != 0);
	}
	// �������� ��������� ����� ����� �� �����
	void loadAims(std::string aimsFilename);

	// �������� ��������� ����� � ����
	void uploadAims(std::string aimsFilename) const;

	// ������� ������������ ������������� ���� �����
	double calculateOccupancyCoefficient() const;

	/* 
	��������� ����� ��� ��������� ������ (���������� ��������� �� �������� ��� ��� ���������� ���������)
	� ������ ������ ����������� actionID ���������� ��� +-(bitNumber), ��� ���� ���� ���������� � ����� ������� ���������� ��� (+ ��� � ���� �� �������)
	*/
	bool forceEnvironment(double actionID);

	// ������� ������� ������ - ��� ���� ���������� ��� ���������� ����� ������ - ���������� �������
	double calculateReward(double actionsIDs[], int actionsQuantity) const;

	// ��������� ��������� ����� �� ���������� ������������ �������������, eps - �������� ���������, ����� ���������� ����������� ��������� ���� � ������������, � ����� ����������� ������������ ���������
	double generateEnvironment(int _environmentResolution, double requiredOccupancyCoef, double eps = 0.001, int maxAimComplexity = 5, int minAimComplexity = 2, int minMaxAimComplexity = 3);

	// ��������� � ������ �������� ���������
	friend class tests;
};

#endif // TENVIRONMENT_H
