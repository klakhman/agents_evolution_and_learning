#define _CRT_SECURE_NO_WARNINGS

#include "TSharedEvolutionaryProcess.h"
#include "mpi.h"
#include <vector>
#include <map>
#include <cstring>
#include <string>
#include <ctime>
#include "settings.h"
#include <sstream>
#include "TPopulation.h"
#include "TAgent.h"

using namespace std;

// ���������� ������ � ��������� � ������ � ���������� �� ��������
string TSharedEvolutionaryProcess::findParameterNote(string inputMessage, string parameterString){
  int featurePosition = 0;
  string parameterNote;
  if ((featurePosition = inputMessage.find(parameterString)) != string::npos){
    int parameterEnd = 0;
    // ���� �� ���������� ��������� ��� ����� ������
    if ((parameterEnd = inputMessage.find("$", featurePosition + parameterString.length())) == string::npos)
      parameterEnd = inputMessage.length();
    parameterNote = inputMessage.substr(featurePosition + parameterString.length(), parameterEnd - featurePosition - parameterString.length());
  }
  return parameterNote;
}

// ����������� ��������� �� �������� �������� ��������
void TSharedEvolutionaryProcess::decodeWorkMessage(const string& workMessage, string& tmpAgentsFilename, int& agentsQuantity){
  tmpAgentsFilename = findParameterNote(workMessage, "$TMPFILE$");
  agentsQuantity = atoi(findParameterNote(workMessage, "$AGQ$").c_str());
}

// ����������� ��������� �� �������� �������� �������� � ������������ �������� (���������� ������� ���� �������)
vector<double> TSharedEvolutionaryProcess::decodeFineshedWorkMessage(const string& finishedWorkMessage, int& process){
  vector<double> rewards;
  // ���������, ��� ������� � ������ �������� � ������� PROCN$R1$R2$R3$...$RN
  process = atoi(finishedWorkMessage.substr(0, finishedWorkMessage.find("$")).c_str());
  unsigned int currentPos = finishedWorkMessage.find("$") + 1;
  while (currentPos <= finishedWorkMessage.length()){
    int nextSepPos = finishedWorkMessage.find("$", currentPos);
    rewards.push_back( (string::npos != nextSepPos) ? atof(finishedWorkMessage.substr(currentPos, nextSepPos-currentPos).c_str()) : 
                                                      atof(finishedWorkMessage.substr(currentPos).c_str()) );
    // �������� ������� �������
    currentPos = (string::npos != nextSepPos) ? (nextSepPos + 1) : (finishedWorkMessage.length() + 1);
  }
  return rewards;
}

