#include "TEnvironment.h"
#include "service.h"

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

// Загрузка структуры целей среды из файла
void TEnvironment::loadAims(string aimsFilename){
	// Стираем текущий вектор среды (так как мы можем загружать среду другой размерности)
	if (environmentResolution) delete []currentEnvironmentVector;

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

	// Создаем новый вектор и сразу заполняем нулями
	currentEnvironmentVector = new bool[environmentResolution];
	memset(currentEnvironmentVector, 0, environmentResolution * sizeof(*currentEnvironmentVector));
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

// Подсчет коэффициента заполненности всей среды
double TEnvironment::calculateOccupancyCoefficient() const{
	if (!environmentResolution) return 0;
	// Массив вхождения какого-либо бита в цель
	bool* bitsOccurrence = new bool[environmentResolution];
	double occupancyCoefficient = 0;

	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
		// Очищаем массив вхождений
		memset(bitsOccurrence, 0, environmentResolution * sizeof(*bitsOccurrence));
		// Заполнения массива вхождений
		for (int currentAction = 1; currentAction <= aimsSet[currentAim - 1].aimComplexity; ++currentAction)
			bitsOccurrence[aimsSet[currentAim - 1].actionsSequence[currentAction-1].bitNumber - 1] = true;

		// Кол-во различных бит, входящих в цель
		int differentBitsQuantity = 0;
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			differentBitsQuantity += 1 * bitsOccurrence[currentBit - 1];
		// Вероятность совершения k последовательных действий (с учетом что перевод из нуля в единицу и обратно - это разные действия)
		double sequenceProbability = 1.0 / pow(2.0*environmentResolution, aimsSet[currentAim - 1].aimComplexity);
		// Часть заполненности, которую вносит эта цель
		double aimOccupancyPart = sequenceProbability * pow(2, aimsSet[currentAim - 1].aimComplexity - differentBitsQuantity);
		
		occupancyCoefficient += aimOccupancyPart;
	}

	delete []bitsOccurrence;
	return occupancyCoefficient;
}

