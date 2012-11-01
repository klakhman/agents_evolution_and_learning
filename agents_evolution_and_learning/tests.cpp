#include "tests.h"

#include "TPoolNetwork.h"
#include "TNeuralNetwork.h"
#include "TEnvironment.h"
#include "service.h"
#include "TPopulation.h"

#include <iostream>
#include <fstream>
#include <ctime>
//#include <string>

using namespace std;

int tests::testPoolNetwork(string outputFilename){
	TPoolNetwork PoolNetwork;
	PoolNetwork.addPool(1, 0, 1, 0.2, 0.003, 1);
	PoolNetwork.addPool(2, 2, 1, 0.4, 0.004, 3);
	PoolNetwork.addPool(3, 1, 1, 0.8, 0.1, 2);
	PoolNetwork.addPool(4, 1, 1, 0.5, 0.1, 2);

	PoolNetwork.addConnection(1, 3, 1, 0.1, 0, 1, 0, 0.5, 1);
	PoolNetwork.addConnection(2, 3, 1, 0.1, 0, 1, 0, 0.5, 1);
	PoolNetwork.addConnection(3, 2, 1, 0.1, 0, 1, 0, 0.5, 1);
	PoolNetwork.addPredConnection(2, 3, 1, 1, 0, 0.6, 1);
	PoolNetwork.addPredConnection(1, 3, 1, 1, 0, 0.6, 1);

	cout << PoolNetwork << endl << endl;

	std::ofstream ofs(outputFilename);
	ofs << PoolNetwork;
	ofs.close();

	std::ifstream ifs(outputFilename);
	ifs >> PoolNetwork;
	ifs.close();

	cout << PoolNetwork << endl << endl;

	PoolNetwork.deletePredConnection(3, 1);
	PoolNetwork.deleteConnection(3, 1);

	cout << PoolNetwork << endl << endl;

	PoolNetwork.deletePool(3);

	cout<< PoolNetwork << endl << endl;
	
	cout<< PoolNetwork.getPoolID(3);
	
	return 0;
}

int tests::testNeuralNetwork(string outputFilename){
	TNeuralNetwork NeuralNetwork;
	NeuralNetwork.addNeuron(1, 0, 1, 1);
	NeuralNetwork.addNeuron(2, 2, 1, 3);
	NeuralNetwork.addNeuron(3, 1, 1, 2);
	NeuralNetwork.addNeuron(4, 1, 1, 2);

	NeuralNetwork.addSynapse(1, 3, 1, 0.1, 1);
	NeuralNetwork.addSynapse(2, 3, 1, 0.1, 1);
	NeuralNetwork.addSynapse(3, 2, 1, 0.1, 1);
	NeuralNetwork.addPredConnection(2, 3, 1, 1);
	NeuralNetwork.addPredConnection(1, 3, 1, 1);

	cout << NeuralNetwork << endl << endl;

	std::ofstream ofs(outputFilename);
	ofs << NeuralNetwork;
	ofs.close();

	std::ifstream ifs(outputFilename);
	ifs >> NeuralNetwork;
	ifs.close();

	cout << NeuralNetwork << endl << endl;

	NeuralNetwork.deletePredConnection(3, 1);
	NeuralNetwork.deleteSynapse(3, 1);

	cout << NeuralNetwork << endl << endl;

	NeuralNetwork.deleteNeuron(3);

	cout<< NeuralNetwork << endl << endl;
	
	cout<< NeuralNetwork.getNeuronID(3);
	
	return 0;
}

