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

  // Нахождение записи о параметре в строке с сообщением от процесса (имя параметра в строке $NAME$)
  std::string findParameterNote(std::string inputMessage, std::string parameterString);
  // Расшифровка сообщения от рутового процесса рабочему
  void decodeWorkMessage(const std::string& workMessage, std::string& tmpAgentsFilename, int& agentsQuantity);
  // Расшифровка сообщения от рабочего процесса рутовому с результатами обсчетов (возвращает награду всех агентов)
  std::vector<double> decodeFineshedWorkMessage(const std::string& finishedWorkMessage, int& process);
  // Рутовый процесс
  void rootProcess(unsigned int randomSeed);
  // Рабочий процесс
  void workProcess();

public:
  TSharedEvolutionaryProcess(){
    strcpy(inputMessage, "");
    strcpy(outputMessage, "");
  }
  ~TSharedEvolutionaryProcess(){
  }
  // Директория в которую записываются временные файлы
  std::string tmpDirectory;
  // Локальный для этого задания пул процессов (рутовый процесс записан первым)
  std::vector<int> processesLocalPool;
  // Знак данного запуска (так как создаются временные файлы, то надо быть уверенным, что они не пересекаются для разных запусков)
  std::string runSign;
	// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
	void start(unsigned int randomSeed = 0);
};

#endif // TSHAREDEVOLUTIONARYPROCESS_H