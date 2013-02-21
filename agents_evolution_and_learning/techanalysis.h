﻿#ifndef TECHANALYSIS_H
#define TECHANALYSIS_H

#include <vector>
#include "config.h"
#include "TAgent.h"

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

  // Разобор файлов с результатами по анализу лучших популяций
  std::vector<double> getValuesBPAFile(std::string bestPopulationAnalysisFile);

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