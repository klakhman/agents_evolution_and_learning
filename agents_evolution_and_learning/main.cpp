#include "TEvolutionaryProcess.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

#include "tests.h"
#include <ctime>

//#include <boost/program_options.hpp>

using namespace std;

int main(int argc, char** argv){
	long startTime = time(0);
	//srand(static_cast<unsigned int>(time(0)));
	
	TEvolutionaryProcess* evolutionaryProcess = new TEvolutionaryProcess;
	evolutionaryProcess->filenameSettings.environmentFilename = "C:/Tests/test_environment.txt";
	evolutionaryProcess->filenameSettings.settingsFilename = "C:/Tests/test_settings.txt";
	evolutionaryProcess->start();
	delete evolutionaryProcess;

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
	//test.testNeuralNetworkProcessing("C:/Tests/test_net.txt");
	//test.testCalculateOccupancyCoeffcient("C:/Tests/test_environment.txt");
	//test.testGenerateEnvironment();*/
	//tests::testEnvironment("C:/Runs/env.txt", "C:/Runs/env1.txt");
	long endTime = time(0);
	cout << endl << "Runtime: "<< (endTime - startTime) / 60 << " min. " << (endTime - startTime) % 60 << " sec."<< endl;
}
