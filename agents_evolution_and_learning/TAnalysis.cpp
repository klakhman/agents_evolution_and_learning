#define _CRT_SECURE_NO_WARNINGS

#include "TAnalysis.h"

#include "TEnvironment.h"
#include "TPopulation.h"

#include "mpi.h"
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

// �������� ���������� ����� �� �����
void TAnalysis::fillEnvironmentSettingsFromFile(TEnvironment& environment, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	while (settingsFile >> optionString){
		if (optionString == "reward-recovery-time") { settingsFile >> optionString; environment.setRewardRecoveryTime(atoi(optionString.c_str())); }
		if (optionString == "nonstationarity-coefficient") { settingsFile >> optionString; environment.setNonstationarityCoefficient(atof(optionString.c_str())); }
	}
	settingsFile.close();
}

// �������� ���������� ��������� �� �����
void TAnalysis::fillPopulationSettingsFromFile(TPopulation& agentsPopulation, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	while (settingsFile >> optionString){
		if (optionString == "population-size") { settingsFile >> optionString; agentsPopulation.setPopulationSize(atoi(optionString.c_str())); }
		if (optionString == "agent-lifetime") { settingsFile >> optionString; agentsPopulation.evolutionSettings.agentLifetime = atoi(optionString.c_str()); }
		if (optionString == "evolution-time") { settingsFile >> optionString; agentsPopulation.evolutionSettings.evolutionTime = atoi(optionString.c_str()); }
		// ����������� ���������
		if (optionString == "mut-weight-probability") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutWeightProbability = atof(optionString.c_str()); }
		if (optionString == "mut-weight-mean-disp") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutWeightMeanDisp = atof(optionString.c_str()); }
		if (optionString == "mut-weight-disp-disp") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutWeightDispDisp = atof(optionString.c_str()); }
		if (optionString == "dis-limit") { settingsFile >> optionString; agentsPopulation.mutationSettings.disLimit = atoi(optionString.c_str()); }
		if (optionString == "enable-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.enableConnectionProb = atof(optionString.c_str()); }
		if (optionString == "disable-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.disableConnectionProb = atof(optionString.c_str()); }
		if (optionString == "add-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.addConnectionProb = atof(optionString.c_str()); }
		if (optionString == "add-predconnection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.addPredConnectionProb = atof(optionString.c_str()); }
		if (optionString == "delete-connection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.deleteConnectionProb = atof(optionString.c_str()); }
		if (optionString == "delete-predconnection-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.deletePredConnectionProb = atof(optionString.c_str()); }
		if (optionString == "duplicate-pool-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.duplicatePoolProb = atof(optionString.c_str()); }
		if (optionString == "pool-division-coef") { settingsFile >> optionString; agentsPopulation.mutationSettings.poolDivisionCoef = atof(optionString.c_str()); }
		if (optionString == "pool-standart-amount") { settingsFile >> optionString; agentsPopulation.mutationSettings.poolStandartAmount = atoi(optionString.c_str()); }
		if (optionString == "connection-standart-amount") { settingsFile >> optionString; agentsPopulation.mutationSettings.connectionStandartAmount = atoi(optionString.c_str()); }
		if (optionString == "mut-develop-con-prob-prob") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutDevelopConProbProb = atof(optionString.c_str()); }
		if (optionString == "mut-develop-con-prob-disp") { settingsFile >> optionString; agentsPopulation.mutationSettings.mutDevelopConProbDisp = atof(optionString.c_str()); }
	}
	settingsFile.close();
}

// �������� ���������� ������ �� �����
void TAnalysis::fillAgentSettingsFromFile(TPopulation& agentsPopulation, string settingsFilename){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	TAgent::SPrimarySystemogenesisSettings* primarySystemogenesisSettings = new TAgent::SPrimarySystemogenesisSettings;
	while (settingsFile >> optionString){
		if (optionString == "initial-pool-capacity") { settingsFile >> optionString; primarySystemogenesisSettings->initialPoolCapacity = atoi(optionString.c_str()); }
		if (optionString == "initial-develop-synapse-probability") { settingsFile >> optionString; primarySystemogenesisSettings->initialDevelopSynapseProbability = atof(optionString.c_str()); }
		if (optionString == "initial-develop-predconnection-probability") { settingsFile >> optionString; primarySystemogenesisSettings->initialDevelopPredConnectionProbability = atof(optionString.c_str()); }
		if (optionString == "primary-systemogenesis-time") { settingsFile >> optionString; primarySystemogenesisSettings->primarySystemogenesisTime = atoi(optionString.c_str()); }
		if (optionString == "spontaneous-activity-prob") { settingsFile >> optionString; primarySystemogenesisSettings->spontaneousActivityProb = atof(optionString.c_str()); }
		if (optionString == "active-neurons-percent") { settingsFile >> optionString; primarySystemogenesisSettings->activeNeuronsPercent = atof(optionString.c_str()); }
		if (optionString == "synapses-activity-treshold") { settingsFile >> optionString; primarySystemogenesisSettings->synapsesActivityTreshold = atof(optionString.c_str()); }
		if (optionString == "significance-treshold") { settingsFile >> optionString; primarySystemogenesisSettings->significanceTreshold = atof(optionString.c_str()); }
	}
	for (int currentAgent = 1; currentAgent <= agentsPopulation.getPopulationSize(); ++currentAgent)
	{
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialPoolCapacity = primarySystemogenesisSettings->initialPoolCapacity;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialDevelopSynapseProbability = primarySystemogenesisSettings->initialDevelopSynapseProbability;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.initialDevelopPredConnectionProbability = primarySystemogenesisSettings->initialDevelopPredConnectionProbability;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.primarySystemogenesisTime = primarySystemogenesisSettings->primarySystemogenesisTime;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.spontaneousActivityProb = primarySystemogenesisSettings->spontaneousActivityProb;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.activeNeuronsPercent = primarySystemogenesisSettings->activeNeuronsPercent;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.synapsesActivityTreshold = primarySystemogenesisSettings->synapsesActivityTreshold;
		agentsPopulation.getPointertoAgent(currentAgent)->primarySystemogenesisSettings.significanceTreshold = primarySystemogenesisSettings->significanceTreshold;
	}
	delete primarySystemogenesisSettings;
	settingsFile.close();
}

// ������ ��������� ������� ����� ������� ������ ��������� (���������� ������� �������� ������� �� ��������� ����� ������� ���� ������� �� ���� ���������)
double TAnalysis::startBestPopulationAnalysis(string bestPopulationFilename, string environmentFilename, string settingsFilename, unsigned int randomSeed /*=0*/){
	// ���� �� ���� �������� ����� ������������
	if (!randomSeed)
		randomSeed = static_cast<unsigned int>(time(0));
	srand(randomSeed);
	// ������� ���������� ��������� �����, ����� �������� ��������
	rand();
	rand();
	rand();
	TEnvironment* environment = new TEnvironment(environmentFilename);
	fillEnvironmentSettingsFromFile(*environment, settingsFilename);
	//!!! �������� ������� �������������� ����� (����� ��� ���� ���������������)
	environment->setNonstationarityCoefficient(0.0);
	TPopulation* agentsPopulation = new TPopulation;
	fillPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	// ��������� ������ � ��������� ��� ������� (����� ����, ��� ���������� ������ ���������), ������� ����� ��������� � ��� ���������
	fillAgentSettingsFromFile(*agentsPopulation, settingsFilename);
	agentsPopulation->loadPopulation(bestPopulationFilename);
	// ������� ������ ������ ���� ������� ��� ������� �� ���� ��������� ��������� (����� ���� ���, �� ����� ���� ��������� ����� ������� ������)
	int intitalStatesQuantity = environment->getInitialStatesQuantity();
	double** agentsRewards = new double*[agentsPopulation->getPopulationSize()];
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent)
		agentsRewards[currentAgent - 1] = new double[intitalStatesQuantity];
	double maxReward = 0.0;
	long double averageReward = 0.0;
	// ��������� ���� ������� � ���������� ������� � ������
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent)
		for (int currentInitialState = 0; currentInitialState < intitalStatesQuantity; ++currentInitialState){
			environment->setEnvironmentState(currentInitialState);
			agentsPopulation->getPointertoAgent(currentAgent)->linearSystemogenesis();
			agentsPopulation->getPointertoAgent(currentAgent)->life(*environment, agentsPopulation->evolutionSettings.agentLifetime);
			agentsRewards[currentAgent - 1][currentInitialState] = agentsPopulation->getPointertoAgent(currentAgent)->getReward();
			
			averageReward += agentsRewards[currentAgent - 1][currentInitialState] / (agentsPopulation->getPopulationSize() *  intitalStatesQuantity);
			if (agentsRewards[currentAgent - 1][currentInitialState] > maxReward)
				maxReward = agentsRewards[currentAgent - 1][currentInitialState];
			//cout << currentAgent << " : " << currentInitialState << " - " << agentsRewards[currentAgent - 1][currentInitialState] << endl;
		}
	// ������� ��� ����������
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent)
		delete []agentsRewards[currentAgent - 1];
	delete []agentsRewards;
	delete agentsPopulation;
	delete environment;
	
	return static_cast<double>(averageReward);
}


// ---------------- ��������� ������������� ������� �� ������ ���������� � ������ ������� -----------------------
// ���������� ���������� ���������� ��� ������ ������
void TAnalysis::fillDirectoriesSettings(string settingsFilename, string& workDirectory, string& environmentDirectory, string& resultsDirectory){
	string optionString;
	ifstream settingsFile;
	settingsFile.open(settingsFilename.c_str());
	while (settingsFile >> optionString){
		if (optionString == "work-directory") { settingsFile >> workDirectory; }
		if (optionString == "environment-directory") { settingsFile >> environmentDirectory; }
		if (optionString == "results-directory") { settingsFile >> resultsDirectory; }
	}
	settingsFile.close();
}

// ����������� ��������� ��������� ������
void TAnalysis::decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, string& runSign){
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
void TAnalysis::decodeFinishedWorkMessage(char inputMessage[], int& processRankSend, int& finishedEnvironment, int& finishedTry, double& averageReward){
	string tmpString;
	for (unsigned int i=0; i < strlen(inputMessage); ++i)
		if ( ((inputMessage[i] >= '0') && (inputMessage[i] <= '9')) || (inputMessage[i] == '.')) // ���� ������ ����� ��� �����
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
				case 'r': // ���� ��� ������� �������
					averageReward = atof(tmpString.c_str());
					break;
			}
			tmpString = "";
		}
}

