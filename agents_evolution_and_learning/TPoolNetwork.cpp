#include "TPoolNetwork.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

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
void TPoolNetwork::addPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer){
	if (poolsQuantity >= poolsStructureSize) // Если не хватает места в массиве
		inflatePoolsStructure(INFLATE_POOLS_SIZE);
	poolsStructure[poolsQuantity++] = new TNeuralPool(newID, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
	// Если пул входной
	if (0 == newType) ++inputResolution;
	// Если пул выходной
	if (2 == newType) ++outputResolution;
}

// Стирание сети
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

//Печать сети в файл или на экран
ostream& operator<<(ostream& ofs, const TPoolNetwork& poolNetwork){
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		ofs << *(poolNetwork.poolsStructure[currentPool - 1]);
	ofs << "|\n"; // Записываем разделитель между пулами и связями
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			ofs << *(poolNetwork.poolsStructure[currentPool-1]->connectednessSet[currentConnection-1]); 
	ofs << "||\n"; // Записываем разделитель между связями и предикторными связями
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			ofs << *(poolNetwork.poolsStructure[currentPool-1]->predConnectednessSet[currentPredConnection-1]); 
	ofs << "|||\n"; // Записываем разделитель между сетями
	return ofs;
}

//Считывание сети из файла или экрана
istream& operator>>(istream& ifs, TPoolNetwork& PoolNetwork){
	PoolNetwork.erasePoolNetwork(); // На всякий случай опустошаем сеть
	string tmp_string;
	// Создаем все пулы сети
	ifs >> tmp_string; // Считываем тип пула
	while (tmp_string != "|"){ // Считываем до разделителя между пулами и связями
		int newType = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем объем пула
		int newCapacity = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем среднее смещения нейронов пула
		double newBiasMean = atof(tmp_string.c_str());
		ifs >> tmp_string; // Считываем дисперсию смещения нейронов пула
		double newBiasVariance = atof(tmp_string.c_str());
		ifs >> tmp_string; // Считываем слой пула
		int newLayer = atoi(tmp_string.c_str());
		PoolNetwork.addPool(PoolNetwork.getPoolsQuantity() + 1, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
		ifs >> tmp_string; // Считываем типа пула
	}
	// Создаем все связи между пулами
	ifs >> tmp_string; // Считываем номер пресинаптического пула
	while (tmp_string != "||"){ // Считываем до разделителя между связями и предикторными связями
		int prePoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем номер постсинаптического пула
		int postPoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем среднее веса связи
		double newWeightMean = atof(tmp_string.c_str());
		ifs >> tmp_string; // Считываем дисперсию веса связи
		double newWeightVariance = atof(tmp_string.c_str());
		ifs >> tmp_string; // Считываем признак экспресии связи
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		ifs >> tmp_string; // Считываем так выключения связи
		int newDisabledStep = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем вероятность образования синапса по этой связи
		double newDevelopSynapseProb = atof(tmp_string.c_str());
		ifs >> tmp_string; // Считываем номер инновации этой связи
		long newInnovationNumber = atoi(tmp_string.c_str());
		PoolNetwork.addConnection(prePoolNumber, postPoolNumber, PoolNetwork.getConnectionsQuantity() + 1, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber);
		ifs >> tmp_string; // Считываем номер пресинаптического пула
	}
	// Создаем все предикторные связи между пулами
	ifs >> tmp_string; // Считываем номер пресинаптического пула
	while (tmp_string != "|||"){ // Считываем до разделителя между предикторными связями и концом сети
		int prePoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем номер постсинаптического пула
		int postPoolNumber = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем признак экспресии связи
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		ifs >> tmp_string; // Считываем так выключения связи
		int newDisabledStep = atoi(tmp_string.c_str());
		ifs >> tmp_string; // Считываем вероятность образования предикторной связи по этой связи
		double newDevelopPredConnectionProb = atof(tmp_string.c_str());
		ifs >> tmp_string; // Считываем номер инновации этой связи
		long newInnovationNumber = atoi(tmp_string.c_str());
		PoolNetwork.addPredConnection(prePoolNumber, postPoolNumber, PoolNetwork.getPredConnectionsQuantity() + 1, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber);
		ifs >> tmp_string; // Считываем номер пресинаптического пула
	}

	return ifs;
}

