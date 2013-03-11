#ifndef TENVIRONMENT_H
#define TENVIRONMENT_H 

#include <string>
#include <vector>
#include <iostream>

/*
Класс окружающей среды - виртуальный (интерфейсный) класс - все среды должны от него наследовать предоставляя правильный интерфейс!!! 
*/
class TEnvironment{
public:
	// Загрузка структуры среды из файла (вместе с целями среды)
	virtual void loadEnvironment(std::string environmentFilename) = 0;
	// Выгрузка структуры целей в файл (вместе с целями среды)
	virtual void uploadEnvironment(std::string environmentFilename) const = 0;
	// Получения размерности вектора признаков среды
	virtual int getEnvironmentResolution() const = 0;
	// Получение размерности вектора воздействия на среду (из скольки значений состоит одно воздействие на среду)
	virtual int getActionResolution() const = 0;
	// Получение текущего вектора признаков среды
	virtual void getCurrentEnvironmentVector(double environmentVector[]) const = 0; 
	// Задание случайного состояния среды
	virtual void setRandomEnvironmentState() = 0;
	// Возоздействие на среду - возвращает успешность воздействия (!!! если такого понятия нет для конкретной среды, то должна возвращать true = 1)
	virtual int forceEnvironment(const std::vector<double>& action) = 0;
	// Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
	virtual double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const = 0;
  // Определения какие цели были достигнуты на фронте текущей последовательности действий (на текущем шаге времени)
  // Возвращает массив достигнутых целей
  virtual std::vector<int> testReachingAims(const std::vector< std::vector<double> >& actions, int actionsQuantity) const = 0;
  
  // Деструктор
  virtual ~TEnvironment(){};
  
  virtual void printSettings(std::ostream& os) = 0;
};

#endif // TENVIRONMENT_H
