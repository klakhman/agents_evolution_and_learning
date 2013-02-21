#define _CRT_SECURE_NO_WARNINGS

#include "TAnalysis.h"

#include "THypercubeEnvironment.h"
#include "TPopulation.h"
#include "TAgent.h"
#include "TBehaviorAnalysis.h"
#include "settings.h"
#include <vector>

#include "mpi.h"
#include <string>
#include <ctime>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

// Запуск процедуры анализа путем прогона лучшей популяции (возвращает среднее значений награды по популяции после прогона всех агентов из всех состояний)
// Если stochasticityCoefficient не равен -1, то он переписывает загруженный из файла настроек.
double TAnalysis::startBestPopulationAnalysis(string bestPopulationFilename, string environmentFilename, string settingsFilename, unsigned int randomSeed /*=0*/, double stochasticityCoefficient /*=-1*/){
	// Если не было передано зерно рандомизации
	if (!randomSeed)
		randomSeed = static_cast<unsigned int>(time(0));
	srand(randomSeed);
	// Запуски генератора случайных чисел, чтобы развести значения
	rand();
	rand();
	rand();
	THypercubeEnvironment* environment = new THypercubeEnvironment(environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, settingsFilename);
  if (stochasticityCoefficient != -1)
    environment->setStochasticityCoefficient(stochasticityCoefficient);
	TPopulation<TAgent>* agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	agentsPopulation->loadPopulation(bestPopulationFilename);
	// Создаем массив наград всех агентов при запуске из всех начальных состояний (чтобы если что, то можно было проводить более сложный анализ)
	int initialStatesQuantity = environment->getInitialStatesQuantity();
	double** agentsRewards = new double*[agentsPopulation->getPopulationSize()];
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent){
		agentsRewards[currentAgent - 1] = new double[initialStatesQuantity];
    memset(agentsRewards[currentAgent - 1], 0, sizeof(*(agentsRewards[currentAgent - 1])) * initialStatesQuantity);
  }
	double maxReward = 0.0;
	long double averageReward = 0.0;
  // Для корректного анализа агентов, которые проходят процедуру первичного системогенеза (и возможно обучения) нужно проводить их построение несколько раз
  int runsQuantity;
  if (0 != agentsPopulation->getPointertoAgent(1)->getSystemogenesisMode())
    runsQuantity = 10;
  else 
    runsQuantity = 1;
	// Прогоняем всех агентов и записываем награды в массив
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent){
    for (int currentRun = 1; currentRun <= runsQuantity; ++currentRun){
      if (1 == agentsPopulation->getPointertoAgent(currentAgent)->getSystemogenesisMode())
        agentsPopulation->getPointertoAgent(currentAgent)->primarySystemogenesis();
      else if (0 == agentsPopulation->getPointertoAgent(currentAgent)->getSystemogenesisMode())
		    agentsPopulation->getPointertoAgent(currentAgent)->linearSystemogenesis();
      else if (2 == agentsPopulation->getPointertoAgent(currentAgent)->getSystemogenesisMode())
        agentsPopulation->getPointertoAgent(currentAgent)->alternativeSystemogenesis();
      // Необходимо сохранять первичную нейронную сеть, так как запуск проходит из всех состояний и возможно обучение
      TNeuralNetwork initialController;
      if (0 != agentsPopulation->getPointertoAgent(currentAgent)->getLearningMode())
        initialController = *(agentsPopulation->getPointertoAgent(currentAgent)->getPointerToAgentController());
		  for (int currentInitialState = 0; currentInitialState < initialStatesQuantity; ++currentInitialState){
        if (0 != agentsPopulation->getPointertoAgent(currentAgent)->getLearningMode())
          *(agentsPopulation->getPointertoAgent(currentAgent)->getPointerToAgentController()) = initialController;
			  environment->setEnvironmentState(currentInitialState);
			  agentsPopulation->getPointertoAgent(currentAgent)->life(*environment, agentsPopulation->evolutionSettings.agentLifetime);
			  double reward = agentsPopulation->getPointertoAgent(currentAgent)->getReward();
        agentsRewards[currentAgent - 1][currentInitialState] += reward/runsQuantity;			
			  averageReward += reward / (agentsPopulation->getPopulationSize() *  initialStatesQuantity);
			  if (reward > maxReward)
				  maxReward = reward;
		  }
    }
	}
  averageReward /= runsQuantity;
	// Удаляем все переменные
	for (int currentAgent = 1; currentAgent <= agentsPopulation->getPopulationSize(); ++currentAgent)
		delete []agentsRewards[currentAgent - 1];
	delete []agentsRewards;
	delete agentsPopulation;
	delete environment;
	return static_cast<double>(averageReward);
}


