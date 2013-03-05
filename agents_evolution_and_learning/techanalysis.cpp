#include "techanalysis.h"

#include <vector>
#define _USE_MATH_DEFINES
#include "math.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <sstream>

#include "TAgent.h"
#include "THypercubeEnvironment.h"
#include "TBehaviorAnalysis.h"
#include "settings.h"
#include "config.h"

using namespace std;

// Проверка разницы двух выброк по t-критерию (подрузамевается, что дисперсии выборок неизвестны и неравны - используется критерий Кохрена-Кокса для этого случая) 
// Опциональные параметры - значения распределения Стьюдента с нужными степенями свобода и нужным уровнем значимости t_(alpha/2)(n-1)
// В случае, если опциональные параметры не передаются, то метод проводит сравнение выборок с уровнем значимости alpha = 0.01 и только в том случае, если размеры выборок равны 80, 100 или 120 - иначе всегда возвращает false
// Возвращает true - если средние неравны, false - если средние равны
bool techanalysis::tTestMeanDifference(const vector<double>& firstSequence, const vector<double>& secondSequence, double firstTValue /*=0*/, double secondTValue /*=0*/){
  double firstMean = 0;
  for (unsigned int i = 0; i < firstSequence.size(); ++i)
    firstMean += firstSequence[i];
  firstMean /= firstSequence.size();
  double firstStandartDeviation = 0;
  for (unsigned int i = 0; i < firstSequence.size(); ++i)
    firstStandartDeviation += (firstSequence[i] - firstMean) * (firstSequence[i] - firstMean);
  firstStandartDeviation /= (firstSequence.size() - 1);

  double secondMean = 0;
  for (unsigned int j = 0; j < secondSequence.size(); ++j)
    secondMean += secondSequence[j];
  secondMean /= secondSequence.size();
  double secondStandartDeviation = 0;
  for (unsigned int j = 0; j < secondSequence.size(); ++j)
    secondStandartDeviation += (secondSequence[j] - secondMean) * (secondSequence[j] - secondMean);
  secondStandartDeviation /= (secondSequence.size() - 1);
  // Находим общее стандартное отклонение
  double aggregateStandartDeviation = 1.0/firstSequence.size()*firstStandartDeviation + 1.0/secondSequence.size()*secondStandartDeviation;
  double t_criteria = fabs(firstMean - secondMean)/sqrt(aggregateStandartDeviation);
  // Подсчитываем критерий для сравнения (по методу Кохрена-Кокса)
  if (!firstTValue){ // Если не задано значение распределения Стьюдента для первой выборки
    if (80 == firstSequence.size()) firstTValue = 2.6380;
    else if (100 == firstSequence.size()) firstTValue = 2.6259;
    else if (120 == firstSequence.size()) firstTValue = 2.6174;
    else return false;
  }
  if (!secondTValue){
    if (80 == secondSequence.size()) secondTValue = 2.6380;
    else if (100 == secondSequence.size()) secondTValue = 2.6259;
    else if (120 == secondSequence.size()) secondTValue = 2.6174;
    else return false;
  }
  double t_value = (firstStandartDeviation / firstSequence.size() * firstTValue + secondStandartDeviation / secondSequence.size() * secondTValue) / 
                    (firstStandartDeviation / firstSequence.size() + secondStandartDeviation / secondSequence.size());
  if (t_criteria > t_value)
    return true;
  else 
    return false;
}

