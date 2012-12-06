#ifndef TPOPULATION_H
#define TPOPULATION_H

#include "TEnvironment.h"
#include "TPoolNetwork.h"
#include <string>
#include <fstream>
#include "service.h"

template <class TemplateNeuralAgent>
/*

Класс популяции нейросетевых агентов - ШАБЛОННЫЙ.
При создании экземпляра класса необходимо указывать какой конкретно класс агента должен использоваться.

*/
class TPopulation{
	// Список агентов в популяции
	TemplateNeuralAgent** agents;
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
				if (agents[currentAgent - 1])
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
	void mutationConnectionsWeight(TemplateNeuralAgent& kidAgent);
	// Процедура мутации - включения/выключения связей
	void mutationEnableDisableConnections(TemplateNeuralAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - включения/выключения предикторных связей
	void mutationEnableDisablePredConnections(TemplateNeuralAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - удаления из агента связей, которые выключены более некоторого количества поколений
	void mutationDeleteConnectionPopulation(TemplateNeuralAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - удаления из агента предикторных связей, которые выключены более некоторого количества поколений
	void mutationDeletePredConnectionPopulation(TemplateNeuralAgent& kidAgent, int currentEvolutionStep);
	// Процедура мутации - добавление связи
	void mutationAddPoolConnection(TemplateNeuralAgent& kidAgent);
	// Процедура мутации - добавление предикторной связи
	void mutationAddPoolPredConnection(TemplateNeuralAgent& kidAgent);
	// Процедура мутации - удаление связи
	void mutationDeletePoolConnection(TemplateNeuralAgent& kidAgent);
	// Процедура мутации - удаление предикторной связи
	void mutationDeletePoolPredConnection(TemplateNeuralAgent& kidAgent);
	// Коэффициент уменьшения вероятности дупликации пула с ростом генома
	double duplicateDivision(int poolsQuantity, int connectionsQuantity);
	// Процедура мутации - дупликация пула
	void mutationPoolDuplication(TemplateNeuralAgent& kidAgent, int evolutionaryTime =0);
	// Процедура мутации вероятности развития синапса по связи между пулами
	void mutationDevelopSynapseProb(TemplateNeuralAgent& KidAgent);
	// Процедура мутации вероятности развития предикторной связи по предикторной связи между пулами
	void mutationDevelopPredConProb(TemplateNeuralAgent& KidAgent);
	// Процедура составления потомка от двух родителей
	void composeOffspringFromParents(TemplateNeuralAgent& kidAgent, const TemplateNeuralAgent& firstParentAgent, const TemplateNeuralAgent& secondParentAgent);
	// Процедура генерации одного потомка
	void generateOffspring(TemplateNeuralAgent& kidAgent, const TemplateNeuralAgent& firstParentAgent, const TemplateNeuralAgent& secondParentAgent, int currentEvolutionStep);
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
		erasePopulation();
	}
	// Геттеры и сеттеры
	int getPopulationSize() const { return populationSize; }
	// Изменение размера популяции (изменяется не только само значение, но и удаляется старая популяция и создается пустая новая)
	void setPopulationSize(int _populationSize){ 
		if (agents)	erasePopulation();
		populationSize = _populationSize;
		agents = new TemplateNeuralAgent*[populationSize];
		for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
			agents[currentAgent - 1] = new TemplateNeuralAgent;
	}
	TemplateNeuralAgent* getPointertoAgent(int agentNumber) const { return agents[agentNumber - 1]; }

	// Загрузка популяции геномов из файла (если размер популяции не передается, то значение берется из текущих параметров популяции)
	void loadPopulation(std::string populationFilename, int _populationSize = 0);

	// Выгрузка популяции геномов в файл
	void uploadPopulation(std::string populationFilename) const;

	// Генерация минимальной популяции (если размер популяции не передается, то значение берется из текущих параметров популяции)
	void generateMinimalPopulation(int inputResolution, int _populationSize = 0);

	// Шаг эволюционного процесса
	void evolutionaryStep(TEnvironment& environment, int evolutionStepNumber);

	// Процедура эволюции популяции
	void evolution(TEnvironment& environment);

	// Копирование популяции (создание всех новых структур)
	TPopulation<TemplateNeuralAgent>& operator=(const TPopulation<TemplateNeuralAgent>& sourcePopulation);

	friend class tests;
};

// Загрузка популяции геномов из файла (если размер популяции не передается, то значение берется из текущих параметров популяции)
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::loadPopulation(std::string populationFilename, int _populationSize /*=0*/){
	//!!! Возможно надо как-то по другому обходится с номерами инноваций
	connectionInnovationNumber = 0;
	predConnectionInnovationNumber = 0;
	if (_populationSize) setPopulationSize(_populationSize);
	std::ifstream populationFile;
	populationFile.open(populationFilename.c_str());
	for (int currentAgent = 1; currentAgent <=populationSize; ++currentAgent){
		agents[currentAgent - 1]->loadGenome(populationFile);
	}
	populationFile.close();
}

// Выгрузка популяции геномов в файл
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::uploadPopulation(std::string populationFilename) const{
	std::ofstream populationFile;
	populationFile.open(populationFilename.c_str());
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->uploadGenome(populationFile);
	populationFile.close();
}

// Генерация минимальной популяции
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::generateMinimalPopulation(int inputResolution, int _populationSize /*= 0*/){
	if (_populationSize) populationSize = _populationSize;
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->generateMinimalAgent(inputResolution);
	if (populationSize){
		connectionInnovationNumber = agents[0]->getPointerToAgentGenome()->getConnectionsQuantity();
		predConnectionInnovationNumber = agents[0]->getPointerToAgentGenome()->getPredConnectionsQuantity();
	}
}
// Шаг эволюционного процесса
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::evolutionaryStep(TEnvironment& environment, int evolutionStepNumber){
	// Сначала мутируем популяцию (чтобы можно потом было отчеты составлять и всегда текущая популяция уже была прогнана на среде)
	if (evolutionStepNumber != 1) generateNextPopulation(evolutionStepNumber);
	// Прогоняем всех агентов
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		// Проводим первичный системогенез (если нужно)
		if (agents[currentAgent - 1]->primarySystemogenesisSettings.primarySystemogensisMode)
			agents[currentAgent - 1]->primarySystemogenesis();
		else
			agents[currentAgent - 1]->linearSystemogenesis();

		environment.setRandomEnvironmentState();
		agents[currentAgent - 1]->life(environment, evolutionSettings.agentLifetime);
	}
}

// Процедура эволюции популяции
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::evolution(TEnvironment& environment){
	for (int evolutionStep = 1; evolutionStep <= evolutionSettings.evolutionTime; ++evolutionStep){
		evolutionaryStep(environment, evolutionStep);
	}
}

// Копирование популяции (создание всех новых структур)
template<class TemplateNeuralAgent>
TPopulation<TemplateNeuralAgent>& TPopulation<TemplateNeuralAgent>::operator=(const TPopulation<TemplateNeuralAgent>& sourcePopulation){
	// Сразу изменяем кол-во агентов
	setPopulationSize(sourcePopulation.getPopulationSize());	
	connectionInnovationNumber = sourcePopulation.connectionInnovationNumber;
	predConnectionInnovationNumber = sourcePopulation.predConnectionInnovationNumber;
	// Копируем параметры эволюции
	evolutionSettings.agentLifetime = sourcePopulation.evolutionSettings.agentLifetime;
	evolutionSettings.evolutionTime = sourcePopulation.evolutionSettings.evolutionTime;
	// Копируем параметры мутаций
	mutationSettings.mutWeightProbability = sourcePopulation.mutationSettings.mutWeightProbability;
	mutationSettings.mutWeightMeanDisp = sourcePopulation.mutationSettings.mutWeightMeanDisp;
	mutationSettings.mutWeightDispDisp = sourcePopulation.mutationSettings.mutWeightDispDisp; 
	mutationSettings.disLimit = sourcePopulation.mutationSettings.disLimit;
	mutationSettings.enableConnectionProb = sourcePopulation.mutationSettings.enableConnectionProb;
	mutationSettings.disableConnectionProb = sourcePopulation.mutationSettings.disableConnectionProb;
	mutationSettings.addConnectionProb = sourcePopulation.mutationSettings.addConnectionProb;
	mutationSettings.addPredConnectionProb = sourcePopulation.mutationSettings.addPredConnectionProb;
	mutationSettings.deleteConnectionProb = sourcePopulation.mutationSettings.deleteConnectionProb;
	mutationSettings.deletePredConnectionProb = sourcePopulation.mutationSettings.deletePredConnectionProb;
	mutationSettings.duplicatePoolProb = sourcePopulation.mutationSettings.duplicatePoolProb;
	mutationSettings.poolDivisionCoef = sourcePopulation.mutationSettings.poolDivisionCoef;
	mutationSettings.poolStandartAmount = sourcePopulation.mutationSettings.poolStandartAmount;
	mutationSettings.connectionStandartAmount = sourcePopulation.mutationSettings.connectionStandartAmount;
	mutationSettings.mutDevelopConProbProb = sourcePopulation.mutationSettings.mutDevelopConProbProb;
	mutationSettings.mutDevelopConProbDisp = sourcePopulation.mutationSettings.mutDevelopConProbDisp;
	// Копируем всех агентов
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		*(agents[currentAgent - 1]) = *(sourcePopulation.agents[currentAgent - 1]);

	return *this;
}

// ДАЛЕЕ ИДУТ МЕТОДЫ КЛАССА - ТАК КАК КЛАСС ШАБЛОННЫЙ, ТО МЕТОДЫ ПРИХОДИТСЯ РАСПОЛАГАТЬ В ЗАГОЛОВОЧНОМ ФАЙЛЕ

// ---------------  Различные процедуры мутации -------------------------
// Процедура мутации - мутация весовых коэффициентов связи
template<class TemplateNeuralAgent>
void  TPopulation<TemplateNeuralAgent>::mutationConnectionsWeight(TemplateNeuralAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool){
		// Сначала мутируем смещение
		if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutWeightProbability){
			kidGenome->setPoolBiasMean(currentPool, kidGenome->getPoolBiasMean(currentPool) +
																service::uniformDistribution(-mutationSettings.mutWeightMeanDisp, mutationSettings.mutWeightMeanDisp));
			// Важно, чтобы дисперсия была не меньше нуля
			kidGenome->setPoolBiasVariance(currentPool, fabs(kidGenome->getPoolBiasVariance(currentPool) + 
															service::uniformDistribution(-mutationSettings.mutWeightDispDisp, mutationSettings.mutWeightDispDisp)));
		}
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			 // Если мутация имеет место быть и связь включена
			if ((service::uniformDistribution(0, 1, true, false) < mutationSettings.mutWeightProbability) && (kidGenome->getConnectionEnabled(currentPool, currentPoolConnection))) {
				kidGenome->setConnectionWeightMean(currentPool, currentPoolConnection, kidGenome->getConnectionWeightMean(currentPool, currentPoolConnection) +
																service::uniformDistribution(-mutationSettings.mutWeightMeanDisp, mutationSettings.mutWeightMeanDisp));
				// Важно, чтобы дисперсия была не меньше нуля
				kidGenome->setConnectionWeightVariance(currentPool, currentPoolConnection, fabs(kidGenome->getConnectionWeightVariance(currentPool, currentPoolConnection) + 
																service::uniformDistribution(-mutationSettings.mutWeightDispDisp, mutationSettings.mutWeightDispDisp)));
			}
	}
}

