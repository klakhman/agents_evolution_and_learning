#include "tests.h"

#include "TPoolNetwork.h"
#include "TNeuralNetwork.h"
#include "TEnvironment.h"
#include "service.h"

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

int tests::testNeuralNetworkProcessing(string inputNetworkFilename){
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
	NeuralNetwork.getOutputVecor(outputVector);
	cout << outputVector[0] << "\t" << outputVector[1] << endl;

	inputVector[0] = 0;
	inputVector[1] = 1;
	inputVector[2] = 1;
	NeuralNetwork.calculateNetwork(inputVector);
	NeuralNetwork.getOutputVecor(outputVector);
	cout << outputVector[0] << "\t" << outputVector[1] << endl;

	delete []inputVector;
	delete []outputVector;

	return 0;
}

void tests::testUnifrormDistr(){
	double rnd;
	int zero_q=0;
	int one_q=0;
	int near_zero_q=0;
	int near_one_q = 0;
	long trials = 1000000000;
	for (int i=0; i<trials; ++i){
		rnd = service::uniformDistribution(0, 1, false, false);
		if ((rnd < 0) || (rnd > 1)) cout << "fault : " << rnd << endl;
		if (rnd == 1) ++one_q;
		if (rnd == 0) ++zero_q;
		if ((rnd>0) && (rnd<=0.1)) ++near_zero_q;
		if ((rnd>=0.9) && (rnd<1)) ++near_one_q;
	}
	cout << zero_q << endl << one_q << endl 
		<< near_zero_q/static_cast<double>(trials) << endl << near_one_q/static_cast<double>(trials) << endl;
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
