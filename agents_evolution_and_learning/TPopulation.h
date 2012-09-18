#ifndef TPOPULATION_H
#define TPOPULATION_H

#include "TAgent.h"
#include "TEnvironment.h"
#include <string>

/*
Класс популяции нейросетевых агентов
*/
class TPopulation{
	// Список агентов в популяции
	TAgent** agents;
	// Кол-во агентов в популяции
	int populationSize;
	// Текущий последний номер инновации связей популяции
	long connectionInnovationNumber;
	// Текущий последний номер инновации предикторных связей популяции
	long predConnectionInnovationNumber;

	// Очистка популяции
	void erasePopulation(){
		if (agents){
			for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
				delete agents[currentAgent - 1];
			delete []agents;
			agents = 0;
			connectionInnovationNumber = 0;
			predConnectionInnovationNumber = 0;
			populationSize = 0;
		}
	}
	// ---------------  Различные процедуры мутации -------------------------
	// Процедура мутации - мутация весовых коэффициентов связи
	void mutationConnectionsWeight(TAgent& kidAgent);
	// Процедура мутации - включения/выключения связей
	void mutationEnableDisableConnections(TAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - включения/выключения предикторных связей
	void mutationEnableDisablePredConnections(TAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - удаления из агента связей, которые выключены более некоторого количества поколений
	void mutationDeleteConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - удаления из агента предикторных связей, которые выключены более некоторого количества поколений
	void mutationDeletePredConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - добавление связи
	void mutationAddPoolConnection(TAgent& kidAgent);
	// Процедура мутации - добавление предикторной связи
	void mutationAddPoolPredConnection(TAgent& kidAgent);
	// Процедура мутации - удаление связи
	void mutationDeletePoolConnection(TAgent& kidAgent);
	// Процедура мутации - удаление предикторной связи
	void mutationDeletePoolPredConnection(TAgent& kidAgent);
	// Коэффициент уменьшения вероятности дупликации пула с ростом генома
	double duplicateDivision(int poolsQuantity, int connectionsQuantity);
	// Процедура мутации - дупликация пула
	void mutationPoolDuplication(TAgent& kidAgent);
	// Процедура мутации вероятности развития синапса по связи между пулами
	void mutationDevelopSynapseProb(TAgent& KidAgent);
	// Процедура мутации вероятности развития предикторной связи по предикторной связи между пулами
	void mutationDevelopPredConProb(TAgent& KidAgent);
	// Процедура составления потомка от двух родителей
	void сomposeOffspringFromParents(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent);
	// Процедура генерации одного потомка
	void generateOffspring(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent, int currentEvolutionStep);
	//Процедура получения номера агента, используемая в рулеточном алгоритме
	int rouletteWheelSelection(double populationFitness[]);
	// Процедура генерации нового поколения и замены старого на новое
	void generateNextPopulation(int currentEvolutionStep);

public:
	// Структура параметры эволюции
	struct SEvolutionSettings{
		int evolutionTime; // Кол-во тактов эволюции
		int agentLifetime; // Кол-во тактов жизни агента
	} evolutionSettings;

	// Структура "Найстройки мутационных процедур" (сюда включаются необходимые константы, относящиеся к мутациям)
	struct SMutationSettings{
		 double mutWeightProbability; // Вероятность мутации веса связи
		double mutWeightMeanDisp; // Дисперсия значения мутации веса связи
		double mutWeightDispDisp; // Дисперсия значения мутации дисперсии связи
		int disLimit;  // Максимальное кол-во тактов, после которого выключенная связь удаляется из генома
		double enableConnectionProb; // Вероятность включения выключенной связи
		double disableConnectionProb; // Вероятность выключения включенной связи
		double addConnectionProb; // Вероятность вставки связи
		double addPredConnectionProb; // Вероятность вставки предикторной связи
		double deleteConnectionProb; // Вероятность удаления связи
		double deletePredConnectionProb; //  Вероятность удаления предикторной связи
		double duplicatePoolProb;  // Вероятность дуплицирования нейронального пула (модернизированный эволюционный алгоритм)
		double poolDivisionCoef; // Коэффициент уменьшения размерности пула при его деления в процедуре дупликации пула
		int poolStandartAmount; // Стандартный размер сети в кол-ве пулов(для использования в процедуре дупликации нейрона)
		int connectionStandartAmount; // Стандартное кол-во связей в сети
		double mutDevelopConProbProb; // Вероятность мутации вероятности образования связи связи
		double mutDevelopConProbDisp; // Дисперсия мутации вероятности образования связи связи
	} mutationSettings;

	// Конструктор по умолчанию
	TPopulation(){
		agents = 0;
		populationSize = 0;
		connectionInnovationNumber = 0;
		predConnectionInnovationNumber = 0;
	}
	// Конструктор сразу с загрузкой геномов агентов
	TPopulation(std::string populationFilename, int _populationSize){
		loadPopulation(populationFilename, _populationSize);
	}
	// Деструктор
	~TPopulation(){ 
		if (agents) {
			for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
				delete agents[currentAgent - 1];
			delete []agents;
		}
	}
	// Геттеры и сеттеры
	int getPopulationSize() const { return populationSize; }
	TAgent* getPointertoAgent(int agentNumber) const { return agents[agentNumber]; }

	// Загрузка популяции геномов из файла
	void loadPopulation(std::string populationFilename, int _populationSize);

	// Выгрузка популяции геномов в файл
	void uploadPopulation(std::string populationFilename) const;

	// Генерация минимальной популяции
	void generateMinimalPopulation(int _populationSize, int inputResolution);

	// Процедура эволюции популяции (если указан ноль, то эволюция идет кол-во тактов, указанное в настройках)
	void evolution(TEnvironment& environment);
};

#endif // TPOPULATION_H
