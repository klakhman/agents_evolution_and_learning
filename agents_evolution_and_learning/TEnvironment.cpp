#include "TEnvironment.h"
#include "service.h"

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

// �������� ��������� ����� ����� �� �����
void TEnvironment::loadAims(string aimsFilename){
	// ������� ������� ������ ����� (��� ��� �� ����� ��������� ����� ������ �����������)
	if (environmentResolution) delete []currentEnvironmentVector;

	ifstream environmentFile(aimsFilename);
	string tmp_str;
	environmentFile >> tmp_str; // ��������� ����������� �����
	environmentResolution = atoi(tmp_str.c_str());
	environmentFile >> tmp_str; // ��������� ���-�� �����
	aimsQuantity = atoi(tmp_str.c_str());
	// ��������� ��� ����
	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
		environmentFile >> tmp_str; // ��������� ��������� ����
		aimsSet[currentAim - 1].aimComplexity = atoi(tmp_str.c_str());
		environmentFile >> tmp_str; // ��������� ������� �� ���������� ����
		aimsSet[currentAim - 1].reward = atof(tmp_str.c_str());
		// ��������� ����
		for (int currentAction = 1; currentAction <= aimsSet[currentAim - 1].aimComplexity; ++currentAction){
			environmentFile >> tmp_str; // ��������� ����� ����������� ����
			aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber = atoi(tmp_str.c_str());
			environmentFile >> tmp_str; // ��������� �������� �������� ����
			aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue = (atoi(tmp_str.c_str()) == 1);
		}
	}
	environmentFile.close();

	// ������� ����� ������ � ����� ��������� ������
	currentEnvironmentVector = new bool[environmentResolution];
	memset(currentEnvironmentVector, 0, environmentResolution * sizeof(*currentEnvironmentVector));
}

// �������� ��������� ����� � ����
void TEnvironment::uploadAims(string aimsFilename) const{
	ofstream environmentFile(aimsFilename);
	// ���������� ����������� ����� � ���-�� �����
	environmentFile << environmentResolution << "\t" << aimsQuantity << endl;
	// ���������� ��� ����
	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
		// ���������� ��������� ���� � �������
		environmentFile << aimsSet[currentAim - 1].aimComplexity << "\t" << aimsSet[currentAim - 1].reward << endl;
		// ���������� ����
		for (int currentAction = 1; currentAction <= aimsSet[currentAim - 1]. aimComplexity; ++currentAction)
			environmentFile << aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber << "\t"
									<< aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue << "\t";
		environmentFile << endl;
	}
}

// ������� ������������ ������������� ���� �����
double TEnvironment::calculateOccupancyCoefficient() const{
	if (!environmentResolution) return 0;
	// ������ ��������� ������-���� ���� � ����
	bool* bitsOccurrence = new bool[environmentResolution];
	double occupancyCoefficient = 0;

	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
		// ������� ������ ���������
		memset(bitsOccurrence, 0, environmentResolution * sizeof(*bitsOccurrence));
		// ���������� ������� ���������
		for (int currentAction = 1; currentAction <= aimsSet[currentAim - 1].aimComplexity; ++currentAction)
			bitsOccurrence[aimsSet[currentAim - 1].actionsSequence[currentAction-1].bitNumber - 1] = true;

		// ���-�� ��������� ���, �������� � ����
		int differentBitsQuantity = 0;
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			differentBitsQuantity += 1 * bitsOccurrence[currentBit - 1];
		// ����������� ���������� k ���������������� �������� (� ������ ��� ������� �� ���� � ������� � ������� - ��� ������ ��������)
		double sequenceProbability = 1.0 / pow(2.0*environmentResolution, aimsSet[currentAim - 1].aimComplexity);
		// ����� �������������, ������� ������ ��� ����
		double aimOccupancyPart = sequenceProbability * pow(2, aimsSet[currentAim - 1].aimComplexity - differentBitsQuantity);
		
		occupancyCoefficient += aimOccupancyPart;
	}

	delete []bitsOccurrence;
	return occupancyCoefficient;
}

