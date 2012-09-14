#include "TPopulation.h"
#include <fstream>

using namespace std;

// �������� ��������� ������� �� �����
void TPopulation::loadPopulation(std::string populationFilename, int _populationSize){
	erasePopulation();
	agents = new TAgent*[_populationSize];
	populationSize = _populationSize;
	ifstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		agents[currentAgent - 1] = new TAgent;
		agents[currentAgent - 1]->loadGenome(populationFile);
	}
	populationFile.close();
}

// �������� ��������� ������� � ����
void TPopulation::uploadPopulation(std::string populationFilename) const{
	ofstream populationFile(populationFilename);
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent)
		agents[currentAgent - 1]->uploadGenome(populationFile);
	populationFile.close();
}

// ��������� ����������� ���������
void TPopulation::generateMinimalPopulation(int _populationSize, int inputResolution){
	erasePopulation();
	agents = new TAgent*[_populationSize];
	populationSize = _populationSize;
	for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
		agents[currentAgent - 1] = new TAgent;
		agents[currentAgent - 1]->generateMinimalAgent(inputResolution);
	}
}

// ��������� �������� ���������
void TPopulation::evolution(TEnvironment& environment, int evolutionTime /* = 0*/){
	for (int evolutionStep = 1; evolutionStep <= evolutionTime; ++evolutionStep){
		// ��������� ���� �������
		for (int currentAgent = 1; currentAgent <= populationSize; ++currentAgent){
			// �������� ��������� ������������
			agents[currentAgent - 1]->linearSystemogenesis();
			environment.setRandomEnvironmentVector();
			agents[currentAgent - 1]->life(environment, 250); // !!! ���� ������� ����� ������

		}
	}
}