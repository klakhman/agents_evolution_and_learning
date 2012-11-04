#include "TNeuron.h"
#include "TSynapse.h"
#include "TPredConnection.h"
#include "service.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

const double TNeuron::EMPTY_OUT = -10.0; // Признак неозначенного выхода нейрона
const double TNeuron::ACTIVITY_TRESHOLD = 0.5; // Порог активности, при котором сигнал передается по выходным связям

// Процедура увеличения размера массива входных синапсов
void TNeuron::inflateSynapsesSet(int inflateSize){
	TSynapse** newInputSynapsesSet = new TSynapse*[inputSynapsesSetSize + inflateSize];
	memset(newInputSynapsesSet, 0, (inputSynapsesSetSize + inflateSize) * sizeof(TSynapse*));
	memcpy(newInputSynapsesSet, inputSynapsesSet, inputSynapsesSetSize * sizeof(TSynapse*));
	delete []inputSynapsesSet;
	inputSynapsesSet = newInputSynapsesSet;
	inputSynapsesSetSize += inflateSize;
}

// Процедура увеличения размера массива входных предикторных связей
void TNeuron::inflatePredConnectionsSet(int inflateSize){
	TPredConnection** newPredConnectionsSet = new TPredConnection*[inputPredConnectionsSetSize + inflateSize];
	memset(newPredConnectionsSet, 0, (inputPredConnectionsSetSize + inflateSize) * sizeof(TPredConnection*));
	memcpy(newPredConnectionsSet, inputPredConnectionsSet, inputPredConnectionsSetSize * sizeof(TPredConnection*));
	delete []inputPredConnectionsSet;
	inputPredConnectionsSet = newPredConnectionsSet;
	inputPredConnectionsSetSize += inflateSize;
}

// Деструктор
TNeuron::~TNeuron(){
	for (int currentSynapse = 1; currentSynapse <= inputSynapsesQuantity; ++currentSynapse)
		delete inputSynapsesSet[currentSynapse-1];
	delete []inputSynapsesSet;
	for (int currentPredConnection = 1; currentPredConnection <= inputPredConnectionsQuantity; ++currentPredConnection)
		delete inputPredConnectionsSet[currentPredConnection-1];
	delete []inputPredConnectionsSet;
}

	// Геттеры и сеттеры для синапсов данного пула (во всех случаях передается номер синапса в массиве синапсов)
int TNeuron::getSynapseID(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1]->getID(); }
void TNeuron::setSynapseID(int synapseNumber, int newID) { inputSynapsesSet[synapseNumber-1]->setID(newID); }
double TNeuron::getSynapseWeight(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1]->getWeight(); }
void TNeuron::setSynapseWeight(int synapseNumber, double newWeight) { inputSynapsesSet[synapseNumber-1]->setWeight(newWeight); }
bool TNeuron::getSynapseEnabled(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1]->getEnabled(); }
void TNeuron::setSynapseEnabled(int synapseNumber, bool newEnabled) { inputSynapsesSet[synapseNumber-1]->setEnabled(newEnabled); }
TNeuron* TNeuron::getSynapsePreNeuron(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1]->getPreNeuron(); }
void TNeuron::setSynapsePreNeuron(int synapseNumber, TNeuron* newPreNeuron) { inputSynapsesSet[synapseNumber-1]->setPreNeuron(newPreNeuron); }
TNeuron* TNeuron::getSynapsePostNeuron(int synapseNumber) const { return inputSynapsesSet[synapseNumber-1]->getPostNeuron(); }
void TNeuron::setSynapsePostNeuron(int synapseNumber, TNeuron* newPostNeuron) { inputSynapsesSet[synapseNumber-1]->setPostNeuron(newPostNeuron); }

// Геттеры и сеттеры для предикторных связей данного нейрона (во всех случаях передается номер связи в массиве связей)
int TNeuron::getPredConnectionID(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1]->getID(); }
void TNeuron::setPredConnectionID(int predConnectionNumber, int newID) { inputPredConnectionsSet[predConnectionNumber-1]->setID(newID); }
bool TNeuron::getPredConnectionEnabled(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1]->getEnabled(); }
void TNeuron::setPredConnectionEnabled(int predConnectionNumber, bool newEnabled) { inputPredConnectionsSet[predConnectionNumber-1]->setEnabled(newEnabled); }
TNeuron* TNeuron::getPredConnectionPreNeuron(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1]->getPreNeuron(); }
void TNeuron::setPredConnectionPreNeuron(int predConnectionNumber, TNeuron* newPreNeuron) { inputPredConnectionsSet[predConnectionNumber-1]->setPreNeuron(newPreNeuron); }
TNeuron* TNeuron::getPredConnectionPostNeuron(int predConnectionNumber) const { return inputPredConnectionsSet[predConnectionNumber-1]->getPostNeuron(); }
void TNeuron::setPredConnectionPostNeuron(int predConnectionNumber, TNeuron* newPostNeuron) { inputPredConnectionsSet[predConnectionNumber-1]->setPostNeuron(newPostNeuron); }

