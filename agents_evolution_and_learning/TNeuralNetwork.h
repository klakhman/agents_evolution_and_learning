#ifndef TNEURALNETWORK_H
#define TNEURALNETWORK_H

#include "TNeuron.h"

#include <iostream>
#include <fstream>
#include <vector>

/*
Класс нейронной сети
*/
class TNeuralNetwork{
	std::vector<TNeuron> neuronsStructure; // Список из нейронов 

	int synapsesQuantity; // Количество синапсов в сети
	int predConnectionsQuantity; // Количество предикторных связей в сети

	int layersQuantity; // Количество слоев в сети (!!!! не уменьшается при удалении нейронов !!!)
	int inputResolution; // Размер входного пр-ва сети
	int outputResolution; // Размер выходного пр-ва сети

	// Корректировка ID нейронов (например после удаления)
	void fixNeuronsIDs();

public:
  static const int INPUT_NEURON = 0;
  static const int HIDDEN_NEURON = 1;
  static const int OUTPUT_NEURON = 2;

	// Конструктор по умолчанию
	TNeuralNetwork(){
		synapsesQuantity = 0;
		predConnectionsQuantity = 0;
		layersQuantity = 0;
		inputResolution = 0;
		outputResolution = 0;
	}
  // Копирующий конструктор (применять только для правильного присваивания с одновременным созданием переменной)
  TNeuralNetwork(const TNeuralNetwork& sourceNetwork);
  // Оператор присваивания (фактически полное копирование сети - создание новых структур)
	TNeuralNetwork& operator=(const TNeuralNetwork& sourceNetwork);
	// Деструктор
  ~TNeuralNetwork() {};

	//Геттеры для параметров сети
	int getNeuronsQuantity() const{ return neuronsStructure.size(); }
	int getSynapsesQuantity() const{ return synapsesQuantity; }
	int getPredConnectionsQuantity() const{ return predConnectionsQuantity; }
	int getInputResolution() const{ return inputResolution; }
	int getOutputResolution() const{ return outputResolution; }

	// Геттеры и сеттеры для всех нейронов сети (во всех случаях передается номер нейрона в массиве нейронов)
	int getNeuronID(int neuronNumber) const { return neuronsStructure[neuronNumber-1].ID; }
	void setNeuronID(int neuronNumber, int newID){ neuronsStructure[neuronNumber-1].ID = newID; }
  int getNeuronType(int neuronNumber) const { return neuronsStructure[neuronNumber-1].type; }
	void setNeuronType(int neuronNumber, int newType) { neuronsStructure[neuronNumber-1].type = newType; }
	double getNeuronBias(int neuronNumber) const { return neuronsStructure[neuronNumber-1].bias; }
	void setNeuronBias(int neuronNumber, double newBias) { neuronsStructure[neuronNumber-1].bias = newBias; }
	int getNeuronLayer(int neuronNumber) const { return neuronsStructure[neuronNumber-1].layer; }
	void setNeuronLayer(int neuronNumber, int newLayer) { neuronsStructure[neuronNumber-1].layer = newLayer; }
	bool getNeuronActive(int neuronNumber) const { return neuronsStructure[neuronNumber-1].active; }
	void setNeuronActive(int neuronNumber, bool newActive) { neuronsStructure[neuronNumber - 1].active = newActive; }
	int getNeuronParentPoolID(int neuronNumber) const { return neuronsStructure[neuronNumber-1].parentPoolID; }
	void setNeuronParentPoolID(int neuronNumber, int newParentPoolID) { neuronsStructure[neuronNumber-1].parentPoolID = newParentPoolID; }

	double getNeuronPotential(int neuronNumber) const { return neuronsStructure[neuronNumber - 1].potential; }
  double getNeuronCurrentOut(int neuronNumber) const { return neuronsStructure[neuronNumber - 1].currentOut; }
  double getNeuronPreviousOut(int neuronNumber) const { return neuronsStructure[neuronNumber - 1].previousOut; }

  int getNeuronInputSynapsesQuantity(int neuronNumber) const { return neuronsStructure[neuronNumber-1].inputSynapsesSet.size(); }
  int getNeuronInputPredConnectionsQuantity(int neuronNumber) const { return neuronsStructure[neuronNumber-1].inputPredConnectionsSet.size(); }

