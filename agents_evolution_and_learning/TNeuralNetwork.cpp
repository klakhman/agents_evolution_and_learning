#include "TNeuralNetwork.h"
#include "TSynapse.h"
#include "TPredConnection.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// ������������� ID �������� (�������� ����� ��������)
void TNeuralNetwork::fixNeuronsIDs(){
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		neuronsStructure[currentNeuron - 1]->setID(currentNeuron);
}

// ��������� ���������� ������� ������� ��������
void TNeuralNetwork::inflateNeuronsStructure(int inflateSize){
	TNeuron** newNeuronsStructure = new TNeuron*[neuronsStructureSize + inflateSize];
	memset(newNeuronsStructure, 0, (neuronsStructureSize + inflateSize) * sizeof(TNeuron*));
	memcpy(newNeuronsStructure, neuronsStructure, neuronsStructureSize * sizeof(TNeuron*));
	delete []neuronsStructure;
	neuronsStructure = newNeuronsStructure;
	neuronsStructureSize += inflateSize;
}

// �����������
TNeuralNetwork::~TNeuralNetwork(){
	if (neuronsStructure){
		for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			delete neuronsStructure[currentNeuron - 1];
		delete []neuronsStructure;
	}
}

//���������� ������� � ����
void TNeuralNetwork::addNeuron(int newID, int newType, double newBias, int newLayer, bool newActive /*=true*/, int newParentneuralID /*=0*/){
	if (neuronsQuantity >= neuronsStructureSize) // ���� �� ������� ����� � �������
		inflateNeuronsStructure(INFLATE_NEURONS_SIZE);
	neuronsStructure[neuronsQuantity++] = new TNeuron(newID, newType, newBias, newLayer, newActive, newParentneuralID);
	// ���� ������ �������
	if (0 == newType) ++inputResolution;
	// ���� ������ ��������
	if (2 == newType) ++outputResolution;
	// ������� �� ���� �������
	if (newLayer > layersQuantity) layersQuantity = newLayer;
}

// �������� ������� �� ���� (� ��������� ����� ���� ������� � �������� ������ �� ����� �������)
void TNeuralNetwork::deleteNeuron(int neuronNumber){
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		if (currentNeuron != neuronNumber){ // ���� ��� �� ��������� ������ (���������� ������ ��� �������� �������)
			for (int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1]->getInputSynapsesQuantity(); ++currentSynapse)
				if (neuronsStructure[currentNeuron - 1]->getSynapsePreNeuron(currentSynapse)->getID() == neuronNumber) // ���� ��� ������ �� ���������� �������
					deleteSynapse(currentNeuron, currentSynapse);
			for (int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
				if (neuronsStructure[currentNeuron - 1]->getPredConnectionPreNeuron(currentPredConnection)->getID() == neuronNumber) // ���� ��� ����. ����� �� ���������� �������
					deletePredConnection(currentNeuron, currentPredConnection);
		}
	// ���� ������ �������
	if (0 == neuronsStructure[neuronNumber - 1]->getType()) --inputResolution;
	// ���� ������ ��������
	if (2 == neuronsStructure[neuronNumber - 1]->getType()) --outputResolution;
	// ������� ������ � �������� ������
	synapsesQuantity -= neuronsStructure[neuronNumber - 1]->getInputSynapsesQuantity();
	predConnectionsQuantity -= neuronsStructure[neuronNumber - 1]->getInputPredConnectionsQuantity();
	delete neuronsStructure[neuronNumber - 1];
	for (int currentNeuron = neuronNumber - 1; currentNeuron < neuronsQuantity - 1; ++currentNeuron)
		neuronsStructure[currentNeuron] = neuronsStructure[currentNeuron + 1];
	neuronsStructure[neuronsQuantity - 1] = 0;
	--neuronsQuantity;
	fixNeuronsIDs();
}

// �������� ����
void TNeuralNetwork::eraseNeuralNetwork(){
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		delete neuronsStructure[currentNeuron - 1];
	delete []neuronsStructure;
	neuronsStructure = 0;
	neuronsStructureSize = 0;
	neuronsQuantity = 0;
	synapsesQuantity = 0;
	predConnectionsQuantity = 0;
	layersQuantity = 0;
	inputResolution = 0;
	outputResolution = 0;
}

// "������������" ����
void TNeuralNetwork::reset(){
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		neuronsStructure[currentNeuron - 1]->reset();
}