// ---------------- Процедуры параллельного анализа по лучшим популяциям в каждом запуске -----------------------

// Расшифровка парaметров командной строки
void TAnalysis::decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, string& runSign, string& analysisRunSign, double& stochasticityCoefficient){
	int currentArgNumber = 1; // Текущий номер параметра
	while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			if (!strcmp("-env", argv[currentArgNumber])) { firstEnvironmentNumber = atoi(argv[++currentArgNumber]); lastEnvironmentNumber = atoi(argv[++currentArgNumber]);}
			else if (!strcmp("-try", argv[currentArgNumber])) { firstTryNumber = atoi(argv[++currentArgNumber]); lastTryNumber = atoi(argv[++currentArgNumber]);}
			else if (!strcmp("-sign", argv[currentArgNumber])) { runSign = argv[++currentArgNumber]; }
      else if (!strcmp("-ansign", argv[currentArgNumber])) { analysisRunSign = argv[++currentArgNumber]; }
      else if (!strcmp("-stoch", argv[currentArgNumber])) { stochasticityCoefficient = atof(argv[++currentArgNumber]); }
		}
		++currentArgNumber;
	}
}

// Составление сообщения для рабочего процесса
stringstream TAnalysis::composeMessageForWorkProcess(int currentEnvironment, int currentTry, string runSign, double stochasticityCoefficient){
  stringstream outStream;
  outStream << "$ENV$" << currentEnvironment << "$TRY$" << currentTry << "$SIGN$" << runSign << "$STOCH$" << stochasticityCoefficient;
  return outStream;
}

