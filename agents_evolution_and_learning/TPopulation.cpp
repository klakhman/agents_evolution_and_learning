#include "TPopulation.h"
#include <fstream>
#include "service.h"

using namespace std;

	// Загрузка популяции геномов из файла (если размер популяции не передается, то значение берется из текущих параметров популяции)
void TPopulation::loadPopulation(string populationFilename, int _populationSize /*=0*/){
	//!!! Возможно надо как-то по другому обходится с номерами инноваций
	connectionInnovationNumber = 0;
	predConnectionInnovationNumber = 0;
	if (_populationSize) setPopulationSize(_populationSize);
	ifstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <=populationSize; ++currentAgent){
		agents[currentAgent - 1]->loadGenome(populationFile);
	}
	populationFile.close();
}

// Выгрузка популяции геномов в файл
void TPopulation::uploadPopulation(string populationFilename) const{
	ofstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->uploadGenome(populationFile);
	populationFile.close();
}

// Генерация минимальной популяции
void TPopulation::generateMinimalPopulation(int inputResolution, int _populationSize /*= 0*/){
	if (_populationSize) populationSize = _populationSize;
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->generateMinimalAgent(inputResolution);
	if (populationSize){
		connectionInnovationNumber = agents[0]->getPointerToAgentGenome()->getConnectionsQuantity();
		predConnectionInnovationNumber = agents[0]->getPointerToAgentGenome()->getPredConnectionsQuantity();
	}
}
// Шаг эволюционного процесса
void TPopulation::evolutionaryStep(TEnvironment& environment, int evolutionStepNumber){
	// Сначала мутируем популяцию (чтобы можно потом было отчеты составлять и всегда текущая популяция уже была прогнана на среде)
	if (evolutionStepNumber != 1) generateNextPopulation(evolutionStepNumber);
	// Прогоняем всех агентов
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		// Проводим первичный системогенез
		agents[currentAgent - 1]->linearSystemogenesis();
		environment.setRandomEnvironmentVector();
		agents[currentAgent - 1]->life(environment, evolutionSettings.agentLifetime);
	}
}

// Процедура эволюции популяции
void TPopulation::evolution(TEnvironment& environment){
	for (int evolutionStep = 1; evolutionStep <= evolutionSettings.evolutionTime; ++evolutionStep){
		evolutionaryStep(environment, evolutionStep);
	}
}


// ---------------  Различные процедуры мутации -------------------------
// Процедура мутации - мутация весовых коэффициентов связи
void  TPopulation::mutationConnectionsWeight(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool){
		// Сначала мутируем смещение
		if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutWeightProbability){
			kidGenome->setPoolBiasMean(currentPool, kidGenome->getPoolBiasMean(currentPool) +
																service::uniformDistribution(-mutationSettings.mutWeightMeanDisp, mutationSettings.mutWeightMeanDisp));
			// Важно, чтобы дисперсия была не меньше нуля
			kidGenome->setPoolBiasVariance(currentPool, abs(kidGenome->getPoolBiasVariance(currentPool) + 
															service::uniformDistribution(-mutationSettings.mutWeightDispDisp, mutationSettings.mutWeightDispDisp)));
		}
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			 // Если мутация имеет место быть и связь включена
			if ((service::uniformDistribution(0, 1, true, false) < mutationSettings.mutWeightProbability) && (kidGenome->getConnectionEnabled(currentPool, currentPoolConnection))) {
				kidGenome->setConnectionWeightMean(currentPool, currentPoolConnection, kidGenome->getConnectionWeightMean(currentPool, currentPoolConnection) +
																service::uniformDistribution(-mutationSettings.mutWeightMeanDisp, mutationSettings.mutWeightMeanDisp));
				// Важно, чтобы дисперсия была не меньше нуля
				kidGenome->setConnectionWeightVariance(currentPool, currentPoolConnection, abs(kidGenome->getConnectionWeightVariance(currentPool, currentPoolConnection) + 
																service::uniformDistribution(-mutationSettings.mutWeightDispDisp, mutationSettings.mutWeightDispDisp)));
			}
	}
}

