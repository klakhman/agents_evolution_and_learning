#define _CRT_SECURE_NO_WARNINGS

#include "TAnalysis.h"

#include "THypercubeEnvironment.h"
#include "TPopulation.h"
#include "TAgent.h"
#include "TBehaviorAnalysis.h"
#include "settings.h"
#include <vector>
#include "techanalysis.h"
#include "RestrictedHypercubeEnv.h"

#include "mpi.h"
#include <string>
#include <ctime>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

double TAnalysis::startBestPopulationCycleLengthAnalysis(string bestPopulationFilename, string environmentFilename, string settingsFilename){
  THypercubeEnvironment* environment = new THypercubeEnvironment(environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, settingsFilename);
  environment->setStochasticityCoefficient(0.0);
  TPopulation<TAgent>* agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	agentsPopulation->loadPopulation(bestPopulationFilename);
  double averageCycle = 0;
  // ЭТО ИНОРОДНАЯ ВСТАВКА :)
  for (int currentAgentNumber = 0; currentAgentNumber < agentsPopulation->getPopulationSize(); ++currentAgentNumber)
    averageCycle += agentsPopulation->getPointertoAgent(currentAgentNumber + 1)->getPointerToAgentGenome()->getPoolsQuantity() / static_cast<double>(agentsPopulation->getPopulationSize());
  return averageCycle;
  // ОКОНЧАНИЕ ИНОРОДНОЙ ВСТАВКИ

  for (int currentAgentNumber = 0; currentAgentNumber < agentsPopulation->getPopulationSize(); ++currentAgentNumber){
    TAgent& currentAgent = *agentsPopulation->getPointertoAgent(currentAgentNumber + 1);
     if (1 == currentAgent.getSystemogenesisMode())
        currentAgent.primarySystemogenesis();
      else if (0 == currentAgent.getSystemogenesisMode())
		    currentAgent.linearSystemogenesis();
      else if (2 == currentAgent.getSystemogenesisMode())
        currentAgent.alternativeSystemogenesis();
      vector< pair<TBehaviorAnalysis::SCycle, vector<int> > > convergence = 
        techanalysis::calculateBehaviorConvergenceData(currentAgent, *environment);
      // Теперь взвшенно (с учетом области сходимости) складываем все длины циклов (если агент никуда не сходится, то мы просто ничего не добавляем)
      //cout << currentAgentNumber << " : ";

      for (unsigned int i = 0; i < convergence.size(); ++i){
        averageCycle += convergence[i].first.cycleSequence.size() * (convergence[i].second.size() / static_cast<double>(environment->getInitialStatesQuantity())) / 
          agentsPopulation->getPopulationSize();
        //cout << convergence[i].first.cycleSequence.size() << " - " << convergence[i].second.size() << ", ";
      }
      //cout << endl;    
  }
  delete agentsPopulation;
	delete environment;
	return averageCycle;
}

// Запуск процедуры анализа путем прогона лучшей популяции (возвращает среднее значений награды по популяции после прогона всех агентов из всех состояний)
// Если stochasticityCoefficient не равен -1, то он переписывает загруженный из файла настроек.
double TAnalysis::startBestPopulationAnalysis(string bestPopulationFilename, string environmentFilename, string settingsFilename, unsigned int randomSeed /*=0*/, double stochasticityCoefficient /*=-1*/, unsigned int envType/*=0*/){
	THypercubeEnvironment* environment;
   switch (envType){
    case 0 :
      environment = new THypercubeEnvironment(environmentFilename);
      break;
    case 1:
      environment = new RestrictedHypercubeEnv(environmentFilename);
      break;
    default:
      environment = new THypercubeEnvironment(environmentFilename);
  }
	settings::fillEnvironmentSettingsFromFile(*environment, settingsFilename);
  if (stochasticityCoefficient != -1)
    environment->setStochasticityCoefficient(stochasticityCoefficient);
	TPopulation<TAgent>* agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	agentsPopulation->loadPopulation(bestPopulationFilename);
  double averageReward = startBestPopulationAnalysis(*agentsPopulation, *environment, randomSeed);
	delete agentsPopulation;
	delete environment;
	return averageReward;
}

