#ifndef TPOPULATION_H
#define TPOPULATION_H

#include "TAgent.h"
#include "TEnvironment.h"
#include <string>

/*
����� ��������� ������������ �������
*/
class TPopulation{
	// ������ ������� � ���������
	TAgent** agents;
	// ���-�� ������� � ���������
	int populationSize;
	// ������� ���������
	void erasePopulation(){
		if (agents){
			for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
				delete agents[currentAgent - 1];
			delete []agents;
			agents = 0;
		}
	}
public:
	// ��������� ��������� ��������
	struct SEvolutionSettings{
		int evolutionTime; // ���-�� ������ ��������
		int agentLifetime; // ���-�� ������ ����� ������
	} evolutionSettings;
	// ��������� "���������� ����������� ��������" (���� ���������� ����������� ���������, ����������� � ��������)
	struct SMutationSettings{
		 double MutWeightProbability; // ����������� ������� ���� �����
		double MutWeightMeanDisp; // ��������� �������� ������� ���� �����
		double MutWeightDispDisp; // ��������� �������� ������� ��������� �����
		int DisLimit;  // ������������ ���-�� ������, ����� �������� ����������� ����� ��������� �� ������
		double EnConProb; // ����������� ��������� ����������� �����
		double DisConProb; // ����������� ���������� ���������� �����
		double AddConnectionProb; // ����������� ������� �����
		double AddPredConnectionProb; // ����������� ������� ������������ �����
		double DeleteConnectionProb; // ����������� �������� �����
		double DeletePredConnectionProb; //  ����������� �������� ������������ �����
		double DuplicatePoolProb;  // ����������� �������������� ������������� ���� (����������������� ������������ ��������)
		double PoolDivisionCoef; // ����������� ���������� ����������� ���� ��� ��� ������� � ��������� ���������� ����
		int PoolStandartAmount; // ����������� ������ ���� � ���-�� �����(��� ������������� � ��������� ���������� �������)
		int ConnectionStandartAmount;
		double MutDevelopConProbProb; // ����������� ������� ����������� ����������� ����� �����
		double MutDevelopConProbDisp; // ��������� ������� ����������� ����������� ����� �����
	} mutationSettings;

	// ����������� �� ���������
	TPopulation(){
		agents = 0;
		populationSize = 0;
	}
	// ����������� ����� � ��������� ������� �������
	TPopulation(std::string populationFilename, int _populationSize){
		loadPopulation(populationFilename, _populationSize);
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

	// �������� ��������� ������� �� �����
	void loadPopulation(std::string populationFilename, int _populationSize);

	// �������� ��������� ������� � ����
	void uploadPopulation(std::string populationFilename) const;

	// ��������� ����������� ���������
	void generateMinimalPopulation(int _populationSize, int inputResolution);

	// ��������� �������� ��������� (���� ������ ����, �� �������� ���� ���-�� ������, ��������� � ����������)
	void evolution(TEnvironment& environment, int evolutionTime = 0);
};

#endif // TPOPULATION_H
