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

// Добавление входного синапса в нейрон
void TNeuron::addSynapse(int newID, TNeuron* newPreNeuron, double newWeight, bool newEnabled /*=true*/){
	if (inputSynapsesQuantity >= inputSynapsesSetSize) // Если у нас не хватает объема массива
		inflateSynapsesSet(INFLATE_SYNAPSES_SIZE);
	inputSynapsesSet[inputSynapsesQuantity++] = new TSynapse(newID, newPreNeuron, this, newWeight, newEnabled);
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
void TNeuron::addPredConnection(int newID, TNeuron* newPreNeuron, bool newEnabled /* = true*/){
	if (inputPredConnectionsQuantity >= inputPredConnectionsSetSize) // Если у нас не хватает объема массива
		inflatePredConnectionsSet(INFLATE_PRED_CONNECTIONS_SIZE);
	inputPredConnectionsSet[inputPredConnectionsQuantity++] = new TPredConnection(newID, newPreNeuron, this, newEnabled);
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
		double preNeuronOut = (layer > inputSynapsesSet[currentSynapse - 1]->preNeuron->layer) ? 
													inputSynapsesSet[currentSynapse - 1]->preNeuron->currentOut:
													inputSynapsesSet[currentSynapse - 1]->preNeuron->previousOut;
		// Если выход не означен, то что-то пошло не так (эта строчка больше для отладки)
		if (preNeuronOut == EMPTY_OUT) exit(2);
		if (preNeuronOut > ACTIVITY_TRESHOLD) // Если пресинаптический сигнал проходит по синапсу
			potential += inputSynapsesSet[currentSynapse - 1]->weight * preNeuronOut;
	}
	// Если нейрон активен, то вычисляем выход
	if (active)
		currentOut = service::tansig(potential);
	else 
		// Иначе нулевой выход
		currentOut = 0;
}

// Печать сведений о нейроне в файл или на экран
std::ostream& operator<<(std::ostream& os, const TNeuron& neuron){
	os << neuron.type << "\t" << neuron.bias << "\t" << neuron.layer 
		<< "\t" << neuron.active << "\t" << neuron.parentPoolID << std::endl;
	return os;
}
