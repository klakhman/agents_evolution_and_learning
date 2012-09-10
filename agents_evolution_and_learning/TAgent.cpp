#include "TAgent.h"
#include "TEnvironment.h"

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