// Нахождение записи о параметре в строке с сообщением от процесса
string TAnalysis::findParameterNote(string inputMessage, string parameterString){
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

// Расшифровка сообщения от рабочего процесса 
void TAnalysis::decodeFinishedWorkMessage(string inputMessage, int& processRankSend, int& finishedEnvironment, int& finishedTry, double& averageReward){
	 string featureNote;
  // Определяем номер среды
  featureNote = findParameterNote(inputMessage, "$ENV$");
  if (featureNote != "") finishedEnvironment = atoi(featureNote.c_str());
  // Определяем номер попытки
  featureNote = findParameterNote(inputMessage, "$TRY$");
  if (featureNote != "") finishedTry = atoi(featureNote.c_str());
  // Определяем номер процесса
  featureNote = findParameterNote(inputMessage, "$PR$");
  if (featureNote != "") processRankSend = atoi(featureNote.c_str());
  // Определяем среднюю награду
  featureNote = findParameterNote(inputMessage, "$AVREW$");
  if (featureNote != "") averageReward = atof(featureNote.c_str());
}

// Выполнение управляющего процесса (важно, чтобы количество общих заданий не было меньше кол-ва выделенных процессов!!!)
void TAnalysis::rootProcess(int argc, char **argv){
	int processesQuantity;	
	MPI_Comm_size(MPI_COMM_WORLD, &processesQuantity); // Определение общего количества процессов
	MPI_Status status;
	const int messageLength = 100;
	const int messageType = 99;

	string settingsFilename = settings::getSettingsFilename(argc, argv);
	string workDirectory, environmentDirectory, resultsDirectory;
	settings::fillDirectoriesSettings(workDirectory, environmentDirectory, resultsDirectory, settingsFilename);

	unsigned long startTime = static_cast<unsigned long>(time(0)); // Время старта процесса анализа
	int firstEnvironmentNumber; // Диапазон номеров сред
	int lastEnvironmentNumber;
	int firstTryNumber; // Диапазон попыток
	int lastTryNumber;
	string runSign; // Некоторый отличительный признак данного конкретного набора параметров или версии алгоритма
  string analysisRunSign; // Некоторый отличительный признак данного конкретного анализа (может быть нулевым, тогда рабочие процессы используют runSign)
  double stochasticityCoefficient = -1; // Коэффициент стохастичности который переписывает для рабочего процесса указанный в файле настроек (только если указывается)
  decodeCommandPromt(argc, argv, firstEnvironmentNumber, lastEnvironmentNumber, firstTryNumber, lastTryNumber, runSign, analysisRunSign, stochasticityCoefficient);
	// Создаем файл с логом
	stringstream logFilename;
	logFilename << workDirectory << "/Analysis_run_log_En" << firstEnvironmentNumber << "-" << lastEnvironmentNumber << "_" << runSign << ".txt";
	ofstream logFile;
	logFile.open(logFilename.str().c_str());
	// Создаем массив со всеми средними наградами по запускам
	double** averageRewards = new double*[lastEnvironmentNumber - firstEnvironmentNumber + 1];
	for (int currentEnvironment = 1; currentEnvironment <= lastEnvironmentNumber - firstEnvironmentNumber + 1; ++currentEnvironment)
		averageRewards[currentEnvironment - 1] = new double[lastTryNumber - firstTryNumber + 1];

	// Выдача дочерним процессам всех заданий данных на выполнение программе
	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			// Если не всем процессам выданы изначальные задания (простой подсчет)
			if ((currentEnvironment - firstEnvironmentNumber) * (lastTryNumber - firstTryNumber + 1) + currentTry - firstTryNumber + 1 <= processesQuantity - 1){
				// Подсчитываем номер следущего процесса для посылки задания
				int processRankSend = (currentEnvironment - firstEnvironmentNumber) * (lastTryNumber - firstTryNumber + 1) + currentTry - firstTryNumber + 1;
				// Составляем сообщение для рабочего процесса
				char outMessage[messageLength];
				composeMessageForWorkProcess(currentEnvironment, currentTry, runSign, stochasticityCoefficient) >> outMessage;
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
				double averageReward;
				decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry, averageReward);
				averageRewards[finishedEnvironment - firstEnvironmentNumber][finishedTry - firstTryNumber] = averageReward;
				// Записываем в лог прием задания
				unsigned long currentTime = static_cast<unsigned long>(time(0));
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60)
					<< "\tEnvironment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from process " << processRankSend << endl; 
				// Составляем сообщение и высылаем задание рабочему процессу
        char outMessage[messageLength];
				composeMessageForWorkProcess(currentEnvironment, currentTry, runSign, stochasticityCoefficient) >> outMessage;				
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
		double averageReward;
		decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry, averageReward);
		averageRewards[finishedEnvironment - firstEnvironmentNumber][finishedTry - firstTryNumber] = averageReward;
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
	// Записываем файл с полными данными по всем попыткам и усредненные по средам
	stringstream analysisResultFilename;
  // Если analysisRunSign не был передан, то используем runSign
	analysisResultFilename << resultsDirectory << "/Analysis/BestPopulation_analysis_En" << firstEnvironmentNumber << "-" << lastEnvironmentNumber << "_" << (analysisRunSign == "" ? runSign : analysisRunSign)  << ".txt";
	ofstream analysisResultFile;
	analysisResultFile.open(analysisResultFilename.str().c_str());

	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			analysisResultFile << "Environment#" << currentEnvironment << "\tTry#" << currentTry << "\t"
				<< averageRewards[currentEnvironment - firstEnvironmentNumber][currentTry - firstTryNumber] << endl;

	analysisResultFile << endl << endl;

	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment){
		double averageEnvironmentReward = 0.0;
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			averageEnvironmentReward += averageRewards[currentEnvironment - firstEnvironmentNumber][currentTry - firstTryNumber] / (lastTryNumber - firstTryNumber + 1);
		analysisResultFile << "Environment#" << currentEnvironment << "\t" << averageEnvironmentReward << endl;
	}
	analysisResultFile.close();
	for (int currentEnvironment = 1; currentEnvironment <= lastEnvironmentNumber - firstEnvironmentNumber + 1; ++currentEnvironment)
		delete []averageRewards[currentEnvironment - 1];
	delete []averageRewards;
}

