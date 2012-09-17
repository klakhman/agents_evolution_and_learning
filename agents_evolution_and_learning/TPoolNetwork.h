#ifndef TPOOLNETWORK_H
#define TPOOLNETWORK_H

#include "TNeuralPool.h"

#include <iostream>
#include <fstream>

/*
Класс cети из нейрональных пулов
*/
class TPoolNetwork{
	TNeuralPool** poolsStructure; // Список из пулов 
	int poolsStructureSize; // Текущий размер массива пулов
	int poolsQuantity; // Количество пулов в сети

	int connectionsQuantity; // Количество связей в сети
	int predConnectionsQuantity; // Количество предикторных связей в сети
	
	int layersQuantity; // Количество слоев в сети (!!!! не уменьшается при удалении пулов !!!)
	int inputResolution; // Размер входного пр-ва сети
	int outputResolution; // Размер выходного пр-ва сети

	static const int INFLATE_POOLS_SIZE = 10; // Размер увеличения массива с пулами в случае переполнения

	// Процедура увеличения размера массива пулов
	void inflatePoolsStructure(int inflateSize);

public:
	// Конструктор по умолчанию
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
	// Деструктор
	~TPoolNetwork();

	//Геттеры для параметров сети
	int getPoolsQuantity() { return poolsQuantity; }
	int getConnectionsQuantity() { return connectionsQuantity; }
	int getPredConnectionsQuantity() { return predConnectionsQuantity; }
	int getInputResolution() { return inputResolution; }
	int getOutputResolution() { return outputResolution; }

	// Геттеры и сеттеры для всех пулов сети (во всех случаях передается номер пула в массиве пула)
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

	// Геттеры и сеттеры для связей данной сети (во всех случаях передается номер пула в массиве пулов и номер связи в массиве связей)
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

	// Геттеры и сеттеры для предикторных связей данной сети (во всех случаях передается номер пула в массиве пула и номер связи в массиве связей)
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

	// Корректировка ID пулов (например после удаления)
	void fixPoolsIDs();
	// Корректировка ID связей (например после удаления)
	void fixConnectionsIDs();
	// Корректировка ID предикторных связей (например после удаления)
	void fixPredConnectionsIDs();

	//Добавление нейронального пула в сеть
	void addPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer);

	//Добавление связи в сеть
	void addConnection(int prePoolNumber, int postPoolNumber, int newID, double newWeightMean, double newWeightVariance, bool newEnabled, int newDisabledStep, double newDevelopSynapseProb, long newInnovationNumber){
		poolsStructure[postPoolNumber-1]->addConnection(newID, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber, poolsStructure[prePoolNumber-1]); 
		++connectionsQuantity;
	}

	// Добавление предикторной связи в сеть
	void addPredConnection(int prePoolNumber, int postPoolNumber, int newID, bool newEnabled, int newDisabledStep, double newDevelopPredConnectionProb, long newInnovationNumber){
		poolsStructure[postPoolNumber-1]->addPredConnection(newID, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber, poolsStructure[prePoolNumber-1]); 
		++predConnectionsQuantity;
	}
	// Удаление пула из сети (с удалением также всех входных и выходных связей из этого пула)
	void deletePool(int poolNumber);

	//Удаление связи из сети
	void deleteConnection(int poolNumber, int connectionNumber){
		poolsStructure[poolNumber-1]->deleteConnection(connectionNumber);
		--connectionsQuantity;
	}

	// Удаление предикторной связи из сети
	void deletePredConnection(int poolNumber, int predConnectionNumber){
		poolsStructure[poolNumber-1]->deletePredConnection(predConnectionNumber);
		--predConnectionsQuantity;
	}

	// Стирание сети
	void erasePoolNetwork();

	//Печать сети в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TPoolNetwork& PoolNetwork);

	//Считывание сети из файла или экрана
	friend std::istream& operator>>(std::istream& is, TPoolNetwork& PoolNetwork);

	//friend int testPoolNetwork();
};

#endif // TPOOLNETWORK_H
