#define _CRT_SECURE_NO_WARNINGS

#include "TParallelEvolutionaryProcess.h"

#include "TEvolutionaryProcess.h"

#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <ctime>

using namespace std;

// ���������� ���������� ���������� ��� ������ ������
void TParallelEvolutionaryProcess::fillDirectoriesSettings(){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename);
	while (settingsFile >> optionString){
		if (optionString == "work-directory") { settingsFile >> directoriesSettings.workDirectory; }
		if (optionString == "environment-directory") { settingsFile >> directoriesSettings.environmentDirectory; }
		if (optionString == "results-directory") { settingsFile >> directoriesSettings.resultsDirectory; }
	}
	settingsFile.close();
}

// ������������ ��������� ��������� ������
void TParallelEvolutionaryProcess::decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, string& runSign){
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
			case 's': // ���� ��� ������� ������������ �������
				++currentArgNumber;
				runSign = argv[currentArgNumber];
		}
		++currentArgNumber;
	}
}

// ����������� ��������� �� �������� �������� 
void TParallelEvolutionaryProcess::decodeFinishedWorkMessage(char inputMessage[], int& processRankSend, int& finishedEnvironment, int& finishedTry){
	string tmpString;
	for (unsigned int i=0; i < strlen(inputMessage); ++i)
		if ( (inputMessage[i] >= '0') && (inputMessage[i] <= '9') ) // ���� ������ �����
			tmpString += inputMessage[i];
		else {
			switch (inputMessage[i]){
				case 'e': // ���� ��� ����� �����
					finishedEnvironment = atoi(tmpString.c_str());
					break;
				case 't': // ���� ��� ����� �������
					finishedTry = atoi(tmpString.c_str());
					break;
				case 'p': // ���� ��� ����� ��������
					processRankSend = atoi(tmpString.c_str());
					break;
			}
			tmpString = "";
		}
}

