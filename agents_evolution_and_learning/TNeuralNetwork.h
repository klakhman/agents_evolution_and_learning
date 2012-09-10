#ifndef TNEURALNETWORK_H
#define TNEURALNETWORK_H

#include "TNeuron.h"

#include <iostream>
#include <fstream>

/*
����� ��������� ����
*/
class TNeuralNetwork{
	TNeuron** neuronsStructure; // ������ �� �������� 
	int neuronsStructureSize; // ������� ������ ������� �������
	int neuronsQuantity; // ���������� �������� � ����

	int synapsesQuantity; // ���������� �������� � ����
	int predConnectionsQuantity; // ���������� ������������ ������ � ����

	int layersQuantity; // ���������� ����� � ���� (!!!! �� ����������� ��� �������� �������� !!!)
	int inputResolution; // ������ �������� ��-�� ����
	int outputResolution; // ������ ��������� ��-�� ����

	static const int INFLATE_NEURONS_SIZE = 10; // ������ ���������� ������� � ��������� � ������ ������������

	// ������������� ID �������� (�������� ����� ��������)
	void fixNeuronsIDs();
	
	// ��������� ���������� ������� ������� ��������
	void inflateNeuronsStructure(int inflateSize);

public:
	// ����������� �� ���������
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
	// ����������
	~TNeuralNetwork();

	//������� ��� ���������� ����
	int getNeuronsQuantity() { return neuronsQuantity; }
	int getSynapsesQuantity() { return synapsesQuantity; }
	int getPredConnectionsQuantity() { return predConnectionsQuantity; }
	int getInputResolution() { return inputResolution; }
	int getOutputResolution() { return outputResolution; }

	// ������� � ������� ��� ���� �������� ���� (�� ���� ������� ���������� ����� ������� � ������� ��������)
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

	// ������� � ������� ��� �������� ������ ���� (�� ���� ������� ���������� ����� ������� � ������� �������� � ����� ������� � ������� ��������)
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

	// ������� � ������� ��� ������������ ������ ������ ���� (�� ���� ������� ���������� ����� ������� � ������� �������� � ����� ����� � ������� ������)
	int getPredConnectionID(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionID(predConnectionNumber); }
	void setPredConnectionID(int neuronNumber, int predConnectionNumber, int newID) { neuronsStructure[neuronNumber-1]->setPredConnectionID(predConnectionNumber, newID); }
	bool getPredConnectionEnabled(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionEnabled(predConnectionNumber); }
	void setPredConnectionEnabled(int neuronNumber, int predConnectionNumber, bool newEnabled) { neuronsStructure[neuronNumber-1]->setPredConnectionEnabled(predConnectionNumber, newEnabled); }
	TNeuron* getPredConnectionPreNeuron(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionPreNeuron(predConnectionNumber); }
	void setPredConnectionPreNeuron(int neuronNumber, int predConnectionNumber, TNeuron* newPreNeuron) { neuronsStructure[neuronNumber-1]->setPredConnectionPreNeuron(predConnectionNumber, newPreNeuron); }
	TNeuron* getPredConnectionPostNeuron(int neuronNumber, int predConnectionNumber) const { return neuronsStructure[neuronNumber-1]->getPredConnectionPostNeuron(predConnectionNumber); }
	void setPredConnectionPostNeuron(int neuronNumber, int predConnectionNumber, TNeuron* newPostNeuron) { neuronsStructure[neuronNumber-1]->setPredConnectionPostNeuron(predConnectionNumber, newPostNeuron); }

	//���������� ������� � ����
	void addNeuron(int newID, int newType, double newBias, int newLayer, bool newActive = true, int newParentNeuronID = 0);

	//���������� ������� � ����
	void addSynapse(int preNeuronNumber, int postNeuronNumber, int newID, double newWeight, bool newEnabled){
		neuronsStructure[postNeuronNumber-1]->addSynapse(newID, newWeight, newEnabled, neuronsStructure[preNeuronNumber-1]); 
		++synapsesQuantity;
	}

	// ���������� ������������ ����� � ����
	void addPredConnection(int preNeuronNumber, int postNeuronNumber, int newID, bool newEnabled){
		neuronsStructure[postNeuronNumber-1]->addPredConnection(newID, newEnabled, neuronsStructure[preNeuronNumber-1]); 
		++predConnectionsQuantity;
	}

	// �������� ������� �� ���� (� ��������� ����� ���� ������� � �������� ������ �� ����� �������)
	void deleteNeuron(int neuronNumber);

	//�������� ����� �� ����
	void deleteSynapse(int neuronNumber, int synapseNumber){
		neuronsStructure[neuronNumber-1]->deleteSynapse(synapseNumber);
		--synapsesQuantity;
	}

	// �������� ������������ ����� �� ����
	void deletePredConnection(int neuronNumber, int predConnectionNumber){
		neuronsStructure[neuronNumber-1]->deletePredConnection(predConnectionNumber);
		--predConnectionsQuantity;
	}

	// �������� ����
	void eraseNeuralNetwork();

	// "������������" ����
	void reset();

	// ������ ������ ����� ������ ����
	void calculateNetwork(double inputVector[]);

	// ��������� �������� ��������� ������� ����
	void getOutputVector(double outputVector[]);

	//������ ���� � ���� ��� �� �����
	friend std::ostream& operator<<(std::ostream& os, const TNeuralNetwork& NeuralNetwork);

	//���������� ���� �� ����� ��� ������
	friend std::istream& operator>>(std::istream& is, TNeuralNetwork& NeuralNetwork);
};

#endif // TNEURALNETWORK_H
