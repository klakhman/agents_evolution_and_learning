#include "TEvolutionaryProcess.h"
#include "TPopulation.h"
#include "TEnvironment.h"
#include <ctime>

// ������ ������������� �������� (���������� ����� ������������, ���� 0, �� ������������ ���������������� ����������)
void TEvolutionaryProcess::start(unsigned int randomSeed /*= 0*/){
	// ���� ���� �������� ����� ������������, �� �������������� ��
	if (randomSeed)
		srand(randomSeed);
	else 
		srand(static_cast<unsigned int>(time(0)));
	// ��������� �����
	if (environment)
		delete environment;
	environment = new TEnvironment(filenameSettings.environmentFilename);
	fillEnvironmentSettingsFromFile();
	// ���� ���� ������� ��� ���������� (������ ���� �� ������� ����������� �������� ����������� ��������)
	if (agentsPopulation)
		delete agentsPopulation;
	agentsPopulation = new TPopulation;
	fillPopulationSettingsFromFile();
	agentsPopulation->generateMinimalPopulation();

	fillAgentSettingsFromFile();

}

