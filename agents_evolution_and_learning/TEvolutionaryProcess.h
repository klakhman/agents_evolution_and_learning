#ifndef TEVOLUTIONARYPROCESS_H
#define TEVOLUTIONARYPROCESS_H

#include "TPopulation.h"
#include "TEnvironment.h"
#include <string>

class TEvolutionaryProcess{
	// �������� ���������� ��������� �� �����
	void fillPopulationSettingsFromFile();
	// �������� ���������� ����� �� �����
	void fillEnvironmentSettingsFromFile();
	// �������� ���������� ������ �� �����
	void fillAgentSettingsFromFile();
	// ��������� �������
	TPopulation* agentsPopulation;
	// ����� ��� �������
	TEnvironment* environment;
public:
	// ��������� ������ ��������
	struct SFilenameSettings{
		std::string environmentFilename;
		std::string resultsFilename;
		std::string settingsFilename;
	} filenameSettings;
	// ����������� �� ���������
	TEvolutionaryProcess(){
		agentsPopulation = 0;
		environment = 0;
	}
	// ����������
	~TEvolutionaryProcess(){
		if (agentsPopulation) delete agentsPopulation;
		if (environment) delete environment;
	}
	
	// ����� ������� ��������� (���������) �� ������� ��� � ����
	void makeLogNote(std::ostream& outputConsole, int currentEvolutionStep =0);

	// ������ ������������� �������� (���������� ����� ������������, ���� 0, �� ������������ ���������������� ����������)
	void start(unsigned int randomSeed = 0);
};


#endif // TEVOLUTIONARYPROCESS_H