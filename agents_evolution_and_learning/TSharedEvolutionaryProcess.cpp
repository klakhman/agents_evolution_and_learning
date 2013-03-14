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
	settings::fillEnvironmentSettingsFromFile(*dynamic_cast<THypercubeEnvironment*>(environment), filenameSettings.settingsFilename);
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
    // Высылаем задание (данные) всем процессам
    for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess){
      // Указываем, что задание началось
      strcpy(outputMessage, "$START$");
      MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[currentProcess], messageType, MPI_COMM_WORLD);
      // Пересылаем кол-во агентов
      MPI_Send(&(processesAgents[processesLocalPool[currentProcess]].second), 1, MPI_INT, processesLocalPool[currentProcess], messageType, MPI_COMM_WORLD);
      // Записываем агентов в файл для рабочего процесса
      for (int currentAgent = 0; currentAgent < processesAgents[processesLocalPool[currentProcess]].second; ++currentAgent)
        sendAgentGenomeViaMPI(*(agentsPopulation->getPointertoAgent(currentAgent + processesAgents[processesLocalPool[currentProcess]].first + 1)->getPointerToAgentGenome()),
                              processesLocalPool[currentProcess]); 
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
      for (int currentAgent = processesAgents[process].first+1; currentAgent <= processesAgents[process].first+processesAgents[process].second; ++currentAgent)
        agentsPopulation->getPointertoAgent(currentAgent)->setReward(rewards[currentAgent-processesAgents[process].first-1]);
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
  strcpy(outputMessage, "$Q$");
  // Посылаем сигнал всем дочерним процессам о завершении
  for (unsigned int currentProcess = 1; currentProcess < processesLocalPool.size(); ++currentProcess)
      MPI_Send(outputMessage, messageLength, MPI_CHAR, processesLocalPool[currentProcess], messageType, MPI_COMM_WORLD); 
}

