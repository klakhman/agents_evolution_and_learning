//
//  TEnkiEnvironment.h
//  enki
//
//  Created by Сергей Муратов on 17.12.12.
//
//

#ifndef TENKIENVIRONMENT_H
#define TENKIENVIRONMENT_H

#include "TEnvironment.h"
#include "service.h"
#include "config.h"
#include <iostream>
#include <vector>

#ifndef NOT_USE_ROBOT_LIB
#include "EPuck.h"
#include "Types.h"

//Данный класс описывает среду, в которой есть один агент - робот E-PUCK а также объекты разных цветов. Цели формулируются как последовательность посещения роботом объектов, причем разные последовательности дают разную награду. Робот имеет инфракрасные сенсоры а также камеру для определения положения объектов и их цвета. Собственно, пользуясь этой информацией, робот должен оптимизировать свои действия для получения максимальной награды - что мы и хотим проверить в рамках этой модели.

/* Формат файла для данной среды нужен следующий:
 
x, y - физический размер среды
Nobjects - кол-во объектов
Xi, Yi, #cccccc - позиция, а также цвет куба (в данной модели мы считаем размер кубов стандартным, а также пусть они располагаются параллельно сторонам арены)
Xn, Yn #cccccc
Ngoals
Li, Ri - длина и награда
k, l, m, n (k,l,m,n - это номера объектов, чьи значения лежат в области от 1 до Nobjects)
 
*/

class TEnkiEnvironment : public TEnvironment {
private:
  
  double worldStep; // Шаг времени в секундах, который используется средой для итерационного развития
  double cubeRadius; // Радиус относительно центра куба (объекта), в рамках которого засчитывается достижение агентом данного объекта
  double cubeSize; // Размер объекта (в данном случае - сторона куба)
  double rewardRecoveryTime; // Время восстановления награды
  
  class TEnkiAim{
  public:
    // Максимальное кол-во действий в цели
    static const int MAX_AIM_COMPLEXITY = 7;
    // Структура одного необходимого действия в цели
    int actionsSequence[MAX_AIM_COMPLEXITY];
    
    int aimComplexity; // Сложность цели - кол-во действий
    double reward; // Полная награда за достижение целей
    
    //Конструктор по умолчанию
    TEnkiAim(){
      aimComplexity = 0;
      reward = 0;
    }
  };
  
  class TEnkiObject{
    public:
      // Длина стороны куба (объекта) является параметром среды и одинакова для всех объектов в среде
      double x; // Положение центра объекта по оси абсцисс
      double y; // Положение центра объекта по оси ординат
      double color[3]; // Цвет объекта
    
      //Конструктор по умолчанию
      TEnkiObject(){
        x = 10;
        y = 10;
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
      }
  };
  
public:

  double xSize, ySize; // Размеры квадратной арены, в которой находится агент
  int objectsNumber; // Количество объектов в среде
  std::vector<TEnkiObject> objectsArray; // Вектор объектов среды
  int goalsNumber; // Количество целей в среде
  std::vector<TEnkiAim> goalsArray; // Вектор целей среды
  
  Enki::EPuck * ePuckBot; // Робот E-Puck
  Enki::World * world; // Симуляция среды, в которой он существует
  
  // Конструктор среды по умолчанию
  TEnkiEnvironment() {
    xSize = 0;
    ySize = 0;
    objectsNumber = 0;
    goalsNumber = 0;
    worldStep = 0;
    cubeRadius = 0;
    cubeSize = 0;
    rewardRecoveryTime = 0;
  }
  
  // Конструктор среды с использованием параметров из файла
  TEnkiEnvironment(std::string aimsFilename, double _worldStep=0, double _cubeSize=0, double _cubeRadius=0, double _rewardRecoveryTime = 0){
    xSize = 0;
    ySize = 0;
    objectsNumber = 0;
    goalsNumber = 0;
		worldStep = _worldStep;
    cubeSize = _cubeSize;
    cubeRadius = _cubeRadius;
    rewardRecoveryTime = _rewardRecoveryTime;
		loadEnvironment(aimsFilename);
	}
  
  // Деструктор
  ~TEnkiEnvironment() {
    objectsArray.clear();
    goalsArray.clear();
  }
  
