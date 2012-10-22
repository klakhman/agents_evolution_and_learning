#include "TPopulation.h"
#include <fstream>
#include "service.h"

using namespace std;

	// �������� ��������� ������� �� ����� (���� ������ ��������� �� ����������, �� �������� ������� �� ������� ���������� ���������)
void TPopulation::loadPopulation(string populationFilename, int _populationSize /*=0*/){
	//!!! �������� ���� ���-�� �� ������� ��������� � �������� ���������
	connectionInnovationNumber = 0;
	predConnectionInnovationNumber = 0;
	if (_populationSize) setPopulationSize(_populationSize);
	ifstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <=populationSize; ++currentAgent){
		agents[currentAgent - 1]->loadGenome(populationFile);
	}
	populationFile.close();
}

// �������� ��������� ������� � ����
void TPopulation::uploadPopulation(string populationFilename) const{
	ofstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->uploadGenome(populationFile);
	populationFile.close();
}

// ��������� ����������� ���������
void TPopulation::generateMinimalPopulation(int inputResolution, int _populationSize /*= 0*/){
	if (_populationSize) populationSize = _populationSize;
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->generateMinimalAgent(inputResolution);
	if (populationSize){
		connectionInnovationNumber = agents[0]->getPointerToAgentGenome()->getConnectionsQuantity();
		predConnectionInnovationNumber = agents[0]->getPointerToAgentGenome()->getPredConnectionsQuantity();
	}
}
// ��� ������������� ��������
void TPopulation::evolutionaryStep(TEnvironment& environment, int evolutionStepNumber){
	// ������� �������� ��������� (����� ����� ����� ���� ������ ���������� � ������ ������� ��������� ��� ���� �������� �� �����)
	if (evolutionStepNumber != 1) generateNextPopulation(evolutionStepNumber);
	// ��������� ���� �������
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		// �������� ��������� ������������
		agents[currentAgent - 1]->linearSystemogenesis();
		environment.setRandomEnvironmentVector();
		agents[currentAgent - 1]->life(environment, evolutionSettings.agentLifetime);
	}
}

// ��������� �������� ���������
void TPopulation::evolution(TEnvironment& environment){
	for (int evolutionStep = 1; evolutionStep <= evolutionSettings.evolutionTime; ++evolutionStep){
		evolutionaryStep(environment, evolutionStep);
	}
}


// ---------------  ��������� ��������� ������� -------------------------
// ��������� ������� - ������� ������� ������������� �����
void  TPopulation::mutationConnectionsWeight(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool){
		// ������� �������� ��������
		if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutWeightProbability){
			kidGenome->setPoolBiasMean(currentPool, kidGenome->getPoolBiasMean(currentPool) +
																service::uniformDistribution(-mutationSettings.mutWeightMeanDisp, mutationSettings.mutWeightMeanDisp));
			// �����, ����� ��������� ���� �� ������ ����
			kidGenome->setPoolBiasVariance(currentPool, abs(kidGenome->getPoolBiasVariance(currentPool) + 
															service::uniformDistribution(-mutationSettings.mutWeightDispDisp, mutationSettings.mutWeightDispDisp)));
		}
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			 // ���� ������� ����� ����� ���� � ����� ��������
			if ((service::uniformDistribution(0, 1, true, false) < mutationSettings.mutWeightProbability) && (kidGenome->getConnectionEnabled(currentPool, currentPoolConnection))) {
				kidGenome->setConnectionWeightMean(currentPool, currentPoolConnection, kidGenome->getConnectionWeightMean(currentPool, currentPoolConnection) +
																service::uniformDistribution(-mutationSettings.mutWeightMeanDisp, mutationSettings.mutWeightMeanDisp));
				// �����, ����� ��������� ���� �� ������ ����
				kidGenome->setConnectionWeightVariance(currentPool, currentPoolConnection, abs(kidGenome->getConnectionWeightVariance(currentPool, currentPoolConnection) + 
																service::uniformDistribution(-mutationSettings.mutWeightDispDisp, mutationSettings.mutWeightDispDisp)));
			}
	}
}

