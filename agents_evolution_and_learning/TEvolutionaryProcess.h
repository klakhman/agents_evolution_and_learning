#ifndef TEVOLUTIONARYPROCESS_H
#define TEVOLUTIONARYPROCESS_H

#include "TPopulation.h"
#include "TEnvironment.h"
#include <string>

class TEvolutionaryProcess{
	// Загрузка параметров популяции из файла
	void fillPopulationSettingsFromFile();
	// Загрузка параметров среды из файла
	void fillEnvironmentSettingsFromFile();
	// Загрузка параметров агента из файла
	void fillAgentSettingsFromFile();
	// Популяция агентов
	TPopulation* agentsPopulation;
	// Среда для агентов
	TEnvironment* environment;
	// Лучшая средняя награда по популяции в текущем запуске
	double bestAverageReward;
public:
	// Настройки файлов процесса
	struct SFilenameSettings{
		std::string environmentFilename;
		std::string resultsFilename;
		std::string bestPopulationFilename;
		std::string bestAgentsFilename;
		std::string settingsFilename;
	} filenameSettings;
	// Конструктор по умолчанию
	TEvolutionaryProcess(){
		agentsPopulation = 0;
		environment = 0;
		bestAverageReward = 0.0;
	}
	// Деструктор
	~TEvolutionaryProcess(){
		if (agentsPopulation) delete agentsPopulation;
		if (environment) delete environment;
	}
	
	// Вывод логовых сообщений (прогресса) на консоль или в файл
	void makeLogNote(std::ostream& outputConsole, int currentEvolutionStep = 0);
	
	// Создание и заполнение предварительного файла основных результатов
	void createMainResultsFile(unsigned int randomSeed);

	// Запуск эволюционного процесса (передается зерно рандомизации, если 0, то рандомизатор инициализируется стандартно)
	void start(unsigned int randomSeed = 0);
};


#endif // TEVOLUTIONARYPROCESS_H