#include "TPoolNetwork.h"
#include "TNeuralNetwork.h"

#include <iostream>
#include <fstream>

using namespace std;

int testPoolNetwork(){
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

	std::ofstream ofs("C:/Runs/net.txt");
	ofs << PoolNetwork;
	ofs.close();

	std::ifstream ifs("C:/Runs/net.txt");
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

int testNeuralNetwork(){
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

	std::ofstream ofs("C:/Runs/net.txt");
	ofs << NeuralNetwork;
	ofs.close();

	std::ifstream ifs("C:/Runs/net.txt");
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

int testNeuralNetworkProcessing(){
	TNeuralNetwork NeuralNetwork;
	ifstream ifs("C:/Runs/net_new.txt");
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

int main(int argc, char** argv){
	testNeuralNetworkProcessing();
}