// Добавление входного синапса в нейрон
void TNeuron::addSynapse(int newID, double newWeight, bool newEnabled /* = true*/, TNeuron* newPreNeuron /* = 0*/){
	if (inputSynapsesQuantity >= inputSynapsesSetSize) // Если у нас не хватает объема массива
		inflateSynapsesSet(INFLATE_SYNAPSES_SIZE);
	inputSynapsesSet[inputSynapsesQuantity++] = new TSynapse(newID, newWeight, newEnabled, newPreNeuron, this);
}

// Удаление синапса из нейрона
void TNeuron::deleteSynapse(int synapseNumber){
	TSynapse* deletingSynapse = inputSynapsesSet[synapseNumber - 1];
	delete deletingSynapse;
	// Сдвигаем массив
	for (int currentSynapse = synapseNumber - 1; currentSynapse < inputSynapsesQuantity - 1; ++currentSynapse)
		inputSynapsesSet[currentSynapse] = inputSynapsesSet[currentSynapse + 1];
	//memcpy(connectednessSet[connectionNumber - 1], connectednessSet[connectionNumber], (inputConnectionsQuantity - connectionNumber) * sizeof(TPoolConnection*));
	inputSynapsesSet[inputSynapsesQuantity - 1] = 0; // Обнуляем указатель последнего синапса
	--inputSynapsesQuantity;
}

// Добавление входной предикторной связи в нейрон
void TNeuron::addPredConnection(int newID, bool newEnabled /* = true*/, TNeuron* newPreNeuron /* = 0*/){
	if (inputPredConnectionsQuantity >= inputPredConnectionsSetSize) // Если у нас не хватает объема массива
		inflatePredConnectionsSet(INFLATE_PRED_CONNECTIONS_SIZE);
	inputPredConnectionsSet[inputPredConnectionsQuantity++] = new TPredConnection(newID, newEnabled, newPreNeuron, this);
}

// Удаление предикторной связи из пула
void TNeuron::deletePredConnection(int predConnectionNumber){
	TPredConnection* deletingPredConnection = inputPredConnectionsSet[predConnectionNumber - 1];
	delete deletingPredConnection;
	// Сдвигаем массив
	for (int currentPredConnection = predConnectionNumber - 1; currentPredConnection < inputPredConnectionsQuantity - 1; ++currentPredConnection)
		inputPredConnectionsSet[currentPredConnection] = inputPredConnectionsSet[currentPredConnection + 1];
	//memcpy(connectednessSet[connectionNumber - 1], connectednessSet[connectionNumber], (inputConnectionsQuantity - connectionNumber) * sizeof(TPoolConnection*));
	inputPredConnectionsSet[inputPredConnectionsQuantity - 1] = 0; // Обнуляем указатель последней связи
	--inputPredConnectionsQuantity;
}

// Рассчет выхода нейрона
void TNeuron::calculateOut(){
	potential = 0; // На всякий случай
	potential += bias; // Добавляем смещение
	for (int currentSynapse = 1; currentSynapse <= inputSynapsesQuantity; ++currentSynapse)
	{
		// Определяем какая связь - рекуррентная или прямая
		double preNeuronOut = (layer > inputSynapsesSet[currentSynapse - 1]->getPreNeuron()->getLayer()) ? 
													inputSynapsesSet[currentSynapse - 1]->getPreNeuron()->getCurrentOut():
													inputSynapsesSet[currentSynapse - 1]->getPreNeuron()->getPreviousOut();
		// Если выход не означен, то что-то пошло не так (эта строчка больше для отладки)
		if (preNeuronOut == EMPTY_OUT) exit(2);
		if (preNeuronOut > ACTIVITY_TRESHOLD) // Если пресинаптический сигнал проходит по синапсу
			potential += inputSynapsesSet[currentSynapse - 1]->getWeight() * preNeuronOut;
	}

	currentOut = service::tansig(potential);
}

// Печать сведений о нейроне в файл или на экран
std::ostream& operator<<(std::ostream& os, const TNeuron& neuron){
	os << neuron.type << "\t" << neuron.bias << "\t" << neuron.layer 
		<< "\t" << neuron.active << "\t" << neuron.parentPoolID << std::endl;
	return os;
}
