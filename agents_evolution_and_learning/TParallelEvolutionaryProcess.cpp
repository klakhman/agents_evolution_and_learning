#define _CRT_SECURE_NO_WARNINGS

#include "TParallelEvolutionaryProcess.h"

#include "TEvolutionaryProcess.h"
#include "settings.h"

#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <cstring>

using namespace std;

// Расишифровка парметров командной строки
void TParallelEvolutionaryProcess::decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, string& runSign){
	int currentArgNumber = 1; // Текущий номер параметра
	while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			if (!strcmp("-env", argv[currentArgNumber])) { firstEnvironmentNumber = atoi(argv[++currentArgNumber]); lastEnvironmentNumber = atoi(argv[++currentArgNumber]);}
			else if (!strcmp("-try", argv[currentArgNumber])) { firstTryNumber = atoi(argv[++currentArgNumber]); lastTryNumber = atoi(argv[++currentArgNumber]);}
			else if (!strcmp("-sign", argv[currentArgNumber])) { runSign = argv[++currentArgNumber]; }
		}
		++currentArgNumber;
	}
}

// Нахождение записи о параметре в строке с сообщением от процесса
string TParallelEvolutionaryProcess::findParameterNote(string inputMessage, string parameterString){
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

//// Расшифровка сообщения от рабочего процесса 
//void TParallelEvolutionaryProcess::decodeFinishedWorkMessage(char inputMessage[], int& processRankSend, int& finishedEnvironment, int& finishedTry){
//	string tmpString;
//	for (unsigned int i=0; i < strlen(inputMessage); ++i)
//		if ( (inputMessage[i] >= '0') && (inputMessage[i] <= '9') ) // Если символ число
//			tmpString += inputMessage[i];
//		else {
//			switch (inputMessage[i]){
//				case 'e': // Если это номер среды
//					finishedEnvironment = atoi(tmpString.c_str());
//					break;
//				case 't': // Если это номер попытки
//					finishedTry = atoi(tmpString.c_str());
//					break;
//				case 'p': // Если это номер процесса
//					processRankSend = atoi(tmpString.c_str());
//					break;
//			}
//			tmpString = "";
//		}
//}

// Расшифровка сообщения от рабочего процесса 
void TParallelEvolutionaryProcess::decodeFinishedWorkMessage(string inputMessage, int& processRankSend, int& finishedEnvironment, int& finishedTry){
  string featureNote;
  // Определяем номер среды процесса
  featureNote = findParameterNote(inputMessage, "$PR$");
  if (featureNote != "") processRankSend = atoi(featureNote.c_str());
  // Определяем номер среды
  featureNote = findParameterNote(inputMessage, "$ENV$");
  if (featureNote != "") finishedEnvironment = atoi(featureNote.c_str());
  // Определяем номер среды попытки
  featureNote = findParameterNote(inputMessage, "$TRY$");
  if (featureNote != "") finishedTry = atoi(featureNote.c_str());
}

// Выполнение управляющего процесса (важно, чтобы количество общих заданий не было меньше кол-ва выделенных процессов!!!)
void TParallelEvolutionaryProcess::rootProcess(int argc, char **argv){
	int firstEnvironmentNumber; // Диапазон номеров сред
 	int lastEnvironmentNumber;
	int firstTryNumber; // Диапазон попыток
	int lastTryNumber;
	string runSign; // Некоторый отличительный признак данного конкретного набора параметров или версии алгоритма
	unsigned long startTime = static_cast<unsigned long>(time(0)); // Время старта процесса эволюции
	decodeCommandPromt(argc, argv, firstEnvironmentNumber, lastEnvironmentNumber, firstTryNumber, lastTryNumber, runSign);
	// Создаем файл с логом
	stringstream logFilename;
	logFilename << directoriesSettings.workDirectory << "/Evolution_run_log_En" << firstEnvironmentNumber << "-" << lastEnvironmentNumber << "_" << runSign << ".txt";
	ofstream logFile;
	logFile.open(logFilename.str().c_str());
	// Выдача дочерним процессам всех заданий данных на выполнение программе
	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			// Если не всем процессам выданы изначальные задания (простой подсчет)
			if ((currentEnvironment - firstEnvironmentNumber) * (lastTryNumber - firstTryNumber + 1) + currentTry - firstTryNumber + 1 <= processesQuantity - 1){
				// Подсчитываем номер следущего процесса для посылки задания
				int processRankSend = (currentEnvironment - firstEnvironmentNumber) * (lastTryNumber - firstTryNumber + 1) + currentTry - firstTryNumber + 1;
				// Составляем сообщение для рабочего процесса
				stringstream outStream;
        outStream << "$ENV$" << currentEnvironment << "$TRY$" << currentTry << "$SIGN$" << runSign;
				char outMessage[messageLength];
				outStream >> outMessage;
				MPI_Send(outMessage, messageLength - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
				// Записываем в лог выдачу задания
				unsigned long currentTime = static_cast<unsigned long>(time(0));
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60) 
					<< "\tEnvironment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for process " << processRankSend << endl; 
			} // Если все процессы получили задание,  то ждем завершения выполнения и по ходу выдаем оставшиеся задания
			else {
				char _inputMessage[messageLength];
				// Ждем входящее сообщение о том, что процесс выполнил задание
				MPI_Recv(_inputMessage, messageLength - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
        string inputMessage = _inputMessage;
				// Расшифровываем сообщение от рабочего процесса
				int processRankSend, finishedEnvironment, finishedTry;
				decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry);
				// Записываем в лог прием задания
				unsigned long currentTime = static_cast<unsigned long>(time(0));
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60)
					<< "\tEnvironment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from process " << processRankSend << endl; 
				// Составляем сообщение и высылаем задание рабочему процессу
				stringstream outStream;
        outStream << "$ENV$" << currentEnvironment << "$TRY$" << currentTry << "$SIGN$" << runSign;
				char outMessage[messageLength];
				outStream >> outMessage;
				MPI_Send(outMessage, messageLength - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
				// Записываем в лог выдачу задания
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60)
					<< "\tEnvironment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for process " << processRankSend << endl; 
			}

	// Когда все задания закончились, ждем пока все они будут выполнены и по ходу посылаем всем процессам команду о завершении
	int processTillQuit; // Количество процессов которые еще выполняются и необходимо дождаться их окончания
  int tasksQuantity = (lastEnvironmentNumber - firstEnvironmentNumber + 1) * (lastTryNumber - firstTryNumber + 1);
  if (tasksQuantity < processesQuantity - 1){
    // Если количество процессов больше, чем кол-во заданий, то посылаем команду о завершении всем оставшимся
    for (int prRankSend = tasksQuantity + 1; prRankSend < processesQuantity; ++prRankSend){
      char outMessage[messageLength];
		  strcpy(outMessage, "$Q$");
		  MPI_Send(outMessage, messageLength - 1, MPI_CHAR, prRankSend, messageType, MPI_COMM_WORLD);
    }
    processTillQuit = (lastEnvironmentNumber - firstEnvironmentNumber + 1) * (lastTryNumber - firstTryNumber + 1);
  }
  else
    processTillQuit = processesQuantity - 1; // Количество процессов которые еще выполняются и необходимо дождаться их окончания

	while (processTillQuit > 0){
		char _inputMessage[messageLength];
		MPI_Recv(_inputMessage, messageLength - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
    string inputMessage = _inputMessage;
		// Расшифровываем сообщение от рабочего процесса
		int processRankSend, finishedEnvironment, finishedTry;
		decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry);
		// Записываем в лог прием задания
		unsigned long currentTime = static_cast<unsigned long>(time(0));
		logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60) 
			<< "Environment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from process " << processRankSend << endl; 
		// Составляем сообщение о выходе и высылаем
		char outMessage[messageLength];
		strcpy(outMessage, "$Q$");
		MPI_Send(outMessage, messageLength - 1, MPI_CHAR, processRankSend, messageType, MPI_COMM_WORLD);
		--processTillQuit;
	}
  logFile << endl << "Done." << endl;
	logFile.close();
}

