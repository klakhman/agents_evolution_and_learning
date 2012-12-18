#ifndef TECHANALYSIS_H
#define TECHANALYSIS_H

#include <vector>

namespace techanalysis{
  // Проверка разницы двух выброк по t-критерию (подрузамевается, что дисперсии выборок неизвестны и неравны - используется критерий Кохрена-Кокса для этого случая) 
  // Опциональные параметры - значения распределения Стьюдента с нужными степенями свобода и нужным уровнем значимости t_(alpha/2)(n-1)
  // В случае, если опциональные параметры не передаются, то метод проводит сравнение выборок с уровнем значимости alpha = 0.01 и только в том случае, если размеры выборок равны 80, 100 или 120 - иначе всегда возвращает false
  // Возвращает true - если средние неравны, false - если средние равны
  bool checkMeanDifSignificance(const std::vector<double>& firstSequence, const std::vector<double>& secondSequence, double firstTValue = 0, double secondTValue = 0);
}

#endif // TECHANALYSIS_H