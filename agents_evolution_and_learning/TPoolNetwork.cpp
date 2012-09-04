#include "TPoolNetwork.h"

#include <cstring>

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
void TPoolNetwork::addPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVatiance, int newLayer){
	if (poolsQuantity >= poolsStructureSize) // ���� �� ������� ����� � �������
		inflatePoolsStructure(INFLATE_POOLS_SIZE);
	poolsStructure[poolsQuantity++] = new TNeuralPool(newID, newType, newCapacity, newBiasMean, newBiasVatiance, newLayer);
	// ���� ��� �������
	if (0 == newType) ++inputResolution;
	// ���� ��� ��������
	if (2 == newType) ++outputResolution;
}


