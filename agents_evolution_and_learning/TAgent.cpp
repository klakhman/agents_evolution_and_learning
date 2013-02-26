#include "TAgent.h"
#include "TEnvironment.h"
#include "TNeuralNetwork.h"
#include "TPoolNetwork.h"
#include "service.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>

using namespace std;

// Загрузка нейроконтроллера агента
void TAgent::loadController(istream& is){
	string tmp_str;
	if (!neuralController) neuralController = new TNeuralNetwork;
	is >> *neuralController;
}
// Загрузка генома нейрононтроллера
void TAgent::loadGenome(istream& is, bool extra /*=false*/){
	string tmp_str;
	is >> tmp_str; // Считываем номер более приспособленного родителя
	parents[0] = atoi(tmp_str.c_str());
	is >> tmp_str; // Считываем номер менее приспособленного родителя
	parents[1] = atoi(tmp_str.c_str());
	if (!genome) genome = new TPoolNetwork;
	if (!extra) is >> *genome;
	else genome->readNetworkExtra(is);
}

void TAgent::loadOldFormatGenome(istream& is, int inputResolution, int outputResolution){
  if (!genome) genome = new TPoolNetwork;
  genome->loadOldFormatNet(is, inputResolution, outputResolution);
}

// Выгрузка нейроконтроллера агета в файл или на экран
void TAgent::uploadController(ostream& os) const{
	if (neuralController) os << *neuralController;
}

// Выгрузка генома агента в файл или на экран (extra = true - печать полных сведений о пуле (вместе с номерами родительских пулов и временами появления в эволюции))
void TAgent::uploadGenome(ostream& os, bool extra /*= false*/) const{
	if (genome){
		os << parents[0] << "\t" << parents[1] << endl;
		if (!extra) os << *genome;
		else genome->printNetworkExtra(os);
	}
}

// Подсчет размера выходного вектора нейроконтроллера и генома агента на основе размера входного вектора среды
int TAgent::calculateOutputResolution(int inputResolution) const{
	int _outputResolution = - 1;
	int codingNeuronsQuantity = 2; // Кол-во нейронов, кодирующих одно действие агента
	// Кол-во доступных для кодирования действий при определенной размерности выходного вектора
	double actionQuantity; 
	do {
		++_outputResolution;
		actionQuantity = 1;
		// Подсчитываем кол-во комбинаций 
		// Комбинаторное кол-во сочетаний при условии, что каждое действие кодируется двумя нейронами и противоположные действия кодируются разными парами нейронов - n!/(k! * (n-k)!), при этом n - output resolution, k - кол-во нейронов кодирующих одно действие
		for (int i = 1; i <= _outputResolution; ++i)
			actionQuantity *= i;
		for (int i = 1; i <= _outputResolution - codingNeuronsQuantity; ++i)
			actionQuantity /= i;
		for (int i = 1; i <= codingNeuronsQuantity; ++i)
			actionQuantity /= i;
	} while (actionQuantity < (2 * inputResolution));
	return _outputResolution;
}

// Генерация случайного минимального возможного генома агента
void TAgent::generateMinimalAgent(int inputResolution){
	if (!genome) 
		genome = new TPoolNetwork;
	else 
		genome->erasePoolNetwork();
	int currentNeuron = 1;
	int outputResolution = calculateOutputResolution(inputResolution);
	for (currentNeuron; currentNeuron <= inputResolution; ++currentNeuron)
    genome->addPool(TPoolNetwork::INPUT_POOL, 1, service::uniformDistribution(-0.5, 0.5), 0, 1);
	for (currentNeuron; currentNeuron <= inputResolution + outputResolution; ++currentNeuron)
    genome->addPool(TPoolNetwork::OUTPUT_POOL, 3, service::uniformDistribution(-0.5, 0.5), 0, 1);
  genome->addPool(TPoolNetwork::HIDDEN_POOL, 2, service::uniformDistribution(-0.5, 0.5), 0, primarySystemogenesisSettings.initialPoolCapacity);
	int currentConnection = 1;
	for (currentNeuron = inputResolution + 1; currentNeuron <= inputResolution + outputResolution; ++currentNeuron){
		genome->addConnection(inputResolution + outputResolution + 1, currentNeuron, service::uniformDistribution(-0.5, 0.5), 0, primarySystemogenesisSettings.initialDevelopSynapseProbability, true, 0, currentConnection); 
		++currentConnection;
	}
	for (currentNeuron = 1; currentNeuron <= inputResolution; ++currentNeuron){
		genome->addConnection(currentNeuron, inputResolution + outputResolution + 1, service::uniformDistribution(-0.5, 0.5), 0, primarySystemogenesisSettings.initialDevelopSynapseProbability, true, 0, currentConnection); 
		++currentConnection;
	}
}

// Линейная процедра первичного системогеназа (когда происходит однозначная трансляция генотипа) - используется, когда нет ни настоящего системогенеза, ни обучения
void TAgent::linearSystemogenesis(){
	if (!neuralController) 
		neuralController = new TNeuralNetwork;
	else 
		neuralController->eraseNeuralNetwork();
	// Сначала создаем все нейроны в сети
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		neuralController->addNeuron(genome->getPoolType(currentPool), genome->getPoolLayer(currentPool), genome->getPoolBiasMean(currentPool), true, currentPool);
	// Потом создаем синапсы
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		for (int currentConnection = 1; currentConnection <= genome->getPoolInputConnectionsQuantity(currentPool); ++currentConnection)
			neuralController->addSynapse(genome->getConnectionPrePoolID(currentPool, currentConnection), currentPool, genome->getConnectionWeightMean(currentPool, currentConnection), genome->getConnectionEnabled(currentPool, currentConnection));
	// И создаем предикторные связи
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= genome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPredConnection)
			neuralController->addPredConnection(genome->getPredConnectionPrePoolID(currentPool, currentPredConnection), currentPool, genome->getPredConnectionEnabled(currentPool, currentPredConnection));
}

