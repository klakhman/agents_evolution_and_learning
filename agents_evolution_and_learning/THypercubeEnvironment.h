﻿#ifndef THYPERCUBE_ENVIRONMENT_H
#define THYPERCUBE_ENVIRONMENT_H 

#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include "service.h"
#include "TEnvironment.h"
/*
Класс окружающей среды - ГИПЕРКУБА
*/
class THypercubeEnvironment: public TEnvironment{
public:
	//Класс цели в среде (вложенный)
	class TAim{	
	public:
		// Максимальное кол-во действий в цели
		static const unsigned int MAX_AIM_COMPLEXITY = 10;
		// Структура одного необходимого действия в цели
		struct SAction{
			int bitNumber; // Номер бита в векторе состояния среды
			bool desiredValue; // Необходимое значение
		} actionsSequence[MAX_AIM_COMPLEXITY];

		int aimComplexity; // Сложность цели - кол-во действий
		double reward; // Полная награда за достижение целей

		//Конструктор по умолчанию
		TAim(){
			aimComplexity = 0;
			reward = 0;
		}
    // Печать цели на экран
    void print(std::ostream& os) const;

    TAim(const TAim& sourceAim){
      *this = sourceAim; 
    }

    TAim& operator=(const TAim& sourceAim){
      aimComplexity = sourceAim.aimComplexity;
      reward = sourceAim.reward;
      for (int currentAction = 0; currentAction < aimComplexity; ++currentAction)
        actionsSequence[currentAction] = sourceAim.actionsSequence[currentAction];
      return *this;
    }

    void specifyAim(const std::vector<int>& _actionsSequence){
      aimComplexity = _actionsSequence.size();
      for (unsigned int action = 0; action < _actionsSequence.size(); ++action){
        actionsSequence[action].bitNumber = abs(_actionsSequence.at(action));
        actionsSequence[action].desiredValue = (_actionsSequence.at(action) > 0);
      }
    }

    friend std::ostream& operator<<(std::ostream& os, const TAim& aim);
	};
protected:
	// Массив целей в среде (включая все подцели)
	std::vector<TAim> aimsSet;
	// Кол-во целей в среде
	int aimsQuantity;
	// Время восстановления награды за цель после ее достижения
	int rewardRecoveryTime;
	// Степень стохастичности среды (передается в виде вероятности изменения одного бита среды)
	double stochasticityCoefficient;
	// Текущий бинарный вектор среды
	bool* currentEnvironmentVector;
	// Размерность вектора среды
	int environmentResolution;

	// Процедура случайных изменений среды согласно степени нестационарности
	void randomizeEnvironment();
	// Сравнение двух полных целей для процедуры генерации среды (возвращает true - если есть хотя бы одна совпадающая подцель)
	bool compareDifferentLengthFullAims(TAim& firstAim, int minFirstSubAimComplexity, TAim& secondAim, int minSecondSubAimComplexity);
  // Генерация непротиворечивой цели
 static TAim generateSelfConsistentAim(int environmentResolution, int aimComplexity);

  void setAimsQuantity(int _aimsQuantity){ 
    aimsSet.resize(_aimsQuantity);
    aimsQuantity = _aimsQuantity;
  }

public:
	// Конструктор по умолчанию
	THypercubeEnvironment(){
		aimsQuantity = 0;
		environmentResolution = 0;
		rewardRecoveryTime = 0;
		stochasticityCoefficient = 0;
    currentEnvironmentVector = 0;
	}
	// Конструктор сразу с загрузкой целей и возможной установкой параметров среды
	THypercubeEnvironment(std::string aimsFilename, int _rewardRecoveryTime = 0, double _nonstaionarityCoefficient = 0){
		aimsQuantity = 0;
		environmentResolution = 0;
		rewardRecoveryTime = _rewardRecoveryTime;
		stochasticityCoefficient = _nonstaionarityCoefficient;
		loadEnvironment(aimsFilename);
	}
	// Деструктор
	~THypercubeEnvironment(){
		if (currentEnvironmentVector) delete []currentEnvironmentVector;
	}

	// Геттеры и сеттеры параметров среды
	int getAimsQuantity() const { return aimsQuantity; }
	int getEnvironmentResolution() const {return environmentResolution; }
	int getRewardRecoveryTime() const {return rewardRecoveryTime; }
	void setRewardRecoveryTime(int _rewardRecoveryTime) {rewardRecoveryTime = _rewardRecoveryTime; }
	double getStochasticityCoefficient() const { return stochasticityCoefficient; }
	void setStochasticityCoefficient(double _stochasticityCoefficient) { stochasticityCoefficient = _stochasticityCoefficient; }
	void getCurrentEnvironmentVector(double environmentVector[]) const {
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			environmentVector[currentBit - 1] = static_cast<double>(currentEnvironmentVector[currentBit - 1]);
	}
  virtual void setEnvResolution(unsigned int _envResolution){
    if (currentEnvironmentVector)
      delete []currentEnvironmentVector;
    environmentResolution = _envResolution;
    currentEnvironmentVector = new bool[_envResolution];
    memset(currentEnvironmentVector, 0, environmentResolution * sizeof(*currentEnvironmentVector));
  }

