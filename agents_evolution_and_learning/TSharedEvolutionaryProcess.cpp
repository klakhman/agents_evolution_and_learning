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

// Нахождение записи о параметре в строке с сообщением от процесса
string TSharedEvolutionaryProcess::findParameterNote(string inputMessage, string parameterString){
  int featurePosition = 0;
  string parameterNote;
  if ((featurePosition = inputMessage.find(parameterString)) != string::npos){
    int parameterEnd = 0;
    // Ищем до следующего параметра или конца строки
    if ((parameterEnd = inputMessage.find("$", featurePosition + parameterString.length())) == string::npos)
      parameterEnd = inputMessage.length();
    parameterNote = inputMessage.substr(featurePosition + parameterString.length(), parameterEnd - featurePosition - parameterString.length());
  }
  return parameterNote;
}

// Расшифровка сообщения от рутового процесса рабочему
void TSharedEvolutionaryProcess::decodeWorkMessage(const string& workMessage, string& tmpAgentsFilename, int& agentsQuantity){
  tmpAgentsFilename = findParameterNote(workMessage, "$TMPFILE$");
  agentsQuantity = atoi(findParameterNote(workMessage, "$AGQ$").c_str());
}

// Расшифровка сообщения от рабочего процесса рутовому с результатами обсчетов (возвращает награду всех агентов)
vector<double> TSharedEvolutionaryProcess::decodeFineshedWorkMessage(const string& finishedWorkMessage, int& process){
  vector<double> rewards;
  // Считается, что награды в агенте записаны в формате PROCN$R1$R2$R3$...$RN
  process = atoi(finishedWorkMessage.substr(0, finishedWorkMessage.find("$")).c_str());
  unsigned int currentPos = finishedWorkMessage.find("$") + 1;
  while (currentPos <= finishedWorkMessage.length()){
    int nextSepPos = finishedWorkMessage.find("$", currentPos);
    rewards.push_back( (string::npos != nextSepPos) ? atof(finishedWorkMessage.substr(currentPos, nextSepPos-currentPos).c_str()) : 
                                                      atof(finishedWorkMessage.substr(currentPos).c_str()) );
    // Сдвигаем текущую позциию
    currentPos = (string::npos != nextSepPos) ? (nextSepPos + 1) : (finishedWorkMessage.length() + 1);
  }
  return rewards;
}

