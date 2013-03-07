#ifndef TSHAREDEVOLUTIONARYPROCESS_H
#define TSHAREDEVOLUTIONARYPROCESS_H

#include "TEvolutionaryProcess.h"
#include "mpi.h"
#include <string>
#include <cstring>

// Запуск эволюционного процесса с "расшаренной" памятью через диск (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
// Передается список процессов, которые находятся в распоряжении данного эволюционного запуска (этот список должен включать также номер данного процесса, в качестве первого в векторе)
class TSharedEvolutionaryProcess : public TEvolutionaryProcess{
  // Стандартная максимальная длина сообщения
	static const int messageLength = 3000;
	// Стандартный служебный тип сообщения
	static const int messageType = 99;
	// Служебная перемення статуса процесса
	MPI_Status status;
  // Номер процесса
	int processRank;
  char outputMessage[messageLength];
  char inputMessage[messageLength];

  // Расшифровка сообщения от рабочего процесса рутовому с результатами обсчетов (возвращает награду всех агентов)
  std::vector<double> decodeFineshedWorkMessage(const std::string& finishedWorkMessage, int& process);
  // Рутовый процесс
  void rootProcess(unsigned int randomSeed);
  // Рабочий процесс
  void workProcess();
  // Внутренние типы MPI для пересылки геномов между процессами
  MPI_Datatype MPI_POOL, MPI_CONNECTION, MPI_PREDCONNECTION;
  // Заготовки "сырых" типов для внутренних типов MPI
  struct _mpi_pool {
    int type;
    int capacity;
    double biasMean;
    double biasVariance;
    int layer;
  };
  struct _mpi_connection {
    int prePoolID;
    int postPoolID;
    double weightMean;
    double weightVariance;
    int enabled;
    int disabledStep;
    double developSynapseProb;
    long innovationNumber;
  };
  struct _mpi_predconnection {
    int prePoolID;
    int postPoolID;
    int enabled;
    int disabledStep;
    double developPredConnectionProb;
    long innovationNumber;
  };
  // Изначальное конструирование всех "сырых" представлений в нотации MPI для сложного типа TPoolNetwork
  // Означивает типы MPI_POOL, MPI_CONNECTION, MPI_PREDCONNECTION
  void constructMPIDataTypes();
  // Отправка генома удаленному процессу по MPI
  void receiveAgentGenomeViaMPI(TPoolNetwork& agent);
  // Прием генома от удаленного процесса по MPI
  void sendAgentGenomeViaMPI(const TPoolNetwork& genome, int processSendRank);

public:
  TSharedEvolutionaryProcess(){
    strcpy(inputMessage, "");
    strcpy(outputMessage, "");
    // Конструируем типы
    constructMPIDataTypes();
  }
  ~TSharedEvolutionaryProcess(){
  }
  // Локальный для этого задания пул процессов (рутовый процесс записан первым)
  std::vector<int> processesLocalPool;
	// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
	void start(unsigned int randomSeed = 0);
};

#endif // TSHAREDEVOLUTIONARYPROCESS_H