void tests::testEnvironment(string firstOutputEnvironmentFilename, string secondOutputEnvironmentFilename){
	ofstream ofs(firstOutputEnvironmentFilename);
	ofs << 8 << "\t" << 5 << endl; // Размерность и кол-во целей
	// Записываем цели
	ofs << 2 << "\t" << 20 << endl << 1 << "\t" << 1 << "\t" << 5 << "\t" << 0 << endl;
	ofs << 3 << "\t" << 30 << endl << 1 << "\t" << 1 << "\t" << 5 << "\t" << 0 << "\t" << 2 << "\t" << 1 << endl;
	ofs << 4 << "\t" << 40 << endl << 1 << "\t" << 1 << "\t" << 5 << "\t" << 0 << "\t" << 2 << "\t" << 1 << "\t" << 1 << "\t" << 0 << endl;
	ofs << 2 << "\t" << 20 << endl << 4 << "\t" << 0 << "\t" << 8 << "\t" << 1 << endl;
	ofs << 2 << "\t" << 20 << endl << 8 << "\t" << 1 << "\t" << 2 << "\t" << 0 << endl;
	ofs.close();
	
	TEnvironment environment(firstOutputEnvironmentFilename, 30);
	environment.uploadAims(secondOutputEnvironmentFilename);
	double life[6];
	life[0] = 1;
	life[1] = -5;
	life[2] = 2;
	life[3] = 0;
	life[4] = 1;
	life[5] = -5;
	cout << environment.calculateReward(life, 6) << endl;
	double _life[4];
	_life[0] = 0;
	_life[1] = -4;
	_life[2] = 8;
	_life[3] = -2;
	cout << environment.calculateReward(_life, 6) << endl;
}

//Процедура тестирования прогона нейронной сети (0 - без ошибок, 1 - есть ошибки)
int tests::testNeuralNetworkProcessing(string inputNetworkFilename){
	int checkStatus = 0;
	cout << endl << "------------------+ TEST FOR NEURAL NETWORK PROCESSING +------------------" << endl;

	TNeuralNetwork NeuralNetwork;
	ifstream ifs(inputNetworkFilename);
	ifs >> NeuralNetwork;
	ifs.close();

	double* inputVector = new double[NeuralNetwork.getInputResolution()];
	double* outputVector = new double[NeuralNetwork.getOutputResolution()];

	NeuralNetwork.reset();
	
	inputVector[0] = 1;
	inputVector[1] = 0;
	inputVector[2] = 1;
	NeuralNetwork.calculateNetwork(inputVector);
	NeuralNetwork.getOutputVector(outputVector);
	cout << "Output vector : [" << outputVector[0] << "; " << outputVector[1] << "], should be [0.343277; 0.610570]" << endl;

	inputVector[0] = 0;
	inputVector[1] = 1;
	inputVector[2] = 1;
	NeuralNetwork.calculateNetwork(inputVector);
	NeuralNetwork.getOutputVector(outputVector);
	cout << "Output vector : [" << outputVector[0] << "; " << outputVector[1] << "], should be [0.406367; 0.702089]" << endl;

	delete []inputVector;
	delete []outputVector;
	
	cout << "---------------------------------------------------------------------------" << endl;
	return checkStatus;
}

// Тестирование процедуры подсчета заполненности среды (0 - без ошибок, 1 - есть ошибки)
int tests::testCalculateOccupancyCoeffcient(string environmentFilename){
	int checkStatus = 0;
	cout << endl << "--------------------+ TEST FOR ENVIRONMENT'S OCCUPANCY +--------------------" << endl;
	double environmentOccupancy = 0.1204;
	TEnvironment*  environment = new TEnvironment(environmentFilename);

	if (environment->calculateOccupancyCoefficient() != environmentOccupancy)
		checkStatus = 1;
	cout << "Calculated environment's occupancy is " << environment->calculateOccupancyCoefficient() << ", should be " << environmentOccupancy << endl;   

	delete environment;
	cout << "---------------------------------------------------------------------------" << endl;
	return checkStatus;
}

