#ifndef TENVIRONMENT_H
#define TENVIRONMENT_H 

#include <string>
/*
NB!
Строго говоря надо сделать этот класс чисто виртуальным, чтобы реальные среды наследовали от него при этом предоставляя правильный интерфейс. 
Таким образом в эволюцию можно будет подгружать любую среду, если еще сделать отдельные сенсорные кодировщики и декодировщики действий агента.
*/


/*
Класс окружающей среды
*/
class TEnvironment{
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
	// Максимальное кол-во целей в среде
	static const int MAX_AIMS_QUANTITY = 3000;
	// Массив целей в среде (включая все подцели)
	TAim aimsSet[MAX_AIMS_QUANTITY];
	// Кол-во целей в среде
	int aimsQuantity;
	// Время восстановления награды за цель после ее достижения
	int rewardRecoveryTime;
	// Степень нестационарности среды (передается в виде вероятности изменения одного бита среды)
	double nonstaionarityCoefficient;
	// Текущий бинарный вектор среды
	bool* currentEnvironmentVector;
	// Размерность вектора среды
	int environmentResolution;

	// Процедура случайных изменений среды согласно степени нестационарности
	void TEnvironment::randomizeEnvironment();
	// Сравнение двух полных целей для процедуры генерации среды (возвращает true - если есть хотя бы одна совпадающая подцель)
	bool TEnvironment::compareDifferentLengthFullAims(TAim& firstAim, int minFirstSubAimComplexity, TAim& secondAim, int minSecondSubAimComplexity);

public:
	// Конструктор по умолчанию
	TEnvironment(){
		aimsQuantity = 0;
		environmentResolution = 0;
		rewardRecoveryTime = 0;
		nonstaionarityCoefficient = 0;
	}
	// Конструктор сразу с загрузкой целей и возможной установкой параметров сети
	TEnvironment(std::string aimsFilename, int _rewardRecoveryTime = 0, double _nonstaionarityCoefficient = 0){
		aimsQuantity = 0;
		environmentResolution = 0;
		rewardRecoveryTime = _rewardRecoveryTime;
		nonstaionarityCoefficient = _nonstaionarityCoefficient;
		loadAims(aimsFilename);
	}
	// Деструктор
	~TEnvironment(){
		if (environmentResolution) delete []currentEnvironmentVector;
	}

	// Геттеры и сеттеры параметров среды
	int getAimsQuantity() const { return aimsQuantity; }
	int getEnvironmentResolution() const {return environmentResolution; }
	int getRewardRecoveryTime() const {return rewardRecoveryTime; }
	void setRewardRecoveryTime(int _rewardRecoveryTime) {rewardRecoveryTime = _rewardRecoveryTime; }
	double getNonstationarityCoefficient() const { return nonstaionarityCoefficient; }
	void setNonstationarityCoefficient(double _nonstationarityCoefficient) { nonstaionarityCoefficient = _nonstationarityCoefficient; }
	void getCurrentEnvironmentVector(double environmentVector[]) const {
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			environmentVector[currentBit - 1] = static_cast<double>(currentEnvironmentVector[currentBit - 1]);
	}
	void setEnvironmentVector(double environmentVector[]) {
		for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
			currentEnvironmentVector[currentBit - 1] = (environmentVector[currentBit - 1] == 1);
	}
	// Загрузка структуры целей среды из файла
	void loadAims(std::string aimsFilename);

	// Выгрузка структуры целей в файл
	void uploadAims(std::string aimsFilename) const;

	// Подсчет коэффициента заполненности всей среды
	double calculateOccupancyCoefficient() const;

	/* 
	Изменение среды под действием агента (возвращает совершено ли действие или его невозможно совершить)
	В рамках данной архитектуры actionID кодируется как +-(bitNumber), при этом знак определяет в какую сторону изменяется бит (+ это с нуля на единицу)
	*/
	bool forceEnvironment(double actionID);

	// Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
	double calculateReward(double actionsIDs[], int actionsQuantity) const;

	// Процедура генерации среды по требуемому коэффициенту заполненности, eps - точность генерации, также передается минимальная сложность цели и максимальная, а также минимальная максимальная сложность
	double generateEnvironment(int _environmentResolution, double requiredOccupancyCoef, double eps = 0.001, int maxAimComplexity = 5, int minAimComplexity = 2, int minMaxAimComplexity = 3);

};

#endif // TENVIRONMENT_H
