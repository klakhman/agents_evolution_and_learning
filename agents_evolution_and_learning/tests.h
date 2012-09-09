#ifndef TESTS_H
#define TESTS_H

#include <string>

namespace tests{

	int testPoolNetwork(std::string outputFilename);

	int testNeuralNetwork(std::string outputFilename);

	int testNeuralNetworkProcessing(std::string inputNetworkFilename);

	void testUnifrormDistr();

	void testEnvironment(std::string firstOutputEnvironmentFilename, std::string secondOutputEnvironmentFilename);
}

#endif // TESTS_H