// ��������� ������� - ���������/���������� ������
void TPopulation::mutationEnableDisableConnections(TAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			if (kidGenome->getConnectionEnabled(currentPool, currentPoolConnection)){
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.disableConnectionProb){
					kidGenome->setConnectionEnabled(currentPool, currentPoolConnection, false);
					kidGenome->setConnectionDisabledStep(currentPool, currentPoolConnection, currentEvolutionStep);
				}
			} else
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.enableConnectionProb){
					kidGenome->setConnectionEnabled(currentPool, currentPoolConnection, true);
					kidGenome->setConnectionDisabledStep(currentPool, currentPoolConnection, 0);
				}
}

// ��������� ������� - ���������/���������� ������������ ������
void TPopulation::mutationEnableDisablePredConnections(TAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			if (kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection)){
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.disableConnectionProb){
					kidGenome->setPredConnectionEnabled(currentPool, currentPoolPredConnection, false);
					kidGenome->setPredConnectionDisabledStep(currentPool, currentPoolPredConnection, currentEvolutionStep);
				}
			} else
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.enableConnectionProb){
					kidGenome->setPredConnectionEnabled(currentPool, currentPoolPredConnection, true);
					kidGenome->setPredConnectionDisabledStep(currentPool, currentPoolPredConnection, 0);
				}
}

// ��������� ������� - �������� �� ������ ������, ������� ��������� ����� ���������� ���������� ���������
void TPopulation::mutationDeleteConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			// ���� ��� ����� ���� ��������� � �� ��� ���� ������� �� ������
			if (( !kidGenome->getConnectionEnabled(currentPool, currentPoolConnection)) &&
								(currentEvolutionStep - kidGenome->getConnectionDisabledStep(currentPool, currentPoolConnection) >= mutationSettings.disLimit)){
				kidGenome->deleteConnection(currentPool, currentPoolConnection);
				--currentPoolConnection; // ����� ��������� ������� �� ��� �� ������ �����
			}
	kidGenome->fixConnectionsIDs();
}

// ��������� ������� - �������� �� ������ ������������ ������, ������� ��������� ����� ���������� ���������� ���������
void TPopulation::mutationDeletePredConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			// ���� ��� ������������ ����� ���� ��������� � �� ��� ���� ������� �� ������
			if (( !kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection)) &&
								(currentEvolutionStep - kidGenome->getPredConnectionDisabledStep(currentPool, currentPoolPredConnection) >= mutationSettings.disLimit)){
				kidGenome->deletePredConnection(currentPool, currentPoolPredConnection);
				--currentPoolPredConnection; // ����� ��������� ������� �� ��� �� ������ ������������ �����
			}
	kidGenome->fixPredConnectionsIDs();
}

// ��������� ������� - ���������� �����
void TPopulation::mutationAddPoolConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.addConnectionProb){ // ���� ����� ����� �������
		int prePoolID, postPoolID; // ���������������� � ����������������� ��� ������������� �����
		int tryCount = 0; //���������� ������ ��������� ����� - �������, ����� �� ���� ������������
		do{ // ���� ���������� ������������� �����
			do{ // ���� ���������� ���������� �����
				prePoolID = service::uniformDiscreteDistribution(1, kidGenome->getPoolsQuantity());
				// ����������������� ��� ����� �� ����� ���� �������
				postPoolID = service::uniformDiscreteDistribution(kidGenome->getInputResolution() + 1, kidGenome->getPoolsQuantity());
				//���� ����������������� � ���������������� ���� �� �������� ������������ ���������, �� ������ �� ����� ���������� �����
			} while (! ((postPoolID > kidGenome->getInputResolution() + kidGenome->getOutputResolution()) || 
																(prePoolID <= kidGenome->getInputResolution()) || (prePoolID > kidGenome->getInputResolution() + kidGenome->getOutputResolution())));
			++tryCount;
		// ���� �� ������ ������������� ����� ��� �������� ���������� ���-�� �������
		}while ((kidGenome->checkConnectionExistance(prePoolID, postPoolID)) && (tryCount <= 1000));
		// ���� ������������� ����� �������
		if (! kidGenome->checkConnectionExistance(prePoolID, postPoolID)){
			kidGenome->addConnection(prePoolID, postPoolID, kidGenome->getConnectionsQuantity() + 1, service::uniformDistribution(-0.5, 0.5), 0, true, 0, 
												kidAgent.primarySystemogenesisSettings.initialDevelopSynapseProbability, ++connectionInnovationNumber);
			// �������� ������������� ������ ������������������ ���� � ��������� ����, ���� ��������� ����� ����� ������ ������ ����
			if ((kidGenome->getPoolLayer(prePoolID) == kidGenome->getPoolLayer(postPoolID)) && (prePoolID != postPoolID))
				for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
					// �������� ���������������� ��� � ��� ���� � ����� ������� �����
					if ((kidGenome->getPoolLayer(currentPool) > kidGenome->getPoolLayer(prePoolID)) || (kidGenome->getPoolID(currentPool) == postPoolID))
						kidGenome->setPoolLayer(currentPool, kidGenome->getPoolLayer(currentPool) + 1); 
			kidGenome->fixConnectionsIDs();
		}
	}
}