// ������ ������ ����� ������ ����
void TNeuralNetwork::calculateNetwork(double inputVector[]){
	// ������� �������������� ��� �������
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		neuronsStructure[currentNeuron - 1]->prepare();
	// ��������� ������� �������
	for (int currentBit = 1; currentBit <= inputResolution; ++currentBit)
		neuronsStructure[currentBit - 1]->setCurrentOut(inputVector[currentBit - 1]);
	// ���������� �� �������� �� ����� (�������� �� �������)
	for (int currentLayer = 2; currentLayer <= layersQuantity; ++currentLayer)
		for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			if (neuronsStructure[currentNeuron - 1]->getLayer() == currentLayer)
				neuronsStructure[currentNeuron - 1]->calculateOut();
}

// ��������� �������� ��������� ������� ����
void TNeuralNetwork::getOutputVector(double outputVector[]){
	for (int currentBit = 1; currentBit <= outputResolution; ++currentBit)
		outputVector[currentBit - 1] = neuronsStructure[currentBit - 1 + inputResolution]->getCurrentOut(); 
}

//������ ���� � ���� ��� �� �����
ostream& operator<<(ostream& os, const TNeuralNetwork& neuralNetwork){
	for (int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsQuantity; ++currentNeuron)
		os << *(neuralNetwork.neuronsStructure[currentNeuron - 1]);
	os << "|\n"; // ���������� ����������� ����� ��������� � ���������
	for (int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsQuantity; ++currentNeuron)
		for (int currentSynapse = 1; currentSynapse <= neuralNetwork.neuronsStructure[currentNeuron - 1]->getInputSynapsesQuantity(); ++currentSynapse)
			os << *(neuralNetwork.neuronsStructure[currentNeuron-1]->inputSynapsesSet[currentSynapse-1]); 
	os << "||\n"; // ���������� ����������� ����� ��������� � ������������� �������
	for (int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsQuantity; ++currentNeuron)
		for (int currentPredConnection = 1; currentPredConnection <= neuralNetwork.neuronsStructure[currentNeuron - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			os << *(neuralNetwork.neuronsStructure[currentNeuron-1]->inputPredConnectionsSet[currentPredConnection-1]); 
	os << "|||\n"; // ���������� ����������� ����� ������
	return os;
}

//���������� ���� �� ����� ��� ������
istream& operator>>(istream& is, TNeuralNetwork& neuralNetwork){
	neuralNetwork.eraseNeuralNetwork(); // �� ������ ������ ���������� ����
	string tmp_string;
	// ������� ��� ������� ����
	is >> tmp_string; // ��������� ��� �������
	while (tmp_string != "|"){ // ��������� �� ����������� ����� ������ � �������
		int newType = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� �������� �������
		double newBias = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ���� �������
		int newLayer = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ������� ���������� �������
		bool newActive = (atoi(tmp_string.c_str()) != 0);
		is >> tmp_string; // ��������� ����� ������������� ����
		int newParentPoolID = atoi(tmp_string.c_str());
		neuralNetwork.addNeuron(neuralNetwork.getNeuronsQuantity() + 1, newType, newBias, newLayer, newActive, newParentPoolID);
		is >> tmp_string; // ��������� ���� �������
	}
	// ������� ��� ������� ����� ������
	is >> tmp_string; // ��������� ����� ����������������� �������
	while (tmp_string != "||"){ // ��������� �� ����������� ����� ��������� � ������������� �������
		int preNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ����� ������������������ �������
		int postNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ��� �������
		double newWeight = atof(tmp_string.c_str());
		is >> tmp_string; // ��������� ������� ��������� �������
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		neuralNetwork.addSynapse(preNeuronNumber, postNeuronNumber, neuralNetwork.getSynapsesQuantity() + 1, newWeight, newEnabled);
		is >> tmp_string; // ��������� ����� ����������������� �������
	}
	// ������� ��� ������������ ����� ����� ���������
	is >> tmp_string; // ��������� ����� ����������������� �������
	while (tmp_string != "|||"){ // ��������� �� ����������� ����� ������������� ������� � ������ ����
		int preNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ����� ������������������ �������
		int postNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // ��������� ������� ��������� �����
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		neuralNetwork.addPredConnection(preNeuronNumber, postNeuronNumber, neuralNetwork.getPredConnectionsQuantity() + 1, newEnabled);
		is >> tmp_string; // ��������� ����� ����������������� �������
	}

	return is;
}
