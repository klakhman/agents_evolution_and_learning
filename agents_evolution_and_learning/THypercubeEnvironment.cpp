#include "THypercubeEnvironment.h"
#include "service.h"

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cmath>
#include <sstream>


using namespace std;

// Печать цели на экран
void THypercubeEnvironment::TAim::print(std::ostream& os) const{
  for (int currentAction = 1; currentAction < aimComplexity; ++currentAction)
    os << actionsSequence[currentAction - 1].bitNumber << "->" << actionsSequence[currentAction - 1].desiredValue << " ; ";
  os << endl;
}


// Загрузка структуры целей среды из файла
void THypercubeEnvironment::loadEnvironment(std::string environmentFilename){
	// Стираем текущий вектор среды (так как мы можем загружать среду другой размерности)
	if (environmentResolution) delete []currentEnvironmentVector;
	ifstream environmentFile;
	environmentFile.open(environmentFilename.c_str());
	string tmp_str;
	environmentFile >> tmp_str; // Считываем размерность среды
	environmentResolution = atoi(tmp_str.c_str());
	environmentFile >> tmp_str; // Считываем кол-во целей
	aimsQuantity = atoi(tmp_str.c_str());
  aimsSet.resize(aimsQuantity);
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
void THypercubeEnvironment::uploadEnvironment(std::string environmentFilename) const{
	ofstream environmentFile;
	environmentFile.open(environmentFilename.c_str());
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
double THypercubeEnvironment::calculateOccupancyCoefficient() const{
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
		double aimOccupancyPart = sequenceProbability * pow(2.0, aimsSet[currentAim - 1].aimComplexity - differentBitsQuantity);
		
		occupancyCoefficient += aimOccupancyPart;
	}

	delete []bitsOccurrence;
	return occupancyCoefficient;
}

// Процедура случайных изменений среды согласно степени нестационарности
void THypercubeEnvironment::randomizeEnvironment(){
	for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit){
		// Не включаем начало интервала, чтобы при нулевой вероятности условие никогда не срабатывало, а при вероятности равной единице, условие всегда срабатывало
		if (service::uniformDistribution(0, 1) < stochasticityCoefficient)
			currentEnvironmentVector[currentBit - 1] = !currentEnvironmentVector[currentBit - 1];
	}
}

/* 
Изменение среды под действием агента (возвращает совершено ли действие (true) или его невозможно совершить(false))
В рамках данной архитектуры actionID кодируется как +-(bitNumber), при этом знак определяет в какую сторону изменяется бит (+ это с нуля на единицу)
*/
int THypercubeEnvironment::forceEnvironment(const std::vector<double>& action){
	// Признак успешности совершаемого действия
	bool actionSuccess = false;
	//Направление изменения
	bool changeDirection = (action[0] > 0);
	// Номер изменяемого бита
	int changeBit = static_cast<int>(fabs(action[0]) + 0.1); // Добавляем 0.1, чтобы избежать неверного трактования double
  // Если был передан ноль, то агент бездействует
  if (changeBit == 0) return true;
	// Если бит в противоположной позиции, то действие совершается
	if (currentEnvironmentVector[changeBit -1] != changeDirection){
		currentEnvironmentVector[changeBit - 1] = changeDirection;
		actionSuccess = true;
	}
	else actionSuccess = false;
	randomizeEnvironment();

	return actionSuccess;
}

// Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
double THypercubeEnvironment::calculateReward(const vector< vector<double> >& actions, int actionsQuantity) const{
  /*double* actionsIDs = new double[actionsQuantity];
  for (int currentAction = 0; currentAction < actionsQuantity; ++currentAction)
    actionsIDs[currentAction] = actions[currentAction][0];*/
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
					bool changedDirection = (actions[currentTimeStep - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction][0] > 0);
					int changedBit = static_cast<int>(fabs(actions[currentTimeStep - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction][0]));
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
					accumulatedReward += aimsSet[currentAim - 1].reward * min(1.0, (currentTimeStep - achievingTime[currentAim - 1])/static_cast<double>(max(1, rewardRecoveryTime)));
					achievingTime[currentAim - 1] = currentTimeStep;
				}
			} // Конец проверки одной цели
		} // Конец проверки всех целей относительно одного фронта времени
	} // Конец проверки всей "жизни"
  //delete []actionsIDs;
	delete []achievingTime;
	return accumulatedReward;
}