// ���������� ������������ �������� (�����, ����� ���������� ����� ������� �� ���� ������ ���-�� ���������� ���������!!!)
void TAnalysis::rootProcess(int argc, char **argv){
	int processesQuantity;	
	MPI_Comm_size(MPI_COMM_WORLD, &processesQuantity); // ����������� ������ ���������� ���������
	MPI_Status status;
	const int messageLength = 100;
	const int messageType = 99;

	string settingsFilename = argv[1];
	string workDirectory, environmentDirectory, resultsDirectory;
	fillDirectoriesSettings(settingsFilename, workDirectory, environmentDirectory, resultsDirectory);

	unsigned long startTime = static_cast<unsigned long>(time(0)); // ����� ������ �������� �������
	int firstEnvironmentNumber; // �������� ������� ����
	int lastEnvironmentNumber;
	int firstTryNumber; // �������� �������
	int lastTryNumber;
	string runSign; // ��������� ������������� ������� ������� ����������� ������ ���������� ��� ������ ���������
	decodeCommandPromt(argc, argv, firstEnvironmentNumber, lastEnvironmentNumber, firstTryNumber, lastTryNumber, runSign);
	// ������� ���� � �����
	stringstream logFilename;
	logFilename << workDirectory << "/Analysis_run_log_En" << firstEnvironmentNumber << "-" << lastEnvironmentNumber << "_" << runSign << ".txt";
	ofstream logFile;
	logFile.open(logFilename.str().c_str());
	// ������� ������ �� ����� �������� ��������� �� ��������
	double** averageRewards = new double*[lastEnvironmentNumber - firstEnvironmentNumber + 1];
	for (int currentEnvironment = 1; currentEnvironment <= lastEnvironmentNumber - firstEnvironmentNumber + 1; ++currentEnvironment)
		averageRewards[currentEnvironment - 1] = new double[lastTryNumber - firstTryNumber + 1];
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
				char outMessage[messageLength];
				outStream >> outMessage;
				MPI_Send(outMessage, messageLength - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
				// ���������� � ��� ������ �������
				unsigned long currentTime = static_cast<unsigned long>(time(0));
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60) 
					<< "\tEnvironment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for process " << processRankSend << endl; 
			} // ���� ��� �������� �������� �������,  �� ���� ���������� ���������� � �� ���� ������ ���������� �������
			else {
				char inputMessage[messageLength];
				// ���� �������� ��������� � ���, ��� ������� �������� �������
				MPI_Recv(inputMessage, messageLength - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
				// �������������� ��������� �� �������� ��������
				int processRankSend, finishedEnvironment, finishedTry;
				double averageReward;
				decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry, averageReward);
				averageRewards[finishedEnvironment - firstEnvironmentNumber][finishedTry - firstTryNumber] = averageReward;
				// ���������� � ��� ����� �������
				unsigned long currentTime = static_cast<unsigned long>(time(0));
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60)
					<< "\tEnvironment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from process " << processRankSend << endl; 
				// ���������� ��������� � �������� ������� �������� ��������
				stringstream outStream;
				outStream << currentEnvironment << "E" << currentTry << "T" << runSign << "S";
				char outMessage[messageLength];
				outStream >> outMessage;
				MPI_Send(outMessage, messageLength - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
				// ���������� � ��� ������ �������
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60)
					<< "\tEnvironment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for process " << processRankSend << endl; 
			}
	// ����� ��� ������� �����������, ���� ���� ��� ��� ����� ��������� � �� ���� �������� ���� ��������� ������� � ����������
	int processTillQuit = processesQuantity - 1; // ���������� ��������� ������� ��� ����������� � ���������� ��������� �� ���������
	while (processTillQuit > 0){
		char inputMessage[messageLength];
		MPI_Recv(inputMessage, messageLength - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
		// �������������� ��������� �� �������� ��������
		int processRankSend, finishedEnvironment, finishedTry;
		double averageReward;
		decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry, averageReward);
		averageRewards[finishedEnvironment - firstEnvironmentNumber][finishedTry - firstTryNumber] = averageReward;
		// ���������� � ��� ����� �������
		unsigned long currentTime = static_cast<unsigned long>(time(0));
		logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60) 
			<< "Environment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from process " << processRankSend << endl; 
		// ���������� ��������� � ������ � ��������
		char outMessage[messageLength];
		strcpy(outMessage, "q");
		MPI_Send(outMessage, messageLength - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
		--processTillQuit;
	}
	logFile.close();
	// ���������� ���� � ������� ������� �� ���� �������� � ����������� �� ������
	stringstream analysisResultFilename;
	analysisResultFilename << resultsDirectory << "/Analysis/BestPopulation_analysis_En" << firstEnvironmentNumber << "-" << lastEnvironmentNumber << "_" << runSign << ".txt";
	ofstream analysisResultFile;
	analysisResultFile.open(analysisResultFilename.str().c_str());

	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			analysisResultFile << "Environment�" << currentEnvironment << "\tTry�" << currentTry << "\t" << averageRewards[currentEnvironment - firstEnvironmentNumber][currentTry - firstTryNumber] << endl;
	analysisResultFile << endl << endl;

	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment){
		double averageEnvironmentReward = 0.0;
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			averageEnvironmentReward += averageRewards[currentEnvironment - firstEnvironmentNumber][currentTry - firstTryNumber] / (lastTryNumber - firstTryNumber + 1);
		analysisResultFile << "Environment�" << currentEnvironment << "\t" << averageEnvironmentReward << endl;
	}
	analysisResultFile.close();
	for (int currentEnvironment = 1; currentEnvironment <= lastEnvironmentNumber - firstEnvironmentNumber + 1; ++currentEnvironment)
		delete []averageRewards[currentEnvironment - 1];
	delete []averageRewards;
}

