#include "TEnvironment.h"

#include <string>
#include <fstream>
#include <cstring>

using namespace std;

// �������� ��������� ����� ����� �� �����
void TEnvironment::loadAims(string aimsFilename){
	// ������� ������� ������ ����� (��� ��� �� ����� ��������� ����� ������ �����������)
	if (0 != environmentResolution) delete []currentEnvironmentVector;

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

	// ������� ����� ������ � ��������� ������
	currentEnvironmentVector = new bool[environmentResolution];
	memset(currentEnvironmentVector, 0, environmentResolution * sizeof(bool));
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