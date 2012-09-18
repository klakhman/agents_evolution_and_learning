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
	// ��������� ������� - ������� ������� ������������� �����
	void mutationConnectionsWeight(TAgent& kidAgent);
	// ��������� ������� - ���������/���������� ������
	void mutationEnableDisableConnections(TAgent& kidAgent, int currentEvolutionStep);
	// ��������� ������� - ���������/���������� ������������ ������
	void mutationEnableDisablePredConnections(TAgent& kidAgent, int currentEvolutionStep);
	// ��������� ������� - �������� �� ������ ������, ������� ��������� ����� ���������� ���������� ���������
	void mutationDeleteConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep);
	// ��������� ������� - �������� �� ������ ������������ ������, ������� ��������� ����� ���������� ���������� ���������
	void mutationDeletePredConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep);
	// ��������� ������� - ���������� �����
	void mutationAddPoolConnection(TAgent& kidAgent);
	// ��������� ������� - ���������� ������������ �����
	void mutationAddPoolPredConnection(TAgent& kidAgent);
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
	// ��������� ����������� ������� �� ���� ���������
	void �omposeOffspringFromParents(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent);
	// ��������� ��������� ������ �������
	void generateOffspring(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent, int currentEvolutionStep);
	//��������� ��������� ������ ������, ������������ � ���������� ���������
	int rouletteWheelSelection(double populationFitness[]);
	// ��������� ��������� ������ ��������� � ������ ������� �� �����
	void generateNextPopulation(int currentEvolutionStep);

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
		double enableConnectionProb; // ����������� ��������� ����������� �����
		double disableConnectionProb; // ����������� ���������� ���������� �����
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
	void evolution(TEnvironment& environment);
};

#endif // TPOPULATION_H
