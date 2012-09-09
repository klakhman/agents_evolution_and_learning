#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

#include "tests.h"

using namespace std;

int main(int argc, char** argv){

	long startTime = time(0);
	srand(static_cast<unsigned int>(time(0)));

	tests::testEnvironment("C:/Runs/env.txt", "C:/Runs/env1.txt");
	//tests::testUnifrormDistr();
	//testNeuralNetworkProcessing();

	long endTime = time(0);
	cout << endl << "Runtime: "<< (endTime - startTime) / 60 << " min. " << (endTime - startTime) % 60 << " sec."<< endl;
}