// ��������� ������� - ���������� ������������ �����
void TPopulation::mutationAddPoolPredConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.addPredConnectionProb){ // ���� ����� ����� �������
		int prePoolID, postPoolID; // ���������������� � ����������������� ��� ������������� ������������ �����
		int tryCount = 0; //���������� ������ ��������� ������������ ����� - �������, ����� �� ���� ������������
		do{ // ���� ���������� ������������� ������������ �����
			prePoolID = service::uniformDiscreteDistribution(1, kidGenome->getPoolsQuantity());
			// ����������������� ��� ����� �� ����� ���� ������� ��� ��������
			postPoolID = service::uniformDiscreteDistribution(kidGenome->getInputResolution() + kidGenome->getOutputResolution() + 1, kidGenome->getPoolsQuantity());
			++tryCount;
		// ���� �� ������ ������������� ������������ ����� ��� �������� ���������� ���-�� �������
		}while ((kidGenome->checkPredConnectionExistance(prePoolID, postPoolID)) && (tryCount <= 1000));
		// ���� ������������� ������������ ����� �������
		if (! kidGenome->checkPredConnectionExistance(prePoolID, postPoolID)){
			kidGenome->addPredConnection(prePoolID, postPoolID, kidGenome->getPredConnectionsQuantity() + 1, true, 0, 
													kidAgent.primarySystemogenesisSettings.initialDevelopPredConnectionProbability, ++predConnectionInnovationNumber); 
			kidGenome->fixPredConnectionsIDs();
		}
	}
}

// ��������� ������� - �������� �����
void TPopulation::mutationDeletePoolConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if ((service::uniformDistribution(0, 1, true, false) < mutationSettings.deleteConnectionProb) && (kidGenome->getConnectionsQuantity())){ //  ���� ����� ����� ������� � ���� ��� �������
		int deletingConnectionID = service::uniformDiscreteDistribution(1, kidGenome->getConnectionsQuantity());
		// ���������� ��������� �����
		for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
			for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
				if (kidGenome->getConnectionID(currentPool, currentPoolConnection) == deletingConnectionID){ // ���� �� ����� ������ �����
					kidGenome->deleteConnection(currentPool, currentPoolConnection);
					break;
				}
		kidGenome->fixConnectionsIDs();
	}
}