// Процедура мутации - включения/выключения связей
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationEnableDisableConnections(TemplateNeuralAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			if (kidGenome->getConnectionEnabled(currentPool, currentPoolConnection)){
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.disableConnectionProb){
					kidGenome->setConnectionEnabled(currentPool, currentPoolConnection, false);
					kidGenome->setConnectionDisabledStep(currentPool, currentPoolConnection, currentEvolutionStep);
				}
			} else
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.enableConnectionProb){
					kidGenome->setConnectionEnabled(currentPool, currentPoolConnection, true);
					kidGenome->setConnectionDisabledStep(currentPool, currentPoolConnection, 0);
				}
}

// Процедура мутации - включения/выключения предикторных связей
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationEnableDisablePredConnections(TemplateNeuralAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			if (kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection)){
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.disableConnectionProb){
					kidGenome->setPredConnectionEnabled(currentPool, currentPoolPredConnection, false);
					kidGenome->setPredConnectionDisabledStep(currentPool, currentPoolPredConnection, currentEvolutionStep);
				}
			} else
				if (service::uniformDistribution(0, 1, true, false) < mutationSettings.enableConnectionProb){
					kidGenome->setPredConnectionEnabled(currentPool, currentPoolPredConnection, true);
					kidGenome->setPredConnectionDisabledStep(currentPool, currentPoolPredConnection, 0);
				}
}

