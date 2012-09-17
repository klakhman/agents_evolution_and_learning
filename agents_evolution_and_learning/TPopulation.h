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
	// ������� ��������� ����� ��������� ������ ���������
	long connectionInnovationNumber;
	// ������� ��������� ����� ��������� ������������ ������ ���������
	long predConnectionInnovationNumber;

	// ������� ���������
	void erasePopulation(){
		if (agents){
			for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
				delete agents[currentAgent - 1];
			delete []agents;
			agents = 0;
			connectionInnovationNumber = 0;
			predConnectionInnovationNumber = 0;
			populationSize = 0;
		}
	}
	// ---------------  ��������� ��������� ������� -------------------------
	// ��������� ������� - �������� �����
	void mutationDeletePoolConnection(TAgent& kidAgent);
	// ��������� ������� - �������� ������������ �����
	void mutationDeletePoolPredConnection(TAgent& kidAgent);
	// ����������� ���������� ����������� ���������� ���� � ������ ������
	double duplicateDivision(int poolsQuantity, int connectionsQuantity);
	// ��������� ������� - ���������� ����
	void mutationPoolDuplication(TAgent& kidAgent);
	// ��������� ������� ����������� �������� ������� �� ����� ����� ������
	void mutationDevelopSynapseProb(TAgent& KidAgent);
	// ��������� ������� ����������� �������� ������������ ����� �� ������������ ����� ����� ������
	void mutationDevelopPredConProb(TAgent& KidAgent);

public:
	// ��������� ��������� ��������
	struct SEvolutionSettings{
		int evolutionTime; // ���-�� ������ ��������
		int agentLifetime; // ���-�� ������ ����� ������
	} evolutionSettings;

	// ��������� "���������� ����������� ��������" (���� ���������� ����������� ���������, ����������� � ��������)
	struct SMutationSettings{
		 double mutWeightProbability; // ����������� ������� ���� �����
		double mutWeightMeanDisp; // ��������� �������� ������� ���� �����
		double mutWeightDispDisp; // ��������� �������� ������� ��������� �����
		int disLimit;  // ������������ ���-�� ������, ����� �������� ����������� ����� ��������� �� ������
		double enConProb; // ����������� ��������� ����������� �����
		double disConProb; // ����������� ���������� ���������� �����
		double addConnectionProb; // ����������� ������� �����
		double addPredConnectionProb; // ����������� ������� ������������ �����
		double deleteConnectionProb; // ����������� �������� �����
		double deletePredConnectionProb; //  ����������� �������� ������������ �����
		double duplicatePoolProb;  // ����������� �������������� ������������� ���� (����������������� ������������ ��������)
		double poolDivisionCoef; // ����������� ���������� ����������� ���� ��� ��� ������� � ��������� ���������� ����
		int poolStandartAmount; // ����������� ������ ���� � ���-�� �����(��� ������������� � ��������� ���������� �������)
		int connectionStandartAmount; // ����������� ���-�� ������ � ����
		double mutDevelopConProbProb; // ����������� ������� ����������� ����������� ����� �����
		double mutDevelopConProbDisp; // ��������� ������� ����������� ����������� ����� �����
	} mutationSettings;

	// ����������� �� ���������
	TPopulation(){
		agents = 0;
		populationSize = 0;
		connectionInnovationNumber = 0;
		predConnectionInnovationNumber = 0;
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
