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
	int calculateOutputResoltion(int inputResolution) const;
	// Запрещаем копирующий конструктор (чтобы экземпляры нельзя было передавать по значению)
	TAgent(const TAgent&);
public:
	// Конструктор по умолчанию
	TAgent(){
		parents[0] = 0;
		parents[1] = 0;
		agentLife = 0;
		reward = 0;
		neuralController = 0;
		genome = 0;
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
	TPoolNetwork* getPointerToAgentGenome(int agentNumber) const { return genome; }
	TNeuralNetwork* getPointerToAgentController(int agentNumber) const { return neuralController; }

	// Загрузка нейроконтроллера агента
	void loadController(std::istream& is);
	// Загрузка генома нейрононтроллера
	void loadGenome(std::istream& is);
	// Выгрузка нейроконтроллера агета в файл или на экран
	void uploadController(std::ostream& os) const;
	// Выгрузка генома агента в файл или на экран
	void uploadGenome(std::ostream& os) const;
	// Генерация случайного минимального возможного генома агента
	void generateMinimalAgent(int inputResolution, int initialPoolCapacity = 1, double initialDevelopProbability = 1);
	// Линейная процедра первичного системогеназа (когда происходит однозначная трансляция генотипа) - используется, когда нет ни настоящего системогенеза, ни обучения
	void linearSystemogenesis();
	// Моделирование жизни агента
	void life(TEnvironment& environment, int agentLifeTime);
};

#endif TAGENT_H // TAGENT_H