// Процедура мутации - удаления из агента связей, которые выключены более некоторого количества поколений
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationDeleteConnectionPopulation(TemplateNeuralAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			// Если эта связь была выключена и ее уже надо удалить из генома
			if (( !kidGenome->getConnectionEnabled(currentPool, currentPoolConnection)) &&
								(currentEvolutionStep - kidGenome->getConnectionDisabledStep(currentPool, currentPoolConnection) >= mutationSettings.disLimit)){
				kidGenome->deleteConnection(currentPool, currentPoolConnection, false);
				--currentPoolConnection; // Чтобы указатель остался на том же номере связи
			}
	kidGenome->fixConnectionsIDs();
}

// Процедура мутации - удаления из агента предикторных связей, которые выключены более некоторого количества поколений
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationDeletePredConnectionPopulation(TemplateNeuralAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			// Если эта предикторная связь была выключена и ее уже надо удалить из генома
			if (( !kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection)) &&
								(currentEvolutionStep - kidGenome->getPredConnectionDisabledStep(currentPool, currentPoolPredConnection) >= mutationSettings.disLimit)){
				kidGenome->deletePredConnection(currentPool, currentPoolPredConnection, false);
				--currentPoolPredConnection; // Чтобы указатель остался на том же номере предикторной связи
			}
	kidGenome->fixPredConnectionsIDs();
}