//// Расшифровка сообщения от рутового процесса 
//void TParallelEvolutionaryProcess::decodeTaskMessage(char inputMessage[], int& currentEnvironment, int& currentTry, string& runSign){
//	string tmpString;
//	for (unsigned int i=0; i < strlen(inputMessage); ++i)
//		if ( ((inputMessage[i] >= '0') && (inputMessage[i] <= '9')) || ((inputMessage[i] >= 'a') && (inputMessage[i] <= 'z')) || (inputMessage[i] == '_')) // Если символ число или маленькая буква или знак подчеркивания 
//			tmpString += inputMessage[i];
//		else {
//			switch (inputMessage[i]){
//				case 'E': // Если это номер среды
//					currentEnvironment = atoi(tmpString.c_str());
//					break;
//				case 'T': // Если это номер попытки
//					currentTry = atoi(tmpString.c_str());
//					break;
//				case 'S': // Если это номер процесса
//					runSign = tmpString;
//					break;
//			}
//			tmpString = "";
//		}
//}

// Расшифровка сообщения от рутового процесса 
void TParallelEvolutionaryProcess::decodeTaskMessage(string inputMessage, int& currentEnvironment, int& currentTry, string& runSign){
  string featureNote;
  // Определяем номер среды $ENV$
  featureNote = findParameterNote(inputMessage, "$ENV$");
  if (featureNote != "") currentEnvironment = atoi(featureNote.c_str());
  // Определяем номер попытки
  featureNote = findParameterNote(inputMessage, "$TRY$");
  if (featureNote != "") currentTry = atoi(featureNote.c_str());
  // Определяем признак запуска
  featureNote = findParameterNote(inputMessage, "$ENV$");
  runSign = featureNote;
}

