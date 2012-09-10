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
	// Записанная жизнь агента
	double* agentLife;
	// Награда, набранная агентом
	double reward;
	int parents[2];
	// Декодирование идентификатора совершаемого агентом действия
	double decodeAction(double outputVector[]);

public:
	TNeuralNetwork* neuralController;
	TPoolNetwork* genome;

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
		if (neuralController) delete []neuralController;
		if (genome) delete []genome;
	}
	// Геттеры и сеттеры
	double getReward() const { return reward; }
	int getMoreFitParent() const { return parents[0]; }
	int getLessFitParent() const { return parents[1]; }
	void setMoreFitParent(int moreFitParent) { parents[0] = moreFitParent; }
	void lessMoreFitParent(int lessFitParent) { parents[1] = lessFitParent; }

	// Загрузка нейроконтроллера агента
	void loadController(std::istream& is);
	// Загрузка генома нейрононтроллера
	void loadGenome(std::istream& is);
	// Генерация случайного минимального возможного генома агента
	void generateMinimalAgent(int inputResolution, int outputResolution, int initialPoolCapacity, int initialDevelopProbability);

	// Моделирование жизни агента
	void life(TEnvironment& environment, int agentLifeTime);
};

#endif TAGENT_H // TAGENT_H
