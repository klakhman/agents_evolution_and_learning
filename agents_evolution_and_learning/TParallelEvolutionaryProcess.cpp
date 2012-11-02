#include "TParallelEvolutionaryProcess.h"

#include <string>
#include <fstream>
#include <cstdlib>

using namespace std;

// ���������� ���������� ���������� ��� ������ ������
void TParallelEvolutionaryProcess::fillDirectoriesSettings(){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename);
	while (settingsFile >> optionString){
		if (optionString == "work-directory") { settingsFile >> directoriesSettings.workDirectory; }
		if (optionString == "environment-directory") { settingsFile >> directoriesSettings.environmentDirectory; }
		if (optionString == "results-directory") { settingsFile >> directoriesSettings.workDirectory; }
	}
	settingsFile.close();
}

// ������������ ��������� ��������� ������
void TParallelEvolutionaryProcess::decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, string runSign){
	int currentArgNumber = 2; // ������� ����� ��������� (� ������ ������� ���� � ����� ��������)
	while (currentArgNumber < argc){
		switch (argv[currentArgNumber][1]){ // �������������� �������� (� ������ ���� "-")
			case 'e': // ���� ��� �������� ������� ����
				++currentArgNumber;
				firstEnvironmentNumber = atoi(argv[currentArgNumber]);
				++currentArgNumber;
				lastEnvironmentNumber = atoi(argv[currentArgNumber]);
				break;
			case 't': // ���� ��� �������� �������
				++currentArgNumber;
				firstTryNumber = atoi(argv[currentArgNumber]);
				++currentArgNumber;
				lastTryNumber = atoi(argv[currentArgNumber]);
				break;
		}
		++currentArgNumber;
	}
}

// ���������� ������������ ��������
void TParallelEvolutionaryProcess::rootProcess(int argc, char **argv){
	int firstEnvironmentNumber; // �������� ������� ����
	int lastEnvironmentNumber;
	int firstTryNumber; // �������� �������
	int lastTryNumber;
	string runSign; // ��������� ������������� ������� ������� ����������� ������ ���������� ��� ������ ���������
	decodeCommandPromt(argc, argv, firstEnvironmentNumber, lastEnvironmentNumber, firstTryNumber, lastTryNumber, runSign);

	// ������ �������� ��������� ���� ������� ������ �� ���������� ���������
	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry){
			// ���� �� ���� ��������� ������ ����������� ������� (������� �������)
		}
}