double TAnalysis::startBestPopulationAnalysis(TPopulation<TAgent>& population, THypercubeEnvironment& environment, unsigned int randomSeed /*=0*/){
  // Если не было передано зерно рандомизации
	if (!randomSeed)
		randomSeed = static_cast<unsigned int>(time(0));
	srand(randomSeed);
	// Запуски генератора случайных чисел, чтобы развести значения
	rand();
	rand();
	rand();
  // Создаем массив наград всех агентов при запуске из всех начальных состояний (чтобы если что, то можно было проводить более сложный анализ)
	int initialStatesQuantity = environment.getInitialStatesQuantity();
	double** agentsRewards = new double*[population.getPopulationSize()];
	for (int currentAgent = 1; currentAgent <= population.getPopulationSize(); ++currentAgent){
		agentsRewards[currentAgent - 1] = new double[initialStatesQuantity];
    memset(agentsRewards[currentAgent - 1], 0, sizeof(*(agentsRewards[currentAgent - 1])) * initialStatesQuantity);
  }
	double maxReward = 0.0;
	long double averageReward = 0.0;
  // Для корректного анализа агентов, которые проходят процедуру первичного системогенеза (и возможно обучения) нужно проводить их построение несколько раз
  int runsQuantity;
  if (0 != population.getPointertoAgent(1)->getSystemogenesisMode())
    runsQuantity = 10;
  else 
    runsQuantity = 1;
	// Прогоняем всех агентов и записываем награды в массив
	for (int currentAgent = 1; currentAgent <= population.getPopulationSize(); ++currentAgent){
    for (int currentRun = 1; currentRun <= runsQuantity; ++currentRun){
      if (1 == population.getPointertoAgent(currentAgent)->getSystemogenesisMode())
        population.getPointertoAgent(currentAgent)->primarySystemogenesis();
      else if (0 == population.getPointertoAgent(currentAgent)->getSystemogenesisMode())
		    population.getPointertoAgent(currentAgent)->linearSystemogenesis();
      else if (2 == population.getPointertoAgent(currentAgent)->getSystemogenesisMode())
        population.getPointertoAgent(currentAgent)->alternativeSystemogenesis();
      // Необходимо сохранять первичную нейронную сеть, так как запуск проходит из всех состояний и возможно обучение
      TNeuralNetwork initialController;
      if (0 != population.getPointertoAgent(currentAgent)->getLearningMode())
        initialController = *(population.getPointertoAgent(currentAgent)->getPointerToAgentController());
		  for (int currentInitialState = 0; currentInitialState < initialStatesQuantity; ++currentInitialState){
        if (0 != population.getPointertoAgent(currentAgent)->getLearningMode())
          *(population.getPointertoAgent(currentAgent)->getPointerToAgentController()) = initialController;
			  environment.setEnvironmentState(currentInitialState);
			  population.getPointertoAgent(currentAgent)->life(environment, population.evolutionSettings.agentLifetime);
			  double reward = population.getPointertoAgent(currentAgent)->getReward();
        agentsRewards[currentAgent - 1][currentInitialState] += reward/runsQuantity;			
			  averageReward += reward / (population.getPopulationSize() *  initialStatesQuantity);
			  if (reward > maxReward)
				  maxReward = reward;
		  }
    }
	}
  averageReward /= runsQuantity;
	// Удаляем все переменные
	for (int currentAgent = 1; currentAgent <= population.getPopulationSize(); ++currentAgent)
		delete []agentsRewards[currentAgent - 1];
	delete []agentsRewards;
	return static_cast<double>(averageReward);
}


// ---------------- Процедуры параллельного анализа по лучшим популяциям в каждом запуске -----------------------

