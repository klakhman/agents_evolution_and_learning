#include "TPoolNetwork.h"

#include <cstring>

// Процедура увеличения размера массива пулов
void TPoolNetwork::inflatePoolsStructure(int inflateSize){
	TNeuralPool** newPoolsStructure = new TNeuralPool*[poolsStructureSize + inflateSize];
	memset(newPoolsStructure, 0, (poolsStructureSize + inflateSize) * sizeof(TNeuralPool*));
	memcpy(newPoolsStructure, poolsStructure, poolsStructureSize * sizeof(TNeuralPool*));
	delete []poolsStructure;
	poolsStructure = newPoolsStructure;
	poolsStructureSize += inflateSize;
}

// Декструктор
TPoolNetwork::~TPoolNetwork(){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		delete poolsStructure[currentPool - 1];
	delete []poolsStructure;
}

//Добавление нейронального пула в сеть
void TPoolNetwork::addPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVatiance, int newLayer){
	if (poolsQuantity >= poolsStructureSize) // Если не хватает места в массиве
		inflatePoolsStructure(INFLATE_POOLS_SIZE);
	poolsStructure[poolsQuantity++] = new TNeuralPool(newID, newType, newCapacity, newBiasMean, newBiasVatiance, newLayer);
	// Если пул входной
	if (0 == newType) ++inputResolution;
	// Если пул выходной
	if (2 == newType) ++outputResolution;
}