// Определения какие цели были достигнуты на фронте текущей последовательности действий (на текущем шаге времени)
// Возвращает массив достигнутых целей
vector<int> THypercubeEnvironment::testReachingAims(const vector< vector<double> >& actions, int actionsQuantity) const{
  vector<int> reachedAims;
  // Проверяем все цели относительно конкретного шага вермени
	for (int currentAim = 1; currentAim <= aimsQuantity; ++currentAim){
	  if (aimsSet[currentAim-1].aimComplexity <= actionsQuantity){ // Проверяем успел ли бы вообще агент достичь эту цель в начале "жизни" (для экономии времени)
		int achivedFlag = 1; // Признак того, что цель достигнута
		int currentAction = 1; // Текущее проверяемое действие
		// Пока не найдено нарушение последовательности цели или проверка цели закончена
		while (achivedFlag && (currentAction <= aimsSet[currentAim - 1].aimComplexity)){
			// Определение направления изменения и изменяемого бита (с откатыванием времени назад)
			bool changedDirection = (actions[actionsQuantity - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction][0] > 0);
			int changedBit = static_cast<int>(fabs(actions[actionsQuantity - 1 - aimsSet[currentAim - 1].aimComplexity + currentAction][0]));
			/* Проверяем совпадает ли реальное действие с действием в цели на нужном месте 
			Ожидается, что бездействие агента будет закодировано с помощью бита 0 и поэтому не совпадет ни с одним действием в цели*/
			if ((changedBit != aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber) ||
			      (changedDirection != aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue))
			  achivedFlag = false;
				++currentAction;
			}
		  // Если не было нарушения последовательности, то цель достигнута
		  if (achivedFlag)
        reachedAims.push_back(currentAim);
    } // Конец проверки одной цели
  } // Конец проверки всех целей относительно одного фронта времени
  return reachedAims;
}