// Расшифровка сообщения от рутового процесса 
void TAnalysis::decodeTaskMessage(string inputMessage, int& currentEnvironment, int& currentTry, string& runSign, double& stochasticityCoefficient){
  string featureNote;
  // Определяем номер среды $ENV$
  featureNote = findParameterNote(inputMessage, "$ENV$");
  if (featureNote != "") currentEnvironment = atoi(featureNote.c_str());
  // Определяем номер попытки
  featureNote = findParameterNote(inputMessage, "$TRY$");
  if (featureNote != "") currentTry = atoi(featureNote.c_str());
  // Определяем признак запуска
  featureNote = findParameterNote(inputMessage, "$SIGN$");
  runSign = featureNote;
  // Определяем степень стохастичности
  featureNote = findParameterNote(inputMessage, "$STOCH$");
  if (featureNote != "") stochasticityCoefficient = atof(featureNote.c_str());

  runSign = featureNote;
}

// Выполнение рабочего процесса
void TAnalysis::workProcess(int argc, char **argv){
	int processRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // Определение процессом своего номера
	int processesQuantity;	
	MPI_Comm_size(MPI_COMM_WORLD, &processesQuantity); // Определение общего количества процессов
	MPI_Status status;
	const int messageLength = 100;
	const int messageType = 99;

	string settingsFilename = settings::getSettingsFilename(argc, argv);
	string workDirectory, environmentDirectory, resultsDirectory;
	settings::fillDirectoriesSettings(workDirectory, environmentDirectory, resultsDirectory, settingsFilename);

	char _inputMessage[messageLength];
	MPI_Recv(_inputMessage, messageLength-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status); // Ждем сообщения с заданием
  string inputMessage = _inputMessage;
	while (inputMessage !=  "$Q$"){ // Пока не было команды о выходе
		// Декодируем сообщение с заданием
		int currentEnvironment, currentTry;
		string runSign;
    double stochasticityCoefficient = -1; // Рабочий процесс в случае, если коэффицент стохастичности не был передан в командной строке, передает рабочему процессу -1
		decodeTaskMessage(inputMessage, currentEnvironment, currentTry, runSign, stochasticityCoefficient);
		// Определяем уникальное ядро рандомизации
		// К ядру инициализации случайных чисел добавляется номер процесса, чтобы развести изначально инициализируемые процессы
		unsigned int randomSeed = static_cast<unsigned int>(time(0)) + processRank;
		// Определяем параметры анализа и запускаем анализ на одной популяции
		stringstream tmpStream;
		tmpStream << environmentDirectory << "/Environment" << currentEnvironment << ".txt";
		string environmentFilename = tmpStream.str();
		tmpStream.str(""); // Очищаем поток
		tmpStream << resultsDirectory << "/En" << currentEnvironment << "/En" << currentEnvironment << "_" << runSign << "(" << currentTry << ")_bestpopulation.txt";
		string bestPopulationFilename = tmpStream.str();
    double averageReward = startBestPopulationAnalysis(bestPopulationFilename, environmentFilename, settingsFilename, randomSeed, stochasticityCoefficient);
		// Посылаем ответ о завершении работы над заданием
		tmpStream.str(""); // Очищаем поток
		tmpStream << "$ENV$" << currentEnvironment << "$TRY$" << currentTry << "$PR$" << processRank << "$AVREW$" << averageReward;
		char outMessage[messageLength];
		tmpStream >> outMessage;
		MPI_Send(outMessage, messageLength - 1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD);

		//Ожидание нового задания
		MPI_Recv(_inputMessage, messageLength-1, MPI_CHAR, 0, messageType, MPI_COMM_WORLD, &status);
    inputMessage = _inputMessage;
	}
}

// Запуск параллельного процесса анализа по методу лучшей популяции в каждом запуске (!!!в папке с результатами должна быть папка "/Analysis"!!!)
void TAnalysis::startParallelBestPopulationAnalysis(int argc, char **argv){
	MPI_Init(&argc, &argv);
	int processRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // Определение процессом своего номера
	if (processRank == 0) // Если это рутовый процесс
		rootProcess(argc, argv);
	else // Если это рабочий процесс
		workProcess(argc, argv);

	MPI_Finalize();
}

