#ifndef TPOOLNETWORK_H
#define TPOOLNETWORK_H

#include "TNeuralPool.h"

#include <iostream>
#include <fstream>

/*
����� c��� �� ������������ �����
*/
class TPoolNetwork{
	TNeuralPool** poolsStructure; // ������ �� ����� 
	int poolsStructureSize; // ������� ������ ������� �����
	int poolsQuantity; // ���������� ����� � ����

	int connectionsQuantity; // ���������� ������ � ����
	int predConnectionsQuantity; // ���������� ������������ ������ � ����
	
	int layersQuantity; // ���������� ����� � ���� (!!!! �� ����������� ��� �������� ����� !!!)
	int inputResolution; // ������ �������� ��-�� ����
	int outputResolution; // ������ ��������� ��-�� ����

	static const int INFLATE_POOLS_SIZE = 10; // ������ ���������� ������� � ������ � ������ ������������

	// ��������� ���������� ������� ������� �����
	void inflatePoolsStructure(int inflateSize);

public:
	// ����������� �� ���������
	TPoolNetwork(){
		poolsStructure = 0;
		poolsStructureSize = 0;
		poolsQuantity = 0;
		connectionsQuantity = 0;
		predConnectionsQuantity = 0;
		layersQuantity = 0;
		inputResolution = 0;
		outputResolution = 0;
	}
	// ����������
	~TPoolNetwork();

	//������� ��� ���������� ����
	int getPoolsQuantity() { return poolsQuantity; }
	int getConnectionsQuantity() { return connectionsQuantity; }
	int getPredConnectionsQuantity() { return predConnectionsQuantity; }
	int getInputResolution() { return inputResolution; }
	int getOutputResolution() { return outputResolution; }

	// ������� � ������� ��� ���� ����� ���� (�� ���� ������� ���������� ����� ���� � ������� ����)
	int getPoolID(int poolNumber){ return poolsStructure[poolNumber-1]->getID(); }
	void setPoolID(int poolNumber, int newID){ poolsStructure[poolNumber-1]->setID(newID); }
	int getPoolType(int poolNumber) { return poolsStructure[poolNumber-1]->getType(); }
	void setPoolType(int poolNumber, int newType) { poolsStructure[poolNumber-1]->setType(newType); }
	int getPoolCapacity(int poolNumber) { return poolsStructure[poolNumber-1]->getCapacity(); }
	void setPoolCapacity(int poolNumber, int newCapacity) { poolsStructure[poolNumber-1]->setCapacity(newCapacity); }
	double getPoolBiasMean(int poolNumber) { return poolsStructure[poolNumber-1]->getBiasMean(); }
	void setPoolBiasMean(int poolNumber, double newBiasMean) { poolsStructure[poolNumber-1]->setBiasMean(newBiasMean); }
	double getPoolBiasVariance(int poolNumber) { return poolsStructure[poolNumber-1]->getBiasVariance(); }
	void setPoolBiasVariance(int poolNumber, double newBiasVariance) { poolsStructure[poolNumber-1]->setBiasVariance(newBiasVariance); }
	int getPoolLayer(int poolNumber) { return poolsStructure[poolNumber-1]->getLayer(); }
	void setPoolLayer(int poolNumber, int newLayer) { poolsStructure[poolNumber-1]->setLayer(newLayer); }

	int getPoolInputConnectionsQuantity(int poolNumber){ return poolsStructure[poolNumber-1]->getInputConnectionsQuantity(); }
	int getPoolInputPredConnectionsQuantity(int poolNumber){ return poolsStructure[poolNumber-1]->getInputPredConnectionsQuantity(); }

	// ������� � ������� ��� ������ ������ ���� (�� ���� ������� ���������� ����� ���� � ������� ����� � ����� ����� � ������� ������)
	int getConnectionID(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionID(connectionNumber); }
	void setConnectionID(int poolNumber, int connectionNumber, int newID) { poolsStructure[poolNumber-1]->setConnectionID(connectionNumber, newID); }
	double getConnectionWeightMean(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionWeightMean(connectionNumber); }
	void setConnectionWeightMean(int poolNumber, int connectionNumber, double newWeightMean) { poolsStructure[poolNumber-1]->setConnectionWeightMean(connectionNumber, newWeightMean); }
	double getConnectionWeightVariance(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionWeightVariance(connectionNumber); }
	void setConnectionWeightVariance(int poolNumber, int connectionNumber, double newWeightVariance) { poolsStructure[poolNumber-1]->setConnectionWeightVariance(connectionNumber, newWeightVariance); } 
	bool getConnectionEnabled(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionEnabled(connectionNumber); }
	void setConnectionEnabled(int poolNumber, int connectionNumber, bool newEnabled) { poolsStructure[poolNumber-1]->setConnectionEnabled(connectionNumber, newEnabled); }
	int getConnectionDisabledStep(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionDisabledStep(connectionNumber); }
	void setConnectionDisabledStep(int poolNumber, int connectionNumber, int newDisabledStep) { poolsStructure[poolNumber-1]->setConnectionDisabledStep(connectionNumber, newDisabledStep); }
	double getConnectionDevelopSynapseProb(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionDevelopSynapseProb(connectionNumber); }
	void setConnectionDevelopSynapseProb(int poolNumber, int connectionNumber, double newDevelopSynapseProb) { poolsStructure[poolNumber-1]->setConnectionDevelopSynapseProb(connectionNumber, newDevelopSynapseProb); }
	long getConnectionInnovationNumber(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionInnovationNumber(connectionNumber); }
	void setConnectionInnovationNumber(int poolNumber, int connectionNumber, long newInnovationNumber) { poolsStructure[poolNumber-1]->setConnectionInnovationNumber(connectionNumber, newInnovationNumber); }
	TNeuralPool* getConnectionPrePool(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionPrePool(connectionNumber); }
	void setConnectionPrePool(int poolNumber, int connectionNumber, TNeuralPool* newPrePool) { poolsStructure[poolNumber-1]->setConnectionPrePool(connectionNumber, newPrePool); }
	TNeuralPool* getConnectionPostPool(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionPostPool(connectionNumber); }
	void setConnectionPostPool(int poolNumber, int connectionNumber, TNeuralPool* newPostPool) { poolsStructure[poolNumber-1]->setConnectionPostPool(connectionNumber, newPostPool); }