// ��������� ��������� ��������� ����� �������� ������� ����������������
void TEnvironment::randomizeEnvironment(){
	for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit){
		// �� �������� ������ ���������, ����� ��� ������� ����������� ������� ������� �� �����������, � ��� ����������� ������ �������, ������� ������ �����������
		if (service::uniformDistribution(0, 1, false) <= nonstaionarityCoefficient)
			currentEnvironmentVector[currentBit - 1] = !currentEnvironmentVector[currentBit - 1];
	}
}

/* 
��������� ����� ��� ��������� ������ (���������� ��������� �� �������� (true) ��� ��� ���������� ���������(false))
� ������ ������ ����������� actionID ���������� ��� +-(bitNumber), ��� ���� ���� ���������� � ����� ������� ���������� ��� (+ ��� � ���� �� �������)
*/
bool TEnvironment::forceEnvironment(double actionID){
	//����������� ���������
	bool changeDirection = (actionID > 0);
	// ����� ����������� ����
	int changeBit = static_cast<int>(abs(actionID));
	// ���� ��� � ��������������� �������, �� �������� �����������
	if (currentEnvironmentVector[changeBit -1 ] != changeDirection){
		currentEnvironmentVector[changeBit - 1] = changeDirection;
		return true;
	}
	else return false;
	// !!!!! ��� ��� ������ ���� �������� ��������� ����� !!!!!!
}

// ������� ������� ������ - ��� ���� ���������� ��� ���������� ����� ������ - ���������� �������
double TEnvironment::calculateReward(double actionsIDs[], int actionsQuantity) const{
	double accumulatedReward = 0.0; // ������� ������
	// ������� ���������� ���������� ���� �������
	int* achievingTime = new int[aimsQuantity];
	// ��������� ������� ���������� ����� ���, ����� ������� ��� ��� ����� ������ �������
	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim)
		achievingTime[currentAim - 1] = -rewardRecoveryTime - 1;
	// ���������� �� ���� ��������� �����
	for (int currentTimeStep = 1; currentTimeStep <= actionsQuantity; ++currentTimeStep){
		// ��������� ��� ���� ������������ ����������� ���� �������
		for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
			if (aimsSet[currentAim-1].aimComplexity <= currentTimeStep){ // ��������� ����� �� �� ������ ����� ������� ��� ���� � ������ "�����" (��� �������� �������)
				int achivedFlag = 1; // ������� ����, ��� ���� ����������
				int currentAction = 1; // ������� ����������� ��������
				// ���� �� ������� ��������� ������������������ ���� ��� �������� ���� ���������
				while (achivedFlag && (currentAction <= aimsSet[currentAim - 1].aimComplexity)){
					// ����������� ����������� ��������� � ����������� ���� (� ������������ ������� �����)
					bool changedDirection = (actionsIDs[currentTimeStep - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction] > 0);
					int changedBit = static_cast<int>(abs(actionsIDs[currentTimeStep - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction]));
					/* ��������� ��������� �� �������� �������� � ��������� � ���� �� ������ ����� 
						���������, ��� ����������� ������ ����� ������������ � ������� ���� 0 � ������� �� �������� �� � ����� ��������� � ����*/
					if ((changedBit != aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber) ||
									(changedDirection != aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue))
						achivedFlag = false;
					++currentAction;
				}
				// ���� �� ���� ��������� ������������������, �� ���� ����������
				if (achivedFlag){
					// ������� ������� ����������������� �� ������������� (���������) ������ �� ������������� ���-�� ������
					accumulatedReward += aimsSet[currentAim - 1].reward * min(1.0, (currentTimeStep - achievingTime[currentAim - 1])/static_cast<double>(rewardRecoveryTime));
					achievingTime[currentAim - 1] = currentTimeStep;
				}
			} // ����� �������� ����� ����
		} // ����� �������� ���� ����� ������������ ������ ������ �������
	} // ����� �������� ���� "�����"
	delete []achievingTime;
	return accumulatedReward;
}

