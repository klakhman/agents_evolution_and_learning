#ifndef SETTINGS_H
#define SETTINGS_H

#include "THypercubeEnvironment.h"
#include "TPopulation.h"
#include "TAgent.h"
#include <string>

namespace settings{
	// Нахождение имени файла парметров
	std::string getSettingsFilename(int argc, char** argv);
	// Заполнение параметров директорий для записи файлов
	void fillDirectoriesSettings(std::string& workDirectory, std::string& environmentDirectory, std::string& resultsDirectory, std::string settingsFilename);
	// Загрузка параметров среды из файла
	void fillEnvironmentSettingsFromFile(THypercubeEnvironment& environment, std::string settingsFilename);
	// Загрузка параметров популяции из файла
	void fillPopulationSettingsFromFile(TPopulation<TAgent>& agentsPopulation, std::string settingsFilename);
	// Загрузка параметров всех агентов в популяции из файла
	void fillAgentsPopulationSettingsFromFile(TPopulation<TAgent>& agentsPopulation, std::string settingsFilename);
	// Загрузка параметров агента из файла
	void fillAgentSettingsFromFile(TAgent& agent, std::string settingsFilename);
};

#endif // SETTINGS_H