// Сравнение двух полных целей для процедуры генерации среды (возвращает true - если есть хотя бы одна совпадающая подцель)
bool THypercubeEnvironment::compareDifferentLengthFullAims(TAim& firstAim, int minFirstSubAimComplexity, TAim& secondAim, int minSecondSubAimComplexity){
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

// Генерация непротиворечивой цели
THypercubeEnvironment::TAim THypercubeEnvironment::generateSelfConsistentAim(int environmentResolution, int aimComplexity){
  TAim desiredAim;
  // Генерируем действия цели
  for (int currentAction = 1; currentAction <= aimComplexity; ++currentAction){
    int currentBit = 0; // Новый бит
		bool currentDirection = false; // Направление изменения
		bool stop = false; // Признак того, что сгенерированный бит противоречит предыдущим
		do {
		  stop = false;
			currentBit = service::uniformDiscreteDistribution(1, environmentResolution);
			currentDirection = (service::uniformDiscreteDistribution(0, 1) == 1);
			// Находим последнее действие в цели, которое использует тот же бит, что и сгенерированное
			int currentCompareAction = 0;
			for (currentCompareAction = currentAction - 1; currentCompareAction > 0; --currentCompareAction){
        if (desiredAim.actionsSequence[currentCompareAction - 1].bitNumber == currentBit){
          if (desiredAim.actionsSequence[currentCompareAction - 1].desiredValue != currentDirection)
					  stop = true;
					break; // Не идем дальше по цели
				}
			}
			// Если мы дошли до конца и не встретили нужного бита
			if (currentCompareAction == 0) stop = true;
		} while (!stop); // Конец генерирования действий цели
		desiredAim.actionsSequence[currentAction - 1].bitNumber = currentBit;
		desiredAim.actionsSequence[currentAction - 1].desiredValue = currentDirection;
	}
  desiredAim.aimComplexity = aimComplexity;
  desiredAim.reward = 20 + 10*(desiredAim.aimComplexity - 2);
  return desiredAim;
}

// Процедура генерации среды по требуемому коэффициенту заполненности, eps - точность генерации, также передается минимальная сложность цели и максимальная, а также минимальная максимальная сложность
double THypercubeEnvironment::generateEnvironment(int _environmentResolution, double requiredOccupancyCoef, int maxAimComplexity /*=5*/, int minAimComplexity /*=2*/, int minMaxAimComplexity /*=3*/, double eps /*=0.001*/){
	if (environmentResolution) delete []currentEnvironmentVector;
	environmentResolution = _environmentResolution;
	currentEnvironmentVector = new bool[environmentResolution];
	memset(currentEnvironmentVector, 0, environmentResolution * sizeof(*currentEnvironmentVector));
  // !!!! Количество полных целей (которые потом будут разбиты на подцели) !!!
	const int MAX_FULL_AIMS = 2000; //1000
  // Чтобы избежать фрагментации мы инициализируем максимальным числом все сначала
  // Создаем среду, в которую будут записываться полные цели
  THypercubeEnvironment environmentWithFullAims;
  environmentWithFullAims.setAimsQuantity(MAX_FULL_AIMS);
  environmentWithFullAims.environmentResolution = environmentResolution;
  // Массив со сложностями минимальной подцели в полной цели
  int* minSubAimComplexity = new int[MAX_FULL_AIMS];
	double occupancyCoef = 0.0;
  int totalAimsQuantity = 0;
	do {
		int fullAimsQuantity = service::uniformDiscreteDistribution(1, MAX_FULL_AIMS);
		// Генерируем полные цели
		for (int currentAim = 1; currentAim <= fullAimsQuantity; ++currentAim){
			// Находим длину полной цели
			int currentAimMaxComplexity = service::uniformDiscreteDistribution(minMaxAimComplexity, maxAimComplexity);
			// Находим длину минимальной подцели
			minSubAimComplexity[currentAim - 1] = service::uniformDiscreteDistribution(minAimComplexity, currentAimMaxComplexity);
			// Генерируем новую (неидентичную старым) цель
			bool identicAimFound = false; // Признак того, что найдена идентичная цель
      TAim aim;
			do {
				// Генерируем непротиворечивую цель
        aim = generateSelfConsistentAim(environmentResolution, currentAimMaxComplexity);
        //cout << currentAim << " : " << fullAimsQuantity << endl;
				// После генерации новой цели нужно сравнить ее с предыдущими на факт идентичности
				identicAimFound = false; // Признак того, что найдена идентичная цель
				int currentCompareAim = 1;
				while ((currentCompareAim < currentAim) && (!identicAimFound)){
					//Сравниваем две цели
					identicAimFound = compareDifferentLengthFullAims(aim, minSubAimComplexity[currentAim - 1], 
            environmentWithFullAims.aimsSet[currentCompareAim - 1], minSubAimComplexity[currentCompareAim - 1]);
					// Если сравнение показывает, что цели идентичны, то пытаемся увеличить минимальную сложность подцели пока цели не станут не идентичны
					while ((identicAimFound) && (minSubAimComplexity[currentAim - 1] < aim.aimComplexity)){
            ++minSubAimComplexity[currentAim - 1];
						identicAimFound = compareDifferentLengthFullAims(aim, minSubAimComplexity[currentAim - 1], 
                              environmentWithFullAims.aimsSet[currentCompareAim - 1], minSubAimComplexity[currentCompareAim - 1]);
          }
					++currentCompareAim;
				}
			} while (identicAimFound);
      environmentWithFullAims.aimsSet[currentAim - 1] = aim;
		}
		// После генерации всех полных целей необходимо сгенерировать итоговую среду с подцелями
    // Определяем общее кол-во целей в среде после генерации подцелей
    totalAimsQuantity = 0;
    for (int currentFullAim = 0; currentFullAim < fullAimsQuantity; ++currentFullAim)
      totalAimsQuantity += environmentWithFullAims.aimsSet[currentFullAim].aimComplexity - minSubAimComplexity[currentFullAim] + 1;
    int _aimsQuantity = 0;
    setAimsQuantity(totalAimsQuantity);
		for (int currentFullAim = 1; currentFullAim <= fullAimsQuantity; ++currentFullAim)
			for (int currentSubAim = 1; currentSubAim <= environmentWithFullAims.aimsSet[currentFullAim - 1].aimComplexity - minSubAimComplexity[currentFullAim - 1] + 1; ++currentSubAim){
				aimsSet[_aimsQuantity++].aimComplexity = minSubAimComplexity[currentFullAim - 1] + currentSubAim - 1;
				//!!!! Здесь можно менять схему начисления награды
				aimsSet[_aimsQuantity - 1].reward = 20 + 10*(aimsSet[_aimsQuantity - 1].aimComplexity - 2);
				for (int currentAction = 1; currentAction <= aimsSet[_aimsQuantity - 1].aimComplexity; ++currentAction){
					aimsSet[_aimsQuantity - 1].actionsSequence[currentAction - 1].bitNumber = environmentWithFullAims.aimsSet[currentFullAim - 1].actionsSequence[currentAction - 1].bitNumber;
					aimsSet[_aimsQuantity - 1].actionsSequence[currentAction - 1].desiredValue = environmentWithFullAims.aimsSet[currentFullAim - 1].actionsSequence[currentAction - 1].desiredValue;
				}
			}
		occupancyCoef = calculateOccupancyCoefficient();    
	} while ( (occupancyCoef < (requiredOccupancyCoef - eps)) || (occupancyCoef > (requiredOccupancyCoef + eps)) );

  delete []minSubAimComplexity;
	return occupancyCoef;
}

// Генерация пула сред
// Входные параметры:
// environmentDirectory - директория, в которую записываеются сгенерированные среды; environmentResolution - размерность среды
// firstEnvironmentNumber - номер (в названии) первой среды
// firstGoalDensity - начальная плотность целей; lastGoalDensity - последняя плотность целей
// goalDensityStep - шаг изменения плотности целей (если между первым и последним значения плотности целей помещается целое число шагов, то генерируются цели и для последней плотности)
// sameGDenvQuantity - количество сред в пуле с одинаковыми значения плотности целей
void THypercubeEnvironment::generateEnvironmentsPool(string environmentDirectory, int environmentResolution, int firstEnvironmentNumber, double firstGoalDensity, double lastGoalDensity, 
                                                     double goalDensityStep, int sameGDenvQuantity, int maxAimComplexity/*=5*/, int minAimComplexity/*=2*/, int minMaxAimComplexity/*=3*/){
  stringstream environmentFilename;
  int currentEnvironmentNumber = firstEnvironmentNumber;
  double eps = 0.0001; // Техническая точность (для генерации правильного количества сред)
  // Файл с результатами генерации сред
  ofstream summaryFile;
  environmentFilename << environmentDirectory << "/EnvironmentSummary_" << firstEnvironmentNumber << "-" 
    << firstEnvironmentNumber - 1 + static_cast<int>((lastGoalDensity + goalDensityStep  - firstGoalDensity)/goalDensityStep + eps) * sameGDenvQuantity << ".txt";
  summaryFile.open(environmentFilename.str().c_str());
  for (double currentGoalDensity = firstGoalDensity; currentGoalDensity <= lastGoalDensity + eps; currentGoalDensity += goalDensityStep)
    // Генерируем нужное количество сред одинаковой заполненности
    for (int currentEnvMember = 0; currentEnvMember < sameGDenvQuantity; ++currentEnvMember){
      THypercubeEnvironment currentEnvironment;
      double realGD = currentEnvironment.generateEnvironment(environmentResolution, currentGoalDensity, maxAimComplexity, minAimComplexity, minMaxAimComplexity);
      environmentFilename.str("");
      environmentFilename << environmentDirectory << "/Environment" << currentEnvironmentNumber << ".txt";
      currentEnvironment.uploadEnvironment(environmentFilename.str());
      summaryFile << "Environment" << currentEnvironmentNumber << "\t" << realGD << "\t" << currentEnvironment.getAimsQuantity() << endl;
      ++currentEnvironmentNumber;
    }
}