  // Получение константной ссылки на цель в среде (номер цели начинается с единица)
  const TAim& getAimReference(int aimNumber) const { return aimsSet[aimNumber-1]; }

	// Получение кол-ва начальных состояний среды (для текущей бинарной среды - фактически общее кол-во состояний)
	int getInitialStatesQuantity() const {
		return static_cast<int>(pow(2.0, environmentResolution));
	}
	// Задание вектора среды (по значению целого вектора)
	virtual void setEnvironmentVector(double environmentVector[]) {
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = (static_cast<int>(environmentVector[currentBit - 1] + 0.1) == 1);
	}
	// Задание вектора среды (по номеру - !!!начиная с нуля!!!!) 
	virtual void setEnvironmentState(double stateNumber){
		bool* requiredEnvironmentVector = new bool[environmentResolution];
		service::decToBin(static_cast<int>(stateNumber + 0.1), requiredEnvironmentVector, environmentResolution);
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = requiredEnvironmentVector[currentBit - 1];
		delete []requiredEnvironmentVector;
	}

	int getEnvironmentState() const { return service::binToDec(currentEnvironmentVector, environmentResolution); }

	// Задание случайного состояния среды
	void setRandomEnvironmentState(){
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = (rand() % 2 != 0);
	}
	// Загрузка структуры целей среды из файла
	virtual void loadEnvironment(std::string environmentFilename);

	// Выгрузка структуры целей в файл
	void uploadEnvironment(std::string environmentFilename) const;

	// Подсчет коэффициента заполненности всей среды
	double calculateOccupancyCoefficient() const;

	// Получение размерности вектора воздействия на среду (из скольки значений состоит одно воздействие на среду) - для среды-гиперкуба из одного числа (ID действия)
	int getActionResolution() const { return 1; }

	/* 
	Изменение среды под действием агента (возвращает совершено ли действие или его невозможно совершить)
	В рамках данной архитектуры actionID кодируется как +-(bitNumber), при этом знак определяет в какую сторону изменяется бит (+ это с нуля на единицу)
	*/
	int forceEnvironment(const std::vector<double>& action);

	// Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
	double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;
  // Определения какие цели были достигнуты на фронте текущей последовательности действий (на текущем шаге времени)
  // Возвращает массив достигнутых целей
  std::vector<int> testReachingAims(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;

	// Процедура генерации среды по требуемому коэффициенту заполненности, eps - точность генерации, также передается минимальная сложность цели и максимальная, а также минимальная максимальная сложность
	double generateEnvironment(int _environmentResolution, double requiredOccupancyCoef, int maxAimComplexity = 5, int minAimComplexity = 2, int minMaxAimComplexity = 3, double eps = 0.0005);

  virtual void printSettings(std::ostream& os) {
    os << "Environment_parameters:" << 
        "\tstochasticity-coefficient=" << getStochasticityCoefficient() << 
        "\treward-recovery-time=" << getRewardRecoveryTime() << 
        "\tenv-type=hypercube" <<
        std::endl << std::endl;
  }
  
	// Добавляем в друзья тестовые процедуры
	friend class tests;
  // Генерация пула сред
  // Входные параметры:
  // environmentDirectory - директория, в которую записываеются сгенерированные среды; environmentResolution - размерность среды
  // firstEnvironmentNumber - номер (в названии) первой среды
  // firstGoalDensity - начальная плотность целей; lastGoalDensity - последняя плотность целей
  // goalDensityStep - шаг изменения плотности целей (если между первым и последним значения плотности целей помещается целое число шагов, то генерируются цели и для последней плотности)
  // sameGDenvQuantity - количество сред в пуле с одинаковыми значения плотности целей
  static void generateEnvironmentsPool(std::string environmentDirectory, int environmentResolution, int firstEnvironmentNumber, double firstGoalDensity, double lastGoalDensity,
                                        double goalDensityStep, int sameGDenvQuantity, int maxAimComplexity = 5, int minAimComplexity = 2, int minMaxAimComplexity =3);
  // Класс дерева действий, приводящих к достижению целей в среде (нужен в основном для отрисовки структуры целей в виде графа)
  class TEnvironmentTree{
  public:
    TEnvironmentTree** subActions;
    int actionsQuantity;
    bool aimTerminate;
    TEnvironmentTree(int _actionsQuantity) : actionsQuantity(_actionsQuantity) {
      subActions = new TEnvironmentTree*[actionsQuantity];
      memset(subActions, 0, sizeof(TEnvironmentTree*) * actionsQuantity);
      aimTerminate = false;
    }
    ~TEnvironmentTree(){
      for (int action = 0; action < actionsQuantity; ++action)
        if (subActions[action])
          delete subActions[action];
      delete []subActions;
    }
  };
  // Отрисовка структуры целей в среде в виде графа (узлы графа - действия, признак окончания цели на этом действии - красный цвет)
  void printEnvironmentsGoalsHierarchy(std::string imageFilename);

  // Возвращает распределение длин целей в среде (нулевой элемент - цель длины два и т.д.) 
  std::vector<int> getAimLengthDistr() const;

  
};

#endif //THYPERCUBE_ENVIRONMENT_H