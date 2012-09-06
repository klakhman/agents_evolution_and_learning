#ifndef TNEURON_H
#define TNEURON_H

#include <iostream>
#include <fstream>

class TNeuralNetwork;
class TSynapse;
class TPredConnection;

/* 
Класс нейрона нейронной сети 
*/
class TNeuron{
	int ID; // ID нейрона
	int type; // Тип нейрона (0 - сенсорный, 1 - скрытый, 2 - выходной)
	double bias; // Смещение нейрона
	int layer; // Номер слоя, в котором находится нейрон
	bool active; // Признак активности нейрона (0 - молчащий нейрон, 1 - активный нейрон)

	//TNeuralPool* ParentPool; // Ссылка на пул из которого образовался нейрон (не всегда есть)
	int parentPoolID; // Номер родительского пула

	TSynapse** inputSynapsesSet; // Массив входных синапсов нейрона
	int inputSynapsesSetSize; // Текущий размер массива входных синапсов
	int inputSynapsesQuantity; // Количество входных синапсов нейрона

	TPredConnection** inputPredConnectionsSet; // Массив входных предикторных связей нейрона
	int inputPredConnectionsSetSize; // Текущий размер массива входных предикторных связей
	int inputPredConnectionsQuantity; // Количество входных предикторных связей нейрона

	double currentOut; // Текущий выход нейрона
	double potential; // Текущий потенциал нейрона
	double previousOut; // Предыдущий выход нейрона

	// Установка значения выхода нейрона (только для дружественного TNeuralNetwork для заполнения нейронов среды)
	void setCurrentOut(double newCurrentOut) { currentOut = newCurrentOut; }

	// Процедура увеличения размера массива входных синапсов
	void inflateSynapsesSet(int inflateSize);
	// Процедура увеличения размера массива входных предикторных связей
	void inflatePredConnectionsSet(int inflateSize);

	static const double EMPTY_OUT; // Признак неозначенного выхода нейрона
	static const double ACTIVITY_TRESHOLD; // Порог активности, при котором сигнал передается по выходным связям
	static const int INFLATE_SYNAPSES_SIZE = 10; // Размер увеличения массива с входным синапсами в случае переполнения
	static const int INFLATE_PRED_CONNECTIONS_SIZE = 10; // Размер увеличения массива с входным предикторными связями в случае переполнения

public:
	// Конструктор по умолчанию
	TNeuron(){
		ID = 0;
		type = 0;
		bias = 0;
		layer = 0;
		active = false;
		parentPoolID = 0;
		inputSynapsesSet = 0;
		inputSynapsesSetSize = 0;
		inputSynapsesQuantity = 0;
		inputPredConnectionsSet = 0;
		inputPredConnectionsSetSize = 0;
		inputPredConnectionsQuantity = 0;

		currentOut = 0;
		potential = 0;
		previousOut = 0;
	}
	// Полный конструктор
	TNeuron(int newID, int newType, double newBias, int newLayer, bool newActive = true, int newParentPoolID = 0){
		setAll(newID, newType, newBias, newLayer, newActive, newParentPoolID);
		inputSynapsesSet = 0;
		inputSynapsesSetSize = 0;
		inputSynapsesQuantity = 0;
		inputPredConnectionsSet = 0;
		inputPredConnectionsSetSize = 0;
		inputPredConnectionsQuantity = 0;

		currentOut = 0;
		potential = 0;
		previousOut = 0;
	}
	// Деструктор
	~TNeuron();

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
	int getInputSynapsesQuantity() const { return inputSynapsesQuantity; }
	// Получение количества входных предикторных связей нейрона
	int getInputPredConnectionsQuantity() const { return inputPredConnectionsQuantity; }
	// Заполнение всех характеристик нейрона
	void setAll(int newID, int newType, double newBias, int newLayer, bool newActive, int newParentPoolID){
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
	int getSynapseID(int synapseNumber) const;
	void setSynapseID(int synapseNumber, int newID);
	double getSynapseWeight(int synapseNumber) const;
	void setSynapseWeight(int synapseNumber, double newWeight);
	bool getSynapseEnabled(int synapseNumber) const;
	void setSynapseEnabled(int synapseNumber, bool newEnabled);
	TNeuron* getSynapsePreNeuron(int synapseNumber) const;
	void setSynapsePreNeuron(int synapseNumber, TNeuron* newPreNeuron);
	TNeuron* getSynapsePostNeuron(int synapseNumber) const;
	void setSynapsePostNeuron(int synapseNumber, TNeuron* newPostNeuron);

	// Геттеры и сеттеры для предикторных связей данного нейрона (во всех случаях передается номер связи в массиве связей)
	int getPredConnectionID(int predConnectionNumber) const;
	void setPredConnectionID(int predConnectionNumber, int newID);
	bool getPredConnectionEnabled(int predConnectionNumber) const;
	void setPredConnectionEnabled(int predConnectionNumber, bool newEnabled);
	TNeuron* getPredConnectionPreNeuron(int predConnectionNumber) const;
	void setPredConnectionPreNeuron(int predConnectionNumber, TNeuron* newPreNeuron);
	TNeuron* getPredConnectionPostNeuron(int predConnectionNumber) const;
	void setPredConnectionPostNeuron(int predConnectionNumber, TNeuron* newPostNeuron);

	// Добавление входного синапса в нейрон
	void addSynapse(int newID, double newWeight, bool newEnabled = true, TNeuron* newPreNeuron = 0);
	// Удаление синапса из нейрона
	void deleteSynapse(int synapseNumber);

	// Добавление входной предикторной связи в нейрон
	void addPredConnection(int newID, bool newEnabled = true, TNeuron* newPreNeuron = 0);
	// Удаление предикторной связи из нейрона
	void deletePredConnection(int predConnectionNumber);

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