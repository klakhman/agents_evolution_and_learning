#include "TAgent.h"
#include "TEnvironment.h"
#include "TNeuralNetwork.h"
#include "TPoolNetwork.h"
#include "service.h"

#include <iostream>

using namespace std;

// Загрузка нейроконтроллера агента
void TAgent::loadController(istream& is){
	string tmp_str;
	is >> tmp_str; // Считываем технический номер сети
	if (!neuralController) neuralController = new TNeuralNetwork;
	is >> *neuralController;
}
// Загрузка генома нейрононтроллера
void TAgent::loadGenome(istream& is){
	string tmp_str;
	is >> tmp_str; // Считываем технический номер сети
	is >> tmp_str; // Считываем номер более приспособленного родителя
	parents[0] = atoi(tmp_str.c_str());
	is >> tmp_str; // Считываем номер менее приспособленного родителя
	parents[1] = atoi(tmp_str.c_str());
	if (!genome) genome = new TPoolNetwork;
	is >> *genome;
}

// Выгрузка нейроконтроллера агета в файл или на экран
void TAgent::uploadController(ostream& os) const{
	if (neuralController) os << *neuralController;
}

// Выгрузка генома агента в файл или на экран
void TAgent::uploadGenome(ostream& os) const{
	if (genome){
		os << parents[0] << "\t" << parents[1] << endl;
		os << *genome;
	}
}

// Подсчет размера выходного вектора нейроконтроллера и генома агента на основе размера входного вектора среды
int TAgent::calculateOutputResoltion(int inputResolution) const{
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
	int currentNeuron = 1;
	int outputResolution = calculateOutputResoltion(inputResolution);
	for (currentNeuron; currentNeuron <= inputResolution; ++currentNeuron)
		genome->addPool(currentNeuron, 0, 1, service::uniformDistribution(-0.5, 0.5), 0, 1);
	for (currentNeuron; currentNeuron <= inputResolution + outputResolution; ++currentNeuron)
		genome->addPool(currentNeuron, 2, 1, service::uniformDistribution(-0.5, 0.5), 0, 3);
	genome->addPool(currentNeuron + 1, 1, primarySystemogenesisSettings.initialPoolCapacity, service::uniformDistribution(-0.5, 0.5), 0, 2);
	int currentConnection = 1;
	for (currentNeuron = 1; currentNeuron <= inputResolution; ++currentNeuron){
		genome->addConnection(currentNeuron, inputResolution + outputResolution + 1, currentConnection, service::uniformDistribution(-0.5, 0.5), 0, true, 0, primarySystemogenesisSettings.initialDevelopSynapseProbability, currentConnection); 
		++currentConnection;
	}
	for (currentNeuron = inputResolution + 1; currentNeuron <= inputResolution + outputResolution; ++currentNeuron){
		genome->addConnection(inputResolution + outputResolution + 1, currentNeuron, currentConnection, service::uniformDistribution(-0.5, 0.5), 0, true, 0, primarySystemogenesisSettings.initialDevelopSynapseProbability, currentConnection); 
		++currentConnection;
	}
}

// Линейная процедра первичного системогеназа (когда происходит однозначная трансляция генотипа) - используется, когда нет ни настоящего системогенеза, ни обучения
void TAgent::linearSystemogenesis(){
	if (neuralController) neuralController->eraseNeuralNetwork();
	// Сначала создаем все нейроны в сети
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		neuralController->addNeuron(neuralController->getNeuronsQuantity() + 1, genome->getPoolType(currentPool), genome->getPoolBiasMean(currentPool), genome->getPoolLayer(currentPool), true, currentPool);
	// Потом создаем синапсы
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		for (int currentConnection = 1; currentConnection <= genome->getPoolInputConnectionsQuantity(currentPool); ++currentConnection)
			neuralController->addSynapse(genome->getConnectionPrePool(currentPool, currentConnection)->getID(), currentPool, neuralController->getSynapsesQuantity() + 1, genome->getConnectionWeightMean(currentPool, currentConnection), genome->getConnectionEnabled(currentPool, currentConnection));
	// И создаем предикторные связи
	for (int currentPool = 1; currentPool <= genome->getPoolsQuantity(); ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= genome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPredConnection)
			neuralController->addPredConnection(genome->getPredConnectionPrePool(currentPool, currentPredConnection)->getID(), currentPool, neuralController->getPredConnectionsQuantity() + 1, genome->getPredConnectionEnabled(currentPool, currentPredConnection));
}

// Декодирование идентификатора совершаемого агентом действия
double TAgent::decodeAction(double outputVector[]){
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
	if (maxOutputNeurons[1] > maxOutputNeurons[0]){
		int tmp = maxOutputNeurons[0];
		maxOutputNeurons[0] = maxOutputNeurons[1];
		maxOutputNeurons[1] = tmp;
	}
	// Расшифровка выходов сети
	int fisrtCheckNeuron = 1; // Номер первого нейрона в проверяемой паре
	int secondCheckNeuron = fisrtCheckNeuron + 1; // Номер второго нейрона в проверяемой паре
	int pairCount = 0; // Количество проверенных пар
	// Считаем кол-во пар до нужной пары нейронов
	while ((fisrtCheckNeuron != maxOutputNeurons[0]) && (secondCheckNeuron != maxOutputNeurons[1])) {
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

	return actionCode;
}

// Моделирование жизни агента
void TAgent::life(TEnvironment& environment, int agentLifeTime){
	double* environmentVector = new double[neuralController->getInputResolution()];
	double* outputVector = new double[neuralController->getOutputResolution()];
	agentLife = new double[agentLifeTime];
	// Здесь в теории надо вместо просто получения вектора среды поставить процедуру кодировщика
	environment.getCurrentEnvironmentVector(environmentVector);

	for (int agentLifeStep = 1; agentLifeStep <= agentLifeTime; ++agentLifeStep){
		neuralController->calculateNetwork(environmentVector);
		neuralController->getOutputVector(outputVector);
		agentLife[agentLifeStep - 1] = decodeAction(outputVector);
		// Действуем на среду и проверяем успешно ли действие
		bool actionSuccess = environment.forceEnvironment(agentLife[agentLifeStep - 1]);
		if (!actionSuccess ) agentLife[agentLifeStep - 1] = 0;
	}
	reward = environment.calculateReward(agentLife, agentLifeTime);
	delete []agentLife;
	agentLife = 0;
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
	primarySystemogenesisSettings.initialPoolCapacity = sourceAgent.primarySystemogenesisSettings.initialPoolCapacity;
	primarySystemogenesisSettings.initialDevelopSynapseProbability = sourceAgent.primarySystemogenesisSettings.initialDevelopSynapseProbability;
	primarySystemogenesisSettings.initialDevelopPredConnectionProbability = sourceAgent.primarySystemogenesisSettings.initialDevelopPredConnectionProbability;
	primarySystemogenesisSettings.primarySystemogenesisTime = sourceAgent.primarySystemogenesisSettings.primarySystemogenesisTime;
	primarySystemogenesisSettings.spontaneousActivityProb = sourceAgent.primarySystemogenesisSettings.spontaneousActivityProb;
	primarySystemogenesisSettings.activeNeuronsPercent = sourceAgent.primarySystemogenesisSettings.activeNeuronsPercent;
	primarySystemogenesisSettings.synapsesActivityTreshold = sourceAgent.primarySystemogenesisSettings.synapsesActivityTreshold; 
	primarySystemogenesisSettings.significanceTreshold = sourceAgent.primarySystemogenesisSettings.significanceTreshold; 
	return *this;
}
