#ifndef TENVIRONMENT_H
#define TENVIRONMENT_H 

#include <string>
#include <vector>
#include <iostream>

/// Виртуальный класс окружающей среды для автономного агента.
/**
Все реализованные типы сред должны наследовать от данного базового класса предоставляя правильный интерфейс.
*/
class TEnvironment{
public:
	/// Метод загрузки структуры среды из файла (вместе с целями среды).
  /**
  * \param [in] environmentFilename - имя файла со структурой целей в среде.
  */
	virtual void loadEnvironment(std::string environmentFilename) = 0;

	/// Метод записи структуры среды в файл (вместе с целями среды).
  /**
  * \param [in] environmentFilename - имя файла, в который необходимо записать среду. 
  */
	virtual void uploadEnvironment(std::string environmentFilename) const = 0;
	
  /// Метод получения размерности вектора признаков среды.
  /**
  * \return Размерность вектора признаков среды.
  */
	virtual int getEnvironmentResolution() const = 0;
	
  /// Метод получения размерности вектора воздействия на среду (из скольки значений состоит одно воздействие на среду).
  /**
  * \return Необходимая размерность вектора воздействия на среду со стороны агента.
  */
  virtual int getActionResolution() const = 0;
	
  /// Метод получения текущего вектора признаков среды.
  /**
  * \param [out] environmentVector - массив, в который записывается текущий вектор признаков.
  */
	virtual void getCurrentEnvironmentVector(double environmentVector[]) const = 0; 
	
  /// Метод задания случайного состояния среды.
	virtual void setRandomEnvironmentState() = 0;

	/// Метод воздействия на среду 
  /**
  * \param [in] action - вектор необходимого воздействия на среду.
  * \return Показатель успешности воздействия на среду (0 - действие успешно, 1 - неуспешно). Если такое понятие как успешность действия неприменимо для конкретной реализации среды, то всегда возвращает 1.
  */
	virtual int forceEnvironment(const std::vector<double>& action) = 0;

	/// Метод подсчета награды агента за "прожитую" жизнь.
  /**
  * \param [in] actions - вектор воздействий на среду за все время жизни "агента".
  * \param [in] actionsQuantity - количество действий в векторе воздействий
  * \return Награда, набранная агентом.
  */
	virtual double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const = 0;

  /// Метод определения целей достигнутых на фронте некоторого временного отсчета
  /**
  * \param [in] actions - вектор воздействий на среду.
  * \param [in] actionsQuantity - рассматриваемый временной отсчет.
  * \return Вектор целей (номера) достигнутых на данном отсчете времени.
  */
  virtual std::vector<int> testReachingAims(const std::vector< std::vector<double> >& actions, int actionsQuantity) const = 0;

  /// Метод вывода параметров среды
  /**
  * \param [out] os - поток, в который необходимо вывести параметры среды
  */
  virtual void printSettings(std::ostream& os) = 0;

  /// Вирутальный деструктор среды
  virtual ~TEnvironment(){};
};

#endif // TENVIRONMENT_H
