#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

#include "tests.h"

using namespace std;

int main(int argc, char** argv){

	long startTime = time(0);
	srand(static_cast<unsigned int>(time(0)));
	
	/*tests test;
	test.testNeuralNetworkProcessing("C:/Tests/test_net.txt");
	test.testCalculateOccupancyCoeffcient("C:/Tests/test_environment.txt");
	test.testGenerateEnvironment();*/
	//tests::testEnvironment("C:/Runs/env.txt", "C:/Runs/env1.txt");
	//tests::testUnifrormDistr();

	long endTime = time(0);
	cout << endl << "Runtime: "<< (endTime - startTime) / 60 << " min. " << (endTime - startTime) % 60 << " sec."<< endl;
}