// Процедура случайных изменений среды согласно степени нестационарности
void TEnvironment::randomizeEnvironment(){
	for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit){
		// Не включаем начало интервала, чтобы при нулевой вероятности условие никогда не срабатывало, а при вероятности равной единице, условие всегда срабатывало
		if (service::uniformDistribution(0, 1, false) <= nonstaionarityCoefficient)
			currentEnvironmentVector[currentBit - 1] = !currentEnvironmentVector[currentBit - 1];
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

// Сравнение двух полных целей для процедуры генерации среды (возвращает true - если есть хотя бы одна совпадающая подцель)
bool TEnvironment::compareDifferentLengthFullAims(TAim& firstAim, int minFirstSubAimComplexity, TAim& secondAim, int minSecondSubAimComplexity){
	// Основная цель данной процедуры понять равны ли минимальные подцели равной длины у двух целей
	// Если длина минимальной подцели у первой цели больше
	if (minFirstSubAimComplexity > minSecondSubAimComplexity)
		// Если длина минимальной подцели первой цели больше, чем общая длина второй цели, то они не могут быть идентичны
		if (minFirstSubAimComplexity > secondAim.aimComplexity) return false;
		else {
			// Проходимся по всем действиям цели до минимальной длины первой цели
			for (int currentAction = 1; currentAction <= minFirstSubAimComplexity; ++currentAction)
				// Если хотя бы одно действие неравно, то цели неравны
					if ((firstAim.actionsSequence[currentAction-1].bitNumber != secondAim.actionsSequence[currentAction - 1].bitNumber) ||
							(firstAim.actionsSequence[currentAction-1].desiredValue != secondAim.actionsSequence[currentAction - 1].desiredValue))
						return false;
		}
	else
		// Если длина минимальной подцели второй цели больше, чем общая длина первой цели, то они не могут быть идентичны
		if (minSecondSubAimComplexity > firstAim.aimComplexity) return false;
		else {
			// Проходимся по всем действиям цели до минимальной длины второй цели
			for (int currentAction = 1; currentAction <= minSecondSubAimComplexity; ++currentAction)
				// Если хотя бы одно действие неравно, то цели неравны
					if ((firstAim.actionsSequence[currentAction-1].bitNumber != secondAim.actionsSequence[currentAction - 1].bitNumber) ||
							(firstAim.actionsSequence[currentAction-1].desiredValue != secondAim.actionsSequence[currentAction - 1].desiredValue))
						return false;
		}

	return true;
}

// Процедура генерации среды по требуемому коэффициенту заполненности, eps - точность генерации, также передается минимальная сложность цели и максимальная, а также минимальная максимальная сложность
double TEnvironment::generateEnvironment(int _environmentResolution, double requiredOccupancyCoef, double eps /*=0.001*/, int maxAimComplexity /*=5*/, int minAimComplexity /*=2*/, int minMaxAimComplexity /*=3*/){
	if (environmentResolution) delete []currentEnvironmentVector;
	environmentResolution = _environmentResolution;
	currentEnvironmentVector = new bool[environmentResolution];
	memset(currentEnvironmentVector, 0, environmentResolution * sizeof(*currentEnvironmentVector));

	double occupancyCoef = 0.0;
	do {
		// !!!! Количество полных целей (которые потом будут разбиты на подцели) !!!
		const int MAX_FULL_AIMS = 1000; //1000
		int fullAimsQuantity = service::uniformDiscreteDistribution(1, MAX_FULL_AIMS);
		// Создаем среду, в которую будут записываться полные цели
		TEnvironment environmentWithFullAims;
		environmentWithFullAims.environmentResolution = environmentResolution;
		// Создаем фиктивный вектор среды (чтобы при удалении типа не происходило ошибки)
		environmentWithFullAims.currentEnvironmentVector = new bool[environmentResolution];
		environmentWithFullAims.aimsQuantity = fullAimsQuantity;
		// Массив со сложностями минимальной подцели в полной цели
		int* minSubAimComplexity = new int[environmentWithFullAims.aimsQuantity];
		// Генерируем полные цели
		for (int currentAim = 1; currentAim <= environmentWithFullAims.aimsQuantity; ++currentAim){
			// Находим длину полной цели
			environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity = service::uniformDiscreteDistribution(minMaxAimComplexity, maxAimComplexity);
			// Находим длину минимальной подцели
			minSubAimComplexity[currentAim - 1] = service::uniformDiscreteDistribution(minAimComplexity, environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity);
			// Генерируем новую (неидентичную старым) цель
			bool identicAimFound = false; // Признак того, что найдена идентичная цель
			do {
				// Генерируем действия цели
				for (int currentAction = 1; currentAction <= environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity; ++currentAction){
					int currentBit = 0; // Новый бит
					bool currentDirection = false; // Направление изменения
					bool stop = false; // Признак того, что сгенерированный бит противоречит предыдущим
					do {
						stop = false;
						currentBit = service::uniformDiscreteDistribution(1, environmentWithFullAims.environmentResolution);
						currentDirection = (service::uniformDiscreteDistribution(0, 1) == 1);
						// Находим последнее действие в цели, которое использует тот же бит, что и сгенерированное
						int currentCompareAction = 0;
						for (currentCompareAction = currentAction - 1; currentCompareAction > 0; --currentCompareAction){
							if (environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentCompareAction - 1].bitNumber == currentBit){
								if (environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentCompareAction - 1].desiredValue != currentDirection)
									stop = true;
								break; // Не идем дальше по цели
							}
						}
						// Если мы дошли до конца и не встретили нужного бита
						if (currentCompareAction == 0) stop = true;
					} while (!stop); // Конец генерирования действий цели
					environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber = currentBit;
					environmentWithFullAims.aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue = currentDirection;
				}
				// После генерации новой цели нужно сравнить ее с предыдущими на факт идентичности
				identicAimFound = false; // Признак того, что найдена идентичная цель
				int currentCompareAim = 1;
				while ((currentCompareAim < currentAim) && (!identicAimFound)){
					//Сравниваем две цели
					identicAimFound = compareDifferentLengthFullAims(environmentWithFullAims.aimsSet[currentAim - 1], minSubAimComplexity[currentAim - 1], 
																					environmentWithFullAims.aimsSet[currentCompareAim - 1], minSubAimComplexity[currentCompareAim - 1]);
					// Если сравнение показывает, что цели идентичны, то пытаемся увеличить минимальную сложность подцели пока цели не станут не идентичны
					while ((identicAimFound) && (++minSubAimComplexity[currentAim - 1] <= environmentWithFullAims.aimsSet[currentAim - 1].aimComplexity))
						identicAimFound = compareDifferentLengthFullAims(environmentWithFullAims.aimsSet[currentAim - 1], minSubAimComplexity[currentAim - 1], 
																						environmentWithFullAims.aimsSet[currentCompareAim - 1], minSubAimComplexity[currentCompareAim - 1]);
					++currentCompareAim;
				}

			} while (identicAimFound);
		}
		// После генерации всех полных целей необходимо сгенерировать итоговую среду с подцелями
		aimsQuantity = 0;
		for (int currentFullAim = 1; currentFullAim <= environmentWithFullAims.aimsQuantity; ++currentFullAim)
			for (int currentSubAim = 1; currentSubAim <= environmentWithFullAims.aimsSet[currentFullAim - 1].aimComplexity - minSubAimComplexity[currentFullAim - 1] + 1; ++currentSubAim){
				aimsSet[aimsQuantity++].aimComplexity = minSubAimComplexity[currentFullAim - 1] + currentSubAim - 1;
				//!!!! Здесь можно менять схему начисления награды
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