	// ������� � ������� ��� ������������ ������ ������ ���� (�� ���� ������� ���������� ����� ���� � ������� ���� � ����� ����� � ������� ������)
	int getPredConnectionID(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionID(predConnectionNumber); }
	void setPredConnectionID(int poolNumber, int predConnectionNumber, int newID) { poolsStructure[poolNumber-1]->setPredConnectionID(predConnectionNumber, newID); }
	bool getPredConnectionEnabled(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionEnabled(predConnectionNumber); }
	void setPredConnectionEnabled(int poolNumber, int predConnectionNumber, bool newEnabled) { poolsStructure[poolNumber-1]->setPredConnectionEnabled(predConnectionNumber, newEnabled); }
	int getPredConnectionDisabledStep(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionDisabledStep(predConnectionNumber); }
	void setPredConnectionDisabledStep(int poolNumber, int predConnectionNumber, int newDisabledStep) { poolsStructure[poolNumber-1]->setPredConnectionDisabledStep(predConnectionNumber, newDisabledStep); }
	double getDevelopPredConnectionProb(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getDevelopPredConnectionProb(predConnectionNumber); }
	void setDevelopPredConnectionProb(int poolNumber, int predConnectionNumber, double newDevelopPredConnectionProb) { poolsStructure[poolNumber-1]->setDevelopPredConnectionProb(predConnectionNumber, newDevelopPredConnectionProb); }
	long getPredConnectionInnovationNumber(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionInnovationNumber(predConnectionNumber); }
	void setPredConnectionInnovationNumber(int poolNumber, int predConnectionNumber, long newInnovationNumber) { poolsStructure[poolNumber-1]->setPredConnectionInnovationNumber(predConnectionNumber, newInnovationNumber); }
	TNeuralPool* getPredConnectionPrePool(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionPrePool(predConnectionNumber); }
	void setPredConnectionPrePool(int poolNumber, int predConnectionNumber, TNeuralPool* newPrePool) { poolsStructure[poolNumber-1]->setPredConnectionPrePool(predConnectionNumber, newPrePool); }
	TNeuralPool* getPredConnectionPostPool(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionPostPool(predConnectionNumber); }
	void setPredConnectionPostPool(int poolNumber, int predConnectionNumber, TNeuralPool* newPostPool) { poolsStructure[poolNumber-1]->setPredConnectionPostPool(predConnectionNumber, newPostPool); }

	// ������������� ID ����� (�������� ����� ��������)
	void fixPoolsIDs();
	// ������������� ID ������ (�������� ����� ��������)
	void fixConnectionsIDs();
	// ������������� ID ������������ ������ (�������� ����� ��������)
	void fixPredConnectionsIDs();

	//���������� ������������� ���� � ����
	void addPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer);

	//���������� ����� � ����
	void addConnection(int prePoolNumber, int postPoolNumber, int newID, double newWeightMean, double newWeightVariance, bool newEnabled, int newDisabledStep, double newDevelopSynapseProb, long newInnovationNumber){
		poolsStructure[postPoolNumber-1]->addConnection(newID, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber, poolsStructure[prePoolNumber-1]); 
		++connectionsQuantity;
	}

	// ���������� ������������ ����� � ����
	void addPredConnection(int prePoolNumber, int postPoolNumber, int newID, bool newEnabled, int newDisabledStep, double newDevelopPredConnectionProb, long newInnovationNumber){
		poolsStructure[postPoolNumber-1]->addPredConnection(newID, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber, poolsStructure[prePoolNumber-1]); 
		++predConnectionsQuantity;
	}
	// �������� ���� �� ���� (� ��������� ����� ���� ������� � �������� ������ �� ����� ����)
	void deletePool(int poolNumber);

	//�������� ����� �� ����
	void deleteConnection(int poolNumber, int connectionNumber){
		poolsStructure[poolNumber-1]->deleteConnection(connectionNumber);
		--connectionsQuantity;
	}

	// �������� ������������ ����� �� ����
	void deletePredConnection(int poolNumber, int predConnectionNumber){
		poolsStructure[poolNumber-1]->deletePredConnection(predConnectionNumber);
		--predConnectionsQuantity;
	}

	// �������� ����
	void erasePoolNetwork();

	//������ ���� � ���� ��� �� �����
	friend std::ostream& operator<<(std::ostream& os, const TPoolNetwork& PoolNetwork);

	//���������� ���� �� ����� ��� ������
	friend std::istream& operator>>(std::istream& is, TPoolNetwork& PoolNetwork);

	//friend int testPoolNetwork();
};

#endif // TPOOLNETWORK_H
