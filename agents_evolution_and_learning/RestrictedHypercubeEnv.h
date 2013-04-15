#ifndef RESTRICTED_HYPERCUBE_ENV_H
#define RESTRICTED_HYPERCUBE_ENV_H

#include "THypercubeEnvironment.h"

#include <vector>
#include <string>

/// Класс среды-гиперкуба с запретом выполнения целей после их достижения
/**
* В данной версии среды происходит запрет выполнения цели после ее достижения в течение некоторого времени выполнения (в противовес просто уменьшению награды в стандартной версии).
* Иерархия целей в данной среде формируется таким образом, что сначала генерируются цели длины 2 и 3, а затем дополнения к этим целям (слева, а не справа как раньше).
* Данная конструкция приводит к древовидной иерархии, где множество целей имеет различное начало выполнения, но одинаковую концовку.
*/
class RestrictedHypercubeEnv : public THypercubeEnvironment{
public:
  /// Конструктор по умолчанию
  RestrictedHypercubeEnv(){
    currentAgentReward = 0;
    
  }
  /// Конструктор с заполнением иерархии целей из файла
  RestrictedHypercubeEnv(std::string aimsFilename, int _rewardRecoveryTime = 0, double _nonstaionarityCoefficient = 0){
    rewardRecoveryTime = _rewardRecoveryTime;
		stochasticityCoefficient = _nonstaionarityCoefficient;
		loadEnvironment(aimsFilename);
    currentAgentReward = 0;
  }
  /// Метод задания случайного состояния среды.
	void setRandomEnvironmentState();
  /// Метод воздействия на среду 
  int forceEnvironment(const std::vector<double>& action);
  /// Метод подсчета награды агента за "прожитую" жизнь.
	inline double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;
  /// Метод задания вектора среды (по значению целого вектора)
  void setEnvironmentVector(double environmentVector[]);
  /// Метода задания вектора среды (по номеру - начиная с нуля) 
  void setEnvironmentState(double stateNumber);
  /// Метод загрузки структуры среды из файла (вместе с целями среды).
  void loadEnvironment(std::string environmentFilename);
  /// Статический метод генерации среды по заданным параметрам
  static RestrictedHypercubeEnv* generateEnvironment(unsigned int _environmentResolution, double requiredOccupancyCoef, 
                                                    unsigned int maxAimComplexity = 5, unsigned int minAimComplexity = 2, 
                                                    double eps = 0.0005);
  /// Метод генерации или продолжения непротиворечивой цели
  static TAim generateSelfConsistentAim(unsigned int environmentResolution, unsigned int desiredComplexity, 
                                        const TAim& currentAim = TAim());
  /// Проверка существования идентичной цели в среде
  bool checkAimExistence(const TAim& aim);
  /// Добавление цели в среду
  inline void addAim(const TAim& aim);
  /// Проверка идентичности двух целей
  static bool equalAims(const TAim& firstAim, const TAim& secondAim);
  /// Проверка является ли одна цель родительской для другой цели
  static bool isParentAim(const TAim& potentialParentAim, const TAim& subAim);

private:
  /// Начальный вектор среды, из которого запускается текущая жизнь агента
  std::vector<bool> startEnvironmentVector;
  /// Награда, полученная за текущую жизнь агента
  double currentAgentReward;
  /// Запись текущей жизни агента
  std::vector< std::vector<double> > agentLifeRecord; 
  /// Времена последнего достижения всех целей в среде
  std::vector<int> reachedAimsTimes;
  /// Метод перезагрузки среды (обнуление жизни агента и времен восстановления награды)
  void resetEnvironment();
};

/// Метод подсчета награды агента за "прожитую" жизнь.
/**
* Данный метод, в отличие от обычной среды-гиперкуба, не подсчитывает награду, а просто возвращает ее значения для текущей жизни агента
* \param [in] actions - вектор воздействий на среду за все время жизни "агента".
* \param [in] actionsQuantity - количество действий в векторе воздействий
* \return Награда, набранная агентом.
*/
inline double RestrictedHypercubeEnv::calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const{
  return currentAgentReward;
}

/// Добавление цели в среду
/** 
* \param [in] aim - цель, которую нужно добавить в среду.
*/
inline void RestrictedHypercubeEnv::addAim(const TAim& aim){
  aimsSet.push_back(aim);
  reachedAimsTimes.resize(reachedAimsTimes.size() + 1, -rewardRecoveryTime);
  ++aimsQuantity;
}

#endif // RESTRICTED_HYPERCUBE_ENV_H