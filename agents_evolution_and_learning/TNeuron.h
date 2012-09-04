#ifndef TNEURON_H
#define TNEURON_H

#include "TSynapse.h"

/* 
����� ������� ��������� ���� 
*/
class TNeuron{
	int ID; // ID �������
	int type; // ��� ������� (0 - ���������, 1 - �������, 2 - ��������)
	double bias; // �������� �������
	int layer; // ����� ����, � ������� ��������� ������
	bool active; // ������� ���������� ������� (0 - �������� ������, 1 - �������� ������)

	TSynapse* inputSynapses; // ������ ������� �������� �������
	int inputSynapsesSize; // ������� ������ ������� ������� ��������
	int inputSynapsesQuantity; // ���������� ������� �������� �������

	TNeuralPool* ParentPool; // ������ �� ��� �� �������� ����������� ������ (�� ������ ����)
	int ParentPoolID; // ����� ������������� ����

	double currentOut; // ������� ����� �������
	double potential; // ������� ��������� �������
	double previousOut; // ���������� ����� �������

	static const double EMPTY_OUT; // ������� ������������� ������ �������
	static const double ACTIVITY_TRESHOLD; // ����� ����������, ��� ������� ������ ���������� �� �������� ������
	static const int INFLATE_SYNAPSES_SIZE = 10; // ������ ���������� ������� � ������� ������� � ������ ������������
}

#endif // TNEURON_H