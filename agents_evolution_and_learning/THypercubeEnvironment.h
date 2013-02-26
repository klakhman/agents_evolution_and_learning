#ifndef THYPERCUBE_ENVIRONMENT_H
#define THYPERCUBE_ENVIRONMENT_H 

#include <string>
#include <cstdlib>
#include <cmath>
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
		static const int MAX_AIM_COMPLEXITY = 7;
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
	};
private:
  // Максимальное кол-во целей в среде
	static const int MAX_AIMS_QUANTITY = 3000;
	// Массив целей в среде (включая все подцели)
	TAim aimsSet[MAX_AIMS_QUANTITY];
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

public:
	// Конструктор по умолчанию
	THypercubeEnvironment(){
		aimsQuantity = 0;
		environmentResolution = 0;
		rewardRecoveryTime = 0;
		stochasticityCoefficient = 0;
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
		if (environmentResolution) delete []currentEnvironmentVector;
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

  // Получение константной ссылки на цель в среде (номер цели начинается с единица)
  const TAim& getAimReference(int aimNumber) const { return aimsSet[aimNumber-1]; }

	// Получение кол-ва начальных состояний среды (для текущей бинарной среды - фактически общее кол-во состояний)
	int getInitialStatesQuantity() const {
		return static_cast<int>(pow(2, environmentResolution));
	}
	// Задание вектора среды (по значению целого вектора)
	void setEnvironmentVector(double environmentVector[]) {
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = (environmentVector[currentBit - 1] == 1);
	}
	// Задание вектора среды (по номеру - !!!начиная с нуля!!!!) 
	void setEnvironmentState(double stateNumber){
		bool* requiredEnvironmentVector = new bool[environmentResolution];
		service::decToBin(static_cast<int>(stateNumber), requiredEnvironmentVector, environmentResolution);
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
	void loadEnvironment(std::string environmentFilename);

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
	bool forceEnvironment(const std::vector<double>& action);

	// Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
	double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;

	// Процедура генерации среды по требуемому коэффициенту заполненности, eps - точность генерации, также передается минимальная сложность цели и максимальная, а также минимальная максимальная сложность
	double generateEnvironment(int _environmentResolution, double requiredOccupancyCoef, double eps = 0.001, int maxAimComplexity = 5, int minAimComplexity = 2, int minMaxAimComplexity = 3);

	// Добавляем в друзья тестовые процедуры
	friend class tests;
};

#endif //THYPERCUBE_ENVIRONMENT_H