// Процедура усреднение параметров анализа по лучшей популяции по глобальным сложностям сред (membersQuantity - кол-во сред в рамках одного коэффициента заполненности)
void TAnalysis::makeBestPopulationAnalysisSummary(string analysisFilename, string summaryFilname, int occupancyCoeffcientsQuantity, int membersQuantity, int tryQuantity){
	ifstream analysisFile;
	analysisFile.open(analysisFilename.c_str());
	ofstream summaryFile;
	summaryFile.open(summaryFilname.c_str());
	for (int currentOccupancyCoefficient = 1; currentOccupancyCoefficient <= occupancyCoeffcientsQuantity; ++currentOccupancyCoefficient){
		double averageReward = 0;
		for (int currentMember = 1; currentMember <= membersQuantity; ++currentMember)
			for (int currentTry = 1; currentTry <= tryQuantity; ++currentTry){
				string tmpStr;
				analysisFile >> tmpStr; // Считываем среду
				analysisFile >> tmpStr; // Считываем попытку
				analysisFile >> tmpStr; // Считываем среднюю награду за попытку
				averageReward += atof(tmpStr.c_str()) / (membersQuantity * tryQuantity);
			}
		summaryFile << averageReward << endl;
	}
	summaryFile.close();
	analysisFile.close();
}

// Процедура прогона случайного агента (на каждом такте времени действие агента определяется случайно)
double TAnalysis::randomAgentLife(THypercubeEnvironment& environment, int agentLifeTime){
	vector< vector<double> > agentLife(agentLifeTime);

	for (int agentLifeStep = 1; agentLifeStep <= agentLifeTime; ++agentLifeStep){
		// Определяем случайное действие агента (отличное от нуля)
		double action = 0;
		while (action == 0){
			action = static_cast<double>(service::uniformDiscreteDistribution(-environment.getEnvironmentResolution(), environment.getEnvironmentResolution()));
		}
		agentLife[agentLifeStep - 1].push_back(action);
		// Действуем на среду и проверяем успешно ли действие
		bool actionSuccess = environment.forceEnvironment(agentLife[agentLifeStep - 1]);
		if (!actionSuccess) agentLife[agentLifeStep - 1][0] = 0;
	}
	
	double reward = environment.calculateReward(agentLife, agentLifeTime);

	return reward;
}

// Процедура анализа базового уровня награды для среды (путем прогона случайного агента)
void TAnalysis::randomAgentAnalysis(string environmentDirectory, int firstEnvNumber, int lastEnvNumber, string settingsFilename, string resultsFilename){
	ofstream resultsFile;
	resultsFile.open(resultsFilename.c_str());
	srand(static_cast<unsigned int>(time(0)));
	for (int currentEnvironment = firstEnvNumber; currentEnvironment <= lastEnvNumber; ++currentEnvironment){
		stringstream environmentFilename;
		environmentFilename << environmentDirectory << "/Environment" << currentEnvironment << ".txt";
		THypercubeEnvironment environment(environmentFilename.str());
		settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
		environment.setStochasticityCoefficient(0.0);
		long double averageReward = 0;
		for (int currentAgent = 1; currentAgent <= 250; ++currentAgent)
			for (int currentState = 0; currentState < environment.getInitialStatesQuantity(); ++currentState){
				environment.setEnvironmentState(currentState);
				averageReward += randomAgentLife(environment, 250);
			}
		averageReward /= 250 * environment.getInitialStatesQuantity();
		cout << "Env" << currentEnvironment << "\t" << averageReward << endl;
		resultsFile << "Environment#" << currentEnvironment << "\tTry#1\t" << averageReward << endl;
	}
	resultsFile.close();
}

// Масштабирование гистограммы (передается последовательность немасшабированной гистограммы - кол-во измерений начиная со значения параметра 1..., scale - значение масштабирования (напр. если передано 10, то масштабируется от 1 до 10, от 11 до 20 и т.д.))
void TAnalysis::scaleHistogram(int* unscaledHistogram, int parametersQuantity, int scale, string resultFilename){
	// Подсчитываем кол-во отсчетов масштабированной гистограммы
	int scaledParamQuantity = parametersQuantity / 10 + 1 * ((parametersQuantity % 10) != 0);
	int* scaledHistogram = new int[scaledParamQuantity];
	memset(scaledHistogram, 0, scaledParamQuantity * sizeof(int));

	for (int currentParameter = 1; currentParameter <= parametersQuantity; ++currentParameter)
		scaledHistogram[(currentParameter - 1) / scale] += unscaledHistogram[currentParameter - 1];

	ofstream resultFile;
	resultFile.open(resultFilename.c_str());
	for (int currentScaledParam = 0; currentScaledParam < scaledParamQuantity; ++currentScaledParam)
		resultFile << scaledHistogram[currentScaledParam] << endl;
	resultFile.close();

	delete []scaledHistogram;
}

