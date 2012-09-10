#include "TPoolNetwork.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// ������������� ID ����� (�������� ����� ��������)
void TPoolNetwork::fixPoolsIDs(){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		poolsStructure[currentPool - 1]->setID(currentPool);
}

// ��������� ���������� ������� ������� �����
void TPoolNetwork::inflatePoolsStructure(int inflateSize){
	TNeuralPool** newPoolsStructure = new TNeuralPool*[poolsStructureSize + inflateSize];
	memset(newPoolsStructure, 0, (poolsStructureSize + inflateSize) * sizeof(TNeuralPool*));
	memcpy(newPoolsStructure, poolsStructure, poolsStructureSize * sizeof(TNeuralPool*));
	delete []poolsStructure;
	poolsStructure = newPoolsStructure;
	poolsStructureSize += inflateSize;
}

// �����������
TPoolNetwork::~TPoolNetwork(){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		delete poolsStructure[currentPool - 1];
	delete []poolsStructure;
}

//���������� ������������� ���� � ����
void TPoolNetwork::addPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer){
	if (poolsQuantity >= poolsStructureSize) // ���� �� ������� ����� � �������
		inflatePoolsStructure(INFLATE_POOLS_SIZE);
	poolsStructure[poolsQuantity++] = new TNeuralPool(newID, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
	// ���� ��� �������
	if (0 == newType) ++inputResolution;
	// ���� ��� ��������
	if (2 == newType) ++outputResolution;
	// ������� �� ���� ����
	if (newLayer > layersQuantity) layersQuantity = newLayer;
}

// �������� ���� �� ���� (� ��������� ����� ���� ������� � �������� ������ �� ����� ����)
void TPoolNetwork::deletePool(int poolNumber){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		if (currentPool != poolNumber){ // ���� ��� �� ��������� ��� (���������� ������ ��� �������� �������)
			for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
				if (poolsStructure[currentPool - 1]->getConnectionPrePool(currentConnection)->getID() == poolNumber) // ���� ��� ����� �� ���������� ����
				deleteConnection(currentPool, currentConnection);
			for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
				if (poolsStructure[currentPool - 1]->getPredConnectionPrePool(currentPredConnection)->getID() == poolNumber) // ���� ��� ����. ����� �� ���������� ����
				deletePredConnection(currentPool, currentPredConnection);
		}
	// ���� ��� �������
	if (0 == poolsStructure[poolNumber - 1]->getType()) --inputResolution;
	// ���� ��� ��������
	if (2 == poolsStructure[poolNumber - 1]->getType()) --outputResolution;
	// ������� ��� � �������� ������
	connectionsQuantity -= poolsStructure[poolNumber - 1]->getInputConnectionsQuantity();
	predConnectionsQuantity -= poolsStructure[poolNumber - 1]->getInputPredConnectionsQuantity();
	delete poolsStructure[poolNumber - 1];
	for (int currentPool = poolNumber - 1; currentPool < poolsQuantity - 1; ++currentPool)
		poolsStructure[currentPool] = poolsStructure[currentPool + 1];
	poolsStructure[poolsQuantity - 1] = 0;
	--poolsQuantity;
	fixPoolsIDs();
}

// �������� ����
void TPoolNetwork::erasePoolNetwork(){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		delete poolsStructure[currentPool - 1];
	delete []poolsStructure;
	poolsStructure = 0;
	poolsStructureSize = 0;
	poolsQuantity = 0;
	connectionsQuantity = 0;
	predConnectionsQuantity = 0;
	layersQuantity = 0;
	inputResolution = 0;
	outputResolution = 0;
}

//������ ���� � ���� ��� �� �����
ostream& operator<<(ostream& os, const TPoolNetwork& poolNetwork){
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		os << *(poolNetwork.poolsStructure[currentPool - 1]);
	os << "|\n"; // ���������� ����������� ����� ������ � �������
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			os << *(poolNetwork.poolsStructure[currentPool-1]->connectednessSet[currentConnection-1]); 
	os << "||\n"; // ���������� ����������� ����� ������� � ������������� �������
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			os << *(poolNetwork.poolsStructure[currentPool-1]->predConnectednessSet[currentPredConnection-1]); 
	os << "|||\n"; // ���������� ����������� ����� ������
	return os;
}

//���������� ���� �� ����� ��� ������
istream& operator>>(istream& is, TPoolNetwork& poolNetwork){
	poolNetwork.erasePoolNetwork(); // �� ������ ������ ���������� ����
	string tmp_string;
	// ������� ��� ���� ����
	is >> tmp_string; // ��������� ��� ����
	while (tmp_string != "|"){ // ��������� �� ����������� ����� ������ � �������
		int newType = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ����� ����
		int newCapacity = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ������� �������� �������� ����
		double newBiasMean = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ��������� �������� �������� ����
		double newBiasVariance = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ���� ����
		int newLayer = atoi(tmp_string.c_str());
		poolNetwork.addPool(poolNetwork.getPoolsQuantity() + 1, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
		is >> tmp_string; // ��������� ���� ����
	}
	// ������� ��� ����� ����� ������
	is >> tmp_string; // ��������� ����� ����������������� ����
	while (tmp_string != "||"){ // ��������� �� ����������� ����� ������� � ������������� �������
		int prePoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ����� ������������������ ����
		int postPoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ������� ���� �����
		double newWeightMean = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ��������� ���� �����
		double newWeightVariance = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ������� ��������� �����
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		is >> tmp_string; // ��������� ��� ���������� �����
		int newDisabledStep = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ����������� ����������� ������� �� ���� �����
		double newDevelopSynapseProb = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ����� ��������� ���� �����
		long newInnovationNumber = atoi(tmp_string.c_str());
		poolNetwork.addConnection(prePoolNumber, postPoolNumber, poolNetwork.getConnectionsQuantity() + 1, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber);
		is >> tmp_string; // ��������� ����� ����������������� ����
	}
	// ������� ��� ������������ ����� ����� ������
	is >> tmp_string; // ��������� ����� ����������������� ����
	while (tmp_string != "|||"){ // ��������� �� ����������� ����� ������������� ������� � ������ ����
		int prePoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ����� ������������������ ����
		int postPoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ������� ��������� �����
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		is >> tmp_string; // ��������� ��� ���������� �����
		int newDisabledStep = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ����������� ����������� ������������ ����� �� ���� �����
		double newDevelopPredConnectionProb = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ����� ��������� ���� �����
		long newInnovationNumber = atoi(tmp_string.c_str());
		poolNetwork.addPredConnection(prePoolNumber, postPoolNumber, poolNetwork.getPredConnectionsQuantity() + 1, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber);
		is >> tmp_string; // ��������� ����� ����������������� ����
	}

	return is;
}
