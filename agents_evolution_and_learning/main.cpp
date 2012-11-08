#include "TEvolutionaryProcess.h"
#include "TParallelEvolutionaryProcess.h"
#include "service.h"
#include "TAnalysis.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstdio>

#include "tests.h"
#include <ctime>
#include <new>

//#include <boost/program_options.hpp>

using namespace std;

// ���������� �������� ������
void out_of_memory(){
	cout << "\nnew() error : not enough memory\n";
	exit(999);
}

// ����������� ������ ������� ��������� ("E" - ��������, "BPA" - ������ ������� ������� ������ ���������)
void decodeProgramMode(string& programMode, int argc, char** argv){
	int currentArgNumber = 2; // ������� ����� ��������� (� ������ ������� ���� � ����� ��������)
	while (currentArgNumber < argc){
		switch (argv[currentArgNumber][1]){ // �������������� �������� (� ������ ���� "-")
			case 'm': // ���� ��� ����� ������� ������������ ���������
				programMode = argv[++currentArgNumber];
				break;
		}
		++currentArgNumber;
	}
}

int main(int argc, char** argv){
	// ������������� ���������� �������� ������
	set_new_handler(out_of_memory);

	TAnalysis analysis;
	analysis.makeBestPopulationAnalysisSummary("C:/Tests/Experiments/Test_on_standart_environments_set/BestPopulation_analysis_En1001-1360_testdeterm.txt",
																	"C:/Tests/Experiments/Test_on_standart_environments_set/analysis_testdeterm_summary.txt", 18, 20, 10); 
	return 0;
	string programMode; // ����� ������ ��������� - "E" - ��������, "BPA" - ������ ������� ������� ������ ���������
	decodeProgramMode(programMode, argc, argv);
	if (programMode == "E"){ // ����� ��������
		TParallelEvolutionaryProcess* parallelProcess = new TParallelEvolutionaryProcess;
		parallelProcess->start(argc, argv);
		delete parallelProcess;	
	}
	else if (programMode == "BPA"){ // ����� ������� ������� ������� ������ ���������
			TAnalysis* bestPopulationAnalysis = new TAnalysis;
			bestPopulationAnalysis->startParallelBestPopulationAnalysis(argc, argv);
			delete bestPopulationAnalysis;
		}
	//long startTime = time(0);
	/*TAnalysis* bestPopulationAnalysis = new TAnalysis;
	bestPopulationAnalysis->startBestPopulationAnalysis("C:/Tests/Results/En1001/En1001_test(1)_bestpopulation.txt", "C:/Tests/Environments/Environment1001.txt", "C:/Tests/test_settings.txt");
	delete bestPopulationAnalysis;*/
	/*TParallelEvolutionaryProcess* parallelProcess = new TParallelEvolutionaryProcess;
	parallelProcess->start(argc, argv);
	delete parallelProcess;*/

	/*srand(static_cast<unsigned int>(time(0)));
	rand();
	rand();
	rand();*/
	/*long low = 0;
	long high = 0;
	long number = 1000000000;
	for (long i = 0; i < number; ++i)
		if (service::uniformDistribution(0,1, true, false) > 0.5)
			++high;
		else 
			++low;
	cout << low/static_cast<float>(number) << "\t" << high/static_cast<float>(number);
	char d=getchar();*/
	/*TNeuralNetwork* neuralNet = new TNeuralNetwork;
	ifstream hTestNet;
	hTestNet.open("C:/Tests/test_net.txt");
	hTestNet >> *neuralNet;
	neuralNet->printGraphNetwork("C:/Tests/net_graph.jpg");
	hTestNet.close();
	delete neuralNet;*/

	/*TPoolNetwork* poolNet = new TPoolNetwork;
	ifstream hTestNet;
	hTestNet.open("C:/Tests/test_pool_net.txt");
	hTestNet >> *poolNet;
	cout << *poolNet;
	poolNet->printGraphNetwork("C:/Tests/pool_net_graph.jpg");
	hTestNet.close();
	delete poolNet;*/

	/*TEvolutionaryProcess* evolutionaryProcess = new TEvolutionaryProcess;
	evolutionaryProcess->filenameSettings.environmentFilename = "C:/Tests/test_environment.txt";
	evolutionaryProcess->filenameSettings.settingsFilename = "C:/Tests/test_settings.txt";
	evolutionaryProcess->filenameSettings.resultsFilename = "C:/Tests/test_results.txt";
	evolutionaryProcess->filenameSettings.bestPopulationFilename = "C:/Tests/test_best_population.txt";
	evolutionaryProcess->filenameSettings.bestAgentsFilename = "C:/Tests/test_best_agents.txt";
	evolutionaryProcess->start();
	delete evolutionaryProcess;*/

	/*TEnvironment* environment = new TEnvironment("D:/tests/test_environment.txt", 30);
	TAgent* agent = new TAgent;
	ifstream genomeFile;
	genomeFile.open("D:/tests/test_genome.txt");
	agent->loadGenome(genomeFile);
	genomeFile.close();
	agent->linearSystemogenesis();
	environment->setRandomEnvironmentVector();
	agent->printLife(*environment, 10);
	cout << "DONE!" << endl;
	delete agent;
	delete environment;*/
	/*string settingsFilename = "C:/Temp/Temp/settings.ini";
	ifstream settingsFile(settingsFilename.c_str(), ifstream::in);
	boost::program_options::options_description settingsOptions("Options in settings file");
	settingsOptions.add_options()
		("mutation-prob", boost::program_options::value<double>())
		("mutation-variance", boost::program_options::value<double>())
		("work-directory", boost::program_options::value<string>())
	;
	boost::program_options::variables_map varMap;
	boost::program_options::store(boost::program_options::parse_config_file(settingsFile, settingsOptions), varMap);
	boost::program_options::notify(varMap);
	if (varMap.count("mutation-prob"))
		cout << varMap["mutation-prob"].as<double>();
	if (varMap.count("work-directory"))
		cout << varMap["work-directory"].as<string>();
	settingsFile.close();*/

	//tests test;
	//test.testWeakUniformDistribution();
	//test.testNeuralNetworkProcessing("D:/tests/test_net.txt");
	//test.testCalculateOccupancyCoeffcient("C:/Tests/test_environment.txt");
	//test.testGenerateEnvironment();*/
	//test.testEnvironment("D:/tests/env.txt", "D:/tests/env1.txt");
	//test.testWeigthsMutation("D:/tests/");
	//test.testDuplicatePool("D:/tests/");
	//long endTime = time(0);
	//cout << endl << "Runtime: "<< (endTime - startTime) / 60 << " min. " << (endTime - startTime) % 60 << " sec."<< endl;
	//char c = getchar();
}