// ����������� ��������� �� �������� �������� 
void TAnalysis::decodeTaskMessage(char inputMessage[], int& currentEnvironment, int& currentTry, string& runSign){
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
void TAnalysis::workProcess(int argc, char **argv){
	int processRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // ����������� ��������� ������ ������
	int processesQuantity;	
	MPI_Comm_size(MPI_COMM_WORLD, &processesQuantity); // ����������� ������ ���������� ���������
	MPI_Status status;
	const int messageLength = 100;
	const int messageType = 99;

	string settingsFilename = argv[1];
	string workDirectory, environmentDirectory, resultsDirectory;
	fillDirectoriesSettings(settingsFilename, workDirectory, environmentDirectory, resultsDirectory);

	char inputMessage[messageLength];
	MPI_Recv(inputMessage, messageLength-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status); // ���� ��������� � ��������
	while (strcmp(inputMessage, "q")){ // ���� �� ���� ������� � ������
		// ���������� ��������� � ��������
		int currentEnvironment, currentTry;
		string runSign;
		decodeTaskMessage(inputMessage, currentEnvironment, currentTry, runSign);
		// ���������� ���������� ���� ������������
		// � ���� ������������� ��������� ����� ����������� ����� ��������, ����� �������� ���������� ���������������� ��������
		unsigned int randomSeed = static_cast<unsigned int>(time(0)) + processRank;
		// ���������� ��������� ������� � ��������� ������ �� ����� ���������
		stringstream tmpStream;
		tmpStream << environmentDirectory << "/Environment" << currentEnvironment << ".txt";
		string environmentFilename = tmpStream.str();
		tmpStream.str(""); // ������� �����
		tmpStream << resultsDirectory << "/En" << currentEnvironment << "/En" << currentEnvironment << "_" << runSign << "(" << currentTry << ")_bestpopulation.txt";
		string bestPopulationFilename = tmpStream.str();
		double averageReward = startBestPopulationAnalysis(bestPopulationFilename, environmentFilename, settingsFilename,randomSeed);
		// �������� ����� � ���������� ������ ��� ��������
		tmpStream.str(""); // ������� �����
		tmpStream << currentEnvironment << "e" << currentTry << "t" << processRank << "p" << averageReward << "r";
		char outMessage[messageLength];
		tmpStream >> outMessage;
		MPI_Send(outMessage, messageLength - 1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD);

		//�������� ������ �������
		MPI_Recv(inputMessage, messageLength-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status);
	}
}

// ������ ������������� �������� ������� �� ������ ������ ��������� � ������ ������� (!!!� ����� � ������������ ������ ���� ����� "/Analysis"!!!)
void TAnalysis::startParallelBestPopulationAnalysis(int argc, char **argv){
	MPI_Init(&argc, &argv);
	int processRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // ����������� ��������� ������ ������
	if (processRank == 0) // ���� ��� ������� �������
		rootProcess(argc, argv);
	else // ���� ��� ������� �������
		workProcess(argc, argv);

	MPI_Finalize();
}