// Расшифровка парaметров командной строки
void TAnalysis::decodeCommandPromt(int argc, char **argv, int& firstEnvironmentNumber, int& lastEnvironmentNumber, int& firstTryNumber, int& lastTryNumber, 
                                   string& runSign, string& analysisRunSign, double& stochasticityCoefficient, unsigned int& sharedCoef, unsigned int& envType){
	int currentArgNumber = 1; // Текущий номер параметра
	while (currentArgNumber < argc){
		if (argv[currentArgNumber][0] == '-'){ // Если это название настройки
			if (!strcmp("-env", argv[currentArgNumber])) { firstEnvironmentNumber = atoi(argv[++currentArgNumber]); lastEnvironmentNumber = atoi(argv[++currentArgNumber]);}
			else if (!strcmp("-try", argv[currentArgNumber])) { firstTryNumber = atoi(argv[++currentArgNumber]); lastTryNumber = atoi(argv[++currentArgNumber]);}
			else if (!strcmp("-sign", argv[currentArgNumber])) { runSign = argv[++currentArgNumber]; }
      else if (!strcmp("-ansign", argv[currentArgNumber])) { analysisRunSign = argv[++currentArgNumber]; }
      else if (!strcmp("-stoch", argv[currentArgNumber])) { stochasticityCoefficient = atof(argv[++currentArgNumber]); }
      else if (!strcmp("-shared", argv[currentArgNumber])) { sharedCoef = atoi(argv[++currentArgNumber]); }
      else if (!strcmp("-envtype", argv[currentArgNumber])) { envType = strtoul(argv[++currentArgNumber], 0, 0); }
		}
		++currentArgNumber;
	}
}