// Проверка непротиворечивости цели (true - цель противоречива, false - цель непротиворечива)
bool tests::checkAimInconsistency(TEnvironment::TAim& aim, int environmentResolution){
	// Массив последних действий в отношении битов среды (-1 - действия еще не было, 0 - перевод в ноль, 1 - перевод в единицу)
	int* lastAction = new int[environmentResolution];
	for (int currentBit = 1; currentBit <= environmentResolution; ++currentBit)
		lastAction[currentBit - 1] = -1;

	for (int currentAction = 1; currentAction <= aim.aimComplexity; ++currentAction)
		// Если еще не совершали действия над этим битом
		if (lastAction[aim.actionsSequence[currentAction - 1].bitNumber - 1] == -1)
			lastAction[aim.actionsSequence[currentAction - 1].bitNumber - 1] = static_cast<int>(aim.actionsSequence[currentAction - 1].desiredValue);
		else
			if ((lastAction[aim.actionsSequence[currentAction - 1].bitNumber - 1] == 1) == aim.actionsSequence[currentAction - 1].desiredValue)
				return true;
			else
				lastAction[aim.actionsSequence[currentAction - 1].bitNumber - 1] = static_cast<int>(aim.actionsSequence[currentAction - 1].desiredValue);
	
	delete []lastAction;
	return false;
}

// Сравнение двух целей среды (true - цели равны, false - цели неравны)
bool tests::compareTwoAims(TEnvironment::TAim& firstAim, TEnvironment::TAim& secondAim){
	if (firstAim.aimComplexity != secondAim.aimComplexity)
		return false;
	else 
		for (int currentAction = 1; currentAction <= firstAim.aimComplexity; ++currentAction)
			if ((firstAim.actionsSequence[currentAction - 1].bitNumber != secondAim.actionsSequence[currentAction - 1].bitNumber) ||
					(firstAim.actionsSequence[currentAction - 1].desiredValue != secondAim.actionsSequence[currentAction - 1].desiredValue)) 
				return false;
	return true;
}

// Тестирование процедуры генерации среды (0 - все хорошо, 1 - есть противоречивая цель, 2 - есть равные цели)
int tests::testGenerateEnvironment(){
	int checkStatus = 0;

	int environmentResolution = 8;
	double occupancySet[] = {0.04, 0.14, 0.4, 0.7};
	TEnvironment* environment = new TEnvironment;
	cout << endl << "--------------------+ TEST FOR ENVIRONMENT'S GENERATING +--------------------" << endl;
	for (int currentOccupancy = 1; currentOccupancy <= sizeof(occupancySet)/sizeof(*occupancySet); ++currentOccupancy){
		environment->generateEnvironment(environmentResolution, occupancySet[currentOccupancy - 1]);
		cout << "Done! Occupancy of the environment : " << environment->calculateOccupancyCoefficient() << 
										", desired occupancy: " << occupancySet[currentOccupancy - 1] << endl;
		// Проверяем непротиворечивость структуры целей
		for (int currentAim = 1; currentAim <= environment->getAimsQuantity(); ++currentAim){
			if (static_cast<int>(checkAimInconsistency(environment->aimsSet[currentAim - 1], environment->getEnvironmentResolution()))){
				cout << "Aim #" << currentAim << "is inconsistent." << endl;
				for (int currentAction = 1; currentAction <= environment->aimsSet[currentAim - 1].aimComplexity; ++currentAction)
					cout << environment->aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber << "->" 
									<< environment->aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue << "\t";
				cout << endl;
				checkStatus = 1;
			}
			// Сравниваем со всеми остальными целями
			for (int currentCompareAim = 1; currentCompareAim <= environment->getAimsQuantity(); ++currentCompareAim)
				if (currentAim != currentCompareAim)
					if (compareTwoAims(environment->aimsSet[currentAim - 1], environment->aimsSet[currentCompareAim-1])){
						cout << "Aims #" << currentAim << "and #" << currentCompareAim << " is equal." << endl;
						checkStatus = 2;
						for (int currentAction = 1; currentAction <= environment->aimsSet[currentAim - 1].aimComplexity; ++currentAction)
							cout << environment->aimsSet[currentAim - 1].actionsSequence[currentAction - 1].bitNumber << "->" 
									<< environment->aimsSet[currentAim - 1].actionsSequence[currentAction - 1].desiredValue << "\t";
						cout << endl;
						for (int currentAction = 1; currentAction <= environment->aimsSet[currentCompareAim - 1].aimComplexity; ++currentAction)
							cout << environment->aimsSet[currentCompareAim - 1].actionsSequence[currentAction - 1].bitNumber << "->" 
									<< environment->aimsSet[currentCompareAim - 1].actionsSequence[currentAction - 1].desiredValue << "\t";
						cout << endl;
					}
		}
	}
	delete environment;
	cout << "---------------------------------------------------------------------------" << endl;
	return checkStatus;
}