// Критерий Колмогорова-Смирнова на принадлежность выборки нормальному распределению 
// alpha - уровень значимости (с учетом используемых приближений alpha = {0.15; 0.10; 0.05; 0.03; 0.01})
bool techanalysis::testSampleNormality_KS(const vector<double>& sample, double alpha /*= 0.05*/){
  vector<double> sorted_sample = sample;
  sort(sorted_sample.begin(), sorted_sample.end());
  // Находим среднее и отклонение
  double mean = 0;
  for (unsigned int currentValue = 0; currentValue < sorted_sample.size(); ++currentValue)
    mean += sorted_sample[currentValue];
  mean /= sorted_sample.size();
  double variance = 0;
  for (unsigned int currentValue = 0; currentValue < sorted_sample.size(); ++currentValue)
    variance += (mean - sorted_sample[currentValue]) * (mean - sorted_sample[currentValue]);
  variance = sqrt(variance / sorted_sample.size());
  // Аппроксимируем нормальное распределение
  // По формуле (Кобзарь, стр.28)
  vector<double> normDistr(sorted_sample.size());
  for (unsigned int currentValue = 0; currentValue < sorted_sample.size(); ++currentValue){
    // Нормируем значение
    double z = (sorted_sample[currentValue] - mean) / variance;
    normDistr[currentValue] = 1.0/(1 + exp(-0.0725*abs(z)*(22+pow(abs(z), 1.96))));; 
    normDistr[currentValue] = (z >= 0) ? normDistr[currentValue] : (1 - normDistr[currentValue]); 
  }
  // Теперь находим максимальные sup и inf множества отклонений эмпирической частоты от теоретической
  double D_minus = -numeric_limits<double>::max();
  double D_plus = -numeric_limits<double>::max();
  for (unsigned int currentValue = 0; currentValue < sorted_sample.size(); ++currentValue){
    D_plus = max(D_plus, (currentValue + 1) / static_cast<double>(sorted_sample.size()) - normDistr[currentValue]);
    D_minus = max(D_minus, normDistr[currentValue] - currentValue / static_cast<double>(sorted_sample.size())); 
  }
  // Используем модифицированную статистику (Кобзарь, стр.215)
  double D = max(D_plus, D_minus) * (sqrt(sorted_sample.size()) - 0.01 +  0.85/sqrt(sorted_sample.size()));
  // Используем точную формулу для нахождения критического значения (Кобзарь, стр.215)
  // double D_crit = sqrt(1.0/(2.0*sorted_sample.size()) * (-log(alpha) - (2*log(alpha)*log(alpha) + 4*log(alpha) - 1)/(18.0 * sorted_sample.size()))) - 1.0 / (6.0 * sorted_sample.size());
  // Для сравнения выборки с нормальным распределением с параметрами, выделенным из выборки нужно использовать данное критическое значение (Кобзарь, стр.233)
  double D_crit = 0;
  if (0.15 == alpha) D_crit = 0.775;
  else if (0.1 == alpha) D_crit = 0.819;
  else if (0.05 == alpha) D_crit = 0.895;
  else if (0.03 == alpha) D_crit = 0.995;
  else if (0.01 == alpha) D_crit = 1.035;
  return (D < D_crit);
}

// Методика анализа стабильности группы нейронов, развивабщихся из пула - передается файл с сетью (геномом), которая эволюционировала с линейным системогенезом
// Подсчитывается средняя награда контроллера, полученного линейным системогенезом, и контроллера, полученного первичным системогенезом, если бы все пулы имели не единичный размер, а некоторый стандартный 
vector<double> techanalysis::poolsStabilityAnalysis(string agentFilename, string environmentFilename, string settingsFilename, int poolsStandartCapacity /*=20*/, double developSynapseStandartProb /*=0.1*/){
  int agentLifetime = 250;
  // Итоговый вектор - первое значение это награда с линейным системогенезом, второе значение - с первичным системогенезом
  vector<double> output;
  // Загружаем агента и среду
  TAgent agent;
  ifstream agentFile;
  agentFile.open(agentFilename.c_str());
  agent.loadGenome(agentFile);
  agentFile.close();
  settings::fillAgentSettingsFromFile(agent, settingsFilename);
  THypercubeEnvironment environment(environmentFilename);
  settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
  // Прогоняем сначала агента полученного в результате линейного системогенеза
  agent.setSystemogenesisMode(0);
  agent.setLearningMode(0);
  environment.setStochasticityCoefficient(0.0);
  int statesQuantity = environment.getInitialStatesQuantity();
  agent.linearSystemogenesis();
  double reward = 0;
  for (int currentState = 0; currentState < statesQuantity; ++currentState){
    environment.setEnvironmentState(currentState);
    agent.life(environment, agentLifetime);
    reward += agent.getReward();
  }
  output.push_back(reward/statesQuantity);
  // Проверяем агента, полученного в результате первичного системогенеза
  // Для этого надо сначала установить стандартные значения емкости пулов и вероятности развития связей
  for (int currenPool = 1; currenPool <= agent.getPointerToAgentGenome()->getPoolsQuantity(); ++ currenPool){
    if (TPoolNetwork::HIDDEN_POOL == agent.getPointerToAgentGenome()->getPoolType(currenPool))
      agent.getPointerToAgentGenome()->setPoolCapacity(currenPool, poolsStandartCapacity);
    for (int currentConnection = 1; currentConnection <= agent.getPointerToAgentGenome()->getPoolInputConnectionsQuantity(currenPool); ++currentConnection)
      agent.getPointerToAgentGenome()->setConnectionDevelopSynapseProb(currenPool, currentConnection, developSynapseStandartProb);
  }
  agent.primarySystemogenesis();
  reward = 0;
  for (int currentState = 0; currentState < statesQuantity; ++currentState){
    environment.setEnvironmentState(currentState);
    agent.life(environment, agentLifetime);
    reward += agent.getReward();
  }
  output.push_back(reward/statesQuantity);

  return output;
}

