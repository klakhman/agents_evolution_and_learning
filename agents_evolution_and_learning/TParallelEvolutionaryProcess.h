#ifndef TPARALLELEVOLUTIONARYPROCESS_H
#define TPARALLELEVOLUTIONARYPROCESS_H

#include "mpi.h"
#include <string>

// ������� ������������� ������� ���������� �������� (� ������� MPI)
class TParallelEvolutionaryProcess{
	// ����� ��������
	int processRank;
	// ����� ���-�� ���������
	int processesQuantity;
	// ����������� ������������ ����� ���������
	static const int messageLentgh = 100;
	// ����������� ��������� ��� ���������
	static const int messageType = 99;
	// ��������� ��������� ������� ��������
	MPI_Status status;

	//���������� ��� ������ ������
	struct {
		std::string workDirectory;
		std::string environmentDirectory;
		std::string resultsDirectory;
	} directoriesSettings;
	// ���� ��������
	std::string settingsFilename;

	// ���������� ���������� ���������� ��� ������ ������
	void fillDirectoriesSettings();
	// ������������ ��������� ��������� ������
	void decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, std::string runSign);
	// ���������� ������������ ��������
	void rootProcess(int argc, char **argv);
	// ���������� �������� ��������
	void workProcess();
public:
	// �����������
	TParallelEvolutionaryProcess(){
		processRank = 0;
		processesQuantity = 0;
	}
	// ����������
	~TParallelEvolutionaryProcess(){ }
	// ������ ������������� ��������
	void start(int argc, char **argv);
};

#endif // TPARALLELEVOLUTIONARYPROCESS_H