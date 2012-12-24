#ifndef TNEURALPOOL_H
#define TNEURALPOOL_H

#include <iostream>
#include <fstream>
#include <vector>
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"

class TPoolNetwork;

/* 
Класс нейронального пула
*/
class TNeuralPool{
	int ID; // ID пула
	int type; // Тип пула (0 - сенсорный, 1 - скрытый, 2 - выходной)
	int capacity; // Объем пула
	double biasMean; // Среднее смещения нейронов пула
	double biasVariance; // Дисперсия смещения нейронов пула
	int layer; // Номер слоя нейронов пула
	
	std::vector<TPoolConnection> connectednessSet; // Массив входных связей пула

	std::vector<TPoolPredConnection> predConnectednessSet; // Массив входных предикторных связей пула

	// Пул, от которого произошла дупликация данного пула
	int rootPoolID;
	// Время появления пула в эволюции
	int appearenceEvolutionTime;

public:
  static const int INPUT_POOL = 0;
  static const int HIDDEN_POOL = 1;
  static const int OUTPUT_POOL = 2;

	// Конструктор по умолчанию
	TNeuralPool(){
		ID = 0;
		type = INPUT_POOL;
		capacity = 0;
		biasMean = 0;
		biasVariance = 0;
		layer = 0;

		rootPoolID = 0;
		appearenceEvolutionTime = 0;
	}
	// Полный конструктор
	TNeuralPool(int newID, int newType, int newLayer, double newBiasMean, double newBiasVariance = 0, int newCapacity = 1){
		setAll(newID, newType, newLayer, newBiasMean, newBiasVariance, newCapacity);

		rootPoolID = 0;
		appearenceEvolutionTime = 0;
	}
	// Деструктор
  ~TNeuralPool(){};

	// Геттеры и сеттеры для всех внутренних переменных
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	int getType() const { return type; }
	void setType(int newType) { type = newType; }
	int getCapacity() const { return capacity; }
	void setCapacity(int newCapacity) { capacity = newCapacity; }
	double getBiasMean() const { return biasMean; }
	void setBiasMean(double newBiasMean) { biasMean = newBiasMean; }
	double getBiasVariance() const { return biasVariance; }
	void setBiasVariance(double newBiasVariance) { biasVariance = newBiasVariance; }
	int getLayer() const { return layer; }
	void setLayer(int newLayer) { layer = newLayer; }
	// Получение количества входных связей пула
	int getInputConnectionsQuantity() const { return connectednessSet.size(); }
	// Получение количества входных предикторных связей пула
	int getInputPredConnectionsQuantity() const { return predConnectednessSet.size(); }
	// Заполнение всех характеристик пула
	void setAll(int newID, int newType, int newLayer, double newBiasMean, double newBiasVariance = 0, int newCapacity = 1){
		ID = newID;
		type = newType;
		capacity = newCapacity;
		biasMean = newBiasMean;
		biasVariance = newBiasVariance;
		layer = newLayer;
	}

	void setRootPoolID(int _rootPoolID) { rootPoolID = _rootPoolID; }
	int getRootPoolID() const { return rootPoolID; }
	void setAppearenceEvolutionTime(int _appearenceEvolutionTime) { appearenceEvolutionTime = _appearenceEvolutionTime; }
	int getAppearenceEvolutionTime() const { return appearenceEvolutionTime; }

	// Геттеры и сеттеры для связей данного пула (во всех случаях передается номер связи в массиве связей)
	int getConnectionID(int connectionNumber) const { return connectednessSet[connectionNumber-1].getID(); }
	void setConnectionID(int connectionNumber, int newID) { connectednessSet[connectionNumber-1].setID(newID); }
	double getConnectionWeightMean(int connectionNumber) const { return connectednessSet[connectionNumber-1].getWeightMean(); }
	void setConnectionWeightMean(int connectionNumber, double newWeightMean) { connectednessSet[connectionNumber-1].setWeightMean(newWeightMean); }
	double getConnectionWeightVariance(int connectionNumber) const { return connectednessSet[connectionNumber-1].getWeightVariance(); }
	void setConnectionWeightVariance(int connectionNumber, double newWeightVariance) { connectednessSet[connectionNumber-1].setWeightVariance(newWeightVariance); } 
	bool getConnectionEnabled(int connectionNumber) const { return connectednessSet[connectionNumber-1].getEnabled(); }
	void setConnectionEnabled(int connectionNumber, bool newEnabled) { connectednessSet[connectionNumber-1].setEnabled(newEnabled); }
	int getConnectionDisabledStep(int connectionNumber) const { return connectednessSet[connectionNumber-1].getDisabledStep(); }
	void setConnectionDisabledStep(int connectionNumber, int newDisabledStep) { connectednessSet[connectionNumber-1].setDisabledStep(newDisabledStep); }
	double getConnectionDevelopSynapseProb(int connectionNumber) const { return connectednessSet[connectionNumber-1].getDevelopSynapseProb(); }
	void setConnectionDevelopSynapseProb(int connectionNumber, double newDevelopSynapseProb) { connectednessSet[connectionNumber-1].setDevelopSynapseProb(newDevelopSynapseProb); }
	long getConnectionInnovationNumber(int connectionNumber) const { return connectednessSet[connectionNumber-1].getInnovationNumber(); }
	void setConnectionInnovationNumber(int connectionNumber, long newInnovationNumber) { connectednessSet[connectionNumber-1].setInnovationNumber(newInnovationNumber); }
	int getConnectionPrePoolID(int connectionNumber) const { return connectednessSet[connectionNumber-1].getPrePoolID(); }
	void setConnectionPrePoolID(int connectionNumber, int newPrePoolID) { connectednessSet[connectionNumber-1].setPrePoolID(newPrePoolID); }
	int getConnectionPostPoolID(int connectionNumber) const { return connectednessSet[connectionNumber-1].getPostPoolID(); }
	void setConnectionPostPoolID(int connectionNumber, int newPostPoolID) { connectednessSet[connectionNumber-1].setPostPoolID(newPostPoolID); }