// Процедура транспонирования записи данных
// На вход подается файл с данными расположенными по столбцам, на выходе файл с данными по строкам
// NB! : Kоличество отсчетов данных должно быть одинаково для всех столбцов
// В основном необходима для дальнешей отрисовки с помощью matplotlib
void  techanalysis::transponceData(string inputFilename, string outputFilename, int columnsQuantity){
  if (!columnsQuantity) return;

  vector< vector<double> > data;
  data.resize(columnsQuantity);
  ifstream inputFile;
  inputFile.open(inputFilename.c_str());
  string dataString;
  while (inputFile >> dataString) {
    data[0].push_back(atof(dataString.c_str()));
    for (int currentData = 1; currentData < columnsQuantity; ++currentData){
      inputFile >> dataString;
      data[currentData].push_back(atof(dataString.c_str()));
    }
  }
  inputFile.close();

  ofstream outputFile;
  outputFile.open(outputFilename.c_str());
  for (int currentData = 0; currentData < columnsQuantity; ++currentData){
    for (unsigned int currentValue = 0; currentValue < data[currentData].size(); ++currentValue)
      outputFile << data[currentData][currentValue] << "\t";
    outputFile << "\n";
  }
  outputFile.close();
}

// Разбор файлов с результатами по анализу лучших популяций
vector<double> techanalysis::getValuesBPAFile(string bestPopulationAnalysisFile, int runsQuantity /*= 100*/){
  string tmpStr;
  vector<double> values;
  ifstream inputFile;
  inputFile.open(bestPopulationAnalysisFile.c_str());
  for (int i = 0; i < runsQuantity; ++i){
    inputFile >> tmpStr; // Считываем номер среды
    inputFile >> tmpStr; // Считываем номер попытки
    inputFile >> tmpStr; // Считываем награду
    values.push_back(atof(tmpStr.c_str()));
  }
  inputFile.close();
  return values;
}