// Процедура мутации - добавление связи
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationAddPoolConnection(TemplateNeuralAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.addConnectionProb){ // Если имеет место мутация
		int prePoolID, postPoolID; // Пресинаптический и постсинаптический пул потенциальной связи
		int tryCount = 0; //Количество ложных генераций связи - введено, чтобы не было зацикливания
		int findedConnection = 0;
		do{ // Цикл нахождения отсутствующей связи
			do{ // Цикл нахождения корректной связи
				prePoolID = service::uniformDiscreteDistribution(1, kidGenome->getPoolsQuantity());
				// Постсинаптический пул точно не может быть входным
				postPoolID = service::uniformDiscreteDistribution(kidGenome->getInputResolution() + 1, kidGenome->getPoolsQuantity());
				//Если постсинаптический и пресинаптический пулы не являются одновременно выходными, то значит мы нашли корректную связь
			} while (! ((postPoolID > kidGenome->getInputResolution() + kidGenome->getOutputResolution()) || 
																(prePoolID <= kidGenome->getInputResolution()) || (prePoolID > kidGenome->getInputResolution() + kidGenome->getOutputResolution())));
			++tryCount;
		// Пока не найдем отсутствующую связь или превысим допустимое кол-во попыток
		}while ((findedConnection = kidGenome->findConnectionNumber(prePoolID, postPoolID)) && (tryCount <= 1000));
		// Если отсутствующая связь найдена
		if (! findedConnection){
			kidGenome->addConnection(prePoolID, postPoolID, service::uniformDistribution(-0.5, 0.5), 0, kidAgent.primarySystemogenesisSettings.initialDevelopSynapseProbability, 
												true, 0, ++connectionInnovationNumber);
			// Детекция необходимости сдвига постсинаптического пула в следующий слой, если появилась связь между пулами одного слоя
			if ((kidGenome->getPoolLayer(prePoolID) == kidGenome->getPoolLayer(postPoolID)) && (prePoolID != postPoolID))
				for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
					// Сдвигаем постсинапчиеский пул и все пулы в более дальних слоях
					if ((kidGenome->getPoolLayer(currentPool) > kidGenome->getPoolLayer(prePoolID)) || (kidGenome->getPoolID(currentPool) == postPoolID))
						kidGenome->setPoolLayer(currentPool, kidGenome->getPoolLayer(currentPool) + 1); 
			kidGenome->fixConnectionsIDs();
		}
	}
}

