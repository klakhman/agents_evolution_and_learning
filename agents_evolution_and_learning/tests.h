#ifndef TESTS_H
#define TESTS_H

#include "TEnvironment.h"
#include "TPoolNetwork.h"
#include <string>

class tests{
	// �������� ������������������ ���� (true - ���� �������������, false - ���� ���������������)
	bool checkAimInconsistency(TEnvironment::TAim& aim, int environmentResolution);
	// ��������� ���� ����� ����� (true - ���� �����, false - ���� �������)
	bool compareTwoAims(TEnvironment::TAim& firstAim, TEnvironment::TAim& secondAim);

public:
	int testPoolNetwork(std::string outputFilename);
	int testNeuralNetwork(std::string outputFilename);
	void testEnvironment(std::string firstOutputEnvironmentFilename, std::string secondOutputEnvironmentFilename);

	//��������� ������������ ������� ��������� ���� (0 - ��� ������, 1 - ���� ������)
	int testNeuralNetworkProcessing(std::string inputNetworkFilename);
	// ������������ ��������� �������� ������������� ����� (0 - ��� ������, 1 - ���� ������)
	int testCalculateOccupancyCoeffcient(std::string environmentFilename);
	// ������������ ��������� ��������� ����� (0 - ��� ������, 1 - ���� �������������� ����, 2 - ���� ������ ����)
	int testGenerateEnvironment();
	// ��������� ������������ ������� ������������� �������������, ����������� � ������� ������ �������
	int testWeakUniformDistribution();

	//------------------ ������������ ��������� �������� ������� ---------------------------

	// �������� �������� ���� �� �����
	void createTestPoolNetwork(TPoolNetwork& poolNetwork);

	// ������������ ��������� ������� �����
	void testWeigthsMutation(std::string testDirectory);

	//������������ ��������� ���������� �����
	void testAddConnection(std::string testDirectory);

	// ������������ ��������� �������� �����
	void testDeleteConnection(std::string testDirectory);

	// ������������ ��������� ���������� ����
	void testDuplicatePool(std::string testDirectory);
};

#endif // TESTS_H