// ��������� ���� ������ ����� ��� ��������� ��������� ����� (���������� true - ���� ���� ���� �� ���� ����������� �������)
bool TEnvironment::compareDifferentLengthFullAims(TAim& firstAim, int minFirstSubAimComplexity, TAim& secondAim, int minSecondSubAimComplexity){
	// �������� ���� ������ ��������� ������ ����� �� ����������� ������� ������ ����� � ���� �����
	// ���� ����� ����������� ������� � ������ ���� ������
	if (minFirstSubAimComplexity > minSecondSubAimComplexity)
		// ���� ����� ����������� ������� ������ ���� ������, ��� ����� ����� ������ ����, �� ��� �� ����� ���� ���������
		if (minFirstSubAimComplexity > secondAim.aimComplexity) return false;
		else {
			// ���������� �� ���� ��������� ���� �� ����������� ����� ������ ����
			for (int currentAction = 1; currentAction <= minFirstSubAimComplexity; ++currentAction)
				// ���� ���� �� ���� �������� �������, �� ���� �������
					if ((firstAim.actionsSequence[currentAction-1].bitNumber != secondAim.actionsSequence[currentAction - 1].bitNumber) ||
							(firstAim.actionsSequence[currentAction-1].desiredValue != secondAim.actionsSequence[currentAction - 1].desiredValue))
						return false;
		}
	else
		// ���� ����� ����������� ������� ������ ���� ������, ��� ����� ����� ������ ����, �� ��� �� ����� ���� ���������
		if (minSecondSubAimComplexity > firstAim.aimComplexity) return false;
		else {
			// ���������� �� ���� ��������� ���� �� ����������� ����� ������ ����
			for (int currentAction = 1; currentAction <= minSecondSubAimComplexity; ++currentAction)
				// ���� ���� �� ���� �������� �������, �� ���� �������
					if ((firstAim.actionsSequence[currentAction-1].bitNumber != secondAim.actionsSequence[currentAction - 1].bitNumber) ||
							(firstAim.actionsSequence[currentAction-1].desiredValue != secondAim.actionsSequence[currentAction - 1].desiredValue))
						return false;
		}

	return true;
}

