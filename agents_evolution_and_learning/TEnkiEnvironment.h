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
#include "enki/robots/e-puck/EPuck.h"
#include "enki/Types.h"

//Данный класс описывает среду, в которой есть один агент - робот E-PUCK а также объекты разных цветов. Цели формулируются как последовательность посещения роботом объектов, причем разные последовательности дают разную награду. Робот имеет инфракрасные сенсоры а также камеру для определения положения объектов и их цвета. Собственно, пользуясь этой информацией, робот должен оптимизировать свои действия для получения максимальной награды - что мы и хотим проверить в рамках этой модели.

//Формат файла для данной среды нужен следующий:

//x, y - физический размер среды
//Nobjects - кол-во объектов
//Xi, Yi, #cccccc - позиция, а также цвет куба (в данной модели мы считаем размер кубов стандартным, а также пусть они располагаются параллельно сторонам арены)
//Xn, Yn #cccccc
//Ngoals
//Li, Ri - длина и награда
//k, l, m, n (k,l,m,n - это номера объектов, чьи значения лежат в области от 1 до Nobjects)

class TEnkiEnvironment : public TEnvironment {
private:
  
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
    public: //по умолчанию длина стороны куба = 10
      double x;
      double y;
      double color[3];
    
      TEnkiObject(){
        x = 10;
        y = 10;
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
      }
  };
  
public:
  
  double worldStep;
  double cubeRadius;
  double cubeSize;
  
  double xSize, ySize;
  int objectsNumber;
  std::vector<TEnkiObject> objectsArray;
  int goalsNumber;
  std::vector<TEnkiAim> goalsArray;
  
  Enki::EPuck * ePuckBot;
  Enki::World * world;
  
  TEnkiEnvironment() {
    xSize = 0;
    ySize = 0;
    objectsNumber = 0;
    goalsNumber = 0;
    worldStep = 0;
    cubeRadius = 0;
    cubeSize = 0;
  }
  
  TEnkiEnvironment(std::string aimsFilename, double _worldStep=0, double _cubeSize=0, double _cubeRadius=0){
    xSize = 0;
    ySize = 0;
    objectsNumber = 0;
    goalsNumber = 0;
		worldStep = _worldStep;
    cubeSize = _cubeSize;
    cubeRadius = _cubeRadius;
		loadEnvironment(aimsFilename);
	}
  
  ~TEnkiEnvironment() {
    objectsArray.clear();
    goalsArray.clear();
  }
  
  void loadEnvironment(std::string environmentFilename);
  // Выгрузка структуры целей в файл (вместе с целями среды)
  void uploadEnvironment(std::string environmentFilename) const;
  // Получения размерности вектора признаков среды
  int getEnvironmentResolution() const;
  // Получение размерности вектора воздействия на среду (из скольки значений состоит одно воздействие на среду)
  int getActionResolution() const;
  // Получение текущего вектора признаков среды
  void getCurrentEnvironmentVector(double environmentVector[]) const;
  // Задание случайного состояния среды
  void setRandomEnvironmentState(); //здесь под случайным состоянием понимается случайное положение робота в среде, а также случайное направление его взгляда
  // Возоздействие на среду - возвращает успешность воздействия (!!! если такого понятия нет для конкретной среды, то должна возвращать true)
  int forceEnvironment(const std::vector<double>& action);
  // Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
  double calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;
  std::vector<int> testReachingAims(const std::vector< std::vector<double> >& actions, int actionsQuantity) const;
  void printSettings(std::ostream& os);
};

#else 

//Данная ветка if предназначена для пользователей, у которых не установлена ENKI, дабы не вызывать проблем на этапе компиляции, здесь создается фиктивный класс, не выполняющий никаких реальных функций.

class TEnkiEnvironment : public TEnvironment {
public:
  
  double worldStep;
  double cubeRadius;
  double cubeSize;
  TEnkiEnvironment(){};
  TEnkiEnvironment(std::string){};
  void loadEnvironment(std::string environmentFilename) {};
  // Выгрузка структуры целей в файл (вместе с целями среды)
  void uploadEnvironment(std::string environmentFilename) const {};
  // Получения размерности вектора признаков среды
  int getEnvironmentResolution() const {
    return 0;
  }
  // Получение размерности вектора воздействия на среду (из скольки значений состоит одно воздействие на среду)
  int getActionResolution() const {
    return 0;
  }
  // Получение текущего вектора признаков среды
  void getCurrentEnvironmentVector(double environmentVector[]) const {};
  // Задание случайного состояния среды
  void setRandomEnvironmentState() {}; //здесь под случайным состоянием понимается случайное положение робота в среде, а также случайное направление его взгляда
  // Возоздействие на среду - возвращает успешность воздействия (!!! если такого понятия нет для конкретной среды, то должна возвращать true)
  int forceEnvironment(const std::vector<double>& action) {
    return 0;
  }
  // Подсчет награды агента - при этом передается вся записанная жизнь агента - возвращает награду
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
  };
  
};
#endif

#endif /* defined(__enki__TEnkiEnvironment__) */
