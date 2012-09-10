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
void TAgent::loadGenome(std::istream& is){
	string tmp_str;
	is >> tmp_str; // Считываем технический номер сети
	is >> tmp_str; // Считываем номер более приспособленного родителя
	parents[0] = atoi(tmp_str.c_str());
	is >> tmp_str; // Считываем номер менее приспособленного родителя
	parents[1] = atoi(tmp_str.c_str());
	if (!genome) genome = new TPoolNetwork;
	is >> *genome;
}

// Генерация случайного минимального возможного генома агента
void TAgent::generateMinimalAgent(int inputResolution, int outputResolution, int initialPoolCapacity, int initialDevelopProbability){
	int currentNeuron = 1;
	for (currentNeuron; currentNeuron <= inputResolution; ++currentNeuron)
		genome->addPool(currentNeuron, 0, 1, service::uniformDistribution(-0.5, 0.5), 0, 1);
	for (currentNeuron; currentNeuron <= inputResolution + outputResolution; ++currentNeuron)
		genome->addPool(currentNeuron, 2, 1, service::uniformDistribution(-0.5, 0.5), 0, 3);
	genome->addPool(currentNeuron + 1, 1, initialPoolCapacity, service::uniformDistribution(-0.5, 0.5), 0, 2);
	int currentConnection = 1;
	for (currentNeuron = 1; currentNeuron <= inputResolution; ++currentNeuron){
		genome->addConnection(currentNeuron, inputResolution + outputResolution + 1, currentConnection, service::uniformDistribution(-0.5, 0.5), 0, true, 0, initialDevelopProbability, currentConnection); 
		++currentConnection;
	}
	for (currentNeuron = inputResolution + 1; currentNeuron <= inputResolution + outputResolution; ++currentNeuron){
		genome->addConnection(inputResolution + outputResolution + 1, currentNeuron, currentConnection, service::uniformDistribution(-0.5, 0.5), 0, true, 0, initialDevelopProbability, currentConnection); 
		++currentConnection;
	}
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
	double* agentLife = new double[agentLifeTime];
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
	delete []outputVector;
	delete []environmentVector;
}