// Процедура мутации - включения/выключения связей
void TPopulation::mutationEnableDisableConnections(TAgent& kidAgent, int currentEvolutionStep){
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
void TPopulation::mutationEnableDisablePredConnections(TAgent& kidAgent, int currentEvolutionStep){
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
void TPopulation::mutationDeleteConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			// Если эта связь была выключена и ее уже надо удалить из генома
			if (( !kidGenome->getConnectionEnabled(currentPool, currentPoolConnection)) &&
								(currentEvolutionStep - kidGenome->getConnectionDisabledStep(currentPool, currentPoolConnection) >= mutationSettings.disLimit)){
				kidGenome->deleteConnection(currentPool, currentPoolConnection);
				--currentPoolConnection; // Чтобы указатель остался на том же номере связи
			}
	kidGenome->fixConnectionsIDs();
}

// Процедура мутации - удаления из агента предикторных связей, которые выключены более некоторого количества поколений
void TPopulation::mutationDeletePredConnectionPopulation(TAgent& kidAgent, int currentEvolutionStep){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			// Если эта предикторная связь была выключена и ее уже надо удалить из генома
			if (( !kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection)) &&
								(currentEvolutionStep - kidGenome->getPredConnectionDisabledStep(currentPool, currentPoolPredConnection) >= mutationSettings.disLimit)){
				kidGenome->deletePredConnection(currentPool, currentPoolPredConnection);
				--currentPoolPredConnection; // Чтобы указатель остался на том же номере предикторной связи
			}
	kidGenome->fixPredConnectionsIDs();
}

// Процедура мутации - добавление связи
void TPopulation::mutationAddPoolConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.addConnectionProb){ // Если имеет место мутация
		int prePoolID, postPoolID; // Пресинаптический и постсинаптический пул потенциальной связи
		int tryCount = 0; //Количество ложных генераций связи - введено, чтобы не было зацикливания
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
		}while ((kidGenome->checkConnectionExistance(prePoolID, postPoolID)) && (tryCount <= 1000));
		// Если отсутствующая связь найдена
		if (! kidGenome->checkConnectionExistance(prePoolID, postPoolID)){
			kidGenome->addConnection(prePoolID, postPoolID, kidGenome->getConnectionsQuantity() + 1, service::uniformDistribution(-0.5, 0.5), 0, true, 0, 
												kidAgent.primarySystemogenesisSettings.initialDevelopSynapseProbability, ++connectionInnovationNumber);
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
void TPopulation::mutationAddPoolPredConnection(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	if (service::uniformDistribution(0, 1, true, false) < mutationSettings.addPredConnectionProb){ // Если имеет место мутация
		int prePoolID, postPoolID; // Пресинаптический и постсинаптический пул потенциальной предикторной связи
		int tryCount = 0; //Количество ложных генераций предикторной связи - введено, чтобы не было зацикливания
		do{ // Цикл нахождения отсутствующей предикторной связи
			prePoolID = service::uniformDiscreteDistribution(1, kidGenome->getPoolsQuantity());
			// Постсинаптический пул точно не может быть входным или выходным
			postPoolID = service::uniformDiscreteDistribution(kidGenome->getInputResolution() + kidGenome->getOutputResolution() + 1, kidGenome->getPoolsQuantity());
			++tryCount;
		// Пока не найдем отсутствующую предикторную связь или превысим допустимое кол-во попыток
		}while ((kidGenome->checkPredConnectionExistance(prePoolID, postPoolID)) && (tryCount <= 1000));
		// Если отсутствующая предикторная связь найдена
		if (! kidGenome->checkPredConnectionExistance(prePoolID, postPoolID)){
			kidGenome->addPredConnection(prePoolID, postPoolID, kidGenome->getPredConnectionsQuantity() + 1, true, 0, 
													kidAgent.primarySystemogenesisSettings.initialDevelopPredConnectionProbability, ++predConnectionInnovationNumber); 
			kidGenome->fixPredConnectionsIDs();
		}
	}
}

// Процедура мутации - удаление связи
void TPopulation::mutationDeletePoolConnection(TAgent& kidAgent){
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
		kidGenome->fixConnectionsIDs();
	}
}

// Процедура мутации - удаление предикторной связи
void TPopulation::mutationDeletePoolPredConnection(TAgent& kidAgent){
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
		kidGenome->fixPredConnectionsIDs();
	}
}

// Коэффициент уменьшения вероятности дупликации пула с ростом генома
double TPopulation::duplicateDivision(int poolsQuantity, int connectionsQuantity){
	return max(1.0, 0.5 * connectionsQuantity/static_cast<double>(mutationSettings.connectionStandartAmount) 
																							+ 0.5 * (poolsQuantity - mutationSettings.poolStandartAmount));
}

