#include "TPoolNetwork.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"
#include "service.h"

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

// ������������� ID ������ (�������� ����� ��������)
void TPoolNetwork::fixConnectionsIDs(){
	int _connectionsQuantity = 0;
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			poolsStructure[currentPool - 1]->setConnectionID(currentConnection, ++_connectionsQuantity);
}

// ������������� ID ������������ ������ (�������� ����� ��������)
void TPoolNetwork::fixPredConnectionsIDs(){
	int _predConnectionsQuantity = 0;
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			poolsStructure[currentPool - 1]->setPredConnectionID(currentPredConnection, ++_predConnectionsQuantity);
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
				if (poolsStructure[currentPool - 1]->getConnectionPrePool(currentConnection)->getID() == poolNumber){ // ���� ��� ����� �� ���������� ����
					deleteConnection(currentPool, currentConnection);
					--currentConnection; // ����� �� �������� �� ��� �� ����� �������
				}
			for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
				if (poolsStructure[currentPool - 1]->getPredConnectionPrePool(currentPredConnection)->getID() == poolNumber){ // ���� ��� ����. ����� �� ���������� ����
					deletePredConnection(currentPool, currentPredConnection);
					--currentPredConnection;
				}
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

// �������� ������������� ����� � ����
bool TPoolNetwork::checkConnectionExistance(int prePoolNumber, int postPoolNumber){
	bool check = false;
	for (int currentPoolConnection = 1; currentPoolConnection <= poolsStructure[postPoolNumber - 1]->getInputConnectionsQuantity(); ++currentPoolConnection)
		if (poolsStructure[postPoolNumber - 1]->getConnectionPrePool(currentPoolConnection)->getID() == prePoolNumber)
			return true;

	return false;
}

// �������� ������������� ������������ ����� � ����
bool TPoolNetwork::checkPredConnectionExistance(int prePoolNumber, int postPoolNumber){
	for (int currentPoolPredConnection = 1; currentPoolPredConnection <= poolsStructure[postPoolNumber - 1]->getInputPredConnectionsQuantity(); ++currentPoolPredConnection)
		if (poolsStructure[postPoolNumber - 1]->getPredConnectionPrePool(currentPoolPredConnection)->getID() == prePoolNumber)
			return true;

	return false;
}

// �������� ����
void TPoolNetwork::erasePoolNetwork(){
	if (poolsStructure){
		for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
			delete poolsStructure[currentPool - 1];
		delete []poolsStructure;
		poolsStructure = 0;
		poolsStructureSize = 0;
	}
	poolsQuantity = 0;
	connectionsQuantity = 0;
	predConnectionsQuantity = 0;
	layersQuantity = 0;
	inputResolution = 0;
	outputResolution = 0;
}

// �������� ������������ (���������� ������ ����������� ���� - �������� ����� ��������)
TPoolNetwork& TPoolNetwork::operator=(const TPoolNetwork& sourcePoolNetwork){
	erasePoolNetwork(); // �� ������ ������ ���������� ����
	// �������� ����
	for (int currentPool = 1; currentPool <= sourcePoolNetwork.getPoolsQuantity(); ++currentPool)
		addPool(sourcePoolNetwork.getPoolID(currentPool), sourcePoolNetwork.getPoolType(currentPool), sourcePoolNetwork.getPoolCapacity(currentPool),
					sourcePoolNetwork.getPoolBiasMean(currentPool), sourcePoolNetwork.getPoolBiasVariance(currentPool), sourcePoolNetwork.getPoolLayer(currentPool));
	// �������� ��� ����� ����
	for (int currentPool = 1; currentPool <= sourcePoolNetwork.getPoolsQuantity(); ++currentPool){
		for (int currentPoolConnection = 1; currentPoolConnection <= sourcePoolNetwork.getPoolInputConnectionsQuantity(currentPool); ++ currentPoolConnection)
			addConnection(sourcePoolNetwork.getConnectionPrePool(currentPool, currentPoolConnection)->getID(), sourcePoolNetwork.getConnectionPostPool(currentPool, currentPoolConnection)->getID(),
								sourcePoolNetwork.getConnectionID(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionWeightMean(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionWeightVariance(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionEnabled(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionDisabledStep(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionDevelopSynapseProb(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionInnovationNumber(currentPool, currentPoolConnection));
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= sourcePoolNetwork.getPoolInputPredConnectionsQuantity(currentPool); ++ currentPoolPredConnection)
			addPredConnection(sourcePoolNetwork.getPredConnectionPrePool(currentPool, currentPoolPredConnection)->getID(), sourcePoolNetwork.getPredConnectionPostPool(currentPool, currentPoolPredConnection)->getID(),
								sourcePoolNetwork.getPredConnectionID(currentPool, currentPoolPredConnection), sourcePoolNetwork.getPredConnectionEnabled(currentPool, currentPoolPredConnection),
								sourcePoolNetwork.getPredConnectionDisabledStep(currentPool, currentPoolPredConnection), sourcePoolNetwork.getDevelopPredConnectionProb(currentPool, currentPoolPredConnection),
								sourcePoolNetwork.getPredConnectionInnovationNumber(currentPool, currentPoolPredConnection));
	}
	return *this;
}

// ����� ���� � ���� ��� ����� (� �������������� ��������� ������� dot.exe �� ������ GraphViz) 
// ��� ���������� ������ ���������� ����� ���� � dot.exe ��� �������� � $PATH
void TPoolNetwork::printGraphNetwork(string graphFilename){
	ofstream hDotGraphFile;
	hDotGraphFile.open((graphFilename + ".dot").c_str());
	// �������������� � ���������, ��� ���� ������ ��������� ����� �� �����
	hDotGraphFile << "digraph G { \n\trankdir=LR;\n";
	// ���������� ���� ��������
	for (int currentLayer = 1; currentLayer <= layersQuantity; ++ currentLayer){
		// ���������, ��� ��� ���� ���� � ���� ������ ���� � �������
		hDotGraphFile << "\t{ rank = same; \n";
		for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
			if (poolsStructure[currentPool - 1]->getLayer() == currentLayer)
				hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "\" [label=\"" << poolsStructure[currentPool - 1]->getID() << "\", shape = \"circle\"];\n" ;
		hDotGraphFile << "\t}\n"; // ����������� ������ ����
	}
	// ���������� �����
	double maxWeightValue = 1.0;
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			if (poolsStructure[currentPool - 1]->getConnectionEnabled(currentConnection)){
				string hex;
				service::decToHex(static_cast<int>(min(fabs(255 * poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) / maxWeightValue), 255.0)), hex, 2);
				string color;
				if (poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) < 0)
					color = "0000" + hex; // ������� ������
				else
					color = hex + "0000"; // ������� ��������
				/*hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getConnectionPrePool(currentConnection)->getID() << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getConnectionPostPool(currentConnection)->getID() << "\" [label=\"" << poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) << 
					"\", arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";*/
				hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getConnectionPrePool(currentConnection)->getID() << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getConnectionPostPool(currentConnection)->getID() << "\" [ arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";
			}
	// ���������� ������������ �����
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			if (poolsStructure[currentPool - 1]->getPredConnectionEnabled(currentPredConnection))
				hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getPredConnectionPrePool(currentPredConnection)->getID() << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getPredConnectionPostPool(currentPredConnection)->getID() << "\" [style=dashed, arrowsize=0.7, color=\"#000000\", penwidth=2.0];\n";
	hDotGraphFile << "}";
	hDotGraphFile.close();
	system(("dot -Tjpg " + graphFilename + ".dot -o " + graphFilename).c_str());
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

