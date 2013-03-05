#ifndef TSHAREDEVOLUTIONARYPROCESS_H
#define TSHAREDEVOLUTIONARYPROCESS_H

#include "TEvolutionaryProcess.h"
#include "mpi.h"
#include <string>
#include <cstring>

// ������ ������������� �������� � "�����������" ������� ����� ���� (���������� ����� ������������, ���� 0, �� ������������ ���������������� ����������)
// ���������� ������ ���������, ������� ��������� � ������������ ������� ������������� ������� (���� ������ ������ �������� ����� ����� ������� ��������, � �������� ������� � �������)
class TSharedEvolutionaryProcess : public TEvolutionaryProcess{
  // ����������� ������������ ����� ���������
	static const int messageLength = 3000;
	// ����������� ��������� ��� ���������
	static const int messageType = 99;
	// ��������� ��������� ������� ��������
	MPI_Status status;
  // ����� ��������
	int processRank;
  char outputMessage[messageLength];
  char inputMessage[messageLength];

  // ���������� ������ � ��������� � ������ � ���������� �� �������� (��� ��������� � ������ $NAME$)
  std::string findParameterNote(std::string inputMessage, std::string parameterString);
  // ����������� ��������� �� �������� �������� ��������
  void decodeWorkMessage(const std::string& workMessage, std::string& tmpAgentsFilename, int& agentsQuantity);
  // ����������� ��������� �� �������� �������� �������� � ������������ �������� (���������� ������� ���� �������)
  std::vector<double> decodeFineshedWorkMessage(const std::string& finishedWorkMessage, int& process);
  // ������� �������
  void rootProcess(unsigned int randomSeed);
  // ������� �������
  void workProcess();

public:
  TSharedEvolutionaryProcess(){
    strcpy(inputMessage, "");
    strcpy(outputMessage, "");
  }
  ~TSharedEvolutionaryProcess(){
  }
  // ���������� � ������� ������������ ��������� �����
  std::string tmpDirectory;
  // ��������� ��� ����� ������� ��� ��������� (������� ������� ������� ������)
  std::vector<int> processesLocalPool;
  // ���� ������� ������� (��� ��� ��������� ��������� �����, �� ���� ���� ���������, ��� ��� �� ������������ ��� ������ ��������)
  std::string runSign;
	// ������ ������������� �������� (���������� ����� ������������, ���� 0, �� ������������ ���������������� ����������)
	void start(unsigned int randomSeed = 0);
};

#endif // TSHAREDEVOLUTIONARYPROCESS_H