// ��������� ��������� ����� �� ���������� ������������ �������������, eps - �������� ���������, ����� ���������� ����������� ��������� ���� � ������������, � ����� ����������� ������������ ���������
double TEnvironment::generateEnvironment(int _environmentResolution, double requiredOccupancyCoef, double eps /*=0.001*/, int maxAimComplexity /*=5*/, int minAimComplexity /*=2*/, int minMaxAimComplexity /*=3*/){
	if (environmentResolution) delete []currentEnvironmentVector;
	environmentResolution = _environmentResolution;
	currentEnvironmentVector = new bool[environmentResolution];
	memset(currentEnvironmentVector, 0, environmentResolution * sizeof(*currentEnvironmentVector));

	double occupancyCoef = 0.0;
	do {
		// !!!! ���������� ������ ����� (������� ����� ����� ������� �� �������) !!!
		const int MAX_FULL_AIMS = 1000; //1000
		int fullAimsQuantity = service::uniformDiscreteDistribution(1, MAX_FULL_AIMS);
		// ������� �����, � ������� ����� ������������ ������ ����
		TEnvironment environmentWithFullAims;
		environmentWithFullAims.environmentResolution = environmentResolution;
		// ������� ��������� ������ ����� (����� ��� �������� ���� �� ����������� ������)
		environmentWithFullAims.currentEnvironmentVector = new bool[environmentResolution];
		environmentWithFullAims.aimsQuantity = fullAimsQuantity;
		// ������ �� ����������� ����������� ������� � ������ ����
		int* minSubAimComplexity = new int[environmentWithFullAims.aimsQuantity];
		// ���������� ������ ����
		for (int currentAim = 1; currentAim <= environmentWithFullAims.aimsQuantity; ++currentAim){
			// ������� ����� ������ ����
			environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity = service::uniformDiscreteDistribution(minMaxAimComplexity, maxAimComplexity);
			// ������� ����� ����������� �������
			minSubAimComplexity[currentAim - 1] = service::uniformDiscreteDistribution(minAimComplexity, environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity);
			// ���������� ����� (������������ ������) ����
			bool identicAimFound = false; // ������� ����, ��� ������� ���������� ����
			do {
				// ���������� �������� ����
				for (int currentAction = 1; currentAction <= environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity; ++currentAction){
					int currentBit = 0; // ����� ���
					bool currentDirection = false; // ����������� ���������
					bool stop = false; // ������� ����, ��� ��������������� ��� ������������ ����������
					do {
						stop = false;
						currentBit = service::uniformDiscreteDistribution(1, environmentWithFullAims.environmentResolution);
						currentDirection = (service::uniformDiscreteDistribution(0, 1) == 1);
						// ������� ��������� �������� � ����, ������� ���������� ��� �� ���, ��� � ���������������
						int currentCompareAction = 0;
						for (currentCompareAction = currentAction - 1; currentCompareAction > 0; --currentCompareAction){
							if (environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentCompareAction - 1].bitNumber == currentBit){
								if (environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentCompareAction - 1].desiredValue != currentDirection)
									stop = true;
								break; // �� ���� ������ �� ����
							}
						}
						// ���� �� ����� �� ����� � �� ��������� ������� ����
						if (currentCompareAction == 0) stop = true;
					} while (!stop); // ����� ������������� �������� ����
					environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber = currentBit;
					environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue = currentDirection;
				}
				// ����� ��������� ����� ���� ����� �������� �� � ����������� �� ���� ������������
				identicAimFound = false; // ������� ����, ��� ������� ���������� ����
				int currentCompareAim = 1;
				while ((currentCompareAim < currentAim) && (!identicAimFound)){
					//���������� ��� ����
					identicAimFound = compareDifferentLengthFullAims(environmentWithFullAims.aimsSet[currentAim - 1], minSubAimComplexity[currentAim - 1], 
																					environmentWithFullAims.aimsSet[currentCompareAim - 1], minSubAimComplexity[currentCompareAim - 1]);
					// ���� ��������� ����������, ��� ���� ���������, �� �������� ��������� ����������� ��������� ������� ���� ���� �� ������ �� ���������
					while ((identicAimFound) && (++minSubAimComplexity[currentAim - 1] <= environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity))
						identicAimFound = compareDifferentLengthFullAims(environmentWithFullAims.aimsSet[currentAim - 1], minSubAimComplexity[currentAim - 1], 
																						environmentWithFullAims.aimsSet[currentCompareAim - 1], minSubAimComplexity[currentCompareAim - 1]);
					++currentCompareAim;
				}

			} while (identicAimFound);
		}
		// ����� ��������� ���� ������ ����� ���������� ������������� �������� ����� � ���������
		aimsQuantity = 0;
		for (int currentFullAim = 1; currentFullAim <= environmentWithFullAims.aimsQuantity; ++currentFullAim)
			for (int currentSubAim = 1; currentSubAim <= environmentWithFullAims.aimsSet[currentFullAim - 1].aimComplexity - minSubAimComplexity[currentFullAim - 1] + 1; ++currentSubAim){
				aimsSet[aimsQuantity++].aimComplexity = minSubAimComplexity[currentFullAim - 1] + currentSubAim - 1;
				//!!!! ����� ����� ������ ����� ���������� �������
				aimsSet[aimsQuantity - 1].reward = 20 + 10*(aimsSet[aimsQuantity - 1].aimComplexity - 2);
				for (int currentAction = 1; currentAction <= aimsSet[aimsQuantity - 1].aimComplexity; ++currentAction){
					aimsSet[aimsQuantity - 1].actionsSequence[currentAction - 1].bitNumber = environmentWithFullAims.aimsSet[currentFullAim - 1].actionsSequence[currentAction - 1].bitNumber;
					aimsSet[aimsQuantity - 1].actionsSequence[currentAction - 1].desiredValue = environmentWithFullAims.aimsSet[currentFullAim - 1].actionsSequence[currentAction - 1].desiredValue;
				}
			}
		delete []minSubAimComplexity;

		occupancyCoef = calculateOccupancyCoefficient(); 
	} while ( (occupancyCoef < (requiredOccupancyCoef - eps)) || (occupancyCoef > (requiredOccupancyCoef + eps)) );
	
	return occupancyCoef;
}
