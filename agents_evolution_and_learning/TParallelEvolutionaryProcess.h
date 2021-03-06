﻿#ifndef TPARALLELEVOLUTIONARYPROCESS_H
#define TPARALLELEVOLUTIONARYPROCESS_H

#include "mpi.h"
#include <string>
#include <vector>

// Процесс параллельного запуска нескольких эволюций (с помощью MPI)
class TParallelEvolutionaryProcess{
	// Номер процесса
	int processRank;
	// Общее кол-во процессов
	int processesQuantity;
	// Стандартная максимальная длина сообщения
	static const int messageLength = 300;
	// Стандартный служебный тип сообщения
	static const int messageType = 99;
	// Служебная перемення статуса процесса
	MPI_Status status;

	//Директории для записи файлов
	struct {
		std::string workDirectory;
		std::string environmentDirectory;
		std::string resultsDirectory;
	} directoriesSettings;
	// Файл настроек
	std::string settingsFilename;
  // Нахождение записи о параметре в строке с сообщением от процесса
  std::string findParameterNote(std::string inputMessage, std::string parameterString);
	// Расишифровка парметров командной строки
	void decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, 
                          int& lastTryNumber, std::string& runSign, unsigned int& sharedCoef, unsigned int& envType);
	// Расшифровка сообщения от рабочего процесса 
	void decodeFinishedWorkMessage(std::string inputMessage, int& processRankSend, int& finishedEnvironment, int& finishedTry);
	// Выполнение управляющего процесса
	void rootProcess(int argc, char **argv);
	// Расшифровка сообщения от рутового процесса 
	void decodeTaskMessage(std::string inputMessage, int& currentEnvironment, int& currentTry, std::string& runSign, std::vector<int>& processesPool, unsigned int& envType);
	// Выполнение рабочего процесса
	void workProcess(int argc, char **argv);
public:
	// Конструктор
	TParallelEvolutionaryProcess(){
		processRank = 0;
		processesQuantity = 0;
	}
	// Деструктор
	~TParallelEvolutionaryProcess(){ }
	// Запуск эволюционного процесса
	void start(int argc, char **argv);
};

#endif // TPARALLELEVOLUTIONARYPROCESS_H