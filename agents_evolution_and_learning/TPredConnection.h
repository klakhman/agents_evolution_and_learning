#ifndef TPREDCONNECTION_H
#define TPREDCONNECTION_H

#include <iostream>
#include <fstream>

class TNeuron;

/* 
����� ������������ ����� ��������� ����
*/
class TPredConnection{
	int ID; // ID ������������ �����
	bool enabled; // ������� ��������� �����
	TNeuron* preNeuron; // ������ �� ���������������� ������
	TNeuron* postNeuron; // ������ �� ����������������� ������
public:
	// ����������� �� ���������
	TPredConnection(){
		ID = 0;
		enabled = false;
		preNeuron = 0;
		postNeuron = 0;
	}
	// ������ �����������
	TPredConnection(int newID, bool newEnabled = true, TNeuron* newPreNeuron = 0, TNeuron* newPostNeuron = 0){
		setAll(newID, newEnabled, newPreNeuron, newPostNeuron);
	}
	// ����������
	~TPredConnection() {}

	// ������� � ������� ��� ���� ���������� ����������
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	bool getEnabled() const { return enabled; }
	void setEnabled(bool newEnabled) { enabled = newEnabled; }
	TNeuron* getPreNeuron() const { return preNeuron; }
	void setPreNeuron(TNeuron* newPreNeuron) { preNeuron = newPreNeuron; }
	TNeuron* getPostNeuron() const { return postNeuron; }
	void setPostNeuron(TNeuron* newPostNeuron) { postNeuron = newPostNeuron; }
	// ���������� ���� ������������� �����
	void setAll(int newID, bool newEnabled = true, TNeuron* newPreNeuron = 0, TNeuron* newPostNeuron = 0){
		ID = newID;
		enabled = newEnabled;
		preNeuron = newPreNeuron;
		postNeuron = newPostNeuron;
	}
	// ������ ������������ ����� � ���� ��� �� �����
	friend std::ostream& operator<<(std::ostream& os, const TPredConnection& poolPredConnection);
};

#endif // TPREDCONNECTION_H
