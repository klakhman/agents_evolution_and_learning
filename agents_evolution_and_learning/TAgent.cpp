#include "TAgent.h"
#include "TEnvironment.h"

// ������������� �������������� ������������ ������� ��������
double TAgent::decodeAction(double outputVector[]){
	// ��� ������������ ������ �������� ����
	double maxOutputs[] = {-1, -1};
	// ������ ���� �������� � ������������ �������
	int maxOutputNeurons[] = {0 , 0};

	// ����������� ���� ������������ ������� ��������
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
	// ������ ������� ������, ���� ��� �� �� ������� (��� �� ���������� �� �������)
	if (maxOutputNeurons[1] > maxOutputNeurons[0]){
		int tmp = maxOutputNeurons[0];
		maxOutputNeurons[0] = maxOutputNeurons[1];
		maxOutputNeurons[1] = tmp;
	}
	// ����������� ������� ����
	int fisrtCheckNeuron = 1; // ����� ������� ������� � ����������� ����
	int secondCheckNeuron = fisrtCheckNeuron + 1; // ����� ������� ������� � ����������� ����
	int pairCount = 0; // ���������� ����������� ���
	// ������� ���-�� ��� �� ������ ���� ��������
	while ((fisrtCheckNeuron != maxOutputNeurons[0]) && (secondCheckNeuron != maxOutputNeurons[1])) {
		if (secondCheckNeuron < neuralController->getOutputResolution())
			++secondCheckNeuron;
		else{
			++fisrtCheckNeuron;
			secondCheckNeuron = fisrtCheckNeuron + 1;
		}
		++pairCount;
	}
		
	int actionNumber = pairCount % (2 * neuralController->getInputResolution()); // �������� ����� ������������ �������� (�� 0 �� 2*environmentResolution)
	// ���� ����������� ����� �������� ������, �� ��� ������� �� 0 � 1, � �������������� �������
	double actionCode = (actionNumber % 2 ? -1 : 1) * (actionNumber / 2 + 1);

	return actionCode;
}

// ������������� ����� ������
void TAgent::life(TEnvironment& environment, int agentLifeTime){
	double* environmentVector = new double[neuralController->getInputResolution()];
	double* outputVector = new double[neuralController->getOutputResolution()];
	double* agentLife = new double[agentLifeTime];
	// ����� � ������ ���� ������ ������ ��������� ������� ����� ��������� ��������� �����������
	environment.getCurrentEnvironmentVector(environmentVector);

	for (int agentLifeStep = 1; agentLifeStep <= agentLifeTime; ++agentLifeStep){
		neuralController->calculateNetwork(environmentVector);
		neuralController->getOutputVector(outputVector);
		agentLife[agentLifeStep - 1] = decodeAction(outputVector);
		// ��������� �� ����� � ��������� ������� �� ��������
		bool actionSuccess = environment.forceEnvironment(agentLife[agentLifeStep - 1]);
		if (!actionSuccess ) agentLife[agentLifeStep - 1] = 0;
	}
	reward = environment.calculateReward(agentLife, agentLifeTime);
	delete []agentLife;
	delete []outputVector;
	delete []environmentVector;
}