// Составление сообщения для рабочего процесса
string TAnalysis::composeMessageForWorkProcess(int currentEnvironment, int currentTry, string runSign, double stochasticityCoefficient, const vector<int>& runPool, unsigned int envType){
  stringstream outStream;
  // Составляем сообщение для рабочих процессов в данном запуске (с полным списком процессов)
  outStream << "$PROCQ$" << runPool.size();
  for (unsigned int currentProcess = 0; currentProcess < runPool.size(); ++currentProcess)
    outStream << "$PROC" << currentProcess + 1 << "$" << runPool[currentProcess];
  outStream << "$ENV$" << currentEnvironment << "$TRY$" << currentTry << "$SIGN$" << runSign << "$STOCH$" << stochasticityCoefficient << "$ENVTYPE$" << envType;
  return outStream.str();
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
	const int messageLength = 500;
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
  unsigned int sharedCoef = 1; // Количество процессов на один эволюционный запуск
  unsigned int envType = 0;
  decodeCommandPromt(argc, argv, firstEnvironmentNumber, lastEnvironmentNumber, firstTryNumber, lastTryNumber, runSign, analysisRunSign, stochasticityCoefficient, sharedCoef, envType);
  // Если кол-во процессов на запуск больше, чем кол-во доступных рабочих процессов, то выходим
  if (sharedCoef > processesQuantity - 1) exit(-1);
  if (sharedCoef < 1) sharedCoef = 1;
	// Создаем файл с логом
	stringstream logFilename;
	logFilename << workDirectory << "/Analysis_run_log_En" << firstEnvironmentNumber << "-" << lastEnvironmentNumber << "_" << (analysisRunSign == "" ? runSign : analysisRunSign) << ".txt";
	ofstream logFile;
	logFile.open(logFilename.str().c_str());
	// Создаем массив со всеми средними наградами по запускам
	double** averageRewards = new double*[lastEnvironmentNumber - firstEnvironmentNumber + 1];
	for (int currentEnvironment = 1; currentEnvironment <= lastEnvironmentNumber - firstEnvironmentNumber + 1; ++currentEnvironment)
		averageRewards[currentEnvironment - 1] = new double[lastTryNumber - firstTryNumber + 1];
  // Создаем стек свободных процессов (исключая рутовый)
  vector<int> processesStack;
  for (int currentProcess = processesQuantity - 1; currentProcess > 0; --currentProcess)
    processesStack.push_back(currentProcess);
	// Выдача дочерним процессам всех заданий данных на выполнение программе
	for (int currentEnvironment = firstEnvironmentNumber; currentEnvironment <= lastEnvironmentNumber; ++currentEnvironment)
		for (int currentTry = firstTryNumber; currentTry <= lastTryNumber; ++currentTry)
			// Если не всем процессам выданы изначальные задания (простой подсчет)
			 if (processesStack.size() >= sharedCoef){ // Если есть достаточное кол-во свободных процессов
        // Извлекаем необходимое кол-во процессов из стека
        vector<int> runPool;
        for (unsigned int i=0; i<sharedCoef; ++i){
          runPool.push_back(processesStack.back());
          processesStack.pop_back();
        }
				// Составляем сообщение для рабочего процесса
				char outMessage[messageLength];
        stringstream out;
				out << composeMessageForWorkProcess(currentEnvironment, currentTry, runSign, stochasticityCoefficient, runPool, envType);
        out >> outMessage; 
        for (unsigned int currentProcess = 0; currentProcess < runPool.size(); ++currentProcess) 
          MPI_Send(outMessage, messageLength - 1, MPI_CHAR, runPool[currentProcess], messageType, MPI_COMM_WORLD);
				// Записываем в лог выдачу задания
				unsigned long currentTime = static_cast<unsigned long>(time(0));
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60) 
					<< "\tEnvironment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for root process " <<  runPool[0] << endl; 
			} // Если все процессы получили задание,  то ждем завершения выполнения и по ходу выдаем оставшиеся задания
			else {
				char _inputMessage[messageLength];
        while (processesStack.size() < sharedCoef){
				  // Ждем входящее сообщение о том, что процесс выполнил задание
				  MPI_Recv(_inputMessage, messageLength - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
          string inputMessage = _inputMessage;
				  // Расшифровываем сообщение от рабочего процесса
				  int processRankSend, finishedEnvironment, finishedTry;
          finishedTry = finishedEnvironment = 0;
				  double averageReward;
				  decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry, averageReward);
          // Записываем в лог прием задания если это сообщение от рутового процесса (в нем есть номера задания)
          if ((0 != finishedTry) && (0 != finishedEnvironment)){
            averageRewards[finishedEnvironment - firstEnvironmentNumber][finishedTry - firstTryNumber] = averageReward;
            unsigned long currentTime = static_cast<unsigned long>(time(0));
				    logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60)
				  	  << "\tEnvironment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from root process " << processRankSend << endl; 
          }
          processesStack.push_back(processRankSend);
        }
        // Извлекаем необходимое кол-во процессов из стека
        vector<int> runPool;
        for (unsigned int i=0; i<sharedCoef; ++i){
          runPool.push_back(processesStack.back());
          processesStack.pop_back();
        }
				// Составляем сообщение и высылаем задание пулу рабочих процессов
        char outMessage[messageLength];
        stringstream out;
        out << composeMessageForWorkProcess(currentEnvironment, currentTry, runSign, stochasticityCoefficient, runPool, envType);
        out >> outMessage;         
        for (unsigned int currentProcess = 0; currentProcess < runPool.size(); ++currentProcess) 
          MPI_Send(outMessage, messageLength - 1, MPI_CHAR, runPool[currentProcess], messageType, MPI_COMM_WORLD);
				// Записываем в лог выдачу задания
        unsigned long currentTime = static_cast<unsigned long>(time(0));
				logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60)
					<< "\tEnvironment: " << currentEnvironment << "\tTry: " << currentTry << "\tIssued for root process " << runPool[0] << endl; 
			}

  // Когда все задания закончились, ждем пока все они будут выполнены
  while (processesStack.size() < processesQuantity - 1){
		char _inputMessage[messageLength];
		MPI_Recv(_inputMessage, messageLength - 1, MPI_CHAR, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);
    string inputMessage = _inputMessage;
		// Расшифровываем сообщение от рабочего процесса
		int processRankSend, finishedEnvironment, finishedTry;
    finishedEnvironment = finishedTry = 0;
		double averageReward;
		decodeFinishedWorkMessage(inputMessage, processRankSend, finishedEnvironment, finishedTry, averageReward);
    // Если это сообщение от рутового процесса, то записываем в лог прием задания
    if ((0 != finishedTry) && (0 != finishedEnvironment)){
      averageRewards[finishedEnvironment - firstEnvironmentNumber][finishedTry - firstTryNumber] = averageReward;
		  unsigned long currentTime = static_cast<unsigned long>(time(0));
		  logFile << (currentTime-startTime)/(3600) << ":" << ((currentTime-startTime)%(3600))/60 << ":" << (currentTime-startTime)%(60) 
			  << "\tEnvironment: " << finishedEnvironment << "\tTry: " << finishedTry << "\tDone from root process " << processRankSend << endl; 
    }
    processesStack.push_back(processRankSend);
	}
  // Составляем сообщение о выходе и высылаем всем процессам
	char outMessage[messageLength];
	strcpy(outMessage, "$Q$");
  while (processesStack.size()){
    MPI_Send(outMessage, messageLength - 1, MPI_CHAR, processesStack[processesStack.size() - 1], messageType, MPI_COMM_WORLD);
    processesStack.pop_back();
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
void TAnalysis::decodeTaskMessage(string inputMessage, int& currentEnvironment, int& currentTry, string& runSign, double& stochasticityCoefficient, vector<int>& processesPool, unsigned int& envType){
  string featureNote;
   // Определяем кол-во процессов в пуле и их структуру
  int processPoolCapacity = 0;
  featureNote = findParameterNote(inputMessage, "$PROCQ$");
  if (featureNote != "") processPoolCapacity = atoi(featureNote.c_str());
  processesPool.resize(processPoolCapacity);
  stringstream currentProcessSign;
  for (int currentProcess = 1; currentProcess <= processPoolCapacity; ++currentProcess){
    currentProcessSign.str("");
    currentProcessSign << "$PROC" << currentProcess << "$";
    featureNote = findParameterNote(inputMessage, currentProcessSign.str());
    processesPool[currentProcess - 1] = atoi(featureNote.c_str());
  }
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
  // Определяем тип среды
  featureNote = findParameterNote(inputMessage, "$ENVTYPE$");
  if (featureNote != "") envType = strtoul(featureNote.c_str(), 0, 0);
}

// Выполнение рабочего процесса
void TAnalysis::workProcess(int argc, char **argv){
	int processRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // Определение процессом своего номера
	int processesQuantity;	
	MPI_Comm_size(MPI_COMM_WORLD, &processesQuantity); // Определение общего количества процессов
	MPI_Status status;
	const int messageLength = 500;
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
    vector<int> processesPool;
    unsigned int envType = 0;
    decodeTaskMessage(inputMessage, currentEnvironment, currentTry, runSign, stochasticityCoefficient, processesPool, envType);
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
    double averageReward = 0;
    if (processesPool.size() > 1){ // Если надо расшарить задание
      THypercubeEnvironment* environment;
      switch (envType){
        case 0 :
          environment = new THypercubeEnvironment(environmentFilename);
          break;
        case 1:
          environment = new RestrictedHypercubeEnv(environmentFilename);
          break;
        default:
          environment = new THypercubeEnvironment(environmentFilename);
      }
	    settings::fillEnvironmentSettingsFromFile(*environment, settingsFilename);
      if (stochasticityCoefficient != -1)
        environment->setStochasticityCoefficient(stochasticityCoefficient);
	    TPopulation<TAgent>* agentsPopulation = new TPopulation<TAgent>;
	    settings::fillPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
      ifstream bestPopulationFile;
      bestPopulationFile.open(bestPopulationFilename.c_str());
      if (processRank == processesPool[0]){// Если это подрутовый процесс
        int fullPopulationSize = agentsPopulation->getPopulationSize();
        int firstAgent = 1;
        // Подрутовый процесс забирает весь остаток агентов от округления
        int lastAgent = agentsPopulation->getPopulationSize() / processesPool.size() + agentsPopulation->getPopulationSize() % processesPool.size();
        agentsPopulation->setPopulationSize(lastAgent - firstAgent + 1);
        settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
        for (int currentAgent = firstAgent; currentAgent <= lastAgent; ++currentAgent)
          agentsPopulation->getPointertoAgent(currentAgent)->loadGenome(bestPopulationFile);
        // Выполняем свою часть работы
        double thisProcessReward = startBestPopulationAnalysis(*agentsPopulation, *environment, randomSeed);
       // Ждем пока выполнятся все дочерние процессы
        int processesToWait = processesPool.size() - 1;
        long double accumulatedSideReward = 0;
        while (processesToWait > 0){
          double currentReward = 0;
          MPI_Recv(&currentReward, 1, MPI_DOUBLE, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status); 
          accumulatedSideReward += currentReward * (fullPopulationSize/processesPool.size());
          --processesToWait;
        }
        //Подсчитываем полную награду (с учетом того, что у подрутового процесса больше агентов)
        averageReward = static_cast<double>(((lastAgent - firstAgent + 1) * thisProcessReward + accumulatedSideReward) / fullPopulationSize);
      }
      else{ // Если это рабочий процесс
        int fullPopulationSize = agentsPopulation->getPopulationSize();
        // Находим какой мы процесс в локальном пуле
        int processLocalRank = 1;
        for (unsigned int currentProcess = 0; currentProcess < processesPool.size(); ++currentProcess)
          if (processesPool[++processLocalRank - 1] == processRank) break;
        int firstAgent = agentsPopulation->getPopulationSize() / processesPool.size() + agentsPopulation->getPopulationSize() % processesPool.size() + 1 + 
                        (processLocalRank - 2) * (agentsPopulation->getPopulationSize() / processesPool.size());
        int lastAgent = firstAgent + agentsPopulation->getPopulationSize() / processesPool.size() - 1;
        agentsPopulation->setPopulationSize(lastAgent - firstAgent + 1);
        settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
        // Сначала пропускаем ненужных нам агентов в файле
        for (int currentAgent = 1; currentAgent < firstAgent; ++currentAgent)
          agentsPopulation->getPointertoAgent(1)->loadGenome(bestPopulationFile);
        //Записываем нужную популяцию
        for (int currentAgent = firstAgent; currentAgent <= lastAgent; ++currentAgent)
          agentsPopulation->getPointertoAgent(currentAgent-firstAgent+1)->loadGenome(bestPopulationFile);
        // Выполняем свою часть работы
        double thisProcessReward = startBestPopulationAnalysis(*agentsPopulation, *environment, randomSeed);
        // Отправляем информацию подрутовому процессу
        MPI_Send(&thisProcessReward, 1, MPI_DOUBLE, processesPool[0], messageType, MPI_COMM_WORLD);
      }
      bestPopulationFile.close();
      delete agentsPopulation;
      delete environment;
    }
    else
      averageReward = startBestPopulationAnalysis(bestPopulationFilename, environmentFilename, settingsFilename, randomSeed, stochasticityCoefficient, envType);
		// Посылаем ответ о завершении работы над заданием
		tmpStream.str(""); // Очищаем поток
    if (processRank == processesPool[0])
		  tmpStream << "$ENV$" << currentEnvironment << "$TRY$" << currentTry << "$PR$" << processRank << "$AVREW$" << averageReward;
    else
      tmpStream << "$PR$" << processRank;
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
		bool actionSuccess = (environment.forceEnvironment(agentLife[agentLifeStep - 1]) != 0);
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


