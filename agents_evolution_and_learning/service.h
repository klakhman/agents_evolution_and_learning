#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <cmath>

namespace service{

	const double PI = 3.14159265;

	// ������������ ����� �������� ������������ �����, ������� ����� ���� ������������ �� ��������� (��� ����������� ��������) 
	const int MAX_HEX_RESOLUTION = 6; 

	// ������������� ������� (������ �������������) - ���������� ������ ������ � ����, ������� ������ ������ ���� �����������
	inline double tansig(double x, double slop = 2) { return ( 1/ (1 + exp(-slop*x)) ); }

	// ����������� ������������� ������� (�� -1 �� 1) - ���������� ������ ������ � ����, ������� ������ ������ ���� �����������
	inline double tansig_standart(double x, double slop = 2) { return (2 / (1 + exp(-slop*x)) - 1); }
	
	// ������� ��������� ���������������� ���������� ��������������� ���������� �������� (����� ���������� ����������)
	int uniformDiscreteDistribution(int A, int B);

	// ������� ��������� ����������������� ������ ���������� ��������������� ����� (���� ����� ��������� ����������, ���� �� ������� �����) - �������� ��������
	double uniformDistribution(double A, double B, bool include_start = true, bool include_end = true);

	// ������� ��������� ����������������� ����� ���������� ��������������� ����� (���� ����� ��������� ����������, ���� �� ������� �����) - �������� ������
	double uniformDistributionWeak(double A, double B, bool include_start = true, bool include_end = true);

	// ������� ��������� ���������������� ��������� ��������������� �����
	double normalDistribution(double mean, double variance);

	// ��������� �������� ����������� ����� � ��������
	void decToBin(int decNumber, bool binNumber[], int binResolution);

	// ��������� �������� ��������� ����� � ���������� (������� ������� ������ ���� � ������ �������)
	int binToDec(bool binNumber[], int binResolution);

	// ��������� ��������������� ����������� ����� � ����������������� - ����������
	// �� ��������� ���������� service::MAX_HEX_RESOLUTION ��������, ��� ��� � ������ ����� ���� ����, � ������� ����� ����� ����������
	void decToHex(int decNumber, std::string& hexNumber, int hexResolution = MAX_HEX_RESOLUTION);

};

#endif // SERVICE_H
