#include "TPopulation.h"
#include <fstream>
#include "service.h"

using namespace std;

// �������� ��������� ������� �� �����
void TPopulation::loadPopulation(std::string populationFilename, int _populationSize){
	erasePopulation();
	//!!! �������� ���� ���-�� �� ������� ��������� � �������� ���������
	connectionInnovationNumber = 0;
	predConnectionInnovationNumber = 0;
	agents = new TAgent*[_populationSize];
	populationSize = _populationSize;
	ifstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		agents[currentAgent - 1] = new TAgent;
		agents[currentAgent - 1]->loadGenome(populationFile);
	}
	populationFile.close();
}

// �������� ��������� ������� � ����
void TPopulation::uploadPopulation(std::string populationFilename) const{
	ofstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->uploadGenome(populationFile);
	populationFile.close();
}

// ��������� ����������� ���������
void TPopulation::generateMinimalPopulation(int _populationSize, int inputResolution){
	erasePopulation();
	agents = new TAgent*[_populationSize];
	populationSize = _populationSize;
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		agents[currentAgent - 1] = new TAgent;
		agents[currentAgent - 1]->generateMinimalAgent(inputResolution);
	}
}

// ��������� �������� ���������
void TPopulation::evolution(TEnvironment& environment, int evolutionTime /* = 0*/){
	for (int evolutionStep = 1; evolutionStep <= evolutionTime; ++evolutionStep){
		// ��������� ���� �������
		for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
			// �������� ��������� ������������
			agents[currentAgent - 1]->linearSystemogenesis();
			environment.setRandomEnvironmentVector();
			agents[currentAgent - 1]->life(environment, 250); // !!! ���� ������� ����� ������

		}
	}
}

// ---------------  ��������� ��������� ������� -------------------------

// ��������� ������� - �������� �����
void TPopulation::mutationDeletePoolConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.deleteConnectionProb){ //  ���� ����� ����� �������
		int deletingConnectionID = service::uniformDiscreteDistribution(1, kidGenome->getConnectionsQuantity());
		// ���������� ��������� �����
		for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
			for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
				if (kidGenome->getConnectionID(currentPool, currentPoolConnection) == deletingConnectionID) // ���� �� ����� ������ �����
					kidGenome->deleteConnection(currentPool, currentPoolConnection);
		kidGenome->fixConnectionsIDs();
	}
}

// ��������� ������� - �������� ������������ �����
void TPopulation::mutationDeletePoolPredConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.deletePredConnectionProb){ //  ���� ����� ����� �������
		int deletingPredConnectionID = service::uniformDiscreteDistribution(1, kidGenome->getPredConnectionsQuantity());
		// ���������� ��������� ������������ �����
		for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
			for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
				if (kidGenome->getPredConnectionID(currentPool, currentPoolPredConnection) == deletingPredConnectionID) // ���� �� ����� ������ ������������ �����
					kidGenome->deletePredConnection(currentPool, currentPoolPredConnection);
		kidGenome->fixPredConnectionsIDs();
	}
}

// ����������� ���������� ����������� ���������� ���� � ������ ������
double TPopulation::duplicateDivision(int poolsQuantity, int connectionsQuantity){
	return max(1.0, 0.5 * connectionsQuantity/static_cast<double>(mutationSettings.connectionStandartAmount) 
																							+ 0.5 * (poolsQuantity - mutationSettings.poolStandartAmount));
}