// ���������� ������������ �������� (�����, ����� ���������� ����� ������� �� ���� ������ ���-�� ���������� ���������!!!)
void TParallelEvolutionaryProcess::rootProcess(int argc, char **argv){
	int firstEnvironmentNumber; // �������� ������� ����
	int lastEnvironmentNumber;
	int firstTryNumber; // �������� �������
	int lastTryNumber;
	string runSign; // ��������� ������������� ������� ������� ����������� ������ ���������� ��� ������ ���������
	decodeCommandPromt(argc, argv, firstEnvironmentNumber, lastEnvironmentNumber, firstTryNumber, lastTryNumber, runSign);
	// ������� ���� � �����
	stringstream logFilename;
	logFilename << directoriesSettings.workDirectory << "/Evolution_run_log_En" << firstEnvironmentNumber << "-" << lastEnvironmentNumber << "_" << runSign << ".txt";
	ofstream logFile;
	logFile.open(logFilename.str());
	// ������ �������� ��������� ���� ������� ������ �� ���������� ���������
	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			// ���� �� ���� ��������� ������ ����������� ������� (������� �������)
			if ((currentEnvironment - firstEnvironmentNumber) * (lastTryNumber - firstTryNumber + 1) + currentTry - firstTryNumber + 1 <= processesQuantity - 1){
				// ������������ ����� ��������� �������� ��� ������� �������
				int processRankSend = (currentEnvironment - firstEnvironmentNumber) * (lastTryNumber - firstTryNumber + 1) + currentTry - firstTryNumber + 1;
				// ���������� ��������� ��� �������� ��������
				stringstream outStream;
				outStream << currentEnvironment << "E" << currentTry << "T" << runSign << "S";
				char outMessage[messageLentgh];
				outStream >> outMessage;
				MPI_Send(outMessage, messageLentgh - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
				// ���������� � ��� ������ �������
				logFile << "Environment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for process " << processRankSend << endl; 
			} // ���� ��� �������� �������� �������,  �� ���� ���������� ���������� � �� ���� ������ ���������� �������
			else {
				char inputMessage[messageLentgh];
				// ���� �������� ��������� � ���, ��� ������� �������� �������
				MPI_Recv(inputMessage, messageLentgh - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
				// �������������� ��������� �� �������� ��������
				int processRankSend, finishedEnvironment, finishedTry;
				decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry);
				// ���������� � ��� ����� �������
				logFile << "Environment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from process " << processRankSend << endl; 
				// ���������� ��������� � �������� ������� �������� ��������
				stringstream outStream;
				outStream << currentEnvironment << "E" << currentTry << "T" << runSign << "S";
				char outMessage[messageLentgh];
				outStream >> outMessage;
				MPI_Send(outMessage, messageLentgh - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
				// ���������� � ��� ������ �������
				logFile << "Environment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for process " << processRankSend << endl; 
			}
	// ����� ��� ������� �����������, ���� ���� ��� ��� ����� ��������� � �� ���� �������� ���� ��������� ������� � ����������
	int processTillQuit = processesQuantity - 1; // ���������� ��������� ������� ��� ����������� � ���������� ��������� �� ���������
	while (processTillQuit > 0){
		char inputMessage[messageLentgh];
		MPI_Recv(inputMessage, messageLentgh - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
		// �������������� ��������� �� �������� ��������
		int processRankSend, finishedEnvironment, finishedTry;
		decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry);
		// ���������� � ��� ����� �������
		logFile << "Environment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from process " << processRankSend << endl; 
		// ���������� ��������� � ������ � ��������
		char outMessage[messageLentgh];
		strcpy(outMessage, "q");
		MPI_Send(outMessage, messageLentgh - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
		--processTillQuit;
	}

	logFile.close();
}

// ����������� ��������� �� �������� �������� 
void TParallelEvolutionaryProcess::decodeTaskMessage(char inputMessage[], int& currentEnvironment, int& currentTry, string& runSign){
	string tmpString;
	for (unsigned int i=0; i < strlen(inputMessage); ++i)
		if ( ((inputMessage[i] >= '0') && (inputMessage[i] <= '9')) || ((inputMessage[i] >= 'a') && (inputMessage[i] <= 'z')) ) // ���� ������ ����� ��� ��������� �����
			tmpString += inputMessage[i];
		else {
			switch (inputMessage[i]){
				case 'E': // ���� ��� ����� �����
					currentEnvironment = atoi(tmpString.c_str());
					break;
				case 'T': // ���� ��� ����� �������
					currentTry = atoi(tmpString.c_str());
					break;
				case 'S': // ���� ��� ����� ��������
					runSign = tmpString;
					break;
			}
			tmpString = "";
		}
}

// ���������� �������� ��������
void TParallelEvolutionaryProcess::workProcess(int argc, char **argv){
	char inputMessage[messageLentgh];
	MPI_Recv(inputMessage, messageLentgh-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status); // ���� ��������� � ��������
	while (strcmp(inputMessage, "q")){ // ���� �� ���� ������� � ������
		// ���������� ��������� � ��������
		int currentEnvironment, currentTry;
		string runSign;
		decodeTaskMessage(inputMessage, currentEnvironment, currentTry, runSign);
		// ���������� ���������� ���� ������������
		// � ���� ������������� ��������� ����� ����������� ����� ��������, ����� �������� ���������� ���������������� ��������
		unsigned int randomSeed = static_cast<unsigned int>(time(0)) + processRank;
		// ��������� ��������� ������������� �������� � ��������� ���
		TEvolutionaryProcess* evolutionaryProcess = new TEvolutionaryProcess;
		stringstream tmpStream;
		tmpStream << directoriesSettings.environmentDirectory << "/Environment" << currentEnvironment << ".txt";
		evolutionaryProcess->filenameSettings.environmentFilename = tmpStream.str();
		tmpStream.str(""); // ������� �����
		tmpStream << directoriesSettings.resultsDirectory << "/En" << currentEnvironment << "/Results_En" << currentEnvironment << "_" << runSign << "(" << currentTry << ").txt";
		evolutionaryProcess->filenameSettings.resultsFilename = tmpStream.str();
		tmpStream.str(""); // ������� �����
		tmpStream << directoriesSettings.resultsDirectory << "/En" << currentEnvironment << "/En" << currentEnvironment << "_" << runSign << "(" << currentTry << ")_bestpopulation.txt";
		evolutionaryProcess->filenameSettings.bestPopulationFilename = tmpStream.str();
		tmpStream.str(""); // ������� �����
		tmpStream << directoriesSettings.resultsDirectory << "/En" << currentEnvironment << "/En" << currentEnvironment << "_" << runSign << "(" << currentTry << ")_bestagents.txt";
		evolutionaryProcess->filenameSettings.bestAgentsFilename = tmpStream.str();
		evolutionaryProcess->filenameSettings.settingsFilename = settingsFilename;
		evolutionaryProcess->start(randomSeed);
		delete evolutionaryProcess;
		// �������� ����� � ���������� ������ ��� ��������
		tmpStream.str(""); // ������� �����
		tmpStream << currentEnvironment << "e" << currentTry << "t" << processRank << "p";
		char outMessage[messageLentgh];
		tmpStream >> outMessage;
		MPI_Send(outMessage, messageLentgh - 1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD);

		//�������� ������ �������
		MPI_Recv(inputMessage, messageLentgh-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status);
	}
}

// ������ ������������� ��������
void TParallelEvolutionaryProcess::start(int argc, char **argv){
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &processesQuantity); // ����������� ������ ���������� ���������
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // ����������� ��������� ������ ������
	settingsFilename = argv[1]; // � ������ ��������� ������ ���� ������� ���� � ����� ��������
	fillDirectoriesSettings();
	if (processRank == 0) // ���� ��� ������� �������
		rootProcess(argc, argv);
	else // ���� ��� ������� �������
		workProcess(argc, argv);

	MPI_Finalize();
}