// Процедура тестирования степени равномерности распределения, получаемого с помощью слабой функции
int tests::testWeakUniformDistribution(){
	int checkStatus = 0;
	cout << endl << "--------------------+ TEST FOR WEAK UNIFORM DISTRIBUTION +--------------------" << endl;
	int valuesQuantity = 1000000000;
	double zeroValues = 0;
	double oneValues = 0;
	double nearZeroValues = 0;
	double nearOneValues = 0;
	for (int currentValue = 1; currentValue <= valuesQuantity; ++currentValue){
		double randNumber = service::uniformDistributionWeak(0, 1);
		if (randNumber == 0) zeroValues += 1;
		if (randNumber == 1) oneValues += 1;
		if ((randNumber > 0) && (randNumber <= 0.1)) nearZeroValues += 1;
		if ((randNumber >= 0.9) && (randNumber < 1)) nearOneValues += 1;
	}
	cout << "Test #1" << endl;
	cout << "Zero value " << zeroValues << " should be equal to one value " << oneValues << endl;
	cout << "Near zero value probability " << nearZeroValues / valuesQuantity << " = near one value probability " << nearOneValues / valuesQuantity << endl;
	oneValues = 0;
	zeroValues = 0;
	for (int currentValue = 1; currentValue <= valuesQuantity/100; ++currentValue){
		double randNumber = service::uniformDistributionWeak(0, 1, false);
		if (randNumber == 0) zeroValues += 1;
		if (randNumber == 1) oneValues += 1;
	}
	cout << "Test #2" << endl;
	cout << "Zero value " << zeroValues << " = 0, but one value " << oneValues << " != 0" << endl;
	oneValues = 0;
	zeroValues = 0;
	for (int currentValue = 1; currentValue <= valuesQuantity/100; ++currentValue){
		double randNumber = service::uniformDistributionWeak(0, 1, true, false);
		if (randNumber == 0) zeroValues += 1;
		if (randNumber == 1) oneValues += 1;
	}
	cout << "Test #3" << endl;
	cout << "Zero value " << zeroValues << " != 0, but one value " << oneValues << " = 0" << endl;
	oneValues = 0;
	zeroValues = 0;
	for (int currentValue = 1; currentValue <= valuesQuantity/100; ++currentValue){
		double randNumber = service::uniformDistributionWeak(0, 1, false, false);
		if (randNumber == 0) zeroValues += 1;
		if (randNumber == 1) oneValues += 1;
	}
	cout << "Test #4" << endl;
	cout << "Zero value " << zeroValues << " = 0, and one value " << oneValues << " = 0" << endl;
	cout << "---------------------------------------------------------------------------" << endl;

	return checkStatus;
}

//------------------ Тестирование различных процедур мутации ---------------------------