// ��������� ������� - �������� ������������ �����
void TPopulation::mutationDeletePoolPredConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if ((service::uniformDistribution(0, 1, true, false) < mutationSettings.deletePredConnectionProb) && (kidGenome->getPredConnectionsQuantity())){ //  ���� ����� ����� ������� � ���� ��� �������
		int deletingPredConnectionID = service::uniformDiscreteDistribution(1, kidGenome->getPredConnectionsQuantity());
		// ���������� ��������� ������������ �����
		for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
			for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
				if (kidGenome->getPredConnectionID(currentPool, currentPoolPredConnection) == deletingPredConnectionID){ // ���� �� ����� ������ ������������ �����
					kidGenome->deletePredConnection(currentPool, currentPoolPredConnection);
					break;
				}
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

// ��������� ����������� ������� �� ���� ���������
void TPopulation::�omposeOffspringFromParents(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent){
	if (firstParentAgent.getReward() >= secondParentAgent.getReward())
		kidAgent = firstParentAgent;
	else 
		kidAgent = secondParentAgent;
}

// ��������� ��������� ������ �������
void TPopulation::generateOffspring(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent, int currentEvolutionStep){
	
	�omposeOffspringFromParents(kidAgent, firstParentAgent, secondParentAgent);
	
	//mutationDeleteConnectionPopulation(kidAgent, currentEvolutionStep);
	//mutationDeletePredConnectionPopulation(kidAgent, currentEvolutionStep);
	//mutationEnableDisableConnections(kidAgent, currentEvolutionStep);
	//mutationEnableDisablePredConnections(kidAgent, currentEvolutionStep);

	mutationPoolDuplication(kidAgent);
	mutationAddPoolConnection(kidAgent);
	mutationAddPoolPredConnection(kidAgent);
	mutationDeletePoolConnection(kidAgent);
	mutationDeletePoolPredConnection(kidAgent);
	mutationConnectionsWeight(kidAgent);
	mutationDevelopSynapseProb(kidAgent);
	mutationDevelopPredConProb(kidAgent);
}

//��������� ��������� ������ ������, ������������ � ���������� ���������
int TPopulation::rouletteWheelSelection(double populationFitness[]){
   double totalFitness = 0; // ������ ������� ���������
	for (int currentAgent = 1; currentAgent <= populationSize; currentAgent++) 
		totalFitness += populationFitness[currentAgent - 1];

   double currentRandomFitness = service::uniformDistribution(0, totalFitness, false, false);
   double currentSum = 0.0;
   int currentAgent = 0;
   while (currentRandomFitness > currentSum)
      currentSum += populationFitness[currentAgent++];

   return currentAgent;
}

// ��������� ��������� ������ ��������� � ������ ������� �� �����
void TPopulation::generateNextPopulation(int currentEvolutionStep){
	double* populationReward = new double[populationSize]; // ������ �� ����� ��������� ���������
   double rewardSum = 0.0; // ����� ���� ������ �������
	for (int currentAgent = 1; currentAgent <= populationSize; currentAgent++) // ��������� ������ � ��������� ���������
		rewardSum += (populationReward[currentAgent - 1] = agents[currentAgent - 1]->getReward());
	TAgent** newAgents = new TAgent*[populationSize];

	for (int currentAgent = 1; currentAgent <= populationSize; currentAgent++){ // ���������� ����� ���������
      int firstParentAgent;
      int secondParentAgent;
      // ���������� ����� ������������� ������
      if (rewardSum){ // ���� ���� ���� ���� �����, ������� ������ ���� �������-�� �������
         firstParentAgent = rouletteWheelSelection(populationReward);
         int checkCount = 0; // �������� ����� ������� ������
         do{
            secondParentAgent =  rouletteWheelSelection(populationReward);
         } while ((firstParentAgent == secondParentAgent) && (++checkCount < 10));

         if (firstParentAgent == secondParentAgent) //���� �� ������� ����� ������� ������ (������ ��� � ���� ��������� ����� ��������� �������)
				secondParentAgent = service::uniformDiscreteDistribution(1, populationSize);
      } else{ // ���� �� ���� �� ������� �� ������ ������� �������
			firstParentAgent = service::uniformDiscreteDistribution(1, populationSize);
         do{
				secondParentAgent =  service::uniformDiscreteDistribution(1, populationSize);
         } while (firstParentAgent == secondParentAgent);
      }
      // ����� ����� ��������������� �������� ������ ����� �� ������ �����
		if (agents[firstParentAgent - 1]->getReward() > agents[secondParentAgent - 1]->getReward()){
			int tmp = firstParentAgent;
			firstParentAgent = secondParentAgent;
			secondParentAgent = tmp;
		}
		newAgents[currentAgent - 1] = new TAgent;
		// ������� ������ ������
		generateOffspring(*newAgents[currentAgent - 1], *agents[firstParentAgent - 1], *agents[secondParentAgent - 1], currentEvolutionStep);
		newAgents[currentAgent - 1]->setMoreFitParent(firstParentAgent);
		newAgents[currentAgent - 1]->setLessFitParent(secondParentAgent);
   }

   delete []populationReward;
   // ������ ��������� ����� ��������� � ������ (�� �� ���������, � ������ ������, ��� ���� ������ �������)
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		delete agents[currentAgent - 1];
		agents[currentAgent - 1] = newAgents[currentAgent - 1];  // �������� ������ ������
   }

   delete []newAgents; // ������� ������ ������ ������, �� �� ����� �������
}

