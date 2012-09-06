#ifndef TNEURON_H
#define TNEURON_H

#include <iostream>
#include <fstream>

class TNeuralNetwork;
class TSynapse;
class TPredConnection;

/* 
����� ������� ��������� ���� 
*/
class TNeuron{
	int ID; // ID �������
	int type; // ��� ������� (0 - ���������, 1 - �������, 2 - ��������)
	double bias; // �������� �������
	int layer; // ����� ����, � ������� ��������� ������
	bool active; // ������� ���������� ������� (0 - �������� ������, 1 - �������� ������)

	//TNeuralPool* ParentPool; // ������ �� ��� �� �������� ����������� ������ (�� ������ ����)
	int parentPoolID; // ����� ������������� ����

	TSynapse** inputSynapsesSet; // ������ ������� �������� �������
	int inputSynapsesSetSize; // ������� ������ ������� ������� ��������
	int inputSynapsesQuantity; // ���������� ������� �������� �������

	TPredConnection** inputPredConnectionsSet; // ������ ������� ������������ ������ �������
	int inputPredConnectionsSetSize; // ������� ������ ������� ������� ������������ ������
	int inputPredConnectionsQuantity; // ���������� ������� ������������ ������ �������

	double currentOut; // ������� ����� �������
	double potential; // ������� ��������� �������
	double previousOut; // ���������� ����� �������

	// ��������� �������� ������ ������� (������ ��� �������������� TNeuralNetwork ��� ���������� �������� �����)
	void setCurrentOut(double newCurrentOut) { currentOut = newCurrentOut; }

	// ��������� ���������� ������� ������� ������� ��������
	void inflateSynapsesSet(int inflateSize);
	// ��������� ���������� ������� ������� ������� ������������ ������
	void inflatePredConnectionsSet(int inflateSize);

	static const double EMPTY_OUT; // ������� ������������� ������ �������
	static const double ACTIVITY_TRESHOLD; // ����� ����������, ��� ������� ������ ���������� �� �������� ������
	static const int INFLATE_SYNAPSES_SIZE = 10; // ������ ���������� ������� � ������� ��������� � ������ ������������
	static const int INFLATE_PRED_CONNECTIONS_SIZE = 10; // ������ ���������� ������� � ������� ������������� ������� � ������ ������������

public:
	// ����������� �� ���������
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
	// ������ �����������
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
	// ����������
	~TNeuron();

	// ������� � ������� ��� ���� ���������� ����������
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
	// ��������� ���������� ������� �������� �������
	int getInputSynapsesQuantity() const { return inputSynapsesQuantity; }
	// ��������� ���������� ������� ������������ ������ �������
	int getInputPredConnectionsQuantity() const { return inputPredConnectionsQuantity; }
	// ���������� ���� ������������� �������
	void setAll(int newID, int newType, double newBias, int newLayer, bool newActive, int newParentPoolID){
		ID = newID;
		type = newType;
		bias = newBias;
		layer = newLayer;
		active = newActive;
		parentPoolID = newParentPoolID;
	}
	// ��������� ������������ ������������� �������
	double getCurrentOut() { return currentOut; }
	double getCurrentPotential() { return potential; }
	double getPreviousOut() { return previousOut; }

	// ������� � ������� ��� �������� ������� ���� (�� ���� ������� ���������� ����� ������� � ������� ��������)
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

	// ������� � ������� ��� ������������ ������ ������� ������� (�� ���� ������� ���������� ����� ����� � ������� ������)
	int getPredConnectionID(int predConnectionNumber) const;
	void setPredConnectionID(int predConnectionNumber, int newID);
	bool getPredConnectionEnabled(int predConnectionNumber) const;
	void setPredConnectionEnabled(int predConnectionNumber, bool newEnabled);
	TNeuron* getPredConnectionPreNeuron(int predConnectionNumber) const;
	void setPredConnectionPreNeuron(int predConnectionNumber, TNeuron* newPreNeuron);
	TNeuron* getPredConnectionPostNeuron(int predConnectionNumber) const;
	void setPredConnectionPostNeuron(int predConnectionNumber, TNeuron* newPostNeuron);

	// ���������� �������� ������� � ������
	void addSynapse(int newID, double newWeight, bool newEnabled = true, TNeuron* newPreNeuron = 0);
	// �������� ������� �� �������
	void deleteSynapse(int synapseNumber);

	// ���������� ������� ������������ ����� � ������
	void addPredConnection(int newID, bool newEnabled = true, TNeuron* newPreNeuron = 0);
	// �������� ������������ ����� �� �������
	void deletePredConnection(int predConnectionNumber);

	// "������������ �������"
	void reset(){
		currentOut = 0;
		potential = 0;
		previousOut = 0;
	}

	// ���������� ������� � ������� �� ��������� ����� �������
	void prepare(){
		previousOut = currentOut;
		currentOut = EMPTY_OUT;
		potential = 0;
	}

	// ������� ������ �������
	void calculateOut();

	// ������ �������� � ������� � ���� ��� �� �����
	friend std::ostream& operator<<(std::ostream& os, const TNeuron& neuron);

	//������ ���� � ���� ��� �� �����
	friend std::ostream& operator<<(std::ostream& os, const TNeuralNetwork& neuralNetwork); // ������� ������ ���� ��������� �������������, ����� ��� ����� ������ ������ � ������ ������ �������

	friend class TNeuralNetwork;
};

#endif // TNEURON_H