// Процедура мутации - дупликация пула
void TPopulation::mutationPoolDuplication(TAgent& kidAgent){
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
				kidGenome->addPool(kidGenome->getPoolsQuantity() + 1, kidGenome->getPoolType(currentPool), kidGenome->getPoolCapacity(currentPool),
											kidGenome->getPoolBiasMean(currentPool), kidGenome->getPoolBiasVariance(currentPool), kidGenome->getPoolLayer(currentPool));
				// Копируем все входящие связи из дуплицирующего пула в новый
				for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
					// Если это старая связь - до процедуры мутации
					if (kidGenome->getConnectionID(currentPool, currentPoolConnection) <= initConnectionsQuantity){
						int prePoolNumber;
						// Если это не связь на самого себя
						if (kidGenome->getConnectionPrePool(currentPool, currentPoolConnection)->getID() != kidGenome->getConnectionPostPool(currentPool, currentPoolConnection)->getID())
							prePoolNumber = kidGenome->getConnectionPrePool(currentPool, currentPoolConnection)->getID();
						else 
							prePoolNumber = kidGenome->getPoolsQuantity();
						kidGenome->addConnection(prePoolNumber, kidGenome->getPoolsQuantity(), kidGenome->getConnectionsQuantity() + 1, kidGenome->getConnectionWeightMean(currentPool, currentPoolConnection),
														kidGenome->getConnectionWeightVariance(currentPool, currentPoolConnection), kidGenome->getConnectionEnabled(currentPool, currentPoolConnection),
														kidGenome->getConnectionDisabledStep(currentPool, currentPoolConnection), kidGenome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection),
														++connectionInnovationNumber);
					}
            // Копируем все выходящие связи из дуплицирующего пула (для этого надо пройтись по всей сети, так как эти связи разбросаны)
				for (int currentPostPool = 1; currentPostPool <= initPoolsQuantity; ++currentPostPool)
					if (currentPostPool != currentPool) // Если это не дуплицирующий пул
						for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPostPool); ++currentPoolConnection)
							if (kidGenome->getConnectionID(currentPostPool, currentPoolConnection) <= initConnectionsQuantity) // Если это старая связь
								// Если у текущего потенциального пула есть связь с текущим дублицирующим пулом
								if (kidGenome->getConnectionPrePool(currentPostPool, currentPoolConnection)->getID() == currentPool){
									kidGenome->addConnection(kidGenome->getPoolsQuantity(), currentPostPool, kidGenome->getConnectionsQuantity() + 1, kidGenome->getConnectionWeightMean(currentPostPool, currentPoolConnection) / 2.0,
														kidGenome->getConnectionWeightVariance(currentPostPool, currentPoolConnection), kidGenome->getConnectionEnabled(currentPostPool, currentPoolConnection),
														kidGenome->getConnectionDisabledStep(currentPostPool, currentPoolConnection), kidGenome->getConnectionDevelopSynapseProb(currentPostPool, currentPoolConnection),
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
						if (kidGenome->getPredConnectionPrePool(currentPool, currentPoolPredConnection)->getID() != kidGenome->getPredConnectionPostPool(currentPool, currentPoolPredConnection)->getID())
							prePoolNumber = kidGenome->getPredConnectionPrePool(currentPool, currentPoolPredConnection)->getID();
						else 
							prePoolNumber = kidGenome->getPoolsQuantity();
						kidGenome->addPredConnection(prePoolNumber, kidGenome->getPoolsQuantity(), kidGenome->getPredConnectionsQuantity() + 1, kidGenome->getPredConnectionEnabled(currentPool, currentPoolPredConnection),
														kidGenome->getPredConnectionDisabledStep(currentPool, currentPoolPredConnection), kidGenome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection),
														++predConnectionInnovationNumber);
					}
            // Копируем все выходящие предикторные связи из дуплицирующего пула (для этого надо пройтись по всей сети, так как эти связи разбросаны)
				for (int currentPostPool = 1; currentPostPool <= initPoolsQuantity; ++currentPostPool)
					if (currentPostPool != currentPool) // Если это не дуплицирующий пул
						for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPostPool); ++currentPoolPredConnection)
							if (kidGenome->getPredConnectionID(currentPostPool, currentPoolPredConnection) <= initPredConnectionsQuantity) // Если это старая связь
								// Если у текущего потенциального пула есть предикторная связь с текущим дублицирующим пулом
								if (kidGenome->getPredConnectionPrePool(currentPostPool, currentPoolPredConnection)->getID() == currentPool)
									kidGenome->addPredConnection(kidGenome->getPoolsQuantity(), currentPostPool, kidGenome->getPredConnectionsQuantity() + 1, kidGenome->getPredConnectionEnabled(currentPostPool, currentPoolPredConnection),
														kidGenome->getPredConnectionDisabledStep(currentPostPool, currentPoolPredConnection), kidGenome->getDevelopPredConnectionProb(currentPostPool, currentPoolPredConnection),
														++predConnectionInnovationNumber);
			} // duplicate
		}
	kidGenome->fixConnectionsIDs();
	kidGenome->fixPredConnectionsIDs();
}

