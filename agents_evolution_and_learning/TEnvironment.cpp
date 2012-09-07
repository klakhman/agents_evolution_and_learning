#include "TEnvironment.h"

#include <string>
#include <fstream>
#include <cstring>

using namespace std;

// Загрузка структуры целей среды из файла
void TEnvironment::loadAims(string aimsFilename){
	// Стираем текущий вектор среды (так как мы можем загружать среду другой размерности)
	if (0 != environmentResolution) delete []currentEnvironmentVector;

	ifstream environmentFile(aimsFilename);
	string tmp_str;
	environmentFile >> tmp_str; // Считываем размерность среды
	environmentResolution = atoi(tmp_str.c_str());
	environmentFile >> tmp_str; // Считываем кол-во целей
	aimsQuantity = atoi(tmp_str.c_str());
	// Заполняем все цели
	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
		environmentFile >> tmp_str; // Считываем сложность цели
		aimsSet[currentAim - 1].aimComplexity = atoi(tmp_str.c_str());
		environmentFile >> tmp_str; // Считываем награду за достижение цели
		aimsSet[currentAim - 1].reward = atof(tmp_str.c_str());
		// Заполняем цель
		for (int currentAction = 1; currentAction <= aimsSet[currentAim - 1].aimComplexity; ++currentAction){
			environmentFile >> tmp_str; // Считываем номер изменяемого бита
			aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber = atoi(tmp_str.c_str());
			environmentFile >> tmp_str; // Считываем желаемое значение бита
			aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue = (atoi(tmp_str.c_str()) == 1);
		}
	}
	environmentFile.close();

	// Создаем новый вектор и заполняем нулями
	currentEnvironmentVector = new bool[environmentResolution];
	memset(currentEnvironmentVector, 0, environmentResolution * sizeof(bool));
}

// Выгрузка структуры целей в файл
void TEnvironment::uploadAims(string aimsFilename) const{
	ofstream environmentFile(aimsFilename);
	// Записываем размерность среды и кол-во целей
	environmentFile << environmentResolution << "\t" << aimsQuantity << endl;
	// Записываем все цели
	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
		// Записываем сложность цели и награду
		environmentFile << aimsSet[currentAim - 1].aimComplexity << "\t" << aimsSet[currentAim - 1].reward << endl;
		// Записываем цель
		for (int currentAction = 1; currentAction <= aimsSet[currentAim - 1]. aimComplexity; ++currentAction)
			environmentFile << aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber << "\t"
									<< aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue << "\t";
		environmentFile << endl;
	}
}

/* 
Изменение среды под действием агента (возвращает совершено ли действие (true) или его невозможно совершить(false))
В рамках данной архитектуры actionID кодируется как +-(bitNumber), при этом знак определяет в какую сторону изменяется бит (+ это с нуля на единицу)
*/
bool TEnvironment::forceEnvironment(double actionID){
	//Направление изменения
	bool changeDirection = (actionID > 0);
	// Номер изменяемого бита
	int changeBit = static_cast<int>(abs(actionID));
	// Если бит в противоположной позиции, то действие совершается
	if (currentEnvironmentVector[changeBit -1 ] != changeDirection){
		currentEnvironmentVector[changeBit - 1] = changeDirection;
		return true;
	}
	else return false;
	// !!!!! Тут еще должно быть случайно изменение среды !!!!!!
}

// Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
double TEnvironment::calculateReward(double actionsIDs[], int actionsQuantity) const{
	double accumulatedReward = 0.0; // Награда агента
	// Времена последнего достижения цели агентом
	int* achievingTime = new int[aimsQuantity];
	// Заполняем времена достижения целей так, чтобы вначале они все имели полную награду
	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim)
		achievingTime[currentAim - 1] = -rewardRecoveryTime - 1;
	// Проходимся по всем действиям агент
	for (int currentTimeStep = 1; currentTimeStep <= actionsQuantity; ++currentTimeStep){
		// Проверяем все цели относительно конкретного шага вермени
		for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
			if (aimsSet[currentAim-1].aimComplexity <= currentTimeStep){ // Проверяем успел ли бы вообще агент достичь эту цель в начале "жизни" (для экономии времени)
				int achivedFlag = 1; // Признак того, что цель достигнута
				int currentAction = 1; // Текущее проверяемое действие
				// Пока не найдено нарушение последовательности цели или проверка цели закончена
				while (achivedFlag && (currentAction <= aimsSet[currentAim - 1].aimComplexity)){
					// Определение направления изменения и изменяемого бита (с откатыванием времени назад)
					bool changedDirection = (actionsIDs[currentTimeStep - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction] > 0);
					int changedBit = static_cast<int>(abs(actionsIDs[currentTimeStep - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction]));
					/* Проверяем совпадает ли реальное действие с действием в цели на нужном месте 
						Ожидается, что бездействие агента будет закодировано с помощью бита 0 и поэтому не совпадет ни с одним действием в цели*/
					if ((changedBit != aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber) ||
									(changedDirection != aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue))
						achivedFlag = false;
					++currentAction;
				}
				// Если не было нарушения последовательности, то цель достигнута
				if (achivedFlag){
					// Награда линейно восстанавливается до максимального (исходного) уровня за фиксированное кол-во тактов
					accumulatedReward += aimsSet[currentAim - 1].reward * min(1.0, (currentTimeStep - achievingTime[currentAim - 1])/static_cast<double>(rewardRecoveryTime));
					achievingTime[currentAim - 1] = currentTimeStep;
				}
			} // Конец проверки одной цели
		} // Конец проверки всех целей относительно одного фронта времени
	} // Конец проверки всей "жизни"
	delete []achievingTime;
	return accumulatedReward;
}