// Процедура мутации - добавление предикторной связи
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationAddPoolPredConnection(TemplateNeuralAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.addPredConnectionProb){ // Если имеет место мутация
		int prePoolID, postPoolID; // Пресинаптический и постсинаптический пул потенциальной предикторной связи
		int tryCount = 0; //Количество ложных генераций предикторной связи - введено, чтобы не было зацикливания
		int findedConnection = 0;
		do{ // Цикл нахождения отсутствующей предикторной связи
			prePoolID = service::uniformDiscreteDistribution(1, kidGenome->getPoolsQuantity());
			// Постсинаптический пул точно не может быть входным или выходным
			postPoolID = service::uniformDiscreteDistribution(kidGenome->getInputResolution() + kidGenome->getOutputResolution() + 1, kidGenome->getPoolsQuantity());
			++tryCount;
		// Пока не найдем отсутствующую предикторную связь или превысим допустимое кол-во попыток
		}while ((findedConnection = kidGenome->findPredConnectionNumber(prePoolID, postPoolID)) && (tryCount <= 1000));
		// Если отсутствующая предикторная связь найдена
		if (! findedConnection){
			kidGenome->addPredConnection(prePoolID, postPoolID, kidAgent.primarySystemogenesisSettings.initialDevelopPredConnectionProbability,  
													true, 0, ++predConnectionInnovationNumber); 
			kidGenome->fixPredConnectionsIDs();
		}
	}
}

// Процедура мутации - удаление связи
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationDeletePoolConnection(TemplateNeuralAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if ((service::uniformDistribution(0, 1, true, false) < mutationSettings.deleteConnectionProb) && (kidGenome->getConnectionsQuantity())){ //  Если имеет место мутация и есть что удалять
		int deletingConnectionID = service::uniformDiscreteDistribution(1, kidGenome->getConnectionsQuantity());
		// Нахождение удаляемой связи
		for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
			for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
				if (kidGenome->getConnectionID(currentPool, currentPoolConnection) == deletingConnectionID){ // Если мы нашли нужную связь
					kidGenome->deleteConnection(currentPool, currentPoolConnection);
					break;
				}
	}
}

// Процедура мутации - удаление предикторной связи
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationDeletePoolPredConnection(TemplateNeuralAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if ((service::uniformDistribution(0, 1, true, false) < mutationSettings.deletePredConnectionProb) && (kidGenome->getPredConnectionsQuantity())){ //  Если имеет место мутация и есть что удалять
		int deletingPredConnectionID = service::uniformDiscreteDistribution(1, kidGenome->getPredConnectionsQuantity());
		// Нахождение удаляемой предикторной связи
		for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
			for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
				if (kidGenome->getPredConnectionID(currentPool, currentPoolPredConnection) == deletingPredConnectionID){ // Если мы нашли нужную предикторную связь
					kidGenome->deletePredConnection(currentPool, currentPoolPredConnection);
					break;
				}
	}
}

// Коэффициент уменьшения вероятности дупликации пула с ростом генома
template<class TemplateNeuralAgent>
double TPopulation<TemplateNeuralAgent>::duplicateDivision(int poolsQuantity, int connectionsQuantity){
	return max(1.0, 0.5 * connectionsQuantity/static_cast<double>(mutationSettings.connectionStandartAmount) 
																							+ 0.5 * (poolsQuantity - mutationSettings.poolStandartAmount));
}

