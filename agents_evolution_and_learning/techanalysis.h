#ifndef TECHANALYSIS_H
#define TECHANALYSIS_H

#include <vector>
#include <string>
#include "config.h"
#include "TAgent.h"
#include "TPopulation.h"
#include "THypercubeEnvironment.h"
#include "TBehaviorAnalysis.h"

namespace techanalysis{
  // Проверка разницы двух выброк по t-критерию (подрузамевается, что дисперсии выборок неизвестны и неравны - используется критерий Кохрена-Кокса для этого случая) 
  // Опциональные параметры - значения распределения Стьюдента с нужными степенями свобода и нужным уровнем значимости t_(alpha/2)(n-1)
  // В случае, если опциональные параметры не передаются, то метод проводит сравнение выборок с уровнем значимости alpha = 0.01 и только в том случае, если размеры выборок равны 80, 100 или 120 - иначе всегда возвращает false
  // Возвращает true - если средние неравны, false - если средние равны
  bool tTestMeanDifference(const std::vector<double>& firstSequence, const std::vector<double>& secondSequence, double firstTValue = 0, double secondTValue = 0);
  
  // Критерий Колмогорова-Смирнова на принадлежность выборки нормальному распределению 
  // alpha - уровень значимости (с учетом используемых приближений alpha = {0.15; 0.10; 0.05; 0.03; 0.01})
  bool testSampleNormality_KS(const std::vector<double>& sample, double alpha = 0.05);

  // Методика анализа стабильности группы нейронов, развивабщихся из пула - передается файл с сетью (геномом), которая эволюционировала с линейным системогенезом
  // Подсчитывается средняя награда контроллера, полученного линейным системогенезом, и контроллера, полученного первичным системогенезом, если бы все пулы имели не единичный размер, а некоторый стандартный 
  std::vector<double> poolsStabilityAnalysis(std::string agentFilename, std::string environmentFilename, std::string settingsFilename, int poolsStandartCapacity = 20, double developSynapseStandartProb = 0.1); 
  void transponceData(std::string inputFilename, std::string outputFilename, int columnsQuantity);

  // Разбор файлов с результатами по анализу лучших популяций
  std::vector<double> getValuesBPAFile(std::string bestPopulationAnalysisFile, int runsQuantity = 100);

  // Удобная обертка для анализа на зависимость размера области сходимости стратегии от ее длины (для одной популяции, циклы находятся независимо для каждого агента)
  // Создает файл с парами - (длина стратегии, размер области сходимости)
  void analysisSLengthVsConvSize(std::string settingsFilename, std::string populationFilename, std::string environmentFilename, std::string outputFilename);
  // Анализ на зависимость показателя размера областей сходимости стратегий от их длин (для одной популяции, циклы находятся независимо для каждого агента)
  // Возвращает вектор пар - (длина стратегии, размер области сходимости)  
  std::vector< std::pair<int, int> > analysisSLengthVsConvSize(TPopulation<TAgent>& population, THypercubeEnvironment& environment);
  // Анализ и подготовка данных для отображения развития в эволюции зависимости показателя размера областей сходимости стратегий от их длины
  // На выходе файл с четыремя строками: 1) Длины стратегий; 2) Области сходимости стратегий; 3) Цвет даннных (в соответствии с номером эволюционного такта); 4) Соответствующий номер эволюционного такта 
  // Цветовая палитра: ранние такты - голубые, поздние - красные (через желтый и зеленый). Дальнейшая отрисовка происходит с помощью matplotlib.
  void evolutionSLengthVsConvSize(std::string settingsFilename, std::string bestAgentsFilename, int evolutionTime, 
                                  std::string environmentFilename, std::string outputFilename, std::string colorPalette ="RGB");
  // Проведение анализа по эволюции поведения на основе лучших агентов в каждой популяции
  // Создает два файла: analysisOutputFileName - файл с парами (эв. такт; номер стратегии), обозначает присутствие стратегии в поведении агента на данном эв. такте;
  // dataOutputFilename - файл со всеми стратегиями
  // По умолчанию проводит анализ на основе циклов действий (при указании параметра true проводит анализ на основе циклов целей)
  void conductBehaviorEvolutionAnalysis(std::string settingsFilename, std::string environmentFilename, std::string bestAgentsFilename, int evolutionTime, 
                                        std::string analysisOutputFilename, std::string dataOutputFilename, bool aimCycles = false);
  // Обертка для прогона популяции - возвращает средние награды (при запуске из всех состояний) для всех агентов в популяции.
  // Системогенез проводится один раз для каждого агента. Если коэффициент стохастичности не указан, то берется значение из файла настроек. 
  std::vector<double> runPopulation(std::string populationFilename, std::string environmentFilename, 
                                    std::string settingsFilename, double stochasticityCoefficient =-1);
  // Прогона популяции - возвращает средние награды (при запуске из всех состояний) для всех агентов в популяции.
  // Системогенез проводится один раз для каждого агента. Если коэффициент стохастичности не указан, то берется значение из файла настроек. 
  std::vector<double> runPopulation(TPopulation<TAgent>& population, THypercubeEnvironment& environment);

  // Подсчет диаграммы сходимости поведения агента (возвращает пары - (поведенческий цикл действий, размер бассейна притяжения в кол-ве начальных состояний))
  // ВАЖНО: агент уже должен иметь нейроконтроллер (т.е. пройти системогенез)
  std::vector< std::pair<TBehaviorAnalysis::SCycle, int> > calculateBehaviorConvergenceData(TAgent& agent, THypercubeEnvironment& environment);

  #ifndef NOT_USE_ALGLIB_LIBRARY
    // Обертка над функцией из ALGLIB - тестирование нормальности выборки по методу Харки — Бера.
    // Возвращает p-value. Чем оно больше - тем лучше, как минимум должно быть выше 0.05.  
    double testSampleNormality_JB_ALGLIB(const std::vector<double>& sample);
    // Обертка над функцией из ALGLIB - тестирование разницы средних двух _НОРМАЛЬНЫХ_ выборок по критерию Стьюдента.
    // Возвращает p-value для two-tailed теста. Разница между средними есть, если p-value меньше заданного уровня значимости.
    double tTestMeanDifference_ALGLIB(const std::vector<double>& firstSample, const std::vector<double>& secondSample);
    // Обертка над функцией из ALGLIB - тестирование разницы средних двух выборок по критерию Манна-Уитни.
    // Возвращает p-value для two-tailed теста. Разница между средними есть, если p-value меньше заданного уровня значимости.
    double mann_whitneyMeanDifference_ALGLIB(const std::vector<double>& firstSample, const std::vector<double>& secondSample);
  #endif
}

#endif // TECHANALYSIS_H