  // Загрузка среды с ее целями, а также других параметров из файла
  void loadEnvironment(std::string environmentFilename);
  
  // Выгрузка структуры целей в файл (вместе с целями среды)
  void uploadEnvironment(std::string environmentFilename) const;
  
  // Получения размерности вектора признаков среды
  int getEnvironmentResolution() const;
  
  // Получение размерности вектора воздействия на среду (из скольки значений состоит одно воздействие на среду)
  int getActionResolution() const;
  
  // Получение текущего вектора признаков среды
  void getCurrentEnvironmentVector(double environmentVector[]) const;
  
  // Задание случайного состояния среды (здесь под случайным состоянием понимается случайное положение робота в среде, а также случайное направление его взгляда)
  void setRandomEnvironmentState();
  
  // Воздействие на среду - возвращает номер объекта, рядом с которым сейчас находится агент
  int forceEnvironment(const std::vector<double>& action);
  
  // Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
  double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;
  
  // Метод, который описан здесь для соблюдения протокола, в данной модели не используется
  std::vector<int> testReachingAims(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;
  
  // Вывод настроек среды на печать
  void printSettings(std::ostream& os) {
    os << "world-step" << std::endl;
    os << worldStep << std::endl;
    os << "cube-size" << std::endl;
    os << cubeSize << std::endl;
    os << "cube-radius" << std::endl;
    os << cubeRadius << std::endl;
    os << "reward-recovery-time" << std::endl;
    os << rewardRecoveryTime << std::endl;
  };
  
  // Геттеры и сеттеры параметров среды
  
  double getRewardRecoveryTime() const { return rewardRecoveryTime; }
	void setRewardRecoveryTime(int _rewardRecoveryTime) { rewardRecoveryTime = _rewardRecoveryTime; }
  
  double getWorldStep() const { return worldStep; }
  void setWorldStep(double _worldStep) { worldStep = _worldStep;}
  
  double getCubeSize() const { return cubeSize; }
  void setCubeSize(double _cubeSize) { cubeSize = _cubeSize;}
  
  double getCubeRadius() const { return cubeRadius; }
  void setCubeRadius(double _cubeRadius) { cubeRadius = _cubeRadius; }
  
};

#else 

//Данное описание класса предназначено для пользователей, у которых не установлена ENKI. Дабы не вызывать проблем на этапе компиляции, здесь создается фиктивный класс, не выполняющий никаких реальных функций. Реальное описание класса находится выше.

class TEnkiEnvironment : public TEnvironment {
public:
  
  double rewardRecoveryTime;
  double worldStep;
  double cubeRadius;
  double cubeSize;
  TEnkiEnvironment(){};
  TEnkiEnvironment(std::string){};
  void loadEnvironment(std::string environmentFilename) {};
  void uploadEnvironment(std::string environmentFilename) const {};
  int getEnvironmentResolution() const {
    return 0;
  }
  int getActionResolution() const {
    return 0;
  }
  void getCurrentEnvironmentVector(double environmentVector[]) const {};
  void setRandomEnvironmentState() {};
  int forceEnvironment(const std::vector<double>& action) {
    return 0;
  }
  double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const {
    return 0;
  }
  std::vector<int> testReachingAims(const std::vector< std::vector<double> >& actions, int actionsQuantity) const {
    std::vector<int> someVector;
    someVector.push_back(0);
    return someVector;
  }
  ~TEnkiEnvironment () {
    worldStep = 0;
    cubeRadius = 0;
    cubeSize = 0;
  }
  void printSettings(std::ostream& os) {
    os << "world-step" << std::endl;
    os << worldStep << std::endl;
    os << "cube-size" << std::endl;
    os << cubeSize << std::endl;
    os << "cube-radius" << std::endl;
    os << cubeRadius << std::endl;
    os << "reward-recovery-time" << std::endl;
    os << rewardRecoveryTime << std::endl;
  };
  double getRewardRecoveryTime() const {return rewardRecoveryTime; }
	void setRewardRecoveryTime(int _rewardRecoveryTime) {rewardRecoveryTime = _rewardRecoveryTime; }
};
#endif

#endif /* defined(__enki__TEnkiEnvironment__) */