// Процедура мутации - дупликация пула
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationPoolDuplication(TemplateNeuralAgent& kidAgent, int evolutionaryTime /*=0*/){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	// Исходные кол-ва элементов в сети
	int initPoolsQuantity = kidGenome->getPoolsQuantity();
	int initConnectionsQuantity = kidGenome->getConnectionsQuantity();
	int initPredConnectionsQuantity = kidGenome->getPredConnectionsQuantity();
	// Проходимся по всем старым пулам
	for (int currentPool = 1; currentPool <= initPoolsQuantity; ++currentPool)
		if (kidGenome->getPoolType(currentPool) == 1){ // Если пул внутренний
			// Проверяем дуплицирует ли пул (с учетом введеной поправки, уменьшающей вероятность дупликации с ростом структуры генома в эволюции)
			bool duplicate = (service::uniformDistribution(0, 1, true, false) < mutationSettings.duplicatePoolProb/duplicateDivision(initPoolsQuantity, initConnectionsQuantity));
			if (duplicate){ // Если пул дуплицирует
				if (kidGenome->getPoolCapacity(currentPool) != 1 ) // Если в пуле не остался только один нейрон (иначе оставляем размерность = 1)
					kidGenome->setPoolCapacity(currentPool, static_cast<int>(kidGenome->getPoolCapacity(currentPool) * mutationSettings.poolDivisionCoef + 0.5));
				kidGenome->addPool(kidGenome->getPoolType(currentPool), kidGenome->getPoolLayer(currentPool),
											kidGenome->getPoolBiasMean(currentPool), kidGenome->getPoolBiasVariance(currentPool), kidGenome->getPoolCapacity(currentPool));
				kidGenome->setPoolRootPoolID(kidGenome->getPoolsQuantity(), currentPool);
				kidGenome->setPoolAppearenceEvolutionTime(kidGenome->getPoolsQuantity(), evolutionaryTime);

				// Копируем все входящие связи из дуплицирующего пула в новый
				for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
					// Если это старая связь - до процедуры мутации
					if (kidGenome->getConnectionID(currentPool, currentPoolConnection) <= initConnectionsQuantity){
						int prePoolNumber;
						// Если это не связь на самого себя
						if (kidGenome->getConnectionPrePoolID(currentPool, currentPoolConnection) != kidGenome->getConnectionPostPoolID(currentPool, currentPoolConnection))
							prePoolNumber = kidGenome->getConnectionPrePoolID(currentPool, currentPoolConnection);
						else 
							prePoolNumber = kidGenome->getPoolsQuantity();
						kidGenome->addConnection(prePoolNumber, kidGenome->getPoolsQuantity(), kidGenome->getConnectionWeightMean(currentPool, currentPoolConnection),
														kidGenome->getConnectionWeightVariance(currentPool, currentPoolConnection), kidGenome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection),
														kidGenome->getConnectionEnabled(currentPool, currentPoolConnection), kidGenome->getConnectionDisabledStep(currentPool, currentPoolConnection), 
														++connectionInnovationNumber);
					}
            // Копируем все выходящие связи из дуплицирующего пула (для этого надо пройтись по всей сети, так как эти связи разбросаны)
				for (int currentPostPool = 1; currentPostPool <= initPoolsQuantity; ++currentPostPool)
					if (currentPostPool != currentPool) // Если это не дуплицирующий пул
						for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPostPool); ++currentPoolConnection)
							if (kidGenome->getConnectionID(currentPostPool, currentPoolConnection) <= initConnectionsQuantity) // Если это старая связь
								// Если у текущего потенциального пула есть связь с текущим дублицирующим пулом
								if (kidGenome->getConnectionPrePoolID(currentPostPool, currentPoolConnection) == currentPool){
									kidGenome->addConnection(kidGenome->getPoolsQuantity(), currentPostPool, kidGenome->getConnectionWeightMean(currentPostPool, currentPoolConnection) / 2.0,
														kidGenome->getConnectionWeightVariance(currentPostPool, currentPoolConnection), kidGenome->getConnectionDevelopSynapseProb(currentPostPool, currentPoolConnection), 
														kidGenome->getConnectionEnabled(currentPostPool, currentPoolConnection), kidGenome->getConnectionDisabledStep(currentPostPool, currentPoolConnection),
														++connectionInnovationNumber);
									// Выходные связи дуплицирующего нейрона мы делим пополам между новым и дуплицирующим
									kidGenome->setConnectionWeightMean(currentPostPool, currentPoolConnection, kidGenome->getConnectionWeightMean(currentPostPool, currentPoolConnection) / 2.0); 
								}
				// ТЕПЕРЬ КОПИРУЕМ ВСЕ ПРЕДИКТОРНЫЕ СВЯЗИ
				// Копируем все входящие предикторные связи из дуплицирующего пула в новый
				for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
					// Если это старая предикторная связь - до процедуры мутации
					if (kidGenome->getPredConnectionID(currentPool, currentPoolPredConnection) <= initPredConnectionsQuantity){
						int prePoolNumber;
						// Если это не предикторная связь на самого себя
						if (kidGenome->getPredConnectionPrePoolID(currentPool, currentPoolPredConnection) != kidGenome->getPredConnectionPostPoolID(currentPool, currentPoolPredConnection))
							prePoolNumber = kidGenome->getPredConnectionPrePoolID(currentPool, currentPoolPredConnection);
						else 
							prePoolNumber = kidGenome->getPoolsQuantity();
						kidGenome->addPredConnection(prePoolNumber, kidGenome->getPoolsQuantity(), kidGenome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection), 
														kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection), kidGenome->getPredConnectionDisabledStep(currentPool, currentPoolPredConnection),
														++predConnectionInnovationNumber);
					}
            // Копируем все выходящие предикторные связи из дуплицирующего пула (для этого надо пройтись по всей сети, так как эти связи разбросаны)
				for (int currentPostPool = 1; currentPostPool <= initPoolsQuantity; ++currentPostPool)
					if (currentPostPool != currentPool) // Если это не дуплицирующий пул
						for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPostPool); ++currentPoolPredConnection)
							if (kidGenome->getPredConnectionID(currentPostPool, currentPoolPredConnection) <= initPredConnectionsQuantity) // Если это старая связь
								// Если у текущего потенциального пула есть предикторная связь с текущим дублицирующим пулом
								if (kidGenome->getPredConnectionPrePoolID(currentPostPool, currentPoolPredConnection) == currentPool)
									kidGenome->addPredConnection(kidGenome->getPoolsQuantity(), currentPostPool, kidGenome->getDevelopPredConnectionProb(currentPostPool, currentPoolPredConnection),
														kidGenome->getPredConnectionEnabled(currentPostPool, currentPoolPredConnection), kidGenome->getPredConnectionDisabledStep(currentPostPool, currentPoolPredConnection),
														++predConnectionInnovationNumber);
			} // duplicate
		}
	kidGenome->fixConnectionsIDs();
	kidGenome->fixPredConnectionsIDs();
}

