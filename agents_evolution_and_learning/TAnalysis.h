#ifndef TANALYSIS_H
#define TANALYSIS_H

#include <string>
#include "TEnvironment.h"
#include "TPopulation.h"

// ����� ��������� �������
class TAnalysis{

	// �������� ���������� ����� �� �����
	void fillEnvironmentSettingsFromFile(TEnvironment& environment, std::string settingsFilename);
	// �������� ���������� ��������� �� �����
	void fillPopulationSettingsFromFile(TPopulation& agentsPopulation, std::string settingsFilename);
	// �������� ���������� ������ �� �����
	void fillAgentSettingsFromFile(TPopulation& agentsPopulation, std::string settingsFilename);
	
	// ----------- ������ ��� ������������� ������������ ������� ������ ��������� ----------------
	// ���������� ���������� ���������� ��� ������ ������
	void fillDirectoriesSettings(std::string settingsFilename, std::string& workDirectory, std::string& environmentDirectory, std::string& resultsDirectory);
	// ������������ ��������� ��������� ������
	void decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, std::string& runSign);
	// ����������� ��������� �� �������� �������� 
	void decodeFinishedWorkMessage(char inputMessage[], int& processRankSend, int& finishedEnvironment, int& finishedTry, double& averageReward);
	// ���������� ������������ �������� (�����, ����� ���������� ����� ������� �� ���� ������ ���-�� ���������� ���������!!!)
	void rootProcess(int argc, char **argv);
	// ����������� ��������� �� �������� �������� 
	void decodeTaskMessage(char inputMessage[], int& currentEnvironment, int& currentTry, std::string& runSign);
	// ���������� �������� ��������
	void workProcess(int argc, char **argv);

public:
	TAnalysis() {};
	~TAnalysis() {};

	// ������ ��������� ������� ����� ������� ������ ��������� (���������� ������� �������� ������� �� ��������� ����� ������� ���� ������� �� ���� ���������)
	double startBestPopulationAnalysis(std::string bestPopulationFilename, std::string environmentFilename, std::string settingsFilename, unsigned int randomSeed = 0);
	// ������ ������������� �������� ������� �� ������ ������ ��������� � ������ ������� (!!!� ����� � ������������ ������ ���� ����� "/Analysis"!!!)
	void startParallelBestPopulationAnalysis(int argc, char **argv);
};

#endif // TANALYSIS_H