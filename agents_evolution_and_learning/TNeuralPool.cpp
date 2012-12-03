#include "TNeuralPool.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"

#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;

// Процедура увеличения размера массива входных связей
void TNeuralPool::inflateConnectednessSet(int inflateSize){
	TPoolConnection** newConnectednessSet = new TPoolConnection*[connectednessSetSize + inflateSize];
	memset(newConnectednessSet, 0, (connectednessSetSize + inflateSize) * sizeof(TPoolConnection*));
	memcpy(newConnectednessSet, connectednessSet, connectednessSetSize * sizeof(TPoolConnection*));
	delete []connectednessSet;
	connectednessSet = newConnectednessSet;
	connectednessSetSize += inflateSize;
}

// Процедура увеличения размера массива входных предикторных связей
void TNeuralPool::inflatePredConnectednessSet(int inflateSize){
	TPoolPredConnection** newPredConnectednessSet = new TPoolPredConnection*[predConnectednessSetSize + inflateSize];
	memset(newPredConnectednessSet, 0, (predConnectednessSetSize + inflateSize) * sizeof(TPoolPredConnection*));
	memcpy(newPredConnectednessSet, predConnectednessSet, predConnectednessSetSize * sizeof(TPoolPredConnection*));
	delete []predConnectednessSet;
	predConnectednessSet = newPredConnectednessSet;
	predConnectednessSetSize += inflateSize;
}

// Деструктор
TNeuralPool::~TNeuralPool(){
	for (int currentConnection = 1; currentConnection <= inputConnectionsQuantity; ++currentConnection)
		delete connectednessSet[currentConnection-1];
	delete []connectednessSet;
	for (int currentPredConnection = 1; currentPredConnection <= inputPredConnectionsQuantity; ++currentPredConnection)
		delete predConnectednessSet[currentPredConnection-1];
	delete []predConnectednessSet;
}

// Добавление входной связи в пул
void TNeuralPool::addConnection(int newID, int newPrePoolID, double newWeightMean, double newWeightVariance /*=0*/, double newDevelopSynapseProb/*=1*/, bool newEnabled /*=true*/, int newDisabledStep /*=0*/, long int newInnovationNumber /*=0*/){
	if (inputConnectionsQuantity >= connectednessSetSize) // Если у нас не хватает объема массива
		inflateConnectednessSet(INFLATE_CONNECTIONS_SIZE);
	connectednessSet[inputConnectionsQuantity++] = new TPoolConnection(newID, newPrePoolID, ID, newWeightMean, newWeightVariance, newDevelopSynapseProb, newEnabled, newDisabledStep, newInnovationNumber);
}

// Удаление связи из пула
void TNeuralPool::deleteConnection(int connectionNumber){
	TPoolConnection* deletingConnection = connectednessSet[connectionNumber - 1];
	delete deletingConnection;
	// Сдвигаем массив
	for (int currentConnection = connectionNumber - 1; currentConnection < inputConnectionsQuantity - 1; ++currentConnection)
		connectednessSet[currentConnection] = connectednessSet[currentConnection + 1];
	//memcpy(connectednessSet[connectionNumber - 1], connectednessSet[connectionNumber], (inputConnectionsQuantity - connectionNumber) * sizeof(TPoolConnection*));
	connectednessSet[inputConnectionsQuantity - 1] = 0; // Обнуляем указатель последней связи
	--inputConnectionsQuantity;
}

// Добавление входной предикторной связи в пул
void TNeuralPool::addPredConnection(int newID, int newPrePoolID, double newDevelopPredConnectionProb /*=1*/, bool newEnabled /*=true*/, int newDisabledStep /*=0*/, long int newInnovationNumber /*=0*/){
	if (inputPredConnectionsQuantity >= predConnectednessSetSize) // Если у нас не хватает объема массива
		inflatePredConnectednessSet(INFLATE_PRED_CONNECTIONS_SIZE);
	predConnectednessSet[inputPredConnectionsQuantity++] = new TPoolPredConnection(newID, newPrePoolID, ID, newDevelopPredConnectionProb, newEnabled, newDisabledStep, newInnovationNumber);
}

// Удаление предикторной связи из пула
void TNeuralPool::deletePredConnection(int predConnectionNumber){
	TPoolPredConnection* deletingPredConnection = predConnectednessSet[predConnectionNumber - 1];
	delete deletingPredConnection;
	// Сдвигаем массив
	for (int currentPredConnection = predConnectionNumber - 1; currentPredConnection < inputPredConnectionsQuantity - 1; ++currentPredConnection)
		predConnectednessSet[currentPredConnection] = predConnectednessSet[currentPredConnection + 1];
	//memcpy(connectednessSet[connectionNumber - 1], connectednessSet[connectionNumber], (inputConnectionsQuantity - connectionNumber) * sizeof(TPoolConnection*));
	predConnectednessSet[inputPredConnectionsQuantity - 1] = 0; // Обнуляем указатель последней связи
	--inputPredConnectionsQuantity;
}

// Печать сведений о пуле в файл или на экран
ostream& operator<<(ostream& os, const TNeuralPool& neuralPool){
	os << neuralPool.type << "\t" << neuralPool.capacity << "\t" << neuralPool.biasMean
		<<	"\t" << neuralPool.biasVariance << "\t" << neuralPool.layer << endl;
	return os;
}

// Печать всех сведений о пуле в файл или на экран (вместе с номером пула родителя и временем появления в эволюции)
std::ostream& TNeuralPool::printPoolExtra(std::ostream& os) const{
	os << type << "\t" << capacity << "\t" << biasMean
		<<	"\t" << biasVariance << "\t" << layer << "\t" << rootPoolID << "\t" << appearenceEvolutionTime << endl;
	return os;
}