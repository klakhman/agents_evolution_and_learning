#ifndef TNEURALNETWORK_H
#define TNEURALNETWORK_H

#include "TNeuron.h"

#include <iostream>
#include <fstream>

/*
Класс нейронной сети
*/
class TNeuralNetwork{
	TNeuron** neuronsStructure; // Список из нейронов 
	int neuronsStructureSize; // Текущий размер массива нейронв
	int neuronsQuantity; // Количество нейронов в сети

	int synapsesQuantity; // Количество синапсов в сети
	int predConnectionsQuantity; // Количество предикторных связей в сети

	int layersQuantity; // Количество слоев в сети (!!!! не уменьшается при удалении нейронов !!!)
	int inputResolution; // Размер входного пр-ва сети
	int outputResolution; // Размер выходного пр-ва сети

	static const int INFLATE_NEURONS_SIZE = 10; // Размер увеличения массива с нейронами в случае переполнения

	// Корректировка ID нейронов (например после удаления)
	void fixNeuronsIDs();
	
	// Процедура увеличения размера массива нейронов
	void inflateNeuronsStructure(int inflateSize);

public:
	// Конструктор по умолчанию
	TNeuralNetwork(){
		neuronsStructure = 0;
		neuronsStructureSize = 0;
		neuronsQuantity = 0;
		synapsesQuantity = 0;
		predConnectionsQuantity = 0;
		layersQuantity = 0;
		inputResolution = 0;
		outputResolution = 0;
	}
	// Деструктор
	~TNeuralNetwork();

	//Геттеры для параметров сети
	int getNeuronsQuantity() { return neuronsQuantity; }
	int getSynapsesQuantity() { return synapsesQuantity; }
	int getPredConnectionsQuantity() { return predConnectionsQuantity; }
	int getInputResolution() { return inputResolution; }
	int getOutputResolution() { return outputResolution; }

	// Геттеры и сеттеры для всех нейронов сети (во всех случаях передается номер нейрона в массиве нейронов)
	int getNeuronID(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getID(); }
	void setNeuronID(int neuronNumber, int newID){ neuronsStructure[neuronNumber-1]->setID(newID); }
	int getNeuronType(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getType(); }
	void setNeuronType(int neuronNumber, int newType) { neuronsStructure[neuronNumber-1]->setType(newType); }
	double getNeuronBias(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getBias(); }
	void setNeuronBias(int neuronNumber, double newBias) { neuronsStructure[neuronNumber-1]->setBias(newBias); }
	int getNeuronLayer(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getLayer(); }
	void setNeuronLayer(int neuronNumber, int newLayer) { neuronsStructure[neuronNumber-1]->setLayer(newLayer); }
	bool getNeuronActive(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getActive(); }
	void setNeuronActive(int neuronNumber, bool newActive) { neuronsStructure[neuronNumber - 1]->setActive(newActive); }
	int getNeuronParentPoolID(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getParentPoolID(); }
	void setNeuronParentPoolID(int neuronNumber, int newParentPoolID) { neuronsStructure[neuronNumber-1]->setParentPoolID(newParentPoolID); }

	double getNeuronPotential(int neuronNumber) const { return neuronsStructure[neuronNumber - 1]->getCurrentPotential(); }
	double getNeuronCurrentOut(int neuronNumber) const { return neuronsStructure[neuronNumber - 1]->getCurrentOut(); }
	double getNeuronPreviousOut(int neuronNumber) const { return neuronsStructure[neuronNumber - 1]->getPreviousOut(); }

	int getNeuronInputSynapsesQuantity(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getInputSynapsesQuantity(); }
	int getNeuronInputPredConnectionsQuantity(int neuronNumber) const { return neuronsStructure[neuronNumber-1]->getInputPredConnectionsQuantity(); }

	// Геттеры и сеттеры для синапсов данной сети (во всех случаях передается номер нейрона в массиве нейронов и номер синапса в массиве синапсов)
	int getSynapseID(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapseID(synapseNumber); }
	void setSynapseID(int neuronNumber, int synapseNumber, int newID) { neuronsStructure[neuronNumber-1]->setSynapseID(synapseNumber, newID); }
	double getSynapseWeight(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapseWeight(synapseNumber); }
	void setSynapseWeight(int neuronNumber, int synapseNumber, double newWeight) { neuronsStructure[neuronNumber-1]->setSynapseWeight(synapseNumber, newWeight); }
	bool getSynapseEnabled(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapseEnabled(synapseNumber); }
	void setSynapseEnabled(int neuronNumber, int synapseNumber, bool newEnabled) { neuronsStructure[neuronNumber-1]->setSynapseEnabled(synapseNumber, newEnabled); }
	int getSynapsePreNeuronID(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapsePreNeuron(synapseNumber)->getID(); }
	void setSynapsePreNeuronID(int neuronNumber, int synapseNumber, int newPreNeuronID) { neuronsStructure[neuronNumber-1]->setSynapsePreNeuron(synapseNumber, neuronsStructure[newPreNeuronID-1]); }
	int getSynapsePostNeuronID(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapsePostNeuron(synapseNumber)->getID(); }
	void setSynapsePostNeuronID(int neuronNumber, int synapseNumber, int newPostNeuronID) { neuronsStructure[neuronNumber-1]->setSynapsePostNeuron(synapseNumber, neuronsStructure[newPostNeuronID-1]); }