// Парсинг файлов углубленного анализа поведения (zeroConvergenceMean - среднее кол-во состояний, из которых агенты никуда не сходятся)
void TAnalysis::advancedBehaviorFilesParsing(int* cyclesConvergenceHist, int statesQuantity, double& zeroConvergenceMean, int* cyclesLengthHist, int maxCyclesLength, string analysisFilename){
	ifstream analysisFile;
	analysisFile.open(analysisFilename.c_str());
	string tmpStr;
	for (int currentState = 0; currentState < statesQuantity; ++currentState){
		analysisFile >> tmpStr;
		cyclesConvergenceHist[currentState] = atoi(tmpStr.c_str());
	}
	analysisFile >> tmpStr;
	zeroConvergenceMean = atof(tmpStr.c_str());
	cout << zeroConvergenceMean << "\t";
	for (int currentLength = 0; currentLength < maxCyclesLength; ++currentLength){
		analysisFile >> tmpStr;
		cyclesLengthHist[currentLength] = atoi(tmpStr.c_str());
	}
	analysisFile.close();
}

// Подсчет среднего количества нейронов (после развертывания пулов) в популяции
double TAnalysis::calculatePopAverageNeurons(const TPopulation<TAgent>& population){
  double averageNeuronsQuantity = 0;
  for (int currentAgent = 1; currentAgent < population.getPopulationSize(); ++currentAgent){
    TPoolNetwork* genome = population.getPointertoAgent(currentAgent)->getPointerToAgentGenome();
    for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
      averageNeuronsQuantity += genome->getPoolCapacity(currentPool);
  }
  return averageNeuronsQuantity/static_cast<double>(population.getPopulationSize());
}

// Подсчет среднего кол-ва нейронов для запуска на одной среде
void TAnalysis::calculateOneEnvironmentAverageNeurons(int argc, char**argv){
  MPI_Init(&argc, &argv);
  string settingsFilename = settings::getSettingsFilename(argc, argv);
  string resultsDirectory;
  string environmentDirectory;
  string workDirectory;
  settings::fillDirectoriesSettings(workDirectory, environmentDirectory, resultsDirectory, settingsFilename);

  string runSign;
  int envNumber;
  int tryQuantity;
  int currentArgNumber = 1; // Текущий номер параметра
	while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			if (!strcmp("-envnumber", argv[currentArgNumber])) envNumber = atoi(argv[++currentArgNumber]);
			else if (!strcmp("-sign", argv[currentArgNumber])) runSign = argv[++currentArgNumber];
      else if (!strcmp("-tryquantity", argv[currentArgNumber])) tryQuantity = atoi(argv[++currentArgNumber]);
		}
		++currentArgNumber;
	}
  stringstream tmpStream;
  tmpStream << workDirectory << "/NeuronsQuantityAnalysis_env" << envNumber << "_try1-" << tryQuantity << "_" << runSign << ".txt";
  ofstream outputFile;
  outputFile.open(tmpStream.str().c_str());

  TPopulation<TAgent> population;
  settings::fillPopulationSettingsFromFile(population, settingsFilename);
  double averageNeuronsQuantity = 0;
  for (int currentPopulation = 0; currentPopulation < tryQuantity; ++currentPopulation){
    double currentAverage = 0;
    tmpStream.str("");
    tmpStream << resultsDirectory << "/En" << envNumber << "/En" << envNumber << "_" << runSign << "(" << currentPopulation + 1 << ")_bestpopulation.txt";
    population.loadPopulation(tmpStream.str());
    currentAverage = calculatePopAverageNeurons(population);
    averageNeuronsQuantity += currentAverage;
    outputFile << "Try: " << currentPopulation + 1 << "\tAverage neurons quantity: " << currentAverage << endl; 
  }
  averageNeuronsQuantity /= tryQuantity;
  outputFile << endl << endl << "Total average: " << averageNeuronsQuantity << endl;
  outputFile.close();
  MPI_Finalize();
}


