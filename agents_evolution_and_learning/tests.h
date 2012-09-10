#ifndef TESTS_H
#define TESTS_H

#include "TEnvironment.h"
#include <string>

class tests{
	// ѕроверка непротиворечивости цели (true - цель противоречива, false - цель непротиворечива)
	bool checkAimInconsistency(TEnvironment::TAim& aim, int environmentResolution);
	// —равнение двух целей среды (true - цели равны, false - цели неравны)
	bool compareTwoAims(TEnvironment::TAim& firstAim, TEnvironment::TAim& secondAim);

public:
	int testPoolNetwork(std::string outputFilename);
	int testNeuralNetwork(std::string outputFilename);
	void testUnifrormDistr();
	void testEnvironment(std::string firstOutputEnvironmentFilename, std::string secondOutputEnvironmentFilename);

	//ѕроцедура тестировани€ прогона нейронной сети (0 - без ошибок, 1 - есть ошибки)
	int testNeuralNetworkProcessing(std::string inputNetworkFilename);
	// “естирование процедуры подсчета заполненности среды (0 - без ошибок, 1 - есть ошибки)
	int tests::testCalculateOccupancyCoeffcient(std::string environmentFilename);
	// “естирование процедуры генерации среды (0 - все хорошо, 1 - есть противоречива€ цель, 2 - есть равные цели)
	int testGenerateEnvironment();
};

#endif // TESTS_H


