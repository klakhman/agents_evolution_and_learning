#ifndef SETTINGS_H
#define SETTINGS_H

#include "THypercubeEnvironment.h"
#include "TPopulation.h"
#include "TAgent.h"
#include "TEnkiAgent.h"
#include <string>

#include "TEnkiEnvironment.h"

namespace settings{
	// Нахождение имени файла парметров
	std::string getSettingsFilename(int argc, char** argv);
	// Заполнение параметров директорий для записи файлов
	void fillDirectoriesSettings(std::string& workDirectory, std::string& environmentDirectory, std::string& resultsDirectory, std::string settingsFilename);
	// Загрузка параметров среды из файла
	void fillEnvironmentSettingsFromFile(THypercubeEnvironment& environment, std::string settingsFilename);
	// Загрузка параметров популяции из файла
	void fillPopulationSettingsFromFile(TPopulation<TAgent>& agentsPopulation, std::string settingsFilename);
  // Загрузка параметров популяции из файла (для агентов ENKI)
  void fillPopulationSettingsFromFile(TPopulation<TEnkiAgent>& agentsPopulation, std::string settingsFilename);
	// Загрузка параметров всех агентов в популяции из файла
	void fillAgentsPopulationSettingsFromFile(TPopulation<TAgent>& agentsPopulation, std::string settingsFilename);
  // Загрузка параметров всех агентов в популяции из файла (для агентов ENKI)
  void fillAgentsPopulationSettingsFromFile(TPopulation<TEnkiAgent>& agentsPopulation, std::string settingsFilename);
	// Загрузка параметров агента из файла
	void fillAgentSettingsFromFile(TAgent& agent, std::string settingsFilename);
  // Загрузка параметров среды ENKI из файла
  void fillEnvironmentSettingsFromFile(TEnkiEnvironment& environment, std::string settingsFilename);
};

#endif // SETTINGS_H