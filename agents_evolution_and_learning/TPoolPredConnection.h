#ifndef TPOOLPREDCONNECTION_H
#define TPOOLPREDCONNECTION_H

#include <iostream>
#include <fstream>

class TNeuralPool;

/* 
����� ������������ ����� ���� �� �����
*/
class TPoolPredConnection{
	int ID; // ID ������������ �����
	bool enabled; // ������� ��������� �����
	int disabledStep; // ����� ��������� (����� ��������), � ������� ����� ���� ��������� (0 - ���� ��������)
	double developPredConnectionProb; // ����������� �������� ������������ ����� ����� ��������� �� ���� �����
	long innovationNumber; // ����� ��������� �����
	TNeuralPool* prePool; // ������ �� ���������������� ���
	TNeuralPool* postPool; // ������ �� ����������������� ���
public:
	// ����������� �� ���������
	TPoolPredConnection(){
		ID = 0;
		enabled = false;
		disabledStep = 0;
		developPredConnectionProb = 0;
		innovationNumber = 0;
		prePool = 0;
		postPool = 0;
	}
	// ������ �����������
	TPoolPredConnection(int newID, bool newEnabled = true, int newDisabledStep = 0, double newDevelopPredConnectionProb = 1, long newInnovationNumber = 0, TNeuralPool* newPrePool = 0, TNeuralPool* newPostPool = 0){
		setAll(newID, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber, newPrePool, newPostPool);
	}
	// ����������
	~TPoolPredConnection() {}

	// ������� � ������� ��� ���� ���������� ����������
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	bool getEnabled() const { return enabled; }
	void setEnabled(bool newEnabled) { enabled = newEnabled; }
	int getDisabledStep() const { return disabledStep; }
	void setDisabledStep(int newDisabledStep) { disabledStep = newDisabledStep; }
	double getDevelopPredConnectionProb() const { return developPredConnectionProb; }
	void setDevelopPredConnectionProb(double newDevelopPredConnectionProb) { developPredConnectionProb = newDevelopPredConnectionProb; }
	long getInnovationNumber() const { return innovationNumber; }
	void setInnovationNumber(long newInnovationNumber) { innovationNumber = newInnovationNumber; }
	TNeuralPool* getPrePool() const { return prePool; }
	void setPrePool(TNeuralPool* newPrePool) { prePool = newPrePool; }
	TNeuralPool* getPostPool() const { return postPool; }
	void setPostPool(TNeuralPool* newPostPool) { postPool = newPostPool; }
	// ���������� ���� ������������� �����
	void setAll(int newID, bool newEnabled = true, int newDisabledStep = 0, double newDevelopPredConnectionProb = 1, long newInnovationNumber = 0, TNeuralPool* newPrePool = 0, TNeuralPool* newPostPool = 0){
		ID = newID;
		enabled = newEnabled;
		disabledStep = newDisabledStep;
		developPredConnectionProb = newDevelopPredConnectionProb;
		innovationNumber = newInnovationNumber;
		prePool = newPrePool;
		postPool = newPostPool;
	}
	// ������ ������������ ����� � ���� ��� �� �����
	friend std::ostream& operator<<(std::ostream& ofs, const TPoolPredConnection& poolPredConnection);
};

#endif // TPOOLPREDCONNECTION_H
