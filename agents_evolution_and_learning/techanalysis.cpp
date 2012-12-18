#include "techanalysis.h"

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

// Проверка разницы двух выброк по t-критерию (подрузамевается, что дисперсии выборок неизвестны и неравны - используется критерий Кохрена-Кокса для этого случая) 
// Опциональные параметры - значения распределения Стьюдента с нужными степенями свобода и нужным уровнем значимости t_(alpha/2)(n-1)
// В случае, если опциональные параметры не передаются, то метод проводит сравнение выборок с уровнем значимости alpha = 0.01 и только в том случае, если размеры выборок равны 80, 100 или 120 - иначе всегда возвращает false
// Возвращает true - если средние неравны, false - если средние равны
bool techanalysis::checkMeanDifSignificance(const vector<double>& firstSequence, const vector<double>& secondSequence, double firstTValue /*=0*/, double secondTValue /*=0*/){
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