// ������� �������
void TSharedEvolutionaryProcess::rootProcess(unsigned int randomSeed){
  // ��������� �����
	if (environment)
		delete environment;
	environment = new THypercubeEnvironment(filenameSettings.environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, filenameSettings.settingsFilename);
  // ������� ���������
  if (agentsPopulation)
		  delete agentsPopulation;
	agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  // ��������� ������ � ��������� ��� ������� (����� ����, ��� ���������� ������ ���������), ������� ����� ��������� � ��� ���������
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  // ��������� ��� ��������� � �������, ������� ����� ������������ ����������� ���������
	agentsPopulation->generateMinimalPopulation(environment->getEnvironmentResolution());
  // ���������� ���� ������ ������� ���� �� ���� � ������� ���� �����������
	ofstream resultsFile;
	createMainResultsFile(resultsFile, randomSeed);
	ofstream bestAgentsFile;
	bestAgentsFile.open(filenameSettings.bestAgentsFilename.c_str());
	// ������� ��������� ������ ��������� (���� ��������� ���������� �������, �� ����������� ��������� ����� ������� ��� ������ ��� ���������� ��������� � ����)
	TPopulation<TAgent>* bestPopulation = new TPopulation<TAgent>;
  // ���������� ���������� � ��������� ��������� � ���� ������� �������� ��������
  // ������������� ������� ��� ������� �� ��������� (���� - ����� ��������, �������� - <����� ������� ������(� 0), ���-�� �������>)
  map< int, pair<int, int> > processesAgents;
  // � ������ (�������) ������� ��� ������� ���������� ���� ������� �� ������� ������ ����� ������� �� ��� ��������
  processesAgents[processesLocalPool[0]] = make_pair(0, agentsPopulation->getPopulationSize()/processesLocalPool.size() +
                                                     agentsPopulation->getPopulationSize()%processesLocalPool.size());
  for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess)
    processesAgents[processesLocalPool[currentProcess]] = 
      make_pair((currentProcess-1)*agentsPopulation->getPopulationSize()/processesLocalPool.size() + processesAgents[processesLocalPool[0]].second,
                agentsPopulation->getPopulationSize()/processesLocalPool.size());
  // ��������� ������������ �������

  stringstream tmpStream;
  for (int currentEvolutionStep = 1; currentEvolutionStep <= agentsPopulation->evolutionSettings.evolutionTime; ++currentEvolutionStep){
    if (currentEvolutionStep != 1) agentsPopulation->generateNextPopulation(currentEvolutionStep);
    // ���������� ��������� ����� c �������� ��� ��������� � �������� ������� ���� ���������
    for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess){
      tmpStream.str("");
      tmpStream << tmpDirectory << "/tmpPop" << processRank << "_" << currentProcess << "_" << randomSeed << ".txt";
      ofstream tmpOutputFile;
      tmpOutputFile.open(tmpStream.str().c_str());
      // ���������� ������� � ���� ��� �������� ��������
      for (int currentAgent = 0; currentAgent < processesAgents[processesLocalPool[currentProcess]].second; ++currentAgent)
        agentsPopulation->getPointertoAgent(currentAgent + processesAgents[processesLocalPool[currentProcess]].first + 1)->uploadGenome(tmpOutputFile);
      tmpOutputFile.close();
      // �������� ������� ��������
      tmpStream.str("");
      tmpStream << "$TMPFILE$" << tmpDirectory << "/tmpPop" << processRank << "_" << currentProcess << ".txt"
              << "$AGQ$" << processesAgents[processesLocalPool[currentProcess]].second;
      strcpy(outputMessage,tmpStream.str().c_str());
      MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[currentProcess], messageType, MPI_COMM_WORLD); 
    }
    // ��������� ���� ����� ������ (������ �������� �������� ��������� � ������)
    for (int currentAgent = 1; currentAgent <= processesAgents[processesLocalPool[0]].second; ++currentAgent){
        TAgent* agent = agentsPopulation->getPointertoAgent(currentAgent); 
		    // �������� ��������� ������������ (���� �����)
		    if (1 == agent->getSystemogenesisMode())
			    agent->primarySystemogenesis();
		    else if (0 == agent->getSystemogenesisMode())
			    agent->linearSystemogenesis();
        else if (2 == agent->getSystemogenesisMode())
          agent->alternativeSystemogenesis();

		    environment->setRandomEnvironmentState();
		    agent->life(*environment, agentsPopulation->evolutionSettings.agentLifetime);
    }
    // ������ ���� ������� �� ������ ���������
    unsigned int processesReturns = 0;
    while (processesReturns < processesLocalPool.size() - 1){
      // ������������ ��������� � ����������� ������
      MPI_Recv(inputMessage, messageLength, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
      string input = inputMessage;
      int process;
      vector<double> rewards = decodeFineshedWorkMessage(input, process);
      for (int currentAgent = processesAgents[processesLocalPool[process]].first+1; 
        currentAgent <= processesAgents[processesLocalPool[process]].first+processesAgents[processesLocalPool[process]].second; ++currentAgent)
        agentsPopulation->getPointertoAgent(currentAgent)->setReward(rewards[currentAgent-processesAgents[processesLocalPool[process]].first-1]);
      ++processesReturns;
    }
    makeLogNote(resultsFile, bestAgentsFile, bestPopulation, currentEvolutionStep);
	}
  // ����������� ������ ���������
	if (!extraPrint) bestPopulation->uploadPopulation(filenameSettings.bestPopulationFilename);
	else	{
				ofstream bestPopulationFile;
				bestPopulationFile.open(filenameSettings.bestPopulationFilename.c_str());
				for (int currentAgent = 1; currentAgent <= bestPopulation->getPopulationSize(); ++ currentAgent)
					bestPopulation->getPointertoAgent(currentAgent)->uploadGenome(bestPopulationFile, extraPrint);
				bestPopulationFile.close();
			}
  delete bestPopulation;
	resultsFile.close();
	bestAgentsFile.close();
  // ������� ��������� ����� � �������� ������ ���� �������� ��������� � ����������
  for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess){
      tmpStream.str("");
      tmpStream << tmpDirectory << "/tmpPop_" << runSign << processRank << "_" << currentProcess << "_" << randomSeed << ".txt";
      remove(tmpStream.str().c_str());
      strcpy(outputMessage, "$Q$");
      MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[currentProcess], messageType, MPI_COMM_WORLD); 
  }
}