// Процедура мутации вероятности развития синапса по связи между пулами
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationDevelopSynapseProb(TemplateNeuralAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutDevelopConProbProb)
				// Важно, чтобы вероятность была между нулем и единицей
					kidGenome->setConnectionDevelopSynapseProb(currentPool, currentPoolConnection, 
						min(1.0, max(0.0, kidGenome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection) + service::uniformDistribution(-mutationSettings.mutDevelopConProbDisp, mutationSettings.mutDevelopConProbDisp))));
}

// Процедура мутации вероятности развития предикторной связи по предикторной связи между пулами
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::mutationDevelopPredConProb(TemplateNeuralAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutDevelopConProbProb)
				// Важно, чтобы вероятность была между нулем и единицей
					kidGenome->setDevelopPredConnectionProb(currentPool, currentPoolPredConnection, 
						min(1.0, max(0.0, kidGenome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection) + service::uniformDistribution(-mutationSettings.mutDevelopConProbDisp, mutationSettings.mutDevelopConProbDisp))));
}

// Процедура составления потомка от двух родителей
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::composeOffspringFromParents(TemplateNeuralAgent& kidAgent, const TemplateNeuralAgent& firstParentAgent, const TemplateNeuralAgent& secondParentAgent){
	if (firstParentAgent.getReward() >= secondParentAgent.getReward())
		kidAgent = firstParentAgent;
	else 
		kidAgent = secondParentAgent;
}

