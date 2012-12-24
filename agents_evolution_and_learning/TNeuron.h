#ifndef TNEURON_H
#define TNEURON_H

#include <iostream>
#include <vector>
#include <fstream>
#include "TSynapse.h"
#include "TPredConnection.h"

class TNeuralNetwork;

/* 
Класс нейрона нейронной сети 
*/
class TNeuron{
	int ID; // ID нейрона
	int type; // Тип нейрона (0 - сенсорный, 1 - скрытый, 2 - выходной)
	double bias; // Смещение нейрона
	int layer; // Номер слоя, в котором находится нейрон
	bool active; // Признак активности нейрона (0 - молчащий нейрон, 1 - активный нейрон)

	int parentPoolID; // Номер родительского пула

	std::vector<TSynapse> inputSynapsesSet; // Массив входных синапсов нейрона

	std::vector<TPredConnection> inputPredConnectionsSet; // Массив входных предикторных связей нейрона

	double currentOut; // Текущий выход нейрона
	double potential; // Текущий потенциал нейрона
	double previousOut; // Предыдущий выход нейрона

	// Установка значения выхода нейрона (только для дружественного TNeuralNetwork для заполнения нейронов среды)
	void setCurrentOut(double newCurrentOut) { currentOut = newCurrentOut; }

	static const double EMPTY_OUT; // Признак неозначенного выхода нейрона

public:
	static const double ACTIVITY_TRESHOLD; // Порог активности, при котором сигнал передается по выходным связям
  static const int INPUT_NEURON = 0;
  static const int HIDDEN_NEURON = 1;
  static const int OUTPUT_NEURON = 2;

	// Конструктор по умолчанию
	TNeuron(){
		ID = 0;
		type = INPUT_NEURON;
		bias = 0;
		layer = 0;
		active = false;
		parentPoolID = 0;

		currentOut = 0;
		potential = 0;
		previousOut = 0;
	}
	// Полный конструктор
	TNeuron(int newID, int newType, int newLayer, double newBias, bool newActive = true, int newParentPoolID = 0){
		setAll(newID, newType, newLayer, newBias, newActive, newParentPoolID);

		currentOut = 0;
		potential = 0;
		previousOut = 0;
	}
	// Деструктор
  ~TNeuron() {}

	// Геттеры и сеттеры для всех внутренних переменных
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	int getType() const { return type; }
	void setType(int newType) { type = newType; }
	double getBias() const { return bias; }
	void setBias(double newBias) { bias = newBias; }
	int getLayer() const { return layer; }
	void setLayer(int newLayer) { layer = newLayer; }
	bool getActive() const { return active; }
	void setActive(bool newActive) { active = newActive; }
	int getParentPoolID() const { return parentPoolID; }
	void setParentPoolID(int newParentPoolID) { parentPoolID = newParentPoolID; }
	// Получение количества входных синапсов нейрона
	int getInputSynapsesQuantity() const { return inputSynapsesSet.size(); }
	// Получение количества входных предикторных связей нейрона
	int getInputPredConnectionsQuantity() const { return inputPredConnectionsSet.size(); }
	// Заполнение всех характеристик нейрона
	void setAll(int newID, int newType, int newLayer, double newBias, bool newActive = true, int newParentPoolID = 0){
		ID = newID;
		type = newType;
		bias = newBias;
		layer = newLayer;
		active = newActive;
		parentPoolID = newParentPoolID;
	}

	// Получение динамических характеристик нейрона
	double getCurrentOut() { return currentOut; }
	double getCurrentPotential() { return potential; }
	double getPreviousOut() { return previousOut; }

	// Геттеры и сеттеры для синапсов данного пула (во всех случаях передается номер синапса в массиве синапсов)
	int getSynapseID(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1].getID(); }
	void setSynapseID(int synapseNumber, int newID) { inputSynapsesSet[synapseNumber-1].setID(newID); }
	double getSynapseWeight(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1].getWeight(); }
	void setSynapseWeight(int synapseNumber, double newWeight) { inputSynapsesSet[synapseNumber-1].setWeight(newWeight); }
	bool getSynapseEnabled(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1].getEnabled(); }
	void setSynapseEnabled(int synapseNumber, bool newEnabled) { inputSynapsesSet[synapseNumber-1].setEnabled(newEnabled); }
	TNeuron* getSynapsePreNeuron(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1].getPreNeuron(); }
	void setSynapsePreNeuron(int synapseNumber, TNeuron* newPreNeuron) { inputSynapsesSet[synapseNumber-1].setPreNeuron(newPreNeuron); }
	TNeuron* getSynapsePostNeuron(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1].getPostNeuron(); }
	void setSynapsePostNeuron(int synapseNumber, TNeuron* newPostNeuron) { inputSynapsesSet[synapseNumber-1].setPostNeuron(newPostNeuron); }

	// Геттеры и сеттеры для предикторных связей данного нейрона (во всех случаях передается номер связи в массиве связей)
	int getPredConnectionID(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1].getID(); }
	void setPredConnectionID(int predConnectionNumber, int newID) { inputPredConnectionsSet[predConnectionNumber-1].setID(newID); }
	bool getPredConnectionEnabled(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1].getEnabled(); }
	void setPredConnectionEnabled(int predConnectionNumber, bool newEnabled) { inputPredConnectionsSet[predConnectionNumber-1].setEnabled(newEnabled); }
	TNeuron* getPredConnectionPreNeuron(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1].getPreNeuron(); }
	void setPredConnectionPreNeuron(int predConnectionNumber, TNeuron* newPreNeuron) { inputPredConnectionsSet[predConnectionNumber-1].setPreNeuron(newPreNeuron); }
	TNeuron* getPredConnectionPostNeuron(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1].getPostNeuron(); }
	void setPredConnectionPostNeuron(int predConnectionNumber, TNeuron* newPostNeuron) { inputPredConnectionsSet[predConnectionNumber-1].setPostNeuron(newPostNeuron); }

  // Добавление входного синапса в нейрон
  void addSynapse(int newID, TNeuron* newPreNeuron, double newWeight, bool newEnabled = true){
    TSynapse newSynapse(newID, newPreNeuron, this, newWeight, newEnabled);
    inputSynapsesSet.push_back(newSynapse);
  }

  // Удаление синапса из нейрона
  void deleteSynapse(int synapseNumber){
    inputSynapsesSet.erase(inputSynapsesSet.begin() + synapseNumber - 1);
  }

	// Добавление входной предикторной связи в нейрон
	void addPredConnection(int newID, TNeuron* newPreNeuron, bool newEnabled = true){
    TPredConnection newPredConnection(newID, newPreNeuron, this, newEnabled);
    inputPredConnectionsSet.push_back(newPredConnection);
  }
	// Удаление предикторной связи из нейрона
	void deletePredConnection(int predConnectionNumber){
    inputPredConnectionsSet.erase(inputPredConnectionsSet.begin() + predConnectionNumber - 1);
  }

	// "Перезагрузка нейрона"
	void reset(){
		currentOut = 0;
		potential = 0;
		previousOut = 0;
	}

	// Подготовка нейрона к обсчету на следующем такте времени
	void prepare(){
		previousOut = currentOut;
		currentOut = EMPTY_OUT;
		potential = 0;
	}

	// Рассчет выхода нейрона
	void calculateOut();

	// Печать сведений о нейроне в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TNeuron& neuron);

	//Печать сети в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TNeuralNetwork& neuralNetwork); // Функция вывода сети объявлена дружественной, чтобы она имела прямой доступ к списку связей нейрона

	friend class TNeuralNetwork;
};

#endif // TNEURON_H