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
	
	// Корректировка ID пулов (например после удаления)
	void fixPoolsIDs();

public:
  static const int INPUT_POOL = 0;
  static const int HIDDEN_POOL = 1;
  static const int OUTPUT_POOL = 2;

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
	int getPoolsQuantity() const{ return poolsQuantity; }
	int getConnectionsQuantity() const { return connectionsQuantity; }
	int getPredConnectionsQuantity() const { return predConnectionsQuantity; }
	int getInputResolution() const { return inputResolution; }
	int getOutputResolution() const { return outputResolution; }

	// Геттеры и сеттеры для всех пулов сети (во всех случаях передается номер пула в массиве пула)
	int getPoolID(int poolNumber) const { return poolsStructure[poolNumber-1]->getID(); }
	void setPoolID(int poolNumber, int newID){ poolsStructure[poolNumber-1]->setID(newID); }
	int getPoolType(int poolNumber) const { return poolsStructure[poolNumber-1]->getType(); }
	void setPoolType(int poolNumber, int newType) { poolsStructure[poolNumber-1]->setType(newType); }
	int getPoolCapacity(int poolNumber) const { return poolsStructure[poolNumber-1]->getCapacity(); }
	void setPoolCapacity(int poolNumber, int newCapacity) { poolsStructure[poolNumber-1]->setCapacity(newCapacity); }
	double getPoolBiasMean(int poolNumber) const { return poolsStructure[poolNumber-1]->getBiasMean(); }
	void setPoolBiasMean(int poolNumber, double newBiasMean) { poolsStructure[poolNumber-1]->setBiasMean(newBiasMean); }
	double getPoolBiasVariance(int poolNumber) const { return poolsStructure[poolNumber-1]->getBiasVariance(); }
	void setPoolBiasVariance(int poolNumber, double newBiasVariance) { poolsStructure[poolNumber-1]->setBiasVariance(newBiasVariance); }
	int getPoolLayer(int poolNumber) const { return poolsStructure[poolNumber-1]->getLayer(); }
	void setPoolLayer(int poolNumber, int newLayer) { poolsStructure[poolNumber-1]->setLayer(newLayer); }

	void setPoolRootPoolID(int poolNumber, int _rootPoolID) { poolsStructure[poolNumber-1]->setRootPoolID(_rootPoolID); }
	int getPoolRootPoolID(int poolNumber) const { return poolsStructure[poolNumber-1]->getRootPoolID(); }
	void setPoolAppearenceEvolutionTime(int poolNumber, int _appearenceEvolutionTime) { poolsStructure[poolNumber-1]->setAppearenceEvolutionTime(_appearenceEvolutionTime); }
	int getPoolAppearenceEvolutionTime(int poolNumber) const { return poolsStructure[poolNumber-1]->getAppearenceEvolutionTime(); }

	int getPoolInputConnectionsQuantity(int poolNumber) const { return poolsStructure[poolNumber-1]->getInputConnectionsQuantity(); }
	int getPoolInputPredConnectionsQuantity(int poolNumber) const { return poolsStructure[poolNumber-1]->getInputPredConnectionsQuantity(); }

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
	int getConnectionPrePoolID(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionPrePoolID(connectionNumber); }
	void setConnectionPrePoolID(int poolNumber, int connectionNumber, int newPrePoolID) { poolsStructure[poolNumber-1]->setConnectionPrePoolID(connectionNumber, newPrePoolID); }
	int getConnectionPostPoolID(int poolNumber, int connectionNumber) const { return poolsStructure[poolNumber-1]->getConnectionPostPoolID(connectionNumber); }
	void setConnectionPostPoolID(int poolNumber, int connectionNumber, int newPostPoolID) { poolsStructure[poolNumber-1]->setConnectionPostPoolID(connectionNumber, newPostPoolID); }

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
	int getPredConnectionPrePoolID(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionPrePoolID(predConnectionNumber); }
	void setPredConnectionPrePoolID(int poolNumber, int predConnectionNumber, int newPrePoolID) { poolsStructure[poolNumber-1]->setPredConnectionPrePoolID(predConnectionNumber, newPrePoolID); }
	int getPredConnectionPostPoolID(int poolNumber, int predConnectionNumber) const { return poolsStructure[poolNumber-1]->getPredConnectionPostPoolID(predConnectionNumber); }
	void setPredConnectionPostPoolID(int poolNumber, int predConnectionNumber, int newPostPoolID) { poolsStructure[poolNumber-1]->setPredConnectionPostPoolID(predConnectionNumber, newPostPoolID); }

	// Нахождение номера связи в структуре постсинаптического пула - возвращает ноль, если связи нет
	int findConnectionNumber(int prePoolNumber, int postPoolNumber);

	// Нахождение номера предикторной связи в структуре постсинаптического пула - возвращает ноль, если предикторной связи нет
	int findPredConnectionNumber(int prePoolNumber, int postPoolNumber);

	// Корректировка ID связей (например после удаления)
	void fixConnectionsIDs();
	// Корректировка ID предикторных связей (например после удаления)
	void fixPredConnectionsIDs();

	//Добавление нейронального пула в сеть
	void addPool(int newType, int newLayer, double newBiasMean, double newBiasVariance = 0, int newCapacity = 1);

	//Добавление связи в сеть
	void addConnection(int prePoolNumber, int postPoolNumber, double newWeightMean, double newWeightVariance = 0, double newDevelopSynapseProb = 1, bool newEnabled = true, int newDisabledStep = 0, long newInnovationNumber = 0){
		poolsStructure[postPoolNumber-1]->addConnection(++connectionsQuantity, prePoolNumber, newWeightMean, newWeightVariance, newDevelopSynapseProb, newEnabled, newDisabledStep, newInnovationNumber); 
	}

	// Добавление предикторной связи в сеть
	void addPredConnection(int prePoolNumber, int postPoolNumber, double newDevelopPredConnectionProb = 1, bool newEnabled = true, int newDisabledStep = 0, long newInnovationNumber = 0){
		poolsStructure[postPoolNumber-1]->addPredConnection(++predConnectionsQuantity, prePoolNumber, newDevelopPredConnectionProb, newEnabled, newDisabledStep, newInnovationNumber); 
	}

	// Удаление пула из сети (с удалением также всех входных и выходных связей из этого пула)
	void deletePool(int poolNumber);

	//Удаление связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
	void deleteConnection(int poolNumber, int connectionNumber, bool fixIDs = true); 

	// Удаление предикторной связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
	void deletePredConnection(int poolNumber, int predConnectionNumber, bool fixIDs = true);

	// Стирание сети
	void erasePoolNetwork();

	// Оператор присваивания (фактически полное копирование сети - создание новых структур)
	TPoolNetwork& operator=(const TPoolNetwork& sourcePoolNetwork);

	// Вывод сети в файл как графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
	// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
	void printGraphNetwork(std::string graphFilename,  bool printWeights = false) const;

	// Вывод сети в файл как супер-графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
	// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
	void printGraphNetworkAlternative(std::string graphFilename, int scale, int genealogy) const;

	//Печать сети в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TPoolNetwork& PoolNetwork);

	//Печать сети со всеми сведений о пулах в файл или на экран (вместе с номером пула родителя и временем появления в эволюции)
	std::ostream& printNetworkExtra(std::ostream& os);

	//Считывание сети из файла или экрана
	friend std::istream& operator>>(std::istream& is, TPoolNetwork& PoolNetwork);

	//Считывание сети со всеми сведениями о пулах из файла или на экрана (вместе с номером пула родителя и временем появления в эволюции)
	std::istream& readNetworkExtra(std::istream& oi);

	//friend class tests;
};

#endif // TPOOLNETWORK_H
