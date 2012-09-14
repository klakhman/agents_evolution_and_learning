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
	// Очистка популяции
	void erasePopulation(){
		if (agents){
			for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
				delete agents[currentAgent - 1];
			delete []agents;
			agents = 0;
		}
	}
public:
	// Структура параметры эволюции
	struct SEvolutionSettings{
		int evolutionTime; // Кол-во тактов эволюции
		int agentLifetime; // Кол-во тактов жизни агента
	} evolutionSettings;
	// Структура "Найстройки мутационных процедур" (сюда включаются необходимые константы, относящиеся к мутациям)
	struct SMutationSettings{
		 double MutWeightProbability; // Вероятность мутации веса связи
		double MutWeightMeanDisp; // Дисперсия значения мутации веса связи
		double MutWeightDispDisp; // Дисперсия значения мутации дисперсии связи
		int DisLimit;  // Максимальное кол-во тактов, после которого выключенная связь удаляется из генома
		double EnConProb; // Вероятность включения выключенной связи
		double DisConProb; // Вероятность выключения включенной связи
		double AddConnectionProb; // Вероятность вставки связи
		double AddPredConnectionProb; // Вероятность вставки предикторной связи
		double DeleteConnectionProb; // Вероятность удаления связи
		double DeletePredConnectionProb; //  Вероятность удаления предикторной связи
		double DuplicatePoolProb;  // Вероятность дуплицирования нейронального пула (модернизированный эволюционный алгоритм)
		double PoolDivisionCoef; // Коэффициент уменьшения размерности пула при его деления в процедуре дупликации пула
		int PoolStandartAmount; // Стандартный размер сети в кол-ве пулов(для использования в процедуре дупликации нейрона)
		int ConnectionStandartAmount;
		double MutDevelopConProbProb; // Вероятность мутации вероятности образования связи связи
		double MutDevelopConProbDisp; // Дисперсия мутации вероятности образования связи связи
	} mutationSettings;

	// Конструктор по умолчанию
	TPopulation(){
		agents = 0;
		populationSize = 0;
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
	void evolution(TEnvironment& environment, int evolutionTime = 0);
};

#endif // TPOPULATION_H