// ������� �������
void TSharedEvolutionaryProcess::workProcess(){
  // ��������� �����
	if (environment)
		delete environment;
	environment = new THypercubeEnvironment(filenameSettings.environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, filenameSettings.settingsFilename);
  // ������� ��������� - ������������, ��� ��� ����� ����� - ��� ����� ����� ������
  agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  // ������� ������
  TAgent agent;
  settings::fillAgentSettingsFromFile(agent, filenameSettings.settingsFilename);
  stringstream tmpStream;
  MPI_Recv(inputMessage, messageLength, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
  string input = inputMessage;
  while (input != "$Q$"){
    string tmpFilename;
    int agentsQuantity;
    decodeWorkMessage(input, tmpFilename, agentsQuantity);
    ifstream tmpFile;
    tmpFile.open(tmpFilename.c_str());
    vector<double> rewards; // ������� �������, �� ������� ������������ ������� �������
    for (int currentAgent = 0; currentAgent < agentsQuantity; ++currentAgent){
      agent.loadGenome(tmpFile);
      // �������� ��������� ������������ (���� �����)
		  if (1 == agent.getSystemogenesisMode())
			  agent.primarySystemogenesis();
		  else if (0 == agent.getSystemogenesisMode())
		   agent.linearSystemogenesis();
      else if (2 == agent.getSystemogenesisMode())
        agent.alternativeSystemogenesis();
      environment->setRandomEnvironmentState();
		  agent.life(*environment, agentsPopulation->evolutionSettings.agentLifetime);
      rewards.push_back(agent.getReward());
    }
    tmpFile.close();
    // ���������� ����� �������� �������� � ��������
    tmpStream.str("");
    tmpStream << processRank;
    for (int i=0; i<agentsQuantity; ++i)
      tmpStream << "$" << rewards[i];
    strcpy(outputMessage, tmpStream.str().c_str());
    MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[0], messageType, MPI_COMM_WORLD); 
    // ���� ���������� ��������
    MPI_Recv(inputMessage, messageLength, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
    input = inputMessage;
  }
}

// ������ ������������� �������� (���������� ����� ������������, ���� 0, �� ������������ ���������������� ����������)
void TSharedEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/){
   // ���� �� ���� �������� ����� ������������
	if (!randomSeed) randomSeed = static_cast<unsigned int>(time(0));
	srand(randomSeed);
	// ������� ���������� ��������� �����, ����� �������� ��������
	rand(); rand(); rand();
  MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // ����������� ��������� ������ ������
  if (processRank == processesLocalPool[0])
    rootProcess(randomSeed);
  else 
    workProcess();
}