// Выполнение рабочего процесса
void TParallelEvolutionaryProcess::workProcess(int argc, char **argv){
	char _inputMessage[messageLength];
	MPI_Recv(_inputMessage, messageLength-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status); // Ждем сообщения с заданием
  string inputMessage = _inputMessage;  
	while (inputMessage != "$Q$"){ // Пока не было команды о выходе
		// Декодируем сообщение с заданием
		int currentEnvironment, currentTry;
		string runSign;
		decodeTaskMessage(inputMessage, currentEnvironment, currentTry, runSign);
		// Определяем уникальное ядро рандомизации
		// К ядру инициализации случайных чисел добавляется номер процесса, чтобы развести изначально инициализируемые процессы
		unsigned int randomSeed = static_cast<unsigned int>(time(0)) + processRank;
		// Заполняем параметры эволюционного процесса и запускаем его
		TEvolutionaryProcess* evolutionaryProcess = new TEvolutionaryProcess;
		stringstream tmpStream;
		tmpStream << directoriesSettings.environmentDirectory << "/Environment" << currentEnvironment << ".txt";
		evolutionaryProcess->filenameSettings.environmentFilename = tmpStream.str();
		tmpStream.str(""); // Очищаем поток
		tmpStream << directoriesSettings.resultsDirectory << "/En" << currentEnvironment << "/Results_En" << currentEnvironment << "_" << runSign << "(" << currentTry << ").txt";
		evolutionaryProcess->filenameSettings.resultsFilename = tmpStream.str();
		tmpStream.str(""); // Очищаем поток
		tmpStream << directoriesSettings.resultsDirectory << "/En" << currentEnvironment << "/En" << currentEnvironment << "_" << runSign << "(" << currentTry << ")_bestpopulation.txt";
		evolutionaryProcess->filenameSettings.bestPopulationFilename = tmpStream.str();
		tmpStream.str(""); // Очищаем поток
		tmpStream << directoriesSettings.resultsDirectory << "/En" << currentEnvironment << "/En" << currentEnvironment << "_" << runSign << "(" << currentTry << ")_bestagents.txt";
		evolutionaryProcess->filenameSettings.bestAgentsFilename = tmpStream.str();
		evolutionaryProcess->filenameSettings.settingsFilename = settingsFilename;
		evolutionaryProcess->start(randomSeed);
		delete evolutionaryProcess;
		// Посылаем ответ о завершении работы над заданием
		tmpStream.str(""); // Очищаем поток
    tmpStream << "$ENV$" << currentEnvironment << "$TRY$" << currentTry << "$PR$" << processRank;
		char outMessage[messageLength];
		tmpStream >> outMessage;
		MPI_Send(outMessage, messageLength - 1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD);

		//Ожидание нового задания
		MPI_Recv(_inputMessage, messageLength-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status);
    inputMessage = _inputMessage;
	}
}

// Запуск эволюционного процесса
void TParallelEvolutionaryProcess::start(int argc, char **argv){
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &processesQuantity); // Определение общего количества процессов
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // Определение процессом своего номера
	settingsFilename = settings::getSettingsFilename(argc, argv);
	settings::fillDirectoriesSettings(directoriesSettings.workDirectory, directoriesSettings.environmentDirectory, directoriesSettings.resultsDirectory, settingsFilename);
	if (processRank == 0) // Если это рутовый процесс
		rootProcess(argc, argv);
	else // Если это рабочий процесс
		workProcess(argc, argv);

	MPI_Finalize();
}