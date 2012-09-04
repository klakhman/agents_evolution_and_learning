#ifndef TNEURALPOOL_H
#define TNEURALPOOL_H

#include <iostream>
#include <fstream>

class TPoolNetwork;
class TPoolConnection;
class TPoolPredConnection;

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
	
	TPoolConnection** connectednessSet; // Массив входных связей пула
	int connectednessSetSize; // Текущий размер массива входных связей
	int inputConnectionsQuantity; // Количество входных связей пула

	TPoolPredConnection** predConnectednessSet; // Массив входных предикторных связей пула
	int predConnectednessSetSize; // Текущий размер массива входных предикторных связей
	int inputPredConnectionsQuantity; // Количество входных предикторных связей пула

	static const int INFLATE_CONNECTIONS_SIZE = 10; // Размер увеличения массива с входным связями в случае переполнения
	static const int INFLATE_PRED_CONNECTIONS_SIZE = 10; // Размер увеличения массива с входным предикторными связями в случае переполнения
	
	// Процедура увеличения размера массива входных связей
	void inflateConnectednessSet(int inflateSize);
	// Процедура увеличения размера массива входных предикторных связей
	void inflatePredConnectednessSet(int inflateSize);

public:
	// Конструктор по умолчанию
	TNeuralPool(){
		ID = 0;
		type = 0;
		capacity = 0;
		biasMean = 0;
		biasVariance = 0;
		layer = 0;
		connectednessSet = 0;
		connectednessSetSize = 0;
		inputConnectionsQuantity = 0;
		predConnectednessSet = 0;
		predConnectednessSetSize = 0;
		inputPredConnectionsQuantity = 0;
	}
	// Полный конструктор
	TNeuralPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer){
		setAll(newID, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
		connectednessSet = 0;
		connectednessSetSize = 0;
		inputConnectionsQuantity = 0;
		predConnectednessSet = 0;
		predConnectednessSetSize = 0;
		inputPredConnectionsQuantity = 0;
	}
	// Деструктор
	~TNeuralPool();

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
	int getInputConnectionsQuantity() const { return inputConnectionsQuantity; }
	// Получение количества входных предикторных связей пула
	int getInputPredConnectionsQuantity() const { return inputPredConnectionsQuantity; }
	// Заполнение всех характеристик пула
	void setAll(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer){
		ID = newID;
		type = newType;
		capacity = newCapacity;
		biasMean = newBiasMean;
		biasVariance = newBiasVariance;
		layer = newLayer;
	}

	// Геттеры и сеттеры для связей данного пула (во всех случаях передается номер связи в массиве связей)
	int getConnectionID(int connectionNumber) const;
	void setConnectionID(int connectionNumber, int newID);
	double getConnectionWeightMean(int connectionNumber) const;
	void setConnectionWeightMean(int connectionNumber, double newWeightMean);
	double getConnectionWeightVariance(int connectionNumber) const;
	void setConnectionWeightVariance(int connectionNumber, double newWeightVariance);
	bool getConnectionEnabled(int connectionNumber) const;
	void setConnectionEnabled(int connectionNumber, bool newEnabled);
	int getConnectionDisabledStep(int connectionNumber) const;
	void setConnectionDisabledStep(int connectionNumber, int newDisabledStep);
	double getConnectionDevelopSynapseProb(int connectionNumber) const;
	void setConnectionDevelopSynapseProb(int connectionNumber, double newDevelopSynapseProb);
	long int getConnectionInnovationNumber(int connectionNumber) const;
	void setConnectionInnovationNumber(int connectionNumber, long int newInnovationNumber);
	TNeuralPool* getConnectionPrePool(int connectionNumber) const;
	void setConnectionPrePool(int connectionNumber, TNeuralPool* newPrePool);
	TNeuralPool* getConnectionPostPool(int connectionNumber) const;
	void setConnectionPostPool(int connectionNumber, TNeuralPool* newPostPool);

	// Геттеры и сеттеры для предикторных связей данного пула (во всех случаях передается номер связи в массиве связей)
	int getPredConnectionID(int predConnectionNumber) const;
	void setPredConnectionID(int predConnectionNumber, int newID);
	bool getPredConnectionEnabled(int predConnectionNumber) const;
	void setPredConnectionEnabled(int predConnectionNumber, bool newEnabled);
	int getPredConnectionDisabledStep(int predConnectionNumber) const;
	void setPredConnectionDisabledStep(int predConnectionNumber, int newDisabledStep);
	double getDevelopPredConnectionProb(int predConnectionNumber) const;
	void setDevelopPredConnectionProb(int predConnectionNumber, double newDevelopPredConnectionProb);
	long int getPredConnectionInnovationNumber(int predConnectionNumber) const;
	void setPredConnectionInnovationNumber(int predConnectionNumber, long int newInnovationNumber);
	TNeuralPool* getPredConnectionPrePool(int predConnectionNumber) const;
	void setPredConnectionPrePool(int predConnectionNumber, TNeuralPool* newPrePool);
	TNeuralPool* getPredConnectionPostPool(int predConnectionNumber) const;
	void setPredConnectionPostPool(int predConnectionNumber, TNeuralPool* newPostPool);

	// Добавление входной связи в пул
	void addConnection(int newID, double newWeightMean, double newWeightVariance, bool newEnabled = true, int newDisabledStep = 0, double newDevelopSynapseProb = 1, long int newInnovationNumber = 0, TNeuralPool* newPrePool = 0);
	// Удаление связи из пула
	void deleteConnection(int connectionNumber);

	// Добавление входной предикторной связи в пул
	void addPredConnection(int newID, bool newEnabled = true, int newDisabledStep = 0, double newDevelopPredConnectionProb = 1, long int newInnovationNumber = 0, TNeuralPool* newPrePool = 0);
	// Удаление предикторной связи из пула
	void deletePredConnection(int predConnectionNumber);

	// Печать сведений о пуле в файл
	friend std::ostream& operator<<(std::ostream& ofs, const TNeuralPool& neuralPool);

	//Печать сети в файл
	friend std::ostream& operator<<(std::ostream& ofs, const TPoolNetwork& PoolNetwork); // Функция вывода сети объявлена дружественной, чтобы она имела прямой доступ к списку связей пула
	//friend int main(int argc, char** argv);
};

#endif // TNEURALPOOL_H