// Декодирование идентификатора совершаемого агентом действия
vector<double> TAgent::decodeAction(double outputVector[]){
	// Два максимальных выхода нейронов сети
	double maxOutputs[] = {-1, -1};
	// Номера двух нейронов с максимальным выходом
	int maxOutputNeurons[] = {0 , 0};

	// Определение двух максимальных выходов нейронов
	for (int currentNeuron = 1; currentNeuron <= neuralController->getOutputResolution(); ++currentNeuron)
		if (outputVector[currentNeuron - 1] > maxOutputs[0]){
			maxOutputs[1] = maxOutputs[0];
			maxOutputNeurons[1] = maxOutputNeurons[0];
			maxOutputs[0] = outputVector[currentNeuron - 1];
			maxOutputNeurons[0] = currentNeuron;
		}
		else if (outputVector[currentNeuron - 1] > maxOutputs[1]){
			maxOutputs[1] = outputVector[currentNeuron - 1];
			maxOutputNeurons[1] = currentNeuron;
		}
	// Меняем местами номера, если они не по порядку (нас не интересует их порядок)
	if (maxOutputNeurons[0] > maxOutputNeurons[1]){
		int tmp = maxOutputNeurons[0];
		maxOutputNeurons[0] = maxOutputNeurons[1];
		maxOutputNeurons[1] = tmp;
	}
	// Расшифровка выходов сети
	int fisrtCheckNeuron = 1; // Номер первого нейрона в проверяемой паре
	int secondCheckNeuron = fisrtCheckNeuron + 1; // Номер второго нейрона в проверяемой паре
	int pairCount = 0; // Количество проверенных пар
	// Считаем кол-во пар до нужной пары нейронов
	while ((fisrtCheckNeuron != maxOutputNeurons[0]) || (secondCheckNeuron != maxOutputNeurons[1])) {
		if (secondCheckNeuron < neuralController->getOutputResolution())
			++secondCheckNeuron;
		else{
			++fisrtCheckNeuron;
			secondCheckNeuron = fisrtCheckNeuron + 1;
		}
		++pairCount;
	}	
	int actionNumber = pairCount % (2 * neuralController->getInputResolution()); // Итоговый номер совершаемого действия (от 0 до 2*environmentResolution)
	// Если вычисленный номер действия четный, то это перевод из 0 в 1, и соответственно наоброт
	double actionCode = (actionNumber % 2 ? -1 : 1) * (actionNumber / 2 + 1);

	vector<double> actions(getActionResolution());
	actions[0] = actionCode;

	return actions;
}

// Моделирование жизни агента (rewardCalculate - опциональный признак автоматического подсчета награды, которую агент достиг в течение жизни (можно выключать для оптимизации для больших сред))
void TAgent::life(TEnvironment& environment, int agentLifeTime, bool rewardCalculate /*= true*/){
	double* environmentVector = new double[neuralController->getInputResolution()];
	double* outputVector = new double[neuralController->getOutputResolution()];

	agentLife.resize(agentLifeTime);

	neuralController->reset();
	for (int agentLifeStep = 1; agentLifeStep <= agentLifeTime; ++agentLifeStep){
		// Здесь в теории надо вместо просто получения вектора среды поставить процедуру кодировщика
		environment.getCurrentEnvironmentVector(environmentVector);
		neuralController->calculateNetwork(environmentVector);
		neuralController->getOutputVector(outputVector);
		agentLife[agentLifeStep - 1] = decodeAction(outputVector);
		// Действуем на среду и проверяем успешно ли действие
		bool actionSuccess = environment.forceEnvironment(agentLife[agentLifeStep - 1]);
		if (!actionSuccess) agentLife[agentLifeStep - 1][0] = 0;
		// Проводим процедуру обучения (если такой режим)
		if (1 == learningSettings.learningMode) learning();
    // !!!! Случайное обучение - только для контроля качества обучения !!!!!
    else if (2 == learningSettings.learningMode) randomLearning();
	}
	if (rewardCalculate) 
		reward = environment.calculateReward(agentLife, agentLifeTime);
	else 
		reward = 0;
	delete []outputVector;
	delete []environmentVector;
}

// Оператор присваивания (фактически полное копирование агента, включая геном, но не включая контроллер - создание новых структур)
TAgent& TAgent::operator=(const TAgent& sourceAgent){
	*genome = *sourceAgent.genome;
	reward = sourceAgent.reward;
	parents[0] = sourceAgent.parents[0];
	parents[1] = sourceAgent.parents[1];
	// Копируем настройки первичного системогенеза
	primarySystemogenesisSettings.primarySystemogenesisMode = sourceAgent.primarySystemogenesisSettings.primarySystemogenesisMode;
	primarySystemogenesisSettings.initialPoolCapacity = sourceAgent.primarySystemogenesisSettings.initialPoolCapacity;
	primarySystemogenesisSettings.initialDevelopSynapseProbability = sourceAgent.primarySystemogenesisSettings.initialDevelopSynapseProbability;
	primarySystemogenesisSettings.initialDevelopPredConnectionProbability = sourceAgent.primarySystemogenesisSettings.initialDevelopPredConnectionProbability;
	primarySystemogenesisSettings.primarySystemogenesisTime = sourceAgent.primarySystemogenesisSettings.primarySystemogenesisTime;
	primarySystemogenesisSettings.spontaneousActivityProb = sourceAgent.primarySystemogenesisSettings.spontaneousActivityProb;
	primarySystemogenesisSettings.activeNeuronsPercent = sourceAgent.primarySystemogenesisSettings.activeNeuronsPercent;
	primarySystemogenesisSettings.synapsesActivityTreshold = sourceAgent.primarySystemogenesisSettings.synapsesActivityTreshold; 
	primarySystemogenesisSettings.significanceTreshold = sourceAgent.primarySystemogenesisSettings.significanceTreshold; 
	// Копируем настройки обучения
	learningSettings = sourceAgent.learningSettings;

	return *this;
}