// Процедура мутации вероятности развития синапса по связи между пулами
void TPopulation::mutationDevelopSynapseProb(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolConnection = 1; currentPoolConnection <= kidGenome->getPoolInputConnectionsQuantity(currentPool); ++currentPoolConnection)
			if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutDevelopConProbProb)
				// Важно, чтобы вероятность была между нулем и единицей
					kidGenome->setConnectionDevelopSynapseProb(currentPool, currentPoolConnection, 
						min(1.0, max(0.0, kidGenome->getConnectionDevelopSynapseProb(currentPool, currentPoolConnection) + service::uniformDistribution(-mutationSettings.mutDevelopConProbDisp, mutationSettings.mutDevelopConProbDisp))));
}

// Процедура мутации вероятности развития предикторной связи по предикторной связи между пулами
void TPopulation::mutationDevelopPredConProb(TAgent& kidAgent){
	TPoolNetwork* kidGenome = kidAgent.getPointerToAgentGenome();
	for (int currentPool = 1; currentPool <= kidGenome->getPoolsQuantity(); ++currentPool)
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= kidGenome->getPoolInputPredConnectionsQuantity(currentPool); ++currentPoolPredConnection)
			if (service::uniformDistribution(0, 1, true, false) < mutationSettings.mutDevelopConProbProb)
				// Важно, чтобы вероятность была между нулем и единицей
					kidGenome->setDevelopPredConnectionProb(currentPool, currentPoolPredConnection, 
						min(1.0, max(0.0, kidGenome->getDevelopPredConnectionProb(currentPool, currentPoolPredConnection) + service::uniformDistribution(-mutationSettings.mutDevelopConProbDisp, mutationSettings.mutDevelopConProbDisp))));
}

// Процедура составления потомка от двух родителей
void TPopulation::сomposeOffspringFromParents(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent){
	if (firstParentAgent.getReward() >= secondParentAgent.getReward())
		kidAgent = firstParentAgent;
	else 
		kidAgent = secondParentAgent;
}

// Процедура генерации одного потомка
void TPopulation::generateOffspring(TAgent& kidAgent, const TAgent& firstParentAgent, const TAgent& secondParentAgent, int currentEvolutionStep){
	
	сomposeOffspringFromParents(kidAgent, firstParentAgent, secondParentAgent);
	
	//mutationDeleteConnectionPopulation(kidAgent, currentEvolutionStep);
	//mutationDeletePredConnectionPopulation(kidAgent, currentEvolutionStep);
	//mutationEnableDisableConnections(kidAgent, currentEvolutionStep);
	//mutationEnableDisablePredConnections(kidAgent, currentEvolutionStep);

	mutationPoolDuplication(kidAgent);
	mutationAddPoolConnection(kidAgent);
	mutationAddPoolPredConnection(kidAgent);
	mutationDeletePoolConnection(kidAgent);
	mutationDeletePoolPredConnection(kidAgent);
	mutationConnectionsWeight(kidAgent);
	mutationDevelopSynapseProb(kidAgent);
	mutationDevelopPredConProb(kidAgent);
}

//Процедура получения номера агента, используемая в рулеточном алгоритме
int TPopulation::rouletteWheelSelection(double populationFitness[]){
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
void TPopulation::generateNextPopulation(int currentEvolutionStep){
	double* populationReward = new double[populationSize]; // Массив со всеми наградами популяции
   double rewardSum = 0.0; // Сумма всех наград агентов
	for (int currentAgent = 1; currentAgent <= populationSize; currentAgent++) // Заполняем массив с наградами популяции
		rewardSum += (populationReward[currentAgent - 1] = agents[currentAgent - 1]->getReward());
	TAgent** newAgents = new TAgent*[populationSize];

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
		newAgents[currentAgent - 1] = new TAgent;
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