// Рабочий процесс
void TSharedEvolutionaryProcess::workProcess(){
  // Загружаем среду
	if (environment)
		delete environment;
	environment = new THypercubeEnvironment(filenameSettings.environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*dynamic_cast<THypercubeEnvironment*>(environment), filenameSettings.settingsFilename);
  // Создаем популяцию
  agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, filenameSettings.settingsFilename);
  stringstream tmpStream;
  MPI_Recv(inputMessage, messageLength, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
  string input = inputMessage;
  while (input != "$Q$"){
    // Принимаем в начале количество агентов
    int agentsQuantity;
    MPI_Recv(&agentsQuantity, 1, MPI_INT, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
    for (int currentAgent = 1; currentAgent <= agentsQuantity; ++currentAgent)
      receiveAgentGenomeViaMPI(*(agentsPopulation->getPointertoAgent(currentAgent)->getPointerToAgentGenome()));

    vector<double> rewards; // Награды агентов, за которые ответственен текущий процесс
    for (int currentAgent = 0; currentAgent < agentsQuantity; ++currentAgent){
      TAgent& agent = *(agentsPopulation->getPointertoAgent(currentAgent+1));
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

// Изначальное конструирование всех "сырых" представлений в нотации MPI для сложного типа TPoolNetwork
void TSharedEvolutionaryProcess::constructMPIDataTypes(){
  // Сначала строим тип пула
  int pool_blocklens[5]; // Длина каждого блока данных  
  MPI_Aint pool_indices[5]; // Смещение каждого блока данных
  MPI_Datatype pool_old_types[5]; // Базовый тип блока
  for (int i=0; i<5; ++i) pool_blocklens[i] = 1;
  pool_old_types[0] = MPI_INT; pool_old_types[1] = MPI_INT; pool_old_types[2] = MPI_DOUBLE;
  pool_old_types[3] = MPI_DOUBLE; pool_old_types[4] = MPI_INT;
  // Подсчитываем смещения
  _mpi_pool tmp_pool;
  MPI_Address(&(tmp_pool.type), &(pool_indices[0]));
  MPI_Address(&(tmp_pool.capacity), &(pool_indices[1]));
  MPI_Address(&(tmp_pool.biasMean), &(pool_indices[2]));
  MPI_Address(&(tmp_pool.biasVariance), &(pool_indices[3]));
  MPI_Address(&(tmp_pool.layer), &(pool_indices[4]));
  for (int i=5; i>=0; --i)
    pool_indices[i] -= pool_indices[0]; 
  // Создаем типа данных и регистрируем его
  MPI_Type_struct(5, pool_blocklens, pool_indices, pool_old_types, &MPI_POOL);
  MPI_Type_commit(&MPI_POOL);
  
  //Строим тип связи между пулами
  int connection_blocklens[8]; // Длина каждого блока данных  
  MPI_Aint connection_indices[8]; // Смещение каждого блока данных
  MPI_Datatype connection_old_types[8]; // Базовый тип блока
  for (int i=0; i<8; ++i) connection_blocklens[i] = 1;
  connection_old_types[0] = MPI_INT; connection_old_types[1] = MPI_INT; connection_old_types[2] = MPI_DOUBLE;
  connection_old_types[3] = MPI_DOUBLE; connection_old_types[4] = MPI_INT; connection_old_types[5] = MPI_INT;
  connection_old_types[6] = MPI_DOUBLE; connection_old_types[7] = MPI_LONG;
  // Подсчитываем смещения
  _mpi_connection tmp_connection;
  MPI_Address(&(tmp_connection.prePoolID), &(connection_indices[0]));
  MPI_Address(&(tmp_connection.postPoolID), &(connection_indices[1]));
  MPI_Address(&(tmp_connection.weightMean), &(connection_indices[2]));
  MPI_Address(&(tmp_connection.weightVariance), &(connection_indices[3]));
  MPI_Address(&(tmp_connection.enabled), &(connection_indices[4]));
  MPI_Address(&(tmp_connection.disabledStep), &(connection_indices[5]));
  MPI_Address(&(tmp_connection.developSynapseProb), &(connection_indices[6]));
  MPI_Address(&(tmp_connection.innovationNumber), &(connection_indices[7]));
  for (int i=7; i>=0; --i)
    connection_indices[i] -= connection_indices[0]; 
  // Создаем типа данных и регистрируем его
  MPI_Type_struct(8, connection_blocklens, connection_indices, connection_old_types, &MPI_CONNECTION);
  MPI_Type_commit(&MPI_CONNECTION);

  //Строим тип предикторной связи между пулами
  int predconnection_blocklens[6]; // Длина каждого блока данных  
  MPI_Aint predconnection_indices[6]; // Смещение каждого блока данных
  MPI_Datatype predconnection_old_types[6]; // Базовый тип блока
  for (int i=0; i<6; ++i) predconnection_blocklens[i] = 1;
  predconnection_old_types[0] = MPI_INT; predconnection_old_types[1] = MPI_INT;
  predconnection_old_types[2] = MPI_INT; predconnection_old_types[3] = MPI_INT;
  predconnection_old_types[4] = MPI_DOUBLE; predconnection_old_types[5] = MPI_LONG;
  // Подсчитываем смещения
  _mpi_predconnection tmp_predconnection;
  MPI_Address(&(tmp_predconnection.prePoolID), &(predconnection_indices[0]));
  MPI_Address(&(tmp_predconnection.postPoolID), &(predconnection_indices[1]));
  MPI_Address(&(tmp_predconnection.enabled), &(predconnection_indices[2]));
  MPI_Address(&(tmp_predconnection.disabledStep), &(predconnection_indices[3]));
  MPI_Address(&(tmp_predconnection.developPredConnectionProb), &(predconnection_indices[4]));
  MPI_Address(&(tmp_predconnection.innovationNumber), &(predconnection_indices[5]));
  for (int i=5; i>=0; --i)
    predconnection_indices[i] -= predconnection_indices[0]; 
  // Создаем типа данных и регистрируем его
  MPI_Type_struct(6, predconnection_blocklens, predconnection_indices, predconnection_old_types, &MPI_PREDCONNECTION);
  MPI_Type_commit(&MPI_PREDCONNECTION);
}

// Отправка генома удаленному процессу по MPI
void TSharedEvolutionaryProcess::sendAgentGenomeViaMPI(const TPoolNetwork& genome, int processSendRank){
  int poolsQuantity = genome.getPoolsQuantity();
  // Сначала посылаем кол-во пулов в сети
  MPI_Send(&poolsQuantity, 1, MPI_INT, processSendRank, messageType, MPI_COMM_WORLD);
  // Записываем информацию о пулах в специальный формат для пересылки
  _mpi_pool* pools = new _mpi_pool[poolsQuantity];
  // Также для оптимизации собираем информацию о максимальном количестве связей и предикторных связей у пулов
  int maxConnectionsQuantity = 0;
  int maxPredConnectionsQuantity = 0;
  for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
    pools[currentPool - 1].type = genome.getPoolType(currentPool); pools[currentPool - 1].capacity = genome.getPoolCapacity(currentPool);
    pools[currentPool - 1].biasMean = genome.getPoolBiasMean(currentPool); pools[currentPool - 1].biasVariance = genome.getPoolBiasVariance(currentPool);
    pools[currentPool - 1].layer = genome.getPoolLayer(currentPool);
    maxConnectionsQuantity = std::max(maxConnectionsQuantity, genome.getPoolInputConnectionsQuantity(currentPool));
    maxPredConnectionsQuantity = std::max(maxPredConnectionsQuantity, genome.getPoolInputPredConnectionsQuantity(currentPool));
  }
  MPI_Send(pools, poolsQuantity, MPI_POOL, processSendRank, messageType, MPI_COMM_WORLD);
  delete []pools;
  // Посылаем все связи
  // Сначала посылаем максимальное кол-во связей (для оптимизации выделения памяти в куче)
  MPI_Send(&maxConnectionsQuantity, 1, MPI_INT, processSendRank, messageType, MPI_COMM_WORLD);
  if (maxConnectionsQuantity){ // Если в сети вообще есть связи
    _mpi_connection* connections = new _mpi_connection[maxConnectionsQuantity];
    for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
      int poolConnectionsQuantity = genome.getPoolInputConnectionsQuantity(currentPool);
      // Посылаем кол-во связей текущего пула и потом подгатавливаем специальную структуру данных и посылаем
      MPI_Send(&poolConnectionsQuantity, 1, MPI_INT, processSendRank, messageType, MPI_COMM_WORLD);
      if (poolConnectionsQuantity){
        for (int currentConnection = 1; currentConnection <= poolConnectionsQuantity; ++currentConnection){
          connections[currentConnection - 1].prePoolID = genome.getConnectionPrePoolID(currentPool, currentConnection);
          connections[currentConnection - 1].postPoolID = genome.getConnectionPostPoolID(currentPool, currentConnection);
          connections[currentConnection - 1].weightMean = genome.getConnectionWeightMean(currentPool, currentConnection);
          connections[currentConnection - 1].weightVariance = genome.getConnectionWeightVariance(currentPool, currentConnection);
          connections[currentConnection - 1].enabled = genome.getConnectionEnabled(currentPool, currentConnection);
          connections[currentConnection - 1].disabledStep = genome.getConnectionDisabledStep(currentPool, currentConnection);
          connections[currentConnection - 1].developSynapseProb = genome.getConnectionDevelopSynapseProb(currentPool, currentConnection);
          connections[currentConnection - 1].innovationNumber = genome.getConnectionInnovationNumber(currentPool, currentConnection);
        }
        MPI_Send(connections, poolConnectionsQuantity, MPI_CONNECTION, processSendRank, messageType, MPI_COMM_WORLD);
      }
    }
    delete []connections;
  }
  // Таким же образом посылаем все предикторные связи
  MPI_Send(&maxPredConnectionsQuantity, 1, MPI_INT, processSendRank, messageType, MPI_COMM_WORLD);
  if (maxPredConnectionsQuantity){
    _mpi_predconnection* predConnections = new _mpi_predconnection[maxPredConnectionsQuantity];
    for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
      int poolPredConnectionsQuantity = genome.getPoolInputPredConnectionsQuantity(currentPool);
      MPI_Send(&poolPredConnectionsQuantity, 1, MPI_INT, processSendRank, messageType, MPI_COMM_WORLD);
      if (poolPredConnectionsQuantity){
        for (int currentPredConnection = 1; currentPredConnection <= poolPredConnectionsQuantity; ++currentPredConnection){
          predConnections[currentPredConnection - 1].prePoolID = genome.getPredConnectionPrePoolID(currentPool, currentPredConnection);
          predConnections[currentPredConnection - 1].postPoolID = genome.getPredConnectionPostPoolID(currentPool, currentPredConnection);
          predConnections[currentPredConnection - 1].enabled = genome.getPredConnectionEnabled(currentPool, currentPredConnection);
          predConnections[currentPredConnection - 1].disabledStep = genome.getPredConnectionDisabledStep(currentPool, currentPredConnection);
          predConnections[currentPredConnection - 1].developPredConnectionProb = genome.getDevelopPredConnectionProb(currentPool, currentPredConnection);
          predConnections[currentPredConnection - 1].innovationNumber = genome.getPredConnectionInnovationNumber(currentPool, currentPredConnection);
        }
        MPI_Send(predConnections, poolPredConnectionsQuantity, MPI_PREDCONNECTION, processSendRank, messageType, MPI_COMM_WORLD);
      }
    }
    delete []predConnections;
  }
}

// Прием генома от удаленного процесса по MPI
void TSharedEvolutionaryProcess::receiveAgentGenomeViaMPI(TPoolNetwork& genome){
  genome.erasePoolNetwork();
  int poolsQuantity;
  MPI_Recv(&poolsQuantity, 1, MPI_INT, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
  // Принимаем список пулов и создаем все пулы
  _mpi_pool* pools = new _mpi_pool[poolsQuantity];
  MPI_Recv(pools, poolsQuantity, MPI_POOL, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
  for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
    genome.addPool(pools[currentPool - 1].type, pools[currentPool - 1].layer, pools[currentPool - 1].biasMean, 
                    pools[currentPool - 1].biasVariance, pools[currentPool - 1].capacity);
  delete []pools;
  // Принимаем максимальное кол-во связей у пула
  int maxConnectionsQuantity;
  MPI_Recv(&maxConnectionsQuantity, 1, MPI_INT, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
  if (maxConnectionsQuantity){
    _mpi_connection* connections = new _mpi_connection[maxConnectionsQuantity];
    for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
      //Принимаем кол-во связей у пула
      int poolConnectionsQuantity;
      MPI_Recv(&poolConnectionsQuantity, 1, MPI_INT, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
      if (poolConnectionsQuantity){
        MPI_Recv(connections, poolConnectionsQuantity, MPI_CONNECTION, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
        for (int currentConnection = 1; currentConnection <= poolConnectionsQuantity; ++currentConnection)
          genome.addConnection(connections[currentConnection - 1].prePoolID, connections[currentConnection - 1].postPoolID, connections[currentConnection - 1].weightMean, 
                                connections[currentConnection - 1].weightVariance, connections[currentConnection - 1].developSynapseProb, 
                                connections[currentConnection - 1].enabled != 0, connections[currentConnection - 1].disabledStep, connections[currentConnection - 1].innovationNumber);
      }
    }
    delete []connections;
  }
  // Теперь принимаем предикторные связи
  int maxPredConnectionsQuantity;
  MPI_Recv(&maxPredConnectionsQuantity, 1, MPI_INT, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
  if (maxPredConnectionsQuantity){
    _mpi_predconnection* predConnections = new _mpi_predconnection[maxPredConnectionsQuantity];
    for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
      //Принимаем кол-во связей у пула
      int poolPredConnectionsQuantity;
      MPI_Recv(&poolPredConnectionsQuantity, 1, MPI_INT, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
      if (poolPredConnectionsQuantity){
        MPI_Recv(predConnections, poolPredConnectionsQuantity, MPI_PREDCONNECTION, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
        for (int currentPredConnection = 1; currentPredConnection <= poolPredConnectionsQuantity; ++currentPredConnection)
          genome.addPredConnection(predConnections[currentPredConnection - 1].prePoolID, predConnections[currentPredConnection - 1].postPoolID, predConnections[currentPredConnection - 1].developPredConnectionProb, 
                                   predConnections[currentPredConnection - 1].enabled != 0, predConnections[currentPredConnection - 1].disabledStep, predConnections[currentPredConnection - 1].innovationNumber);
      }
    }
    delete []predConnections;
  }
}