// Создание тестовой сети из пулов
void tests::createTestPoolNetwork(TPoolNetwork& poolNetwork){
	poolNetwork.erasePoolNetwork();
	// Входные пулы
	poolNetwork.addPool(1, 0, 1, -0.5, 0.1, 1);
	poolNetwork.addPool(2, 0, 1, 0.4, 0.1, 1);
	poolNetwork.addPool(3, 0, 1, 0.1, 0.1, 1);
	// Выходные пулы
	poolNetwork.addPool(4, 2, 1, 0.5, 0.1, 3);
	poolNetwork.addPool(5, 2, 1, 0.6, 0.1, 3);
	// Интер-пулы
	poolNetwork.addPool(6, 1, 10, -0.5, 0.1, 2);
	poolNetwork.addPool(7, 1, 1, 0.3, 0.1, 2);

	// Добавляем связи
	poolNetwork.addConnection(1, 6, 1, 0.4, 0.1, 1, 0, 1, 1);
	poolNetwork.addConnection(1, 7, 2, 0.3, 0.1, 1, 0, 1, 2);
	poolNetwork.addConnection(2, 6, 3, 0.21, 0.1, 1, 0, 1, 3);
	poolNetwork.addConnection(3, 7, 4, 0.39, 0.1, 1, 0, 1, 4);

	poolNetwork.addConnection(6, 6, 5, 0.6, 0.1, 1, 0, 1, 5);
	poolNetwork.addConnection(7, 6, 6, 0.43, 0.1, 1, 0, 1, 6);

	poolNetwork.addConnection(6, 5, 7, 0.70, 0.1, 1, 0, 1, 7);
	poolNetwork.addConnection(7, 4, 8, 0.50, 0.1, 1, 0, 1, 8);
	poolNetwork.addConnection(5, 7, 9, 0.33, 0.1, 1, 0, 1, 9);

	// Добавляем предикторные связи
	poolNetwork.addPredConnection(6, 7, 1, 1, 0, 1, 1);
	poolNetwork.addPredConnection(4, 6, 2, 1, 0, 1, 2);
}

// Тестирование процедуры мутации весов
void tests::testWeigthsMutation(string testDirectory){
	TPoolNetwork* testNetwork = new TPoolNetwork;
	createTestPoolNetwork(*testNetwork);
	cout << *testNetwork << endl;

	TPopulation* testPopulation = new TPopulation;
	testPopulation->setPopulationSize(1);
	*(testPopulation->getPointertoAgent(1)->getPointerToAgentGenome()) = *testNetwork; 
	testPopulation->mutationSettings.mutWeightProbability = 0.5;
	testPopulation->mutationSettings.mutWeightMeanDisp = 0.08;
	testPopulation->mutationSettings.mutWeightDispDisp = 0.005;
	testPopulation->mutationConnectionsWeight(*(testPopulation->getPointertoAgent(1)));

	TPoolNetwork* mutatedNetwork = (testPopulation->getPointertoAgent(1)->getPointerToAgentGenome());

	cout << *(testPopulation->getPointertoAgent(1)->getPointerToAgentGenome()) << endl;
	cout << "----- Statistics ----" << endl << "Standart deviation: " << testPopulation->mutationSettings.mutWeightMeanDisp << endl;
	cout << "Real non-zero values: ";
	int zero_values = 0;
	for (int currentPool = 1; currentPool <= testNetwork->getPoolsQuantity(); ++currentPool){
		if (testNetwork->getPoolBiasMean(currentPool) != mutatedNetwork->getPoolBiasMean(currentPool))
			cout << mutatedNetwork->getPoolBiasMean(currentPool) - testNetwork->getPoolBiasMean(currentPool) << "\t";
		else 
			++zero_values;
		for (int currentConnection = 1; currentConnection <= testNetwork->getPoolInputConnectionsQuantity(currentPool); ++currentConnection)
			if (testNetwork->getConnectionWeightMean(currentPool, currentConnection) != mutatedNetwork->getConnectionWeightMean(currentPool, currentConnection))
				cout << mutatedNetwork->getConnectionWeightMean(currentPool, currentConnection) - testNetwork->getConnectionWeightMean(currentPool, currentConnection) << "\t";
			else 
				++zero_values;
	}
	cout << endl << endl << "Probability: " << testPopulation->mutationSettings.mutWeightProbability << "\t Real number of zero-values: " << zero_values << ", all connections: " << testNetwork->getPoolsQuantity() + testNetwork->getConnectionsQuantity();

	delete testPopulation;
	delete testNetwork;
}