// Рутовый процесс
void TSharedEvolutionaryProcess::rootProcess(unsigned int randomSeed){
  // Загружаем среду
	if (environment)
		delete environment;
	environment = new THypercubeEnvironment(filenameSettings.environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, filenameSettings.settingsFilename);
  // Создаем популяцию
  if (agentsPopulation)
		  delete agentsPopulation;
	agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  // Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  // Настройки уже загружены в агентов, поэтому можем генерировать минимальную популяцию
	agentsPopulation->generateMinimalPopulation(environment->getEnvironmentResolution());
  // Опустошаем файл лучших агентов если он есть и создаем файл результатов
	ofstream resultsFile;
	createMainResultsFile(resultsFile, randomSeed);
	ofstream bestAgentsFile;
	bestAgentsFile.open(filenameSettings.bestAgentsFilename.c_str());
	// Создаем структуру лучшей популяции (если процессор достаточно быстрый, то копирование популяций будет быстрее чем каждый раз записывать популяцию в файл)
	TPopulation<TAgent>* bestPopulation = new TPopulation<TAgent>;
  // Вычисление информации о различных процессах в пуле данного рутового процесса
  // Распределение агентов для обсчета по процессам (ключ - номер процесса, значение - <номер первого агента(с 0), кол-во агентов>)
  map< int, pair<int, int> > processesAgents;
  // В первый (рутовый) процесс для обсчета помещается весь остаток от деления общего числа агентов на все процессы
  processesAgents[processesLocalPool[0]] = make_pair(0, agentsPopulation->getPopulationSize()/processesLocalPool.size() +
                                                     agentsPopulation->getPopulationSize()%processesLocalPool.size());
  for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess)
    processesAgents[processesLocalPool[currentProcess]] = 
      make_pair((currentProcess-1)*agentsPopulation->getPopulationSize()/processesLocalPool.size() + processesAgents[processesLocalPool[0]].second,
                agentsPopulation->getPopulationSize()/processesLocalPool.size());
  // Запускаем эволюционный процесс

  stringstream tmpStream;
  for (int currentEvolutionStep = 1; currentEvolutionStep <= agentsPopulation->evolutionSettings.evolutionTime; ++currentEvolutionStep){
    if (currentEvolutionStep != 1) agentsPopulation->generateNextPopulation(currentEvolutionStep);
    // Записываем временные файлы c агентами для процессов и высылаем задание всем процессам
    for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess){
      tmpStream.str("");
      tmpStream << tmpDirectory << "/tmpPop" << processRank << "_" << currentProcess << "_" << randomSeed << ".txt";
      ofstream tmpOutputFile;
      tmpOutputFile.open(tmpStream.str().c_str());
      // Записываем агентов в файл для рабочего процесса
      for (int currentAgent = 0; currentAgent < processesAgents[processesLocalPool[currentProcess]].second; ++currentAgent)
        agentsPopulation->getPointertoAgent(currentAgent + processesAgents[processesLocalPool[currentProcess]].first + 1)->uploadGenome(tmpOutputFile);
      tmpOutputFile.close();
      // Посылаем задание процессу
      tmpStream.str("");
      tmpStream << "$TMPFILE$" << tmpDirectory << "/tmpPop" << processRank << "_" << currentProcess << ".txt"
              << "$AGQ$" << processesAgents[processesLocalPool[currentProcess]].second;
      strcpy(outputMessage,tmpStream.str().c_str());
      MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[currentProcess], messageType, MPI_COMM_WORLD); 
    }
    // Выполняем свою часть работы (агенты рутового процесса находятся в начале)
    for (int currentAgent = 1; currentAgent <= processesAgents[processesLocalPool[0]].second; ++currentAgent){
        TAgent* agent = agentsPopulation->getPointertoAgent(currentAgent); 
		    // Проводим первичный системогенез (если нужно)
		    if (1 == agent->getSystemogenesisMode())
			    agent->primarySystemogenesis();
		    else if (0 == agent->getSystemogenesisMode())
			    agent->linearSystemogenesis();
        else if (2 == agent->getSystemogenesisMode())
          agent->alternativeSystemogenesis();

		    environment->setRandomEnvironmentState();
		    agent->life(*environment, agentsPopulation->evolutionSettings.agentLifetime);
    }
    // Теперь ждем сообщей от других процессов
    unsigned int processesReturns = 0;
    while (processesReturns < processesLocalPool.size() - 1){
      // Обрабатываем сообщение о выполненной работе
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
  // Засписываем лучшую популяцию
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
  // Удаляем временные файлы и посылаем сигнал всем дочерним процессам о завершении
  for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess){
      tmpStream.str("");
      tmpStream << tmpDirectory << "/tmpPop_" << runSign << processRank << "_" << currentProcess << "_" << randomSeed << ".txt";
      remove(tmpStream.str().c_str());
      strcpy(outputMessage, "$Q$");
      MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[currentProcess], messageType, MPI_COMM_WORLD); 
  }
}

// Рабочий процесс
void TSharedEvolutionaryProcess::workProcess(){
  // Загружаем среду
	if (environment)
		delete environment;
	environment = new THypercubeEnvironment(filenameSettings.environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, filenameSettings.settingsFilename);
  // Создаем популяцию - единственное, что нам нужно знать - это время жизни агента
  agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  // Создаем агента
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
    vector<double> rewards; // Награды агентов, за которые ответственен текущий процесс
    for (int currentAgent = 0; currentAgent < agentsQuantity; ++currentAgent){
      agent.loadGenome(tmpFile);
      // Проводим первичный системогенез (если нужно)
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
    // Составляем ответ рутовому процессу и посылаем
    tmpStream.str("");
    tmpStream << processRank;
    for (int i=0; i<agentsQuantity; ++i)
      tmpStream << "$" << rewards[i];
    strcpy(outputMessage, tmpStream.str().c_str());
    MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[0], messageType, MPI_COMM_WORLD); 
    // Ждем дальнейших указаний
    MPI_Recv(inputMessage, messageLength, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
    input = inputMessage;
  }
}

// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
void TSharedEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/){
   // Если не было передано зерно рандомизации
	if (!randomSeed) randomSeed = static_cast<unsigned int>(time(0));
	srand(randomSeed);
	// Запуски генератора случайных чисел, чтобы развести значения
	rand(); rand(); rand();
  MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // Определение процессом своего номера
  if (processRank == processesLocalPool[0])
    rootProcess(randomSeed);
  else 
    workProcess();
}