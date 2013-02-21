#include "techanalysis.h"

#include <vector>
#define _USE_MATH_DEFINES
#include "math.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>

#include "TAgent.h"
#include "THypercubeEnvironment.h"
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
vector<double> techanalysis::getValuesBPAFile(string bestPopulationAnalysisFile){
  string tmpStr;
  vector<double> values;
  ifstream inputFile;
  inputFile.open(bestPopulationAnalysisFile.c_str());
  for (int i = 0; i < 100; ++i){
    inputFile >> tmpStr; // Считываем номер среды
    inputFile >> tmpStr; // Считываем номер попытки
    inputFile >> tmpStr; // Считываем награду
    values.push_back(atof(tmpStr.c_str()));
  }
  inputFile.close();
  return values;
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