	// Геттеры и сеттеры для предикторных связей данного пула (во всех случаях передается номер связи в массиве связей)
	int getPredConnectionID(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1].getID(); }
	void setPredConnectionID(int predConnectionNumber, int newID) { predConnectednessSet[predConnectionNumber-1].setID(newID); }
	bool getPredConnectionEnabled(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1].getEnabled(); }
	void setPredConnectionEnabled(int predConnectionNumber, bool newEnabled) { predConnectednessSet[predConnectionNumber-1].setEnabled(newEnabled); }
	int getPredConnectionDisabledStep(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1].getDisabledStep(); }
	void setPredConnectionDisabledStep(int predConnectionNumber, int newDisabledStep) { predConnectednessSet[predConnectionNumber-1].setDisabledStep(newDisabledStep); }
	double getDevelopPredConnectionProb(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1].getDevelopPredConnectionProb(); }
	void setDevelopPredConnectionProb(int predConnectionNumber, double newDevelopPredConnectionProb) { predConnectednessSet[predConnectionNumber-1].setDevelopPredConnectionProb(newDevelopPredConnectionProb); }
	long getPredConnectionInnovationNumber(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1].getInnovationNumber(); }
	void setPredConnectionInnovationNumber(int predConnectionNumber, long newInnovationNumber) { predConnectednessSet[predConnectionNumber-1].setInnovationNumber(newInnovationNumber); }
	int getPredConnectionPrePoolID(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1].getPrePoolID(); }
	void setPredConnectionPrePoolID(int predConnectionNumber, int newPrePoolID) { predConnectednessSet[predConnectionNumber-1].setPrePoolID(newPrePoolID); }
	int getPredConnectionPostPoolID(int predConnectionNumber) const { return predConnectednessSet[predConnectionNumber-1].getPostPoolID(); }
	void setPredConnectionPostPoolID(int predConnectionNumber, int newPostPoolID) { predConnectednessSet[predConnectionNumber-1].setPostPoolID(newPostPoolID); }

	// Добавление входной связи в пул
	void addConnection(int newID, int newPrePoolID, double newWeightMean, double newWeightVariance = 0, double newDevelopSynapseProb = 1, bool newEnabled = true, int newDisabledStep = 0, long int newInnovationNumber = 0){
    TPoolConnection poolConnection(newID, newPrePoolID, ID, newWeightMean, newWeightVariance, newDevelopSynapseProb, newEnabled, newDisabledStep, newInnovationNumber);
    connectednessSet.push_back(poolConnection);
  }
	// Удаление связи из пула
	void deleteConnection(int connectionNumber){
    connectednessSet.erase(connectednessSet.begin() + connectionNumber - 1);
  }

	// Добавление входной предикторной связи в пул
	void addPredConnection(int newID, int newPrePoolID, double newDevelopPredConnectionProb = 1, bool newEnabled = true, int newDisabledStep = 0, long int newInnovationNumber = 0){
    TPoolPredConnection poolPredConnection(newID, newPrePoolID, ID, newDevelopPredConnectionProb, newEnabled, newDisabledStep, newInnovationNumber);
    predConnectednessSet.push_back(poolPredConnection);
  }
	// Удаление предикторной связи из пула
	void deletePredConnection(int predConnectionNumber){
    predConnectednessSet.erase(predConnectednessSet.begin() + predConnectionNumber - 1);
  }

	// Печать сведений о пуле в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TNeuralPool& neuralPool);

	// Печать всех сведений о пуле в файл или на экран (вместе с номером пула родителя и временем появления в эволюции)
	std::ostream& printPoolExtra(std::ostream& os) const;

	//Печать сети в файл или на экран
	friend std::ostream& operator<<(std::ostream& ofs, const TPoolNetwork& PoolNetwork); // Функция вывода сети объявлена дружественной, чтобы она имела прямой доступ к списку связей пула

	friend class TPoolNetwork; // Функция вывода сети объявлена дружественной, чтобы она имела прямой доступ к списку связей пула
};

#endif // TNEURALPOOL_H
