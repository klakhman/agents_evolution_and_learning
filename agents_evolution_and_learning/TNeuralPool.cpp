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

// Геттеры и сеттеры для связей данного пула (во всех случаях передается номер связи в массиве связей)
int TNeuralPool::getConnectionID(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getID(); }
void TNeuralPool::setConnectionID(int connectionNumber, int newID) { connectednessSet[connectionNumber-1]->setID(newID); }
double TNeuralPool::getConnectionWeightMean(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getWeightMean(); }
void TNeuralPool::setConnectionWeightMean(int connectionNumber, double newWeightMean) { connectednessSet[connectionNumber-1]->setWeightMean(newWeightMean); }
double TNeuralPool::getConnectionWeightVariance(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getWeightVariance(); }
void TNeuralPool::setConnectionWeightVariance(int connectionNumber, double newWeightVariance) { connectednessSet[connectionNumber-1]->setWeightVariance(newWeightVariance); } 
bool TNeuralPool::getConnectionEnabled(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getEnabled(); }
void TNeuralPool::setConnectionEnabled(int connectionNumber, bool newEnabled) { connectednessSet[connectionNumber-1]->setEnabled(newEnabled); }
int TNeuralPool::getConnectionDisabledStep(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getDisabledStep(); }
void TNeuralPool::setConnectionDisabledStep(int connectionNumber, int newDisabledStep) { connectednessSet[connectionNumber-1]->setDisabledStep(newDisabledStep); }
double TNeuralPool::getConnectionDevelopSynapseProb(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getDevelopSynapseProb(); }
void TNeuralPool::setConnectionDevelopSynapseProb(int connectionNumber, double newDevelopSynapseProb) { connectednessSet[connectionNumber-1]->setDevelopSynapseProb(newDevelopSynapseProb); }
long int TNeuralPool::getConnectionInnovationNumber(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getInnovationNumber(); }
void TNeuralPool::setConnectionInnovationNumber(int connectionNumber, long int newInnovationNumber) { connectednessSet[connectionNumber-1]->setInnovationNumber(newInnovationNumber); }
TNeuralPool* TNeuralPool::getConnectionPrePool(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getPrePool(); }
void TNeuralPool::setConnectionPrePool(int connectionNumber, TNeuralPool* newPrePool) { connectednessSet[connectionNumber-1]->setPrePool(newPrePool); }
TNeuralPool* TNeuralPool::getConnectionPostPool(int connectionNumber) const { return connectednessSet[connectionNumber-1]->getPostPool(); }
void TNeuralPool::setConnectionPostPool(int connectionNumber, TNeuralPool* newPostPool) { connectednessSet[connectionNumber-1]->setPostPool(newPostPool); }

// Геттеры и сеттеры для предикторных связей данного пула (во всех случаях передается номер связи в массиве связей)
int TNeuralPool::getPredConnectionID(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1]->getID(); }
void TNeuralPool::setPredConnectionID(int predConnectionNumber, int newID) { predConnectednessSet[predConnectionNumber-1]->setID(newID); }
bool TNeuralPool::getPredConnectionEnabled(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1]->getEnabled(); }
void TNeuralPool::setPredConnectionEnabled(int predConnectionNumber, bool newEnabled) { predConnectednessSet[predConnectionNumber-1]->setEnabled(newEnabled); }
int TNeuralPool::getPredConnectionDisabledStep(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1]->getDisabledStep(); }
void TNeuralPool::setPredConnectionDisabledStep(int predConnectionNumber, int newDisabledStep) { predConnectednessSet[predConnectionNumber-1]->setDisabledStep(newDisabledStep); }
double TNeuralPool::getDevelopPredConnectionProb(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1]->getDevelopPredConnectionProb(); }
void TNeuralPool::setDevelopPredConnectionProb(int predConnectionNumber, double newDevelopPredConnectionProb) { predConnectednessSet[predConnectionNumber-1]->setDevelopPredConnectionProb(newDevelopPredConnectionProb); }
long int TNeuralPool::getPredConnectionInnovationNumber(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1]->getInnovationNumber(); }
void TNeuralPool::setPredConnectionInnovationNumber(int predConnectionNumber, long int newInnovationNumber) { predConnectednessSet[predConnectionNumber-1]->setInnovationNumber(newInnovationNumber); }
TNeuralPool* TNeuralPool::getPredConnectionPrePool(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1]->getPrePool(); }
void TNeuralPool::setPredConnectionPrePool(int predConnectionNumber, TNeuralPool* newPrePool) { predConnectednessSet[predConnectionNumber-1]->setPrePool(newPrePool); }
TNeuralPool* TNeuralPool::getPredConnectionPostPool(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1]->getPostPool(); }
void TNeuralPool::setPredConnectionPostPool(int predConnectionNumber, TNeuralPool* newPostPool) { predConnectednessSet[predConnectionNumber-1]->setPostPool(newPostPool); }

// Добавление входной связи в пул
void TNeuralPool::addConnection(int newID, double newWeightMean, double newWeightVariance, bool newEnabled, int newDisabledStep, double newDevelopSynapseProb, long int newInnovationNumber, TNeuralPool* newPrePool){
	if (inputConnectionsQuantity >= connectednessSetSize) // Если у нас не хватает объема массива
		inflateConnectednessSet(INFLATE_CONNECTIONS_SIZE);
	connectednessSet[inputConnectionsQuantity++] = new TPoolConnection(newID, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber, newPrePool, this);
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
void TNeuralPool::addPredConnection(int newID, bool newEnabled, int newDisabledStep, double newDevelopPredConnectionProb, long int newInnovationNumber, TNeuralPool* newPrePool){
	if (inputPredConnectionsQuantity >= predConnectednessSetSize) // Если у нас не хватает объема массива
		inflatePredConnectednessSet(INFLATE_PRED_CONNECTIONS_SIZE);
	predConnectednessSet[inputPredConnectionsQuantity++] = new TPoolPredConnection(newID, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber, newPrePool, this);
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

// Печать сведений о пуле в файл
std::ostream& operator<<(std::ostream& ofs, const TNeuralPool& neuralPool){
	ofs << neuralPool.type << "\t" << neuralPool.capacity << "\t" << neuralPool.biasMean
		<<	"\t" << neuralPool.biasVariance << "\t" << neuralPool.layer << std::endl;
	return ofs;
}