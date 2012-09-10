#ifndef TAGENT_H
#define TAGENT_H

#include "TNeuralNetwork.h"
#include "TPoolNetwork.h"
#include "TEnvironment.h"

/*
Класс нейросетевого агента
*/
class TAgent{
	// Записанная жизнь агента
	double* agentLife;
	// Награда, набранная агентом
	double reward;
	// Декодирование идентификатора совершаемого агентом действия
	double decodeAction(double outputVector[]);

public:
	TNeuralNetwork* neuralController;
	TPoolNetwork* genome;

	// Конструктор по умолчанию
	TAgent(){
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

	// Моделирование жизни агента
	void life(TEnvironment& environment, int agentLifeTime);
};

#endif TAGENT_H // TAGENT_H