//// Временная процедура печати жизни агента
//void TAgent::printLife(TEnvironment& environment, int agentLifeTime){
//	double* environmentVector = new double[neuralController->getInputResolution()];
//	double* outputVector = new double[neuralController->getOutputResolution()];
//	agentLife = new double[agentLifeTime];
//	// Здесь в теории надо вместо просто получения вектора среды поставить процедуру кодировщика
//	environment.getCurrentEnvironmentVector(environmentVector);
//	cout << "Environment: ";
//	for (int currentBit = 1; currentBit <= neuralController->getInputResolution(); ++currentBit)
//		cout << environmentVector[currentBit - 1] << "\t";
//	cout << endl << endl;
//
//	neuralController->reset();
//	for (int agentLifeStep = 1; agentLifeStep <= agentLifeTime; ++agentLifeStep){
//		neuralController->calculateNetwork(environmentVector);
//		neuralController->getOutputVector(outputVector);
//		cout << "Output: ";
//		for (int currentBit = 1; currentBit <= neuralController->getOutputResolution(); ++currentBit)
//			cout << outputVector[currentBit - 1] << "\t";
//		cout << endl;
//		agentLife[agentLifeStep - 1] = decodeAction(outputVector);
//		// Действуем на среду и проверяем успешно ли действие
//		bool actionSuccess = environment.forceEnvironment(agentLife[agentLifeStep - 1]);
//		environment.getCurrentEnvironmentVector(environmentVector);
//		cout << "Environment: ";
//		for (int currentBit = 1; currentBit <= neuralController->getInputResolution(); ++currentBit)
//			cout << environmentVector[currentBit - 1] << "\t";
//		cout << endl;
//		if (!actionSuccess) agentLife[agentLifeStep - 1] = 0;
//		cout << "Action: " <<  agentLife[agentLifeStep - 1] << endl;
//	}
//	reward = environment.calculateReward(agentLife, agentLifeTime);
//	delete []agentLife;
//	agentLife = 0;
//	delete []outputVector;
//	delete []environmentVector;
//}

//-------------------- МЕТОДЫ, ОБЕСПЕЧИВАЮЩИЕ ПЕРВИЧНЫЙ СИСТЕМОГЕНЕЗ --------------------

// Построение первичного контроллера агента
void TAgent::buildPrimaryNeuralController(){
	int maxCapacity = 0; // Максимальная размерность пула
	// Находим максимальную размерность пула
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		if (maxCapacity < genome->getPoolCapacity(currentPool))
			maxCapacity = genome->getPoolCapacity(currentPool);
	// Создаем двумерный массив принадлежности различных нейронов пулам (по идентификатору), чтобы потом не проходить несколько раз по сети из пулов или нейросети
	int** poolsAndNeurons = new int*[genome->getPoolsQuantity()];
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		poolsAndNeurons[currentPool - 1] = new int[maxCapacity];
	// Проходим по всем пулам и создаем нейроны
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		// Создаем все нейроны пула
		for (int currentNeuron = 1; currentNeuron <= genome->getPoolCapacity(currentPool); ++currentNeuron){
			neuralController->addNeuron(genome->getPoolType(currentPool), genome->getPoolLayer(currentPool),  
												service::normalDistribution(genome->getPoolBiasMean(currentPool), genome->getPoolBiasVariance(currentPool)), 
												true, genome->getPoolID(currentPool));
			poolsAndNeurons[currentPool - 1][currentNeuron - 1] = neuralController->getNeuronsQuantity();
		}
	// Проходимся по все пулам и нейронам и создаем синапсы
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
    for (int currentNeuron = 1; currentNeuron <= genome->getPoolCapacity(currentPool); ++currentNeuron)
			// Проходимся по всем входным связям пула
			for (int currentPoolConnection = 1; currentPoolConnection <= genome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
				if (genome->getConnectionEnabled(currentPool, currentPoolConnection)){
					int currentPrePool = genome->getConnectionPrePoolID(currentPool, currentPoolConnection);
					// Проходимся по всем потенциальным пресинаптическим нейронам для текущего нейрона
					for (int currentPreNeuron = 1; currentPreNeuron <= genome->getPoolCapacity(currentPrePool); ++currentPreNeuron)
						if (service::uniformDistribution(0, 1) < genome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection))
							neuralController->addSynapse(poolsAndNeurons[currentPrePool-1][currentPreNeuron-1], poolsAndNeurons[currentPool-1][currentNeuron-1], 
																service::normalDistribution(genome->getConnectionWeightMean(currentPool, currentPoolConnection), genome->getConnectionWeightVariance(currentPool, currentPoolConnection)), true);
        }
	// Создаем все предикторные связи
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
    for (int currentNeuron = 1; currentNeuron <= genome->getPoolCapacity(currentPool); ++currentNeuron)
			// Проходимся по всем входным предикторным связям пула
			for (int currentPoolPredConnection = 1; currentPoolPredConnection <= genome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
				if (genome->getPredConnectionEnabled(currentPool, currentPoolPredConnection)){
					int currentPrePool = genome->getPredConnectionPrePoolID(currentPool, currentPoolPredConnection);
					// Проходимся по всем потенциальным пресинаптическим нейронам для текущего нейрона
					for (int currentPreNeuron = 1; currentPreNeuron <= genome->getPoolCapacity(currentPrePool); ++currentPreNeuron)
						if (service::uniformDistribution(0, 1) < genome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection))
							neuralController->addPredConnection(poolsAndNeurons[currentPrePool-1][currentPreNeuron-1], poolsAndNeurons[currentPool-1][currentNeuron-1], true);

				}

	// Проводим зачистку
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		delete [](poolsAndNeurons[currentPool - 1]);
	delete []poolsAndNeurons;
}

