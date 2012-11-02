#ifndef TPARALLELEVOLUTIONARYPROCESS_H
#define TPARALLELEVOLUTIONARYPROCESS_H

#include "mpi.h"
#include <string>

// Процесс параллельного запуска нескольких эволюций (с помощью MPI)
class TParallelEvolutionaryProcess{
	// Номер процесса
	int processRank;
	// Общее кол-во процессов
	int processesQuantity;
	// Стандартная максимальная длина сообщения
	static const int messageLentgh = 100;
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

	// Заполнение параметров директорий для записи файлов
	void fillDirectoriesSettings();
	// Расишифровка парметров командной строки
	void decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, std::string runSign);
	// Выполнение управляющего процесса
	void rootProcess(int argc, char **argv);
	// Выполнение рабочего процесса
	void workProcess();
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