	// Геттеры и сеттеры для синапсов данной сети (во всех случаях передается номер нейрона в массиве нейронов и номер синапса в массиве синапсов)
  int getSynapseID(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].ID; }
	void setSynapseID(int neuronNumber, int synapseNumber, int newID) { neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].ID = newID; }
	double getSynapseWeight(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].weight; }
	void setSynapseWeight(int neuronNumber, int synapseNumber, double newWeight) { neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].weight = newWeight; }
  bool getSynapseEnabled(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].enabled; }
	void setSynapseEnabled(int neuronNumber, int synapseNumber, bool newEnabled) { neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].enabled = newEnabled; }
  int getSynapsePreNeuronID(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].preNeuron->ID; }
	void setSynapsePreNeuronID(int neuronNumber, int synapseNumber, int newPreNeuronID) { neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].preNeuron = &(neuronsStructure[newPreNeuronID-1]); }
	int getSynapsePostNeuronID(int neuronNumber, int synapseNumber) const { return neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].postNeuron->ID; }
	void setSynapsePostNeuronID(int neuronNumber, int synapseNumber, int newPostNeuronID) { neuronsStructure[neuronNumber-1].inputSynapsesSet[synapseNumber-1].postNeuron = &(neuronsStructure[newPostNeuronID-1]); }

	// Геттеры и сеттеры для предикторных связей данной сети (во всех случаях передается номер нейрона в массиве нейронов и номер связи в массиве связей)
  int getPredConnectionID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].ID; }
	void setPredConnectionID(int neuronNumber, int predConnectionNumber, int newID) { neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].ID = newID; }
	bool getPredConnectionEnabled(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].enabled; }
  void setPredConnectionEnabled(int neuronNumber, int predConnectionNumber, bool newEnabled) { neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].enabled = newEnabled; }
	int getPredConnectionPreNeuronID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].preNeuron->ID; }
	void setPredConnectionPreNeuronID(int neuronNumber, int predConnectionNumber, int newPreNeuronID) { neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].preNeuron = &(neuronsStructure[newPreNeuronID-1]); }
  int getPredConnectionPostNeuronID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].postNeuron->ID; }
  void setPredConnectionPostNeuronID(int neuronNumber, int predConnectionNumber, int newPostNeuronID) { neuronsStructure[neuronNumber-1].inputPredConnectionsSet[predConnectionNumber-1].postNeuron = &(neuronsStructure[newPostNeuronID-1]); }

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
		neuronsStructure[postNeuronNumber-1].addSynapse(++synapsesQuantity, &(neuronsStructure[preNeuronNumber-1]), newWeight, newEnabled); 
	}

	// Добавление предикторной связи в сеть
	void addPredConnection(int preNeuronNumber, int postNeuronNumber, bool newEnabled = true){
		neuronsStructure[postNeuronNumber-1].addPredConnection(++predConnectionsQuantity, &(neuronsStructure[preNeuronNumber-1]), newEnabled); 
	}

	// Удаление нейрона из сети (с удалением также всех входных и выходных связей из этого нейрона)
	void deleteNeuron(unsigned int neuronNumber);

	//Удаление связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
	void deleteSynapse(unsigned int neuronNumber,unsigned int synapseNumber, bool fixIDs = true);

	// Удаление предикторной связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
	void deletePredConnection(unsigned int neuronNumber,unsigned int predConnectionNumber, bool fixIDs = true);

	// Стирание сети
	void eraseNeuralNetwork();

	// "Перезагрузка" сети
	void reset(){
	  for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
	  	neuronsStructure[currentNeuron - 1].reset();
  }

	// Обсчет одного такта работы сети
	void calculateNetwork(double inputVector[]);

	// Обсчет одного такта работы сети, в условиях спонтанной активации нейронов
	void calculateSpontaneousNetwork(double spontaneousActivityProb);

	// Получение текущего выходного вектора сети
	void getOutputVector(double outputVector[]);

	// Вывод сети в файл как графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
	// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH	
	void printGraphNetwork(std::string graphFilename, bool printWeights = false, bool printSilentNeurons = false, bool printActivities = false) const;

	//Печать сети в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TNeuralNetwork& NeuralNetwork);

	//Считывание сети из файла или экрана
	friend std::istream& operator>>(std::istream& is, TNeuralNetwork& NeuralNetwork);
};

#endif // TNEURALNETWORK_H