// Процедура генерации одного потомка
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::generateOffspring(TemplateNeuralAgent& kidAgent, const TemplateNeuralAgent& firstParentAgent, const TemplateNeuralAgent& secondParentAgent, int currentEvolutionStep){
	
	composeOffspringFromParents(kidAgent, firstParentAgent, secondParentAgent);

	mutationAddPoolConnection(kidAgent);
	//mutationAddPoolPredConnection(kidAgent);
	mutationDeletePoolConnection(kidAgent);
	//mutationDeletePoolPredConnection(kidAgent);

	mutationPoolDuplication(kidAgent, currentEvolutionStep);

	//mutationDeleteConnectionPopulation(kidAgent, currentEvolutionStep);
	//mutationDeletePredConnectionPopulation(kidAgent, currentEvolutionStep);
	//mutationEnableDisableConnections(kidAgent, currentEvolutionStep);
	//mutationEnableDisablePredConnections(kidAgent, currentEvolutionStep);

	mutationConnectionsWeight(kidAgent);
	//mutationDevelopSynapseProb(kidAgent);
	//mutationDevelopPredConProb(kidAgent);
}

//Процедура получения номера агента, используемая в рулеточном алгоритме
template<class TemplateNeuralAgent>
int TPopulation<TemplateNeuralAgent>::rouletteWheelSelection(double populationFitness[]){
   double totalFitness = 0; // Полная награда популяции
	for (int currentAgent = 1; currentAgent <= populationSize; currentAgent++) 
		totalFitness += populationFitness[currentAgent - 1];

   double currentRandomFitness = service::uniformDistribution(0, totalFitness, false, false);
   double currentSum = 0.0;
   int currentAgent = 0;
   while (currentRandomFitness > currentSum)
      currentSum += populationFitness[currentAgent++];

   return currentAgent;
}

// Процедура генерации нового поколения и замены старого на новое
template<class TemplateNeuralAgent>
void TPopulation<TemplateNeuralAgent>::generateNextPopulation(int currentEvolutionStep){
	double* populationReward = new double[populationSize]; // Массив со всеми наградами популяции
   double rewardSum = 0.0; // Сумма всех наград агентов
	for (int currentAgent = 1; currentAgent <= populationSize; currentAgent++) // Заполняем массив с наградами популяции
		rewardSum += (populationReward[currentAgent - 1] = agents[currentAgent - 1]->getReward());
	TemplateNeuralAgent** newAgents = new TemplateNeuralAgent*[populationSize];

	for (int currentAgent = 1; currentAgent <= populationSize; currentAgent++){ // Генерируем новую популяцию
      int firstParentAgent;
      int secondParentAgent;
      // Определяем номер родительского агента
      if (rewardSum){ // Если есть хоть один агент, который набрал хоть сколько-то награды
         firstParentAgent = rouletteWheelSelection(populationReward);
         int checkCount = 0; // Пытаемся найти другого агента
         do{
            secondParentAgent =  rouletteWheelSelection(populationReward);
         } while ((firstParentAgent == secondParentAgent) && (++checkCount < 10));

         if (firstParentAgent == secondParentAgent) //Если не удается найти другого агента (потому что у всех остальных очень маленькие награды)
				secondParentAgent = service::uniformDiscreteDistribution(1, populationSize);
      } else{ // Если ни один из агентов не набрал никакой награды
			firstParentAgent = service::uniformDiscreteDistribution(1, populationSize);
         do{
				secondParentAgent =  service::uniformDiscreteDistribution(1, populationSize);
         } while (firstParentAgent == secondParentAgent);
      }
      // Пусть более приспособленный родитель всегда будет на первом месте
		if (agents[firstParentAgent - 1]->getReward() > agents[secondParentAgent - 1]->getReward()){
			int tmp = firstParentAgent;
			firstParentAgent = secondParentAgent;
			secondParentAgent = tmp;
		}
		newAgents[currentAgent - 1] = new TemplateNeuralAgent;
		// Создаем нового агента
		generateOffspring(*newAgents[currentAgent - 1], *agents[firstParentAgent - 1], *agents[secondParentAgent - 1], currentEvolutionStep);
		newAgents[currentAgent - 1]->setMoreFitParent(firstParentAgent);
		newAgents[currentAgent - 1]->setLessFitParent(secondParentAgent);
   }

   delete []populationReward;
   // Теперь перепишем новую популяцию в старую (но не физически, а только ссылки, при этом старую удаляем)
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		delete agents[currentAgent - 1];
		agents[currentAgent - 1] = newAgents[currentAgent - 1];  // Копируем только ссылку
   }

   delete []newAgents; // Удаляем только массив ссылок, но не самих агентов
}

#endif // TPOPULATION_H
