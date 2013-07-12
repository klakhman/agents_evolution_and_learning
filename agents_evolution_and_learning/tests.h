#ifndef TESTS_H
#define TESTS_H

#include "THypercubeEnvironment.h"
#include "TPoolNetwork.h"
#include "TAgent.h"
#include <string>
#include <vector>

class tests{
	/*// Проверка непротиворечивости цели (true - цель противоречива, false - цель непротиворечива)
	bool checkAimInconsistency(THypercubeEnvironment::TAim& aim, int environmentResolution);
	// Сравнение двух целей среды (true - цели равны, false - цели неравны)
	bool compareTwoAims(THypercubeEnvironment::TAim& firstAim, THypercubeEnvironment::TAim& secondAim);

public:
	int testPoolNetwork(std::string outputFilename);
	int testNeuralNetwork(std::string outputFilename);
	void testEnvironment(std::string firstOutputEnvironmentFilename, std::string secondOutputEnvironmentFilename);

	//Процедура тестирования прогона нейронной сети (0 - без ошибок, 1 - есть ошибки)
	int testNeuralNetworkProcessing(std::string inputNetworkFilename);
	// Тестирование процедуры подсчета заполненности среды (0 - без ошибок, 1 - есть ошибки)
	int testCalculateOccupancyCoeffcient(std::string environmentFilename);
	// Тестирование процедуры генерации среды (0 - все хорошо, 1 - есть противоречивая цель, 2 - есть равные цели)
	int testGenerateEnvironment();
	// Процедура тестирования степени равномерности распределения, получаемого с помощью слабой функции
	int testWeakUniformDistribution();

	//------------------ Тестирование различных процедур мутации ---------------------------

	// Создание тестовой сети из пулов
	void createTestPoolNetwork(TPoolNetwork& poolNetwork);

	// Тестирование процедуры мутации весов
	void testWeigthsMutation(std::string testDirectory);

	//Тестирование процедуры добавления связи
	void testAddConnection(std::string testDirectory);

	// Тестирование процедуры удаления связи
	void testDeleteConnection(std::string testDirectory);

	// Тестирование процедуры дупликации пула
	void testDuplicatePool(std::string testDirectory);*/
public:
	static void createTestPoolNetwork(TPoolNetwork& poolNetwork);
	static void testPrimarySystemogenesis(std::string testDirectory);
  static void lifeViz(TAgent& agent, std::vector< std::vector<double> >&  inputs, 
                      const std::string& outputDirectory);
  static void testLearning(const std::string& directory);
};

#endif // TESTS_H