// Функция отбора активирующихся нейронов
void TAgent::neuronsSelection(double neuronsSummaryPotential[]){
	// Считаем кол-во активирующихся нейронов
	int neededActiveNeurons = static_cast<int>(primarySystemogenesisSettings.activeNeuronsPercent / 100.0 * (neuralController->getNeuronsQuantity() - neuralController->getInputResolution() - neuralController->getOutputResolution()) + 0.5);
	// Для отбора нейронов используем рулеточный алгоритм (работаем только с той частью списка в которой записаны интернейроны)
	// Сначала нормируем все значения - так, чтобы у нейронов с отрицательным потенциалом была возможность отобраться
	double minSummaryPotential = 10000.0;
	for (int currentNeuron = neuralController->getInputResolution() + neuralController->getOutputResolution() + 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
		if (minSummaryPotential > neuronsSummaryPotential[currentNeuron - 1])
			minSummaryPotential = neuronsSummaryPotential[currentNeuron - 1];
	// Нормируем все потенциалы
	for (int currentNeuron = neuralController->getInputResolution() + neuralController->getOutputResolution() + 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
		neuronsSummaryPotential[currentNeuron - 1] -= minSummaryPotential - 1; // Делаем так, чтобы ни у одного нейрона не было нулевого суммарного потенциала (или отрицательного, если это вообще возможно)
	
	double totalPotential = 0;
	for (int currentNeuron = neuralController->getInputResolution() + neuralController->getOutputResolution() + 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
		totalPotential += neuronsSummaryPotential[currentNeuron - 1];
	const double checkChoice = -5000.0; // Признак выбранного нейрона (выбран произвольно)
	int currentActiveNeuronsQuantity = 0;
	// Пока мы не набрали достаточное кол-во нейронов
	while (currentActiveNeuronsQuantity < neededActiveNeurons){
		double randomPotential = service::uniformDistribution(0.0001, totalPotential);
		double currentPotential = 0;
		int currentNeuron = neuralController->getInputResolution() + neuralController->getOutputResolution();
		while (randomPotential > currentPotential)
			currentPotential += max(neuronsSummaryPotential[++currentNeuron - 1], 0.0); // Для того, чтобы не сумировать уже отобранные нейроны
		totalPotential -= neuronsSummaryPotential[currentNeuron - 1];
		neuronsSummaryPotential[currentNeuron - 1] = checkChoice;
		++currentActiveNeuronsQuantity;
	}
	// Изменяем информацию о нейронах (разделяем их на активные и молчащие)
	for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
    if (neuralController->getNeuronType(currentNeuron) == TNeuralNetwork::HIDDEN_NEURON) // Если нейрон внутренний
			if (neuronsSummaryPotential[currentNeuron - 1] == checkChoice)
				neuralController->setNeuronActive(currentNeuron, true);
			else 
				neuralController->setNeuronActive(currentNeuron, false);
		else
			neuralController->setNeuronActive(currentNeuron, true);
}

//Техническая структура, используемая в методе отбора синапсов (не может быть локальной, так как стандарт 2003 не позволяет)
struct SSelectionSynapse{
	int synapseID;
	double synapseSummaryPotential;
  // Функция сравнения структур (для функции qsort())
  /*static int compare(const void* first, const void* second){
      double compare = reinterpret_cast<const SSelectionSynapse*>(first)->synapseSummaryPotential - reinterpret_cast<const SSelectionSynapse*>(second)->synapseSummaryPotential;
      if (!compare) return 0;
      else if (compare > 0) return 1;
      else return -1;
    }*/
  // Функция сравнения структур (для функции std::sort())
  bool operator<(const SSelectionSynapse& right) const {
    return (synapseSummaryPotential < right.synapseSummaryPotential);
  }
};

// Функция отбора наиболее активных синапсов
void TAgent::synapsesSelection(double synapsesSummaryPotential[]){
  // Массив связей вместе с их суммарными потенциалами
	SSelectionSynapse* selectionSynapses = new SSelectionSynapse[neuralController->getSynapsesQuantity()];
	// Проходимся по всем синапсам
	for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
		for (int currentSynapse = 1; currentSynapse <= neuralController->getNeuronInputSynapsesQuantity(currentNeuron); ++currentSynapse){
			selectionSynapses[neuralController->getSynapseID(currentNeuron, currentSynapse) - 1].synapseID = neuralController->getSynapseID(currentNeuron, currentSynapse) - 1;
			selectionSynapses[neuralController->getSynapseID(currentNeuron, currentSynapse) - 1].synapseSummaryPotential = synapsesSummaryPotential[neuralController->getSynapseID(currentNeuron, currentSynapse) - 1];
		}
	// Сортируем связи по значению суммарного потенциала
	/*for (int i = 0; i < neuralController->getSynapsesQuantity(); ++i)
		for (int j = 0; j < neuralController->getSynapsesQuantity() - i - 1; ++j)
			if (selectionSynapses[j].synapseSummaryPotential > selectionSynapses[j+1].synapseSummaryPotential){
				SSelectionSynapse tmp = selectionSynapses[j+1];
				selectionSynapses[j+1] = selectionSynapses[j];
				selectionSynapses[j] = tmp;
			}*/
  //qsort(selectionSynapses, neuralController->getSynapsesQuantity(), sizeof(*selectionSynapses), SSelectionSynapse::compare);
  sort(selectionSynapses, selectionSynapses + neuralController->getSynapsesQuantity());
	// Находим порог среднего потенциала по синапсу (через персентиль общего дискретного распределения среднего потенциала через синапсы)
	double percentileValue = selectionSynapses[static_cast<int>((100 - primarySystemogenesisSettings.synapsesActivityTreshold)/100.0*neuralController->getSynapsesQuantity())].synapseSummaryPotential;
	// Проходимся через все синапсы и удаляем те, у которых суммарный потенциал меньше порога (с определенными условиями - смотри далее)
	for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
		// Связь не подлежит проверке на отбор если один из неронов не является активным !!! или если пресинаптический нейрон входной или постсинаптический нейрон выходной !!!
    if ((neuralController->getNeuronActive(currentNeuron)) && (neuralController->getNeuronType(currentNeuron) != TNeuralNetwork::OUTPUT_NEURON))
			for (int currentSynapse = 1; currentSynapse <= neuralController->getNeuronInputSynapsesQuantity(currentNeuron); ++currentSynapse){
				int currentPreNeuron = neuralController->getSynapsePreNeuronID(currentNeuron, currentSynapse);
				// Если пресинаптический нейрон не входной и он активен
        if ((neuralController->getNeuronActive(currentPreNeuron)) && (neuralController->getNeuronType(currentPreNeuron) != TNeuralNetwork::INPUT_NEURON))
					// Если синапс подлежит отбору и сумарный потенциал ниже порога, то надо его удалить
					if (synapsesSummaryPotential[neuralController->getSynapseID(currentNeuron, currentSynapse) - 1] < percentileValue){
						neuralController->deleteSynapse(currentNeuron, currentSynapse, false);
						--currentSynapse; // Остаемся на том же самом номере синапса, так как мы удалили предыдущий
					}
			}

	neuralController->fixSynapsesIDs();
	delete []selectionSynapses;
}

// Функция отбора предикторных связей
void TAgent::predConnectionsSelection(double predictorSignificance[]){
	// Проходимся по всем связям, которые подлежат отбору (только между активными нейронами)
	for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
		if (neuralController->getNeuronActive(currentNeuron)) // Если нейрон активен
			for (int currentPredConnection = 1; currentPredConnection <= neuralController->getNeuronInputPredConnectionsQuantity(currentNeuron); ++currentPredConnection)
				// Если пресинаптический нейрон тоже активен
				if (neuralController->getNeuronActive(neuralController->getPredConnectionPreNeuronID(currentNeuron, currentPredConnection)))
					// Если значимость предсказания ниже порога, то удаляем связь
					if (predictorSignificance[neuralController->getPredConnectionID(currentNeuron, currentPredConnection) - 1] <= primarySystemogenesisSettings.significanceTreshold){
						neuralController->deletePredConnection(currentNeuron, currentPredConnection, false);
						--currentPredConnection; // Остаемся на том же номере связи
					}
	neuralController->fixPredConnectionsIDs();
}

// Основной метод первичного системогенеза 
void TAgent::primarySystemogenesis(){
	neuralController->eraseNeuralNetwork();
	// Строим полную первичную сеть агента
	buildPrimaryNeuralController();
	// Сначала создаем все необходимые структуры для записи статистики
	double* neuronsSummaryPotential = new double[neuralController->getNeuronsQuantity()];
	memset(neuronsSummaryPotential, 0, neuralController->getNeuronsQuantity() * sizeof(*neuronsSummaryPotential));
	double* synapsesSummaryPotential = new double[neuralController->getSynapsesQuantity()];
	memset(synapsesSummaryPotential, 0, neuralController->getSynapsesQuantity() * sizeof(*synapsesSummaryPotential));
	double* predictorSignificance = new double[neuralController->getPredConnectionsQuantity()];
	memset(predictorSignificance, 0, neuralController->getPredConnectionsQuantity()*sizeof(*predictorSignificance));
	neuralController->reset();
	// Проводим прогон первичного системогенеза и подсчет суммарных характеристик
	for (int systemogenesisTime = 1; systemogenesisTime <= primarySystemogenesisSettings.primarySystemogenesisTime; ++systemogenesisTime){
		neuralController->calculateSpontaneousNetwork(primarySystemogenesisSettings.spontaneousActivityProb);
		// Проходимся по всем нейронам и обновляем суммарные характеристики
		for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron){
			neuronsSummaryPotential[currentNeuron - 1] += neuralController->getNeuronPotential(currentNeuron);
			int inputSynapsesQuantity = neuralController->getNeuronInputSynapsesQuantity(currentNeuron);
			for (int currentSynapse = 1; currentSynapse <= inputSynapsesQuantity; ++currentSynapse)
				// Если связь прямая
				if (neuralController->getNeuronLayer(neuralController->getSynapsePreNeuronID(currentNeuron, currentSynapse)) < neuralController->getNeuronLayer(currentNeuron))
					synapsesSummaryPotential[neuralController->getSynapseID(currentNeuron, currentSynapse) - 1] += 
						fabs(neuralController->getSynapseWeight(currentNeuron, currentSynapse) * neuralController->getNeuronCurrentOut(neuralController->getSynapsePreNeuronID(currentNeuron, currentSynapse)));
				// Если связь обратная
				else
					synapsesSummaryPotential[neuralController->getSynapseID(currentNeuron, currentSynapse) - 1] += 
							fabs(neuralController->getSynapseWeight(currentNeuron, currentSynapse) * neuralController->getNeuronPreviousOut(neuralController->getSynapsePreNeuronID(currentNeuron, currentSynapse)));
			int inputPredConnectionsQuantity = neuralController->getNeuronInputPredConnectionsQuantity(currentNeuron);
			for (int currentPredConnection = 1; currentPredConnection <= inputPredConnectionsQuantity; ++currentPredConnection){
				int preNeuronID = neuralController->getPredConnectionPreNeuronID(currentNeuron, currentPredConnection);
        // Если пресинаптический нейрон правильно предсказал активацию / молчание постсинаптического
				if (((neuralController->getNeuronCurrentOut(currentNeuron) > TNeuron::ACTIVITY_TRESHOLD) && (neuralController->getNeuronPreviousOut(preNeuronID) > TNeuron::ACTIVITY_TRESHOLD)) ||
            ((neuralController->getNeuronCurrentOut(currentNeuron) <= TNeuron::ACTIVITY_TRESHOLD) && (neuralController->getNeuronPreviousOut(preNeuronID) <= TNeuron::ACTIVITY_TRESHOLD)))
					predictorSignificance[neuralController->getPredConnectionID(currentNeuron, currentPredConnection) - 1] += 1;
      }
		}
	}
	// Нормировка показателей статистической значимости предикторных связей
	for (int currentPredConnection = 1; currentPredConnection <= neuralController->getPredConnectionsQuantity(); ++currentPredConnection)
		predictorSignificance[currentPredConnection - 1] /= (primarySystemogenesisSettings.primarySystemogenesisTime - 1);
	// Производим отбор
	neuronsSelection(neuronsSummaryPotential);
	synapsesSelection(synapsesSummaryPotential);
	predConnectionsSelection(predictorSignificance);

	// Производим очистку
	delete []neuronsSummaryPotential;
	delete []synapsesSummaryPotential;
	delete []predictorSignificance;
}

// Альтернативный системогенез (активным становится только один нейрон в пуле с детерминированной структурой связей)
void TAgent::alternativeSystemogenesis(){
  int maxCapacity = 0; // Максимальная размерность пула
	// Находим максимальную размерность пула
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		if (maxCapacity < genome->getPoolCapacity(currentPool))
			maxCapacity = genome->getPoolCapacity(currentPool);
	// Создаем двумерный массив принадлежности различных нейронов пулам (по идентификатору), чтобы потом не проходить несколько раз по сети из пулов или нейросети
	int** poolsAndNeurons = new int*[genome->getPoolsQuantity()];
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		poolsAndNeurons[currentPool - 1] = new int[maxCapacity];
	// Проходим по всем пулам и создаем нейроны
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		// Создаем все нейроны пула
		for (int currentNeuron = 1; currentNeuron <= genome->getPoolCapacity(currentPool); ++currentNeuron){
      if (1 == currentNeuron) // Первый нейрон в пуле всегда активен и детерминирован
        neuralController->addNeuron(genome->getPoolType(currentPool), genome->getPoolLayer(currentPool),  
												            genome->getPoolBiasMean(currentPool), 
												            true, genome->getPoolID(currentPool));
      else
			  neuralController->addNeuron(genome->getPoolType(currentPool), genome->getPoolLayer(currentPool),  
				  								          service::normalDistribution(genome->getPoolBiasMean(currentPool), genome->getPoolBiasVariance(currentPool)), 
					  							          false, genome->getPoolID(currentPool));
			poolsAndNeurons[currentPool - 1][currentNeuron - 1] = neuralController->getNeuronsQuantity();
		}
	// Проходимся по все пулам и нейронам и создаем синапсы
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
    for (int currentNeuron = 1; currentNeuron <= genome->getPoolCapacity(currentPool); ++currentNeuron)
			// Проходимся по всем входным связям пула
			for (int currentPoolConnection = 1; currentPoolConnection <= genome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
				if (genome->getConnectionEnabled(currentPool, currentPoolConnection)){
					int currentPrePool = genome->getConnectionPrePoolID(currentPool, currentPoolConnection);
					// Проходимся по всем потенциальным пресинаптическим нейронам для текущего нейрона
					for (int currentPreNeuron = 1; currentPreNeuron <= genome->getPoolCapacity(currentPrePool); ++currentPreNeuron)
            if ((1 == currentNeuron) && (1 == currentPreNeuron)) // Если это связь между активными нейронами, то она детерминирована
              neuralController->addSynapse(poolsAndNeurons[currentPrePool-1][currentPreNeuron-1], poolsAndNeurons[currentPool-1][currentNeuron-1], 
																          genome->getConnectionWeightMean(currentPool, currentPoolConnection), true);
						else if (service::uniformDistribution(0, 1) < genome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection))
							neuralController->addSynapse(poolsAndNeurons[currentPrePool-1][currentPreNeuron-1], poolsAndNeurons[currentPool-1][currentNeuron-1], 
															          	service::normalDistribution(genome->getConnectionWeightMean(currentPool, currentPoolConnection), genome->getConnectionWeightVariance(currentPool, currentPoolConnection)), true);
        }
	// Создаем все предикторные связи
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
    for (int currentNeuron = 1; currentNeuron <= genome->getPoolCapacity(currentPool); ++currentNeuron)
			// Проходимся по всем входным предикторным связям пула
			for (int currentPoolPredConnection = 1; currentPoolPredConnection <= genome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
				if (genome->getPredConnectionEnabled(currentPool, currentPoolPredConnection)){
					int currentPrePool = genome->getPredConnectionPrePoolID(currentPool, currentPoolPredConnection);
					// Проходимся по всем потенциальным пресинаптическим нейронам для текущего нейрона
					for (int currentPreNeuron = 1; currentPreNeuron <= genome->getPoolCapacity(currentPrePool); ++currentPreNeuron)
            if ((1 == currentNeuron) && (1 == currentPreNeuron)) // Если это связь между активными нейронами, то она детерминирована
              neuralController->addPredConnection(poolsAndNeurons[currentPrePool-1][currentPreNeuron-1], poolsAndNeurons[currentPool-1][currentNeuron-1], true);
						else if (service::uniformDistribution(0, 1) < genome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection))
							neuralController->addPredConnection(poolsAndNeurons[currentPrePool-1][currentPreNeuron-1], poolsAndNeurons[currentPool-1][currentNeuron-1], true);

				}

	// Проводим зачистку
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		delete [](poolsAndNeurons[currentPool - 1]);
	delete []poolsAndNeurons;

}

//-------------------- МЕТОДЫ, ОБЕСПЕЧИВАЮЩИЕ ОБУЧЕНИЕ АГЕНТА --------------------

// Детекция рассогласования на нейроне
// 0 - отсутствие рассогласования, 1 - рассогласование типа "предсказана активация - ее нет", 2 - рассогласование типа "предсказано молчание - есть активация"
int TAgent::mismatchDetection(int neuronNumber){
	// Если нейрон не активен или он не внутренний, то он не может быть рассогласован
  if ((! neuralController->getNeuronActive(neuronNumber)) || (neuralController->getNeuronType(neuronNumber) != TNeuralNetwork::HIDDEN_NEURON))
		return 0;

	int activePrediction = 0; // Кол-во связей, предсказавшее активацию
	int silentPrediction = 0; // Кол-во связей, предсказавшее молчание
  int inputPredConnectionsQuantity = neuralController->getNeuronInputPredConnectionsQuantity(neuronNumber);
	for (int currentPredConnection = 1; currentPredConnection <= inputPredConnectionsQuantity; ++currentPredConnection){
		int currentPreNeuron = neuralController->getPredConnectionPreNeuronID(neuronNumber, currentPredConnection);
		// Если предсказывающий нейрон активен
		if (neuralController->getNeuronActive(currentPreNeuron))
			if (neuralController->getNeuronPreviousOut(currentPreNeuron) > TNeuron::ACTIVITY_TRESHOLD) // Если предсказывающий нейрон был возбужден на предыдущем такте
				++activePrediction;
			else
				++silentPrediction;
	}
	int mismatchCheck = 0;
	if (activePrediction + silentPrediction){ // Если есть хотя бы один предсказывающий нейрон
		// Если была предсказана активация, но ее нет
		if ((activePrediction/static_cast<double>(activePrediction+silentPrediction) > learningSettings.mismatchSignificanceTreshold) && (neuralController->getNeuronCurrentOut(neuronNumber) <= TNeuron::ACTIVITY_TRESHOLD))
			mismatchCheck = 1;
		// Если было предсказано молчание, но была активация
		else if ((silentPrediction/static_cast<double>(activePrediction+silentPrediction) > learningSettings.mismatchSignificanceTreshold) && (neuralController->getNeuronCurrentOut(neuronNumber) > TNeuron::ACTIVITY_TRESHOLD))
			mismatchCheck = 2;
		else 
			mismatchCheck = 0;
	}
	else
		mismatchCheck = 0;
	
	return mismatchCheck;
}

// Процедура нахождения наиболее активного "спящего" нейрона в пуле (возвращает ноль, если нет подходящего нейрона - нет активных на данном такте времени или не осталось молчащих)
int TAgent::findMostActiveSilentNeuron(int poolNumber){
	int mostActiveNeuronNumber = 0;
	double mostActiveNeuronPotential = 0;
	for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
    // Если нейрон спящий и из нужного на пула и он активен на текущем такте времени
		if (! neuralController->getNeuronActive(currentNeuron) && (neuralController->getNeuronParentPoolID(currentNeuron) == poolNumber) &&
        (neuralController->getNeuronPotential(currentNeuron) > mostActiveNeuronPotential)){
			mostActiveNeuronPotential = neuralController->getNeuronPotential(currentNeuron);
			mostActiveNeuronNumber = currentNeuron;
    }
	return mostActiveNeuronNumber;
}

// Процедура модификации синаптических связей при обучении рассогласованного нейрона (для активирующегося нейрона оставляем только связи от активных на текущем такте нейронов и добавляем связь на рассогласованный нейрон)
void TAgent::modifySynapsesStructure(int mismatchedNeuron, int activatedNeuron, int mismatchType){
	for (int currentSynapse = 1; currentSynapse <= neuralController->getNeuronInputSynapsesQuantity(activatedNeuron); ++currentSynapse)
		if ((neuralController->getNeuronActive(neuralController->getSynapsePreNeuronID(activatedNeuron, currentSynapse))) 
				&& (neuralController->getNeuronCurrentOut(neuralController->getSynapsePreNeuronID(activatedNeuron, currentSynapse)) <= TNeuron::ACTIVITY_TRESHOLD)){
			neuralController->deleteSynapse(activatedNeuron, currentSynapse, false); // !!! ЗДЕСЬ И ДАЛЕЕ МЫ НЕ КОРРЕКТИРУЕМ ID СВЯЗЕЙ ПОСЛЕ УДАЛЕНИЯ И ДОБАВЛЕНИЯ, ПОЭТОМУ ТАМ МОЖЕТ БЫТЬ КАША (ПОКА ЭТО НЕ КРИТИЧНО)
			--currentSynapse; // Остаемся на том же синапсе
		}
	// Добавляем связь между активирующимся нейроном и рассогласованным
	double interSynapseWeight;
	if (mismatchType == 1)
		interSynapseWeight = service::uniformDistribution(0.5, 1);
	else
		interSynapseWeight = service::uniformDistribution(-1, -0.5);
	// Если связь уже существует, то просто меняем вес
	int addSynapseNumber = neuralController->findSynapseNumber(activatedNeuron, mismatchedNeuron);
	if (addSynapseNumber)
		neuralController->setSynapseWeight(mismatchedNeuron, addSynapseNumber, interSynapseWeight);
	else
		neuralController->addSynapse(activatedNeuron, mismatchedNeuron, interSynapseWeight, true);
}

// Процедура модификации структуры предикторных связей (переносим связи, которые предсказали активацию с рассогласованного нейрона на включающийся)
void TAgent::modifyPredConnectionsStructure(int mismatchedNeuron, int activatedNeuron){
	for (int currentPredConnection = 1; currentPredConnection <= neuralController->getNeuronInputPredConnectionsQuantity(mismatchedNeuron); ++currentPredConnection)
		// Если связь предсказала активацию (отсекаем таким образом сразу связи от молчащих нейронов)
		if (neuralController->getNeuronPreviousOut(neuralController->getPredConnectionPreNeuronID(mismatchedNeuron, currentPredConnection)) >TNeuron::ACTIVITY_TRESHOLD)
			// Если такой связи еще нет у активирующегося нейрона, то добавляем ее
			if (! neuralController->findPredConnectionNumber(neuralController->getPredConnectionPreNeuronID(mismatchedNeuron, currentPredConnection), activatedNeuron))
				neuralController->addPredConnection(neuralController->getPredConnectionPreNeuronID(mismatchedNeuron, currentPredConnection), activatedNeuron, true);
				// !!! ЗДЕСЬ И ДАЛЕЕ МЫ НЕ КОРРЕКТИРУЕМ ID СВЯЗЕЙ ПОСЛЕ УДАЛЕНИЯ И ДОБАВЛЕНИЯ, ПОЭТОМУ ТАМ МОЖЕТ БЫТЬ КАША (ПОКА ЭТО НЕ КРИТИЧНО)
}

// Процедура самообучения рассогласованного нейрона
void TAgent::selfLearnNeuron(int mismatchedNeuron, int mismatchType){
	// Сначала находим наиболее активный нейрон в пуле рассогласованного (он распознает текущую ситуацию)
	int mostActiveNeuronNumber = findMostActiveSilentNeuron(neuralController->getNeuronParentPoolID(mismatchedNeuron));
	// Если мы не нашли подходящего нейрона, то выходим без обучения
	if (!mostActiveNeuronNumber) return;

	modifySynapsesStructure(mismatchedNeuron, mostActiveNeuronNumber, mismatchType);
	modifyPredConnectionsStructure(mismatchedNeuron, mostActiveNeuronNumber);

	neuralController->setNeuronActive(mostActiveNeuronNumber, true);
}

// Основной метод обучения нейроконтроллера на одном такте времени
void TAgent::learning(){
	// Массив признаков рассогласованности нейронов сети
	int* mismatchCheck = new int[neuralController->getNeuronsQuantity()];

	// Сначала определяем рассогласованность всех нейронов (чтобы избежать проверки рассогласованности вновь включающихся нейронов)
	for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
			mismatchCheck[currentNeuron - 1] = mismatchDetection(currentNeuron);

	// Проводим процедуру обучения каждого нейрона
	for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
		if (mismatchCheck[currentNeuron - 1]) // Если нейрон рассогласован
			selfLearnNeuron(currentNeuron, mismatchCheck[currentNeuron - 1]);	

	delete []mismatchCheck;
}

// Процедура случайного обучения агента - случайное включение нейронов с некоторой вероятностью (для контроля качества разработанного алгоритма обучения)
void TAgent::randomLearning(){
  const double activeProb = 0.5; // Вероятность случайного включения молчащего нейрона (на всю сеть)
  // Считаем кол-во молчащих нейронв
  if (service::uniformDistribution(0, 1) < activeProb){ // Если должен включиться нейрон
    int silentNeuronsQuantity = 0;
    for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron)
      if (!neuralController->getNeuronActive(currentNeuron))
        ++silentNeuronsQuantity;
    if (silentNeuronsQuantity){// Если вообще остались молчащие нейроны
      int activateNeuron = service::uniformDiscreteDistribution(1, silentNeuronsQuantity);
      silentNeuronsQuantity = 0;
      for (int currentNeuron = 1; currentNeuron <= neuralController->getNeuronsQuantity(); ++currentNeuron){
        if (!neuralController->getNeuronActive(currentNeuron))
          ++silentNeuronsQuantity;
        if (silentNeuronsQuantity == activateNeuron){
          neuralController->setNeuronActive(activateNeuron, true);
          break;
        }
      }
    }
  }
}