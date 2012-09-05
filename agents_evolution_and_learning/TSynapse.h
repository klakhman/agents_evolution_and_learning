#ifndef TSYNAPSE_H
#define TSYNAPSE_H

#include <iostream>
#include <fstream>

class TNeuron;

/* 
����� ������� ��������� ���� 
*/
class TSynapse{
	int ID; // ID �������
	double weight; // ��� ������� (������������� �����������)
	bool enabled; // ������� ��������� �������
	TNeuron* preNeuron; // ������ �� ���������������� ������
	TNeuron* postNeuron; // ������ �� ����������������� ������

public:
	// ����������� �� ���������
	TSynapse(){
		ID = 0;
		weight = 0;
		enabled = false;
		preNeuron = NULL;
		postNeuron = NULL;
	}
	// ������ �����������
	TSynapse(int newID, double newWeight, bool newEnabled = true, TNeuron* newPreNeuron = NULL, TNeuron* newPostNeuron = NULL){
		setAll(newID, newWeight, newEnabled, newPreNeuron, newPostNeuron);
	}
	// ����������
	~TSynapse() {}

	// ������� � ������� ��� ���� ���������� ����������
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	double getWeight() const { return weight; }
	void setWeight(double newWeight) { weight = newWeight; }
	bool getEnabled() const { return enabled; }
	void setEnabled(bool newEnabled) { enabled = newEnabled; }
	TNeuron* getPreNeuron() const { return preNeuron; }
	void setPreNeuron(TNeuron* newPreNeuron) { preNeuron = newPreNeuron; }
	TNeuron* getPostNeuron() const { return postNeuron; }
	void setPostNeuron(TNeuron* newPostNeuron) { postNeuron = newPostNeuron; }
	// ���������� ���� ������������� �������
	void setAll(int newID, double newWeight, bool newEnabled = true, TNeuron* newPreNeuron = NULL, TNeuron* newPostNeuron = NULL){
		ID = newID;
		weight = newWeight;
		enabled = newEnabled;
		preNeuron = newPreNeuron;
		postNeuron = newPostNeuron;
	}
	// ������ ������� � ���� ��� �� �����
	friend std::ostream& operator<<(std::ostream& ofs, const TSynapse& synapse);
};

#endif // TSYNAPSE_H