// Удобная обертка для анализа на зависимость размера области сходимости стратегии от ее длины
// Создает файл с парами - (длина стратегии, размер области сходимости)
void techanalysis::analysisSLengthVsConvSize(string settingsFilename, string populationFilename, string environmentFilename, string outputFilename){
  THypercubeEnvironment environment(environmentFilename);
  settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
	TPopulation<TAgent> agentsPopulation;
  settings::fillPopulationSettingsFromFile(agentsPopulation, settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(agentsPopulation, settingsFilename);  
  agentsPopulation.loadPopulation(populationFilename);
  vector< pair<int, int> > data = analysisSLengthVsConvSize(agentsPopulation, environment);
  ofstream outFile;
  outFile.open(outputFilename.c_str());
  for (unsigned i=0; i < data.size(); ++i)
    outFile << data[i].first << "\t" << data[i].second << endl;
  outFile.close();
}

// Анализ на зависимость показателя размера областей сходимости стратегий от их длин (для одной популяции, циклы находятся независимо для каждого агента)
// Возвращает вектор пар - (длина стратегии, размер области сходимости)
vector< pair<int, int> > techanalysis::analysisSLengthVsConvSize(TPopulation<TAgent>& population, THypercubeEnvironment& environment){
  environment.setStochasticityCoefficient(0.0);
  vector< pair<int, int> > strategyLengthVsConvSize;
  // Прогоняем всех агентов и находим соответствие длины цикла и его области сходимости
  for (int agentNumber = 1; agentNumber <= population.getPopulationSize(); ++agentNumber){
    TAgent& currentAgent = *population.getPointertoAgent(agentNumber);
    currentAgent.linearSystemogenesis();
    // Находим все стратегии агента и определяем размер их областей притяжения
    vector< pair< TBehaviorAnalysis::SCycle, vector<int> > >convergenceData = calculateBehaviorConvergenceData(currentAgent, environment);
    // Записываем найденные стратегии
    for (unsigned int currentStrategy = 0; currentStrategy < convergenceData.size(); ++currentStrategy)
      strategyLengthVsConvSize.push_back(make_pair(convergenceData[currentStrategy].first.cycleSequence.size(), convergenceData[currentStrategy].second.size()));
  }
  return strategyLengthVsConvSize;
}

// Анализ и подготовка данных для отображения развития в эволюции зависимости показателя размера областей сходимости стратегий от их длины
// На выходе файл с четыремя строками: 1) Длины стратегий; 2) Области сходимости стратегий; 3) Цвет даннных (в соответствии с номером эволюционного такта); 4) Соответствующий номер эволюционного такта 
// Цветовая палитра: ранние такты - голубые, поздние - красные (через желтый и зеленый). Дальнейшая отрисовка происходит с помощью matplotlib.
void techanalysis::evolutionSLengthVsConvSize(string settingsFilename, string bestAgentsFilename, int evolutionTime, 
                                              string environmentFilename, string outputFilename, string colorPalette /*="RGB"*/){
  vector< pair<int, int> > strategyLengthVsConvSize; // Соответствующие пары
  vector<int> evolutionTacts; // Номера эволюционных тактов для всех пар
  THypercubeEnvironment environment(environmentFilename);
  settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
  environment.setStochasticityCoefficient(0.0);
  TAgent currentAgent;
  settings::fillAgentSettingsFromFile(currentAgent, settingsFilename);
  ifstream bestAgentsFile;
  bestAgentsFile.open(bestAgentsFilename.c_str());
   // Прогоняем всех лучших агентов и находим соответствие длины цикла и его области сходимости
  for (int curEvolutionTime = 1; curEvolutionTime <= evolutionTime; ++curEvolutionTime){
    currentAgent.loadGenome(bestAgentsFile);
    currentAgent.linearSystemogenesis();
    vector< pair< TBehaviorAnalysis::SCycle, vector<int> > > convergenceData = calculateBehaviorConvergenceData(currentAgent, environment);
    // Записываем найденные стратегии
    for (unsigned int currentStrategy = 0; currentStrategy < convergenceData.size(); ++currentStrategy){
      strategyLengthVsConvSize.push_back(make_pair(convergenceData[currentStrategy].first.cycleSequence.size(), convergenceData[currentStrategy].second.size()));
      evolutionTacts.push_back(curEvolutionTime);
    }
    cout << curEvolutionTime << endl;
  }
  bestAgentsFile.close();
  // Записываем результаты
  ofstream outputFile;
  outputFile.open(outputFilename.c_str());
  for (unsigned int currentStrategy = 0; currentStrategy < strategyLengthVsConvSize.size(); ++currentStrategy)
    outputFile << strategyLengthVsConvSize[currentStrategy].first << "\t";
  outputFile << endl;
  for (unsigned int currentStrategy = 0; currentStrategy < strategyLengthVsConvSize.size(); ++currentStrategy)
    outputFile << strategyLengthVsConvSize[currentStrategy].second << "\t";
  outputFile << endl;
  for (unsigned int currentStrategy = 0; currentStrategy < evolutionTacts.size(); ++currentStrategy)
    outputFile << evolutionTacts[currentStrategy] << "\t";
  outputFile << endl;
  // Теперь записываем подготовленные цвета в нужной палитре (от голубого к красному через желтый и зеленый)
  for (unsigned int currentStrategy = 0; currentStrategy < evolutionTacts.size(); ++currentStrategy){
    double H = 360 * (0.5 - (0.5 * evolutionTacts[currentStrategy]) / evolutionTime); // Hue 
    double S = 0.9; // Saturation
    double V = 0.9; // Brightness
    if (colorPalette == "HSB")
      outputFile << H << ";" << S << ";" << V << "\t";
    else if (colorPalette == "RGB"){
      int R, G, B;
      service::HSVtoRGB(R, G, B, H, S, V);
      string Rhex, Ghex, Bhex; 
      service::decToHex(R, Rhex, 2);
      service::decToHex(G, Ghex, 2);
      service::decToHex(B, Bhex, 2);
      outputFile << "#" << Rhex + Ghex + Bhex << "\t";
    }
  }
  outputFile.close();
}

// Проведение анализа по эволюции поведения на основе лучших агентов в каждой популяции
// Создает два файла: analysisOutputFileName - файл с парами (эв. такт; номер стратегии), обозначает присутствие стратегии в поведении агента на данном эв. такте;
// dataOutputFilename - файл со всеми стратегиями
// По умолчанию проводит анализ на основе циклов действий (при указании параметра true проводит анализ на основе циклов целей)
void techanalysis::conductBehaviorEvolutionAnalysis(string settingsFilename, string environmentFilename, string bestAgentsFilename, int evolutionTime, 
                                                    string analysisOutputFilename, string dataOutputFilename, bool aimCycles /*=false*/){
  THypercubeEnvironment environment(environmentFilename);
  settings::fillEnvironmentSettingsFromFile(environment, settingsFilename);
  environment.setStochasticityCoefficient(0.0);
  TAgent currentAgent;
  settings::fillAgentSettingsFromFile(currentAgent, settingsFilename);
  ifstream bestAgentsFile;
  bestAgentsFile.open(bestAgentsFilename.c_str());
  ofstream analysisOutputFile;
  analysisOutputFile.open(analysisOutputFilename.c_str());
  vector<TBehaviorAnalysis::SCycle> cyclesData;
  for (int curEvolutionTime = 1; curEvolutionTime <= evolutionTime; ++curEvolutionTime){
    currentAgent.loadGenome(bestAgentsFile);
    vector<TBehaviorAnalysis::SCycle> currentAgentCycles = TBehaviorAnalysis::findAllCyclesOfAgent(currentAgent, environment, aimCycles);
    for (unsigned int currentCycle = 1; currentCycle <= currentAgentCycles.size(); ++currentCycle){
      int cycleNumber = TBehaviorAnalysis::findCycleInExistingCycles(currentAgentCycles[currentCycle - 1], cyclesData);
      if (cycleNumber) // Если такой цикл уже был
        analysisOutputFile << curEvolutionTime << "\t" << cycleNumber << endl;
      else{
        cyclesData.push_back(currentAgentCycles[currentCycle - 1]);
        analysisOutputFile << curEvolutionTime << "\t" << cyclesData.size() << endl;
      }
    }
    cout << curEvolutionTime << "\t" << cyclesData.size() << endl;
  }
  bestAgentsFile.close();
  analysisOutputFile.close();
  TBehaviorAnalysis::uploadCycles(cyclesData, dataOutputFilename);
}

// Обертка для прогона популяции - возвращает средние награды (при запуске из всех состояний) для всех агентов в популяции.
// Системогенез проводится один раз для каждого агента. Если коэффициент стохастичности не указан, то берется значение из файла настроек. 
vector<double> techanalysis::runPopulation(string populationFilename, string environmentFilename, 
                                           string settingsFilename, double stochasticityCoefficient /*=-1*/){
  THypercubeEnvironment* environment = new THypercubeEnvironment(environmentFilename);
	settings::fillEnvironmentSettingsFromFile(*environment, settingsFilename);
  if (stochasticityCoefficient != -1)
    environment->setStochasticityCoefficient(stochasticityCoefficient);
	TPopulation<TAgent>* agentsPopulation = new TPopulation<TAgent>;
	settings::fillPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	// Физически агенты в популяции уже созданы (после того, как загрузился размер популяции), поэтому можем загрузить в них настройки
	settings::fillAgentsPopulationSettingsFromFile(*agentsPopulation, settingsFilename);
	agentsPopulation->loadPopulation(populationFilename);
  return runPopulation(*agentsPopulation, *environment);
}
// Прогона популяции - возвращает средние награды (при запуске из всех состояний) для всех агентов в популяции.
// Системогенез проводится один раз для каждого агента. Если коэффициент стохастичности не указан, то берется значение из файла настроек. 
vector<double> techanalysis::runPopulation(TPopulation<TAgent>& population, THypercubeEnvironment& environment){
  vector<double> agentsRewards;
  int initialStatesQuantity = environment.getInitialStatesQuantity();
  // Прогоняем всех агентов и записываем награды в массив
	for (int currentAgent = 1; currentAgent <= population.getPopulationSize(); ++currentAgent){
    double averageReward = 0;
    if (1 == population.getPointertoAgent(currentAgent)->getSystemogenesisMode())
      population.getPointertoAgent(currentAgent)->primarySystemogenesis();
    else if (0 == population.getPointertoAgent(currentAgent)->getSystemogenesisMode())
		  population.getPointertoAgent(currentAgent)->linearSystemogenesis();
    else if (2 == population.getPointertoAgent(currentAgent)->getSystemogenesisMode())
      population.getPointertoAgent(currentAgent)->alternativeSystemogenesis();
    // Необходимо сохранять первичную нейронную сеть, так как запуск проходит из всех состояний и возможно обучение
    TNeuralNetwork initialController;
    if (0 != population.getPointertoAgent(currentAgent)->getLearningMode())
      initialController = *(population.getPointertoAgent(currentAgent)->getPointerToAgentController());
		for (int currentInitialState = 0; currentInitialState < initialStatesQuantity; ++currentInitialState){
      if (0 != population.getPointertoAgent(currentAgent)->getLearningMode())
        *(population.getPointertoAgent(currentAgent)->getPointerToAgentController()) = initialController;
			environment.setEnvironmentState(currentInitialState);
			population.getPointertoAgent(currentAgent)->life(environment, population.evolutionSettings.agentLifetime);
      averageReward += population.getPointertoAgent(currentAgent)->getReward() / initialStatesQuantity;
    }
    agentsRewards.push_back(averageReward);
    cout << currentAgent << "\t" << averageReward << endl;
	}
  return agentsRewards;
}

// Подсчет диаграммы сходимости поведения агента (возвращает пары - (поведенческий цикл действий, список состояний в бассейне притяжения поведенческой стратегии))
// ВАЖНО: агент уже должен иметь нейроконтроллер (т.е. пройти системогенез)
vector< pair< TBehaviorAnalysis::SCycle, vector<int> > > techanalysis::calculateBehaviorConvergenceData(TAgent& agent, THypercubeEnvironment& environment){
  vector< pair< TBehaviorAnalysis::SCycle, vector<int> > > convergenceData;
  vector< TBehaviorAnalysis::SCycle > agentStrategies;
  environment.setStochasticityCoefficient(0.0);
  // Так как может происходить обучение, то записываем изначальную сеть
  TNeuralNetwork initialController = *(agent.getPointerToAgentController());
  // Находим все стратегии агента и определяем размер их областей притяжения
  for (int currentState = 0; currentState < environment.getInitialStatesQuantity(); ++currentState){
      environment.setEnvironmentState(currentState);
      TBehaviorAnalysis::SCycle currentStrategy  = TBehaviorAnalysis::findCycleInAgentLife(agent, environment);
      *(agent.getPointerToAgentController()) = initialController;
      if (currentStrategy.cycleSequence.size()){
        int strategyNumber = TBehaviorAnalysis::findCycleInExistingCycles(currentStrategy, agentStrategies);
        if (strategyNumber) // Если такая стратегия уже была
          convergenceData[strategyNumber - 1].second.push_back(currentState);
        else{
          agentStrategies.push_back(currentStrategy);
          vector<int> tmp;
          tmp.push_back(currentState);
          convergenceData.push_back(make_pair(currentStrategy, tmp));
        }
      }
  }
  return convergenceData;
}

void techanalysis::conductLearningVsNonLearningAnalysis(TAgent& agent, THypercubeEnvironment& environment, string outputFilename, 
                                          int runsQuantity /*=200*/, int agentLifetime /*=100*/){
  ofstream outputFile;
  outputFile.open(outputFilename.c_str());
  environment.setStochasticityCoefficient(0.0);
  for (int currentRun = 0; currentRun < runsQuantity; ++currentRun){  
    agent.primarySystemogenesis();
    agent.setLearningMode(1);
    double averageDiffNeurons = 0;
    int initNeurons = agent.getActiveNeuronsQuantity();
    double averageLearningReward = 0;
    TNeuralNetwork initController = *(agent.getPointerToAgentController());
    for (int currentState = 0; currentState < environment.getInitialStatesQuantity(); ++currentState){
      environment.setEnvironmentState(currentState);
      *(agent.getPointerToAgentController()) = initController;
      agent.life(environment, agentLifetime);
      averageLearningReward += agent.getReward() / environment.getInitialStatesQuantity();
      averageDiffNeurons += (agent.getActiveNeuronsQuantity() - initNeurons)/static_cast<double>(environment.getInitialStatesQuantity());
    }
    *(agent.getPointerToAgentController()) = initController;
    double averageNonLearningReward = 0;
    agent.setLearningMode(0);
    for (int currentState = 0; currentState < environment.getInitialStatesQuantity(); ++currentState){
      environment.setEnvironmentState(currentState);
      agent.life(environment, agentLifetime);
      averageNonLearningReward += agent.getReward() / environment.getInitialStatesQuantity();
    }
    cout << currentRun << ":\t" << averageLearningReward << "\t" << (averageLearningReward - averageNonLearningReward) << "\t" << averageDiffNeurons << endl;
    outputFile << averageLearningReward << "\t" << (averageLearningReward - averageNonLearningReward) << "\t" << averageDiffNeurons << endl;
  }
}

void techanalysis::makeBehaviorConvergenceDiagram(TAgent& agent, THypercubeEnvironment& environment, string imageFilename){
  vector< pair< TBehaviorAnalysis::SCycle, vector<int> > > convergenceData = calculateBehaviorConvergenceData(agent, environment);
  double sizeScaleCoef = 0.1; // Масштабирование размера круга в зависимости от длины цикла
  ofstream outputDotFile;
  outputDotFile.open((imageFilename + ".dot").c_str());
  outputDotFile << "graph G{" << endl << "dpi = \"600\";" << endl;
  // Необходимо дополнительно определить состояния, из которых агент никуда не сходится
  vector<bool> noCycleStatesInd(environment.getInitialStatesQuantity(), false); 
  for (unsigned int currentCycle = 1; currentCycle <= convergenceData.size(); ++currentCycle){
    double circleSize = convergenceData[currentCycle - 1].first.cycleSequence.size() * sizeScaleCoef;
    string color = "grey60";
    outputDotFile << "\tc" << currentCycle << " [label=\"" << currentCycle << "\", shape=doublecircle, fillcolor=" << color
                  << ", style=filled, height=" << circleSize << ", fixedsize=true]" << endl;
    // Записываем бассейн притяжения
    for (unsigned int currentState = 1; currentState <= convergenceData[currentCycle - 1].second.size(); ++currentState){
      int stateNumber = convergenceData[currentCycle - 1].second[currentState - 1];
      outputDotFile << "\ts" << stateNumber << " [label = \"" << stateNumber << "\", fontname=\"Arial\"]" << endl;
      outputDotFile << "\ts" << stateNumber << " -- c" << currentCycle << endl;
      noCycleStatesInd[stateNumber] = true;
    }
  }
  vector<int> noCycleStates;
  for (unsigned int currenState = 0; currenState < noCycleStatesInd.size(); ++currenState)
    if (!noCycleStatesInd[currenState]) noCycleStates.push_back(currenState);
  // Если есть хотя бы одно состояние, из которого агент никуда не сходится
  if (noCycleStates.size()){
    outputDotFile << "\tc0 [height=0, fixedsize=true]" << endl;
    for (unsigned int currentState = 1; currentState <= noCycleStates.size(); ++currentState){
      int stateNumber = noCycleStates[currentState - 1];
      outputDotFile << "\ts" << stateNumber << " [label = \"" << stateNumber << "\", fontname=\"Arial\"]" << endl;
      outputDotFile << "\ts" << stateNumber << " -- c0" << endl;
    }
  }
  outputDotFile << "}" << endl;
  // Записываем циклы
  vector<TBehaviorAnalysis::SCycle> cycles;
  for (unsigned int currentCycle = 0; currentCycle < convergenceData.size(); ++currentCycle)
    cycles.push_back(convergenceData[currentCycle].first);
  TBehaviorAnalysis::uploadCycles(cycles, imageFilename + ".txt");
  // Отрисовываем картинку
  system(("fdp -Tjpg " + imageFilename + ".dot -o " + imageFilename).c_str());
}

#ifndef NOT_USE_ALGLIB_LIBRARY
  #include "statistics.h"

  // Обертка над функцией из ALGLIB - тестирование нормальности выборки по методу Харки — Бера.
  // Возвращает p-value. Чем оно больше - тем лучше, как минимум должно быть выше 0.05.  
  double testSampleNormality_JB_ALGLIB(const std::vector<double>& sample){
    alglib::real_1d_array _sample;
    _sample.setcontent(sample.size(), &(sample[0]));
    double p_value;
    alglib::jarqueberatest(_sample, sample.size(), p_value);
    return p_value;
  }
  // Обертка над функцией из ALGLIB - тестирование разницы средних двух _НОРМАЛЬНЫХ_ выборок по критерию Стьюдента.
  // Возвращает p-value для two-tailed теста. Разница между средними есть, если p-value меньше заданного уровня значимости.
  double tTestMeanDifference_ALGLIB(const std::vector<double>& firstSample, const std::vector<double>& secondSample){
     alglib::real_1d_array _firstSample;
    _firstSample.setcontent(firstSample.size(), &(firstSample[0]));
    alglib::real_1d_array _secondSample;
    _secondSample.setcontent(secondSample.size(), &(secondSample[0]));
    double p_value, p_left, p_right;
    alglib::unequalvariancettest(_firstSample, firstSample.size(), _secondSample, secondSample.size(), p_value, p_left, p_right);
    return p_value;
  }
  // Обертка над функцией из ALGLIB - тестирование разницы средних двух выборок по критерию Манна-Уитни.
  // Возвращает p-value для two-tailed теста. Разница между средними есть, если p-value меньше заданного уровня значимости.
  double mann_whitneyMeanDifference_ALGLIB(const std::vector<double>& firstSample, const std::vector<double>& secondSample){
    alglib::real_1d_array _firstSample;
    _firstSample.setcontent(firstSample.size(), &(firstSample[0]));
    alglib::real_1d_array _secondSample;
    _secondSample.setcontent(secondSample.size(), &(secondSample[0]));
    double p_value, p_left, p_right;
    alglib::mannwhitneyutest(_firstSample, firstSample.size(), _secondSample, secondSample.size(), p_value, p_left, p_right);
    return p_value;
  }
#endif