//Тестирование процедуры добавления связи
void tests::testAddConnection(string testDirectory){
	TPoolNetwork* testNetwork = new TPoolNetwork;
	createTestPoolNetwork(*testNetwork);
	cout << *testNetwork << endl;

	TPopulation* testPopulation = new TPopulation;
	testPopulation->setPopulationSize(1);
	*(testPopulation->getPointertoAgent(1)->getPointerToAgentGenome()) = *testNetwork; 
	testPopulation->mutationSettings.addConnectionProb = 1;
	testPopulation->connectionInnovationNumber = testNetwork->getConnectionsQuantity();

	testPopulation->mutationAddPoolConnection(*(testPopulation->getPointertoAgent(1)));
	TPoolNetwork* mutatedNetwork = (testPopulation->getPointertoAgent(1)->getPointerToAgentGenome());
	cout << *mutatedNetwork << endl << endl;
	cout << "Previous connections quantity = " << testNetwork->getConnectionsQuantity() << " Current = " << mutatedNetwork->getConnectionsQuantity() << endl;
	delete testPopulation;
	delete testNetwork;
}

// Тестирование процедуры удаления связи
void tests::testDeleteConnection(string testDirectory){
	TPoolNetwork* testNetwork = new TPoolNetwork;
	createTestPoolNetwork(*testNetwork);
	cout << *testNetwork << endl;

	TPopulation* testPopulation = new TPopulation;
	testPopulation->setPopulationSize(1);
	*(testPopulation->getPointertoAgent(1)->getPointerToAgentGenome()) = *testNetwork; 
	testPopulation->mutationSettings.deleteConnectionProb = 1;
	testPopulation->connectionInnovationNumber = testNetwork->getConnectionsQuantity();

	testPopulation->mutationDeletePoolConnection(*(testPopulation->getPointertoAgent(1)));
	TPoolNetwork* mutatedNetwork = (testPopulation->getPointertoAgent(1)->getPointerToAgentGenome());
	cout << *mutatedNetwork << endl << endl;
	cout << "Previous connections quantity = " << testNetwork->getConnectionsQuantity() << " Current = " << mutatedNetwork->getConnectionsQuantity() << endl;
	delete testPopulation;
	delete testNetwork;
}

// Тестирование процедуры дупликации пула
void tests::testDuplicatePool(string testDirectory){
	TPoolNetwork* testNetwork = new TPoolNetwork;
	createTestPoolNetwork(*testNetwork);
	cout << *testNetwork << endl;
	//testNetwork->printGraphNetwork(testDirectory + "/test_pool_net.jpg");
	TPopulation* testPopulation = new TPopulation;
	testPopulation->setPopulationSize(1);
	*(testPopulation->getPointertoAgent(1)->getPointerToAgentGenome()) = *testNetwork; 
	testPopulation->mutationSettings.duplicatePoolProb = 1;
	testPopulation->mutationSettings.connectionStandartAmount = 9;
	testPopulation->mutationSettings.poolStandartAmount = 7;
	testPopulation->mutationSettings.poolDivisionCoef = 2.0 / 3.0;
	testPopulation->connectionInnovationNumber = testNetwork->getConnectionsQuantity();
	testPopulation->predConnectionInnovationNumber = testNetwork->getPredConnectionsQuantity();

	testPopulation->mutationPoolDuplication(*(testPopulation->getPointertoAgent(1)));
	TPoolNetwork* mutatedNetwork = (testPopulation->getPointertoAgent(1)->getPointerToAgentGenome());
	cout << *mutatedNetwork << endl << endl;
	mutatedNetwork->printGraphNetwork(testDirectory + "/mutated_pool_net.jpg");
	cout << "Previous connections quantity = " << testNetwork->getConnectionsQuantity() << " Current = " << mutatedNetwork->getConnectionsQuantity() << endl;

	delete testPopulation;
	delete testNetwork;
}