// ��������� ������� - ���������� ����
void TPopulation::mutationPoolDuplication(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	// �������� ���-�� ��������� � ����
	int initPoolsQuantity = kidGenome->getPoolsQuantity();
	int initConnectionsQuantity = kidGenome->getConnectionsQuantity();
	int initPredConnectionsQuantity = kidGenome->getPredConnectionsQuantity();
	// ���������� �� ���� ������ �����
	for (int currentPool = 1; currentPool <= initPoolsQuantity; ++currentPool)
		if (kidGenome->getPoolType(currentPool) == 1){ // ���� ��� ����������
			// ��������� ����������� �� ��� (� ������ �������� ��������, ����������� ����������� ���������� � ������ ��������� ������ � ��������)
			bool duplicate = (service::uniformDistribution(0, 1, true, false) < mutationSettings.duplicatePoolProb/duplicateDivision(initPoolsQuantity, initConnectionsQuantity));
			if (duplicate){ // ���� ��� �����������
				if (kidGenome->getPoolCapacity(currentPool) != 1 ) // ���� � ���� �� ������� ������ ���� ������ (����� ��������� ����������� = 1)
					kidGenome->setPoolCapacity(currentPool, static_cast<int>(kidGenome->getPoolCapacity(currentPool) * mutationSettings.poolDivisionCoef + 0.5));
				kidGenome->addPool(kidGenome->getPoolsQuantity() + 1, kidGenome->getPoolType(currentPool), kidGenome->getPoolCapacity(currentPool),
											kidGenome->getPoolBiasMean(currentPool), kidGenome->getPoolBiasVariance(currentPool), kidGenome->getPoolLayer(currentPool));
				// �������� ��� �������� ����� �� �������������� ���� � �����
				for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
					// ���� ��� ������ ����� - �� ��������� �������
					if (kidGenome->getConnectionID(currentPool, currentPoolConnection) <= initConnectionsQuantity){
						int prePoolNumber;
						// ���� ��� �� ����� �� ������ ����
						if (kidGenome->getConnectionPrePool(currentPool, currentPoolConnection)->getID() != kidGenome->getConnectionPostPool(currentPool, currentPoolConnection)->getID())
							prePoolNumber = kidGenome->getConnectionPrePool(currentPool, currentPoolConnection)->getID();
						else 
							prePoolNumber = kidGenome->getPoolsQuantity();
						kidGenome->addConnection(prePoolNumber, kidGenome->getPoolsQuantity(), kidGenome->getConnectionsQuantity() + 1, kidGenome->getConnectionWeightMean(currentPool, currentPoolConnection),
														kidGenome->getConnectionWeightVariance(currentPool, currentPoolConnection), kidGenome->getConnectionEnabled(currentPool, currentPoolConnection),
														kidGenome->getConnectionDisabledStep(currentPool, currentPoolConnection), kidGenome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection),
														++connectionInnovationNumber);
					}
            // �������� ��� ��������� ����� �� �������������� ���� (��� ����� ���� �������� �� ���� ����, ��� ��� ��� ����� ����������)
				for (int currentPostPool = 1; currentPostPool <= initPoolsQuantity; ++currentPostPool)
					if (currentPostPool != currentPool) // ���� ��� �� ������������� ���
						for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPostPool); ++currentPoolConnection)
							if (kidGenome->getConnectionID(currentPostPool, currentPoolConnection) <= initConnectionsQuantity) // ���� ��� ������ �����
								// ���� � �������� �������������� ���� ���� ����� � ������� ������������� �����
								if (kidGenome->getConnectionPrePool(currentPostPool, currentPoolConnection)->getID() == currentPool){
									kidGenome->addConnection(kidGenome->getPoolsQuantity(), currentPostPool, kidGenome->getConnectionsQuantity() + 1, kidGenome->getConnectionWeightMean(currentPostPool, currentPoolConnection) / 2.0,
														kidGenome->getConnectionWeightVariance(currentPostPool, currentPoolConnection), kidGenome->getConnectionEnabled(currentPostPool, currentPoolConnection),
														kidGenome->getConnectionDisabledStep(currentPostPool, currentPoolConnection), kidGenome->getConnectionDevelopSynapseProb(currentPostPool, currentPoolConnection),
														++connectionInnovationNumber);
									// �������� ����� �������������� ������� �� ����� ������� ����� ����� � �������������
									kidGenome->setConnectionWeightMean(currentPostPool, currentPoolConnection, kidGenome->getConnectionWeightMean(currentPostPool, currentPoolConnection) / 2.0); 
								}
				// ������ �������� ��� ������������ �����
				// �������� ��� �������� ������������ ����� �� �������������� ���� � �����
				for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
					// ���� ��� ������ ������������ ����� - �� ��������� �������
					if (kidGenome->getPredConnectionID(currentPool, currentPoolPredConnection) <= initPredConnectionsQuantity){
						int prePoolNumber;
						// ���� ��� �� ������������ ����� �� ������ ����
						if (kidGenome->getPredConnectionPrePool(currentPool, currentPoolPredConnection)->getID() != kidGenome->getPredConnectionPostPool(currentPool, currentPoolPredConnection)->getID())
							prePoolNumber = kidGenome->getPredConnectionPrePool(currentPool, currentPoolPredConnection)->getID();
						else 
							prePoolNumber = kidGenome->getPoolsQuantity();
						kidGenome->addPredConnection(prePoolNumber, kidGenome->getPoolsQuantity(), kidGenome->getPredConnectionsQuantity() + 1, kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection),
														kidGenome->getPredConnectionDisabledStep(currentPool, currentPoolPredConnection), kidGenome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection),
														++predConnectionInnovationNumber);
					}
            // �������� ��� ��������� ������������ ����� �� �������������� ���� (��� ����� ���� �������� �� ���� ����, ��� ��� ��� ����� ����������)
				for (int currentPostPool = 1; currentPostPool <= initPoolsQuantity; ++currentPostPool)
					if (currentPostPool != currentPool) // ���� ��� �� ������������� ���
						for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPostPool); ++currentPoolPredConnection)
							if (kidGenome->getPredConnectionID(currentPostPool, currentPoolPredConnection) <= initPredConnectionsQuantity) // ���� ��� ������ �����
								// ���� � �������� �������������� ���� ���� ������������ ����� � ������� ������������� �����
								if (kidGenome->getPredConnectionPrePool(currentPostPool, currentPoolPredConnection)->getID() == currentPool)
									kidGenome->addPredConnection(kidGenome->getPoolsQuantity(), currentPostPool, kidGenome->getPredConnectionsQuantity() + 1, kidGenome->getPredConnectionEnabled(currentPostPool, currentPoolPredConnection),
														kidGenome->getPredConnectionDisabledStep(currentPostPool, currentPoolPredConnection), kidGenome->getDevelopPredConnectionProb(currentPostPool, currentPoolPredConnection),
														++predConnectionInnovationNumber);
			} // duplicate
		}
	kidGenome->fixConnectionsIDs();
	kidGenome->fixPredConnectionsIDs();
}

// ��������� ������� ����������� �������� ������� �� ����� ����� ������
void TPopulation::mutationDevelopSynapseProb(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutDevelopConProbProb)
				// �����, ����� ����������� ���� ����� ����� � ��������
					kidGenome->setConnectionDevelopSynapseProb(currentPool, currentPoolConnection, 
						min(1.0, max(0.0, kidGenome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection) + service::uniformDistribution(-mutationSettings.mutDevelopConProbDisp, mutationSettings.mutDevelopConProbDisp))));
}

// ��������� ������� ����������� �������� ������������ ����� �� ������������ ����� ����� ������
void TPopulation::mutationDevelopPredConProb(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutDevelopConProbProb)
				// �����, ����� ����������� ���� ����� ����� � ��������
					kidGenome->setDevelopPredConnectionProb(currentPool, currentPoolPredConnection, 
						min(1.0, max(0.0, kidGenome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection) + service::uniformDistribution(-mutationSettings.mutDevelopConProbDisp, mutationSettings.mutDevelopConProbDisp))));
}