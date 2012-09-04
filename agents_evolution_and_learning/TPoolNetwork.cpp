#include "TPoolNetwork.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

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
	inputResolution = 0;
	outputResolution = 0;
}

//������ ���� � ���� ��� �� �����
ostream& operator<<(ostream& ofs, const TPoolNetwork& poolNetwork){
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		ofs << *(poolNetwork.poolsStructure[currentPool - 1]);
	ofs << "|\n"; // ���������� ����������� ����� ������ � �������
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			ofs << *(poolNetwork.poolsStructure[currentPool-1]->connectednessSet[currentConnection-1]); 
	ofs << "||\n"; // ���������� ����������� ����� ������� � ������������� �������
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			ofs << *(poolNetwork.poolsStructure[currentPool-1]->predConnectednessSet[currentPredConnection-1]); 
	ofs << "|||\n"; // ���������� ����������� ����� ������
	return ofs;
}

//���������� ���� �� ����� ��� ������
istream& operator>>(istream& ifs, TPoolNetwork& PoolNetwork){
	PoolNetwork.erasePoolNetwork(); // �� ������ ������ ���������� ����
	string tmp_string;
	// ������� ��� ���� ����
	ifs >> tmp_string; // ��������� ��� ����
	while (tmp_string != "|"){ // ��������� �� ����������� ����� ������ � �������
		int newType = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ����� ����
		int newCapacity = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ������� �������� �������� ����
		double newBiasMean = atof(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ��������� �������� �������� ����
		double newBiasVariance = atof(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ���� ����
		int newLayer = atoi(tmp_string.c_str());
		PoolNetwork.addPool(PoolNetwork.getPoolsQuantity() + 1, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
		ifs >> tmp_string; // ��������� ���� ����
	}
	// ������� ��� ����� ����� ������
	ifs >> tmp_string; // ��������� ����� ����������������� ����
	while (tmp_string != "||"){ // ��������� �� ����������� ����� ������� � ������������� �������
		int prePoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ����� ������������������ ����
		int postPoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ������� ���� �����
		double newWeightMean = atof(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ��������� ���� �����
		double newWeightVariance = atof(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ������� ��������� �����
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		ifs >> tmp_string; // ��������� ��� ���������� �����
		int newDisabledStep = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ����������� ����������� ������� �� ���� �����
		double newDevelopSynapseProb = atof(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ����� ��������� ���� �����
		long newInnovationNumber = atoi(tmp_string.c_str());
		PoolNetwork.addConnection(prePoolNumber, postPoolNumber, PoolNetwork.getConnectionsQuantity() + 1, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber);
		ifs >> tmp_string; // ��������� ����� ����������������� ����
	}
	// ������� ��� ������������ ����� ����� ������
	ifs >> tmp_string; // ��������� ����� ����������������� ����
	while (tmp_string != "|||"){ // ��������� �� ����������� ����� ������������� ������� � ������ ����
		int prePoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ����� ������������������ ����
		int postPoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ������� ��������� �����
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		ifs >> tmp_string; // ��������� ��� ���������� �����
		int newDisabledStep = atoi(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ����������� ����������� ������������ ����� �� ���� �����
		double newDevelopPredConnectionProb = atof(tmp_string.c_str());
		ifs >> tmp_string; // ��������� ����� ��������� ���� �����
		long newInnovationNumber = atoi(tmp_string.c_str());
		PoolNetwork.addPredConnection(prePoolNumber, postPoolNumber, PoolNetwork.getPredConnectionsQuantity() + 1, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber);
		ifs >> tmp_string; // ��������� ����� ����������������� ����
	}

	return ifs;
}

