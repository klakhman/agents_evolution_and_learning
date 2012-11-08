#ifndef TAGENT_H
#define TAGENT_H

#include "TNeuralNetwork.h"
#include "TPoolNetwork.h"
#include "TEnvironment.h"

#include <iostream>

/*
Класс нейросетевого агента
*/
class TAgent{
	// Геном агента из которого транслируется контроллер
	TPoolNetwork* genome;
	// Контроллер агент
	TNeuralNetwork* neuralController;
	// Награда, набранная агентом
	double reward;
	// Родители агента (первый более приспособленный, второй - менее)
	int parents[2];
	// Записанная жизнь агента
	double* agentLife;
	// Декодирование идентификатора совершаемого агентом действия (!!!! по идее декодирование действия, как и кодирование состояния среды во входной вектор, так и определение необходимой размерности выходного вектора должны совершаться некоторым сторонним от агента образом, так как это средоспецифические функции, но при этом не принадлежащие среде !!!!!)
	double decodeAction(double outputVector[]);
	// Подсчет размера выходного вектора нейроконтроллера и генома агента на основе размера входного вектора среды
	int calculateOutputResolution(int inputResolution) const;
	// Запрещаем копирующий конструктор (чтобы экземпляры нельзя было передавать по значению)
	TAgent(const TAgent&);
public:
	// Структура настройки первичного системогенеза
	struct SPrimarySystemogenesisSettings
	{
		int initialPoolCapacity; // Изначальная размерность каждого пула
		double initialDevelopSynapseProbability; // Вероятность образования синапса связи в процессе построения первичной сети
		double initialDevelopPredConnectionProbability; // Вероятность образования предикторной связи в процессе построения первичной сети
		int primarySystemogenesisTime; // Время (в тактах), в течение которого происходит первичный системогенез
		double spontaneousActivityProb; // Вероятность спонтанной активации нейрона
		double activeNeuronsPercent; // Процент наиболее активных нейронов, которые становятся активными после ПС
		double synapsesActivityTreshold; //! Порог общего сигнала, прошедшего по синапсу в ПС, по которому происходит отбор связей между активными нейронами (или процент синапсов которые надо отобрать)
		double significanceTreshold; // Порог значимости предсказания, осуществляемого по пред. связи в ПС, по которому происходит отбор связей между активными нейронами
	} primarySystemogenesisSettings;

	// Конструктор по умолчанию
	TAgent(){
		parents[0] = 0;
		parents[1] = 0;
		agentLife = 0;
		reward = 0;
		neuralController = new TNeuralNetwork;
		genome = new TPoolNetwork;
		primarySystemogenesisSettings.initialPoolCapacity = 1;
		primarySystemogenesisSettings.initialDevelopSynapseProbability = 1;
		primarySystemogenesisSettings.initialDevelopPredConnectionProbability = 1;
		primarySystemogenesisSettings.primarySystemogenesisTime = 0;
		primarySystemogenesisSettings.spontaneousActivityProb = 0;
		primarySystemogenesisSettings.activeNeuronsPercent = 0;
		primarySystemogenesisSettings.synapsesActivityTreshold = 0;
		primarySystemogenesisSettings.significanceTreshold = 0;
	}

	// Деструктор
	~TAgent(){
		if (agentLife) delete []agentLife;
		if (neuralController) delete neuralController;
		if (genome) delete genome;
	}
	// Геттеры и сеттеры
	double getReward() const { return reward; }
	int getMoreFitParent() const { return parents[0]; }
	int getLessFitParent() const { return parents[1]; }
	void setMoreFitParent(int moreFitParent) { parents[0] = moreFitParent; }
	void setLessFitParent(int lessFitParent) { parents[1] = lessFitParent; }
	TPoolNetwork* getPointerToAgentGenome() const { return genome; }
	TNeuralNetwork* getPointerToAgentController() const { return neuralController; }
	double* getPointerAgentLife() const { return agentLife; }

	// Загрузка нейроконтроллера агента
	void loadController(std::istream& is);
	// Загрузка генома нейрононтроллера
	void loadGenome(std::istream& is);
	// Выгрузка нейроконтроллера агета в файл или на экран
	void uploadController(std::ostream& os) const;
	// Выгрузка генома агента в файл или на экран
	void uploadGenome(std::ostream& os) const;
	// Генерация случайного минимального возможного генома агента
	void generateMinimalAgent(int inputResolution);
	// Линейная процедра первичного системогеназа (когда происходит однозначная трансляция генотипа) - используется, когда нет ни настоящего системогенеза, ни обучения
	void linearSystemogenesis();
	// Моделирование жизни агента
	void life(TEnvironment& environment, int agentLifeTime);
	// Оператор присваивания (фактически полное копирование агента, включая геном, но не включая контроллер - создание новых структур)
	TAgent& operator=(const TAgent& sourceAgent);

	// Временная процедура печати жизни агента
	void printLife(TEnvironment& environment, int agentLifeTime);

};

#endif // TAGENT_H