	// Геттеры и сеттеры для предикторных связей данной сети (во всех случаях передается номер нейрона в массиве нейронов и номер связи в массиве связей)
	int getPredConnectionID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionID(predConnectionNumber); }
	void setPredConnectionID(int neuronNumber, int predConnectionNumber, int newID) { neuronsStructure[neuronNumber-1]->setPredConnectionID(predConnectionNumber, newID); }
	bool getPredConnectionEnabled(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionEnabled(predConnectionNumber); }
	void setPredConnectionEnabled(int neuronNumber, int predConnectionNumber, bool newEnabled) { neuronsStructure[neuronNumber-1]->setPredConnectionEnabled(predConnectionNumber, newEnabled); }
	int getPredConnectionPreNeuronID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionPreNeuron(predConnectionNumber)->getID(); }
	void setPredConnectionPreNeuronID(int neuronNumber, int predConnectionNumber, int newPreNeuronID) { neuronsStructure[neuronNumber-1]->setPredConnectionPreNeuron(predConnectionNumber, neuronsStructure[newPreNeuronID-1]); }
	int getPredConnectionPostNeuronID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionPostNeuron(predConnectionNumber)->getID(); }
	void setPredConnectionPostNeuronID(int neuronNumber, int predConnectionNumber, int newPostNeuronID) { neuronsStructure[neuronNumber-1]->setPredConnectionPostNeuron(predConnectionNumber, neuronsStructure[newPostNeuronID-1]); }

	// Нахождение номера связи в структуре постсинаптического нейрона - возвращает ноль, если связи нет
	int findSynapseNumber(int preNeuronNumber, int postNeuronNumber);

	// Нахождение номера предикторной связи в структуре постсинаптического нейрона - возвращает ноль, если предикторной связи нет
	int findPredConnectionNumber(int preNeuronNumber, int postNeuronNumber);

	// Корректировка ID связей (например после удаления)
	void fixSynapsesIDs();
	// Корректировка ID предикторных связей (например после удаления)
	void fixPredConnectionsIDs();

	//Добавление нейрона в сеть
	void addNeuron(int newType, int newLayer, double newBias, bool newActive = true, int newParentNeuronID = 0);

	//Добавление синапса в сеть
	void addSynapse(int preNeuronNumber, int postNeuronNumber, double newWeight, bool newEnabled = true){
		neuronsStructure[postNeuronNumber-1]->addSynapse(++synapsesQuantity, neuronsStructure[preNeuronNumber-1], newWeight, newEnabled); 
	}

	// Добавление предикторной связи в сеть
	void addPredConnection(int preNeuronNumber, int postNeuronNumber, bool newEnabled = true){
		neuronsStructure[postNeuronNumber-1]->addPredConnection(++predConnectionsQuantity, neuronsStructure[preNeuronNumber-1], newEnabled); 
	}

	// Удаление нейрона из сети (с удалением также всех входных и выходных связей из этого нейрона)
	void deleteNeuron(int neuronNumber);

	//Удаление связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
	void deleteSynapse(int neuronNumber, int synapseNumber, bool fixIDs = true);

	// Удаление предикторной связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
	void deletePredConnection(int neuronNumber, int predConnectionNumber, bool fixIDs = true);

	// Стирание сети
	void eraseNeuralNetwork();

	// "Перезагрузка" сети
	void reset();

	// Обсчет одного такта работы сети
	void calculateNetwork(double inputVector[]);

	// Обсчет одного такта работы сети, в условиях спонтанной активации нейронов
	void calculateSpontaneousNetwork(double spontaneousActivityProb);

	// Получение текущего выходного вектора сети
	void getOutputVector(double outputVector[]);

	// Вывод сети в файл как графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
	// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH	
	void printGraphNetwork(std::string graphFilename, bool printWeights = false) const;

	//Печать сети в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TNeuralNetwork& NeuralNetwork);

	//Считывание сети из файла или экрана
	friend std::istream& operator>>(std::istream& is, TNeuralNetwork& NeuralNetwork);
};

#endif // TNEURALNETWORK_H
