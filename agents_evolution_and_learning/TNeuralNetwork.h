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
	int getNeuronID(int neuronNumber){ return neuronsStructure[neuronNumber-1]->getID(); }
	void setNeuronID(int neuronNumber, int newID){ neuronsStructure[neuronNumber-1]->setID(newID); }
	int getNeuronType(int neuronNumber) { return neuronsStructure[neuronNumber-1]->getType(); }
	void setNeuronType(int neuronNumber, int newType) { neuronsStructure[neuronNumber-1]->setType(newType); }
	double getNeuronBias(int neuronNumber) { return neuronsStructure[neuronNumber-1]->getBias(); }
	void setNeuronBias(int neuronNumber, double newBias) { neuronsStructure[neuronNumber-1]->setBias(newBias); }
	int getNeuronLayer(int neuronNumber) { return neuronsStructure[neuronNumber-1]->getLayer(); }
	void setNeuronLayer(int neuronNumber, int newLayer) { neuronsStructure[neuronNumber-1]->setLayer(newLayer); }

	int getNeuronInputSynapsesQuantity(int neuronNumber){ return neuronsStructure[neuronNumber-1]->getInputSynapsesQuantity(); }
	int getNeuronInputPredConnectionsQuantity(int neuronNumber){ return neuronsStructure[neuronNumber-1]->getInputPredConnectionsQuantity(); }

	// Геттеры и сеттеры для синапсов данной сети (во всех случаях передается номер нейрона в массиве нейронов и номер синапса в массиве синапсов)
	int getSynapseID(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapseID(synapseNumber); }
	void setSynapseID(int neuronNumber, int synapseNumber, int newID) { neuronsStructure[neuronNumber-1]->setSynapseID(synapseNumber, newID); }
	double getSynapseWeight(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapseWeight(synapseNumber); }
	void setSynapseWeight(int neuronNumber, int synapseNumber, double newWeight) { neuronsStructure[neuronNumber-1]->setSynapseWeight(synapseNumber, newWeight); }
	bool getSynapseEnabled(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapseEnabled(synapseNumber); }
	void setSynapseEnabled(int neuronNumber, int synapseNumber, bool newEnabled) { neuronsStructure[neuronNumber-1]->setSynapseEnabled(synapseNumber, newEnabled); }
	TNeuron* getSynapsePreNeuron(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapsePreNeuron(synapseNumber); }
	void setSynapsePreNeuron(int neuronNumber, int synapseNumber, TNeuron* newPreNeuron) { neuronsStructure[neuronNumber-1]->setSynapsePreNeuron(synapseNumber, newPreNeuron); }
	TNeuron* getSynapsePostNeuron(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1]->getSynapsePostNeuron(synapseNumber); }
	void setSynapsePostNeuron(int neuronNumber, int synapseNumber, TNeuron* newPostNeuron) { neuronsStructure[neuronNumber-1]->setSynapsePostNeuron(synapseNumber, newPostNeuron); }

	// Геттеры и сеттеры для предикторных связей данной сети (во всех случаях передается номер нейрона в массиве нейронов и номер связи в массиве связей)
	int getPredConnectionID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionID(predConnectionNumber); }
	void setPredConnectionID(int neuronNumber, int predConnectionNumber, int newID) { neuronsStructure[neuronNumber-1]->setPredConnectionID(predConnectionNumber, newID); }
	bool getPredConnectionEnabled(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionEnabled(predConnectionNumber); }
	void setPredConnectionEnabled(int neuronNumber, int predConnectionNumber, bool newEnabled) { neuronsStructure[neuronNumber-1]->setPredConnectionEnabled(predConnectionNumber, newEnabled); }
	TNeuron* getPredConnectionPreNeuron(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionPreNeuron(predConnectionNumber); }
	void setPredConnectionPreNeuron(int neuronNumber, int predConnectionNumber, TNeuron* newPreNeuron) { neuronsStructure[neuronNumber-1]->setPredConnectionPreNeuron(predConnectionNumber, newPreNeuron); }
	TNeuron* getPredConnectionPostNeuron(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionPostNeuron(predConnectionNumber); }
	void setPredConnectionPostNeuron(int neuronNumber, int predConnectionNumber, TNeuron* newPostNeuron) { neuronsStructure[neuronNumber-1]->setPredConnectionPostNeuron(predConnectionNumber, newPostNeuron); }

	//Добавление нейрона в сеть
	void addNeuron(int newID, int newType, double newBias, int newLayer, bool newActive = true, int newParentNeuronID = 0);

	//Добавление синапса в сеть
	void addSynapse(int preNeuronNumber, int postNeuronNumber, int newID, double newWeight, bool newEnabled){
		neuronsStructure[postNeuronNumber-1]->addSynapse(newID, newWeight, newEnabled, neuronsStructure[preNeuronNumber-1]); 
		++synapsesQuantity;
	}

	// Добавление предикторной связи в сеть
	void addPredConnection(int preNeuronNumber, int postNeuronNumber, int newID, bool newEnabled){
		neuronsStructure[postNeuronNumber-1]->addPredConnection(newID, newEnabled, neuronsStructure[preNeuronNumber-1]); 
		++predConnectionsQuantity;
	}

	// Удаление нейрона из сети (с удалением также всех входных и выходных связей из этого нейрона)
	void deleteNeuron(int neuronNumber);

	//Удаление связи из сети
	void deleteSynapse(int neuronNumber, int synapseNumber){
		neuronsStructure[neuronNumber-1]->deleteSynapse(synapseNumber);
		--synapsesQuantity;
	}

	// Удаление предикторной связи из сети
	void deletePredConnection(int neuronNumber, int predConnectionNumber){
		neuronsStructure[neuronNumber-1]->deletePredConnection(predConnectionNumber);
		--predConnectionsQuantity;
	}

	// Стирание сети
	void eraseNeuralNetwork();

	// "Перезагрузка" сети
	void reset();

	// Обсчет одного такта работы сети
	void calculateNetwork(double inputVector[]);

	// Получение текущего выходного вектора сети
	void getOutputVector(double outputVector[]);

	//Печать сети в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TNeuralNetwork& NeuralNetwork);

	//Считывание сети из файла или экрана
	friend std::istream& operator>>(std::istream& is, TNeuralNetwork& NeuralNetwork);
};

#endif // TNEURALNETWORK_H
