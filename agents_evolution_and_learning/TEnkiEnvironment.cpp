//
//  TEnkiEnvironment.cpp
//  enki
//
//  Created by Сергей Муратов on 17.12.12.
//
//
#include "config.h"
#ifndef NOT_USE_ROBOT_LIB

#include "TEnkiEnvironment.h"
#include <iostream>

using namespace std;

void TEnkiEnvironment::loadEnvironment(std::string environmentFilename) {
    // Получаем параметры среды из файла
    std::ifstream environmentFile;
    environmentFile.open(environmentFilename.c_str());
    string tmp_str;
    environmentFile >> tmp_str; // Считываем размеры прямоугольной арены
    xSize = atof(tmp_str.c_str());
    environmentFile >> tmp_str;
    ySize = atof(tmp_str.c_str());
    environmentFile >> tmp_str; // Считываем количество объектов в среде (по умолчанию - кубы)
    objectsNumber = atoi(tmp_str.c_str());
    for (int i=0; i<objectsNumber; i++) { // Заполняем массив объектов, каждый из которых имеет позицию своего центра и цвет
      TEnkiObject someNewObject = TEnkiObject();
      environmentFile >> tmp_str;
      someNewObject.x = atof(tmp_str.c_str());
      environmentFile >> tmp_str;
      someNewObject.y = atof(tmp_str.c_str());
      environmentFile >> tmp_str;
      someNewObject.color[0] = atof(tmp_str.c_str());
      environmentFile >> tmp_str;
      someNewObject.color[1] = atof(tmp_str.c_str());
      environmentFile >> tmp_str;
      someNewObject.color[2] = atof(tmp_str.c_str());
      objectsArray.push_back(someNewObject);
    }
    environmentFile >> tmp_str;
    goalsNumber = atoi(tmp_str.c_str()); // Считываем количество целей
    for (int i=0; i<goalsNumber; i++) { // Заполняем массив целей, каждая из которых имеет длину последовательности, награду и саму последовательность посещения объектов
      TEnkiAim someNewAim = TEnkiAim();
      environmentFile >> tmp_str;
      someNewAim.aimComplexity = atoi(tmp_str.c_str());
      if (someNewAim.aimComplexity > someNewAim.MAX_AIM_COMPLEXITY) {
        cout << "Warning in TEnkiEnvironment: aim complexity can't be greater than the MAX_AIM_COMPLEXITY. Setting it to the MAX_AIM_COMPLEXITY by force now to avoid crash.";
        someNewAim.aimComplexity = someNewAim.MAX_AIM_COMPLEXITY;
      }
      environmentFile >> tmp_str;
      someNewAim.reward = atof(tmp_str.c_str());
      for (int j=0; j<someNewAim.aimComplexity; j++) {
        environmentFile >> tmp_str;
        someNewAim.actionsSequence[j] = atoi(tmp_str.c_str());
        if (someNewAim.actionsSequence[j] > objectsNumber) {
          cout << "Warning in TEnkiEnvironment: action number can't be greater than the number of objects. Setting it to the number of objects by force now to avoid crash.";
          someNewAim.actionsSequence[j] = objectsNumber;
        }
      }
      goalsArray.push_back(someNewAim);
    }
  
  //Переходим к постройке мира исходя из параметров, полученных из файла
    
  // Создаем саму арену
  Enki::World world(xSize, ySize, Enki::Color(0.9, 0.9, 0.9));
	
	// Создаем E-PUCK и размещаем его в среде
  Enki::EPuck *epuck = new Enki::EPuck;
  epuck->pos = Enki::Point(20, 50);
  epuck->angle = 0;
  epuck->leftSpeed = 10;
  epuck->rightSpeed = 10;
  epuck->setColor(Enki::Color(1, 0, 0));
  world.addObject(epuck);
  
  // Создаем объекты
  Enki::Polygone p2;
  p2.push_back(Enki::Point(cubeSize/2.0,cubeSize/2.0));
  p2.push_back(Enki::Point(-cubeSize/2.0,cubeSize/2.0));
  p2.push_back(Enki::Point(-cubeSize/2.0,-cubeSize/2.0));
  p2.push_back(Enki::Point(cubeSize/2.0,-cubeSize/2.0));
  for (int i = 0; i < objectsNumber; i++)
  {
    Enki::PhysicalObject* o = new Enki::PhysicalObject;
    Enki::PhysicalObject::Hull hull(Enki::PhysicalObject::Part(p2, cubeSize));
    o->setCustomHull(hull, -1); // Отрицательная масса - то же, что бесконечная, поэтому объекты сдвинуть с места будет невозможно
    o->setColor(Enki::Color(objectsArray.at(i).color[0], objectsArray.at(i).color[1], objectsArray.at(i).color[2]));
    o->collisionElasticity = 0; // Все тепло от столкновения поглощается
    o->pos = Enki::Point(objectsArray.at(i).x, objectsArray.at(i).y);
    world.addObject(o);
  }
  
}

void TEnkiEnvironment::uploadEnvironment(std::string environmentFilename) const {
    //Записываем информацию о среде (если вдруг мы делали ее случайным образом, то нам понадобится этот метод)
    ofstream environmentFile;
    environmentFile.open(environmentFilename.c_str());
    // Записываем размеры среды
    environmentFile << xSize << "\t" << ySize << endl;
    // Записываем количество объектов
    environmentFile << objectsNumber << endl;
    //Записываем положение центров объектов, а также их цвет
    for (int currentObject = 0; currentObject < objectsNumber; currentObject++){
      environmentFile << objectsArray.at(currentObject).x << "\t" << objectsArray.at(currentObject).y << "\t" << objectsArray.at(currentObject).color[0] << "\t" << objectsArray.at(currentObject).color[1] << "\t" << objectsArray.at(currentObject).color[2] << endl;
    }
    //Записываем количество целей
    environmentFile << goalsNumber << endl;
    for (int currentAim = 0; currentAim < goalsNumber; currentAim++){
      environmentFile << goalsArray.at(currentAim).aimComplexity << "\t" << goalsArray.at(currentAim).reward << endl;
      for (int currentAction = 0; currentAction < goalsArray.at(currentAim).aimComplexity; currentAction++) {
        environmentFile << goalsArray.at(currentAim).actionsSequence[currentAction];
      }
      environmentFile << endl;
    }
}

int TEnkiEnvironment::getEnvironmentResolution() const {
    return 15;
}

int TEnkiEnvironment::getActionResolution() const {
    return 2;
}

void TEnkiEnvironment::getCurrentEnvironmentVector(double environmentVector[]) const {
    Enki::Color firstSection, secondSection, thirdSection;
    
    int numberOfPixelsToCoverTheFullView = 180;
    
    //Сперва просто суммируем значения по каждому из пикселей для его R,G,B и A составляющих. Это проделываем для трех секторов - левого, центрального и правого. Также нормируем на интервал [0; 1], деля на 255.
  
    for (int i=0; i<numberOfPixelsToCoverTheFullView/3; i++) {
        firstSection.components[0]+=ePuckBot->camera.image[i].components[0]/255.0;
        firstSection.components[1]+=ePuckBot->camera.image[i].components[1]/255.0;
        firstSection.components[2]+=ePuckBot->camera.image[i].components[2]/255.0;
        firstSection.components[3]+=ePuckBot->camera.image[i].components[3];
    }
    
    for (int i=numberOfPixelsToCoverTheFullView/3; i<2*numberOfPixelsToCoverTheFullView/3; i++) {
        secondSection.components[0]+=ePuckBot->camera.image[i].components[0]/255.0;
        secondSection.components[1]+=ePuckBot->camera.image[i].components[1]/255.0;
        secondSection.components[2]+=ePuckBot->camera.image[i].components[2]/255.0;
        secondSection.components[3]+=ePuckBot->camera.image[i].components[3];
    }
    
    for (int i=2*numberOfPixelsToCoverTheFullView/3; i<numberOfPixelsToCoverTheFullView; i++) {
        thirdSection.components[0]+=ePuckBot->camera.image[i].components[0]/255.0;
        thirdSection.components[1]+=ePuckBot->camera.image[i].components[1]/255.0;
        thirdSection.components[2]+=ePuckBot->camera.image[i].components[2]/255.0;
        thirdSection.components[3]+=ePuckBot->camera.image[i].components[3];
    }
    
    //Затем усредняем по количеству пикселей в каждой секции (исключаем альфа-канал номер 4)
    for (int i=0; i<3; i++) {
        firstSection.components[i]=3*firstSection.components[i]/(double)numberOfPixelsToCoverTheFullView;
        secondSection.components[i]=3*secondSection.components[i]/(double)numberOfPixelsToCoverTheFullView;
        thirdSection.components[i]=3*thirdSection.components[i]/(double)numberOfPixelsToCoverTheFullView;
    }
    
    //Получаем значения скоростей
    double leftSpeed;
    double rightSpeed;
    
    if (ePuckBot->leftSpeed>12.8) {
        leftSpeed = 1.0;
    } else if (ePuckBot->leftSpeed<-12.8) {
        leftSpeed = -1.0;
    } else {
        leftSpeed = ePuckBot->leftSpeed/12.8;
    }

    if (ePuckBot->rightSpeed>12.8) {
        rightSpeed = 1.0;
    } else if (ePuckBot->rightSpeed<-12.8) {
        rightSpeed = -1.0;
    } else {
        rightSpeed = ePuckBot->rightSpeed/12.8;
    }
    
    double frontFrontRightSensorValue = ePuckBot->infraredSensor0.finalValue/4000.0;
    double rightSensorValue = ePuckBot->infraredSensor2.finalValue/4000.0;
    double leftSensorValue = ePuckBot->infraredSensor5.finalValue/4000.0;
    double frontFrontLeftSensorValue = ePuckBot->infraredSensor7.finalValue/4000.0;
    
    //std::cout << "E-puck current state vector is (" << firstSection.components[0] << ";" << firstSection.components[1] << ";" << firstSection.components[2] << ";" << secondSection.components[0] << ";" << secondSection.components[1] << ";" << secondSection.components[2] << ";" <<thirdSection.components[0] << ";" << thirdSection.components[1] << ";" << thirdSection.components[2] << ";" << leftSpeed << ";" << rightSpeed << ";" << frontFrontRightSensorValue << ";" << rightSensorValue << ";" << leftSensorValue << ";" << frontFrontLeftSensorValue << ")" << std::endl;
    
    environmentVector[0] = firstSection.components[0];
    environmentVector[1] = firstSection.components[1];
    environmentVector[2] = firstSection.components[2];
    environmentVector[3] = secondSection.components[0];
    environmentVector[4] = secondSection.components[1];
    environmentVector[5] = secondSection.components[2];
    environmentVector[6] = thirdSection.components[0];
    environmentVector[7] = thirdSection.components[1];
    environmentVector[8] = thirdSection.components[2];
    environmentVector[9] = frontFrontLeftSensorValue;
    environmentVector[10] = leftSensorValue;
    environmentVector[11] = rightSensorValue;
    environmentVector[12] = frontFrontRightSensorValue;
    environmentVector[13] = leftSpeed;
    environmentVector[14] = rightSpeed;
}

void TEnkiEnvironment::setRandomEnvironmentState() {
    double xRandomValue;
    double yRandomValue;
    double angleRandomValue;
    srand(time(0));
    xRandomValue = service::uniformDistribution(0.0, xSize);//(xSize)*(((double) rand())/ (double) RAND_MAX);
    srand(time(0));
    yRandomValue = service::uniformDistribution(0.0, ySize);
    srand(time(0));
    angleRandomValue = service::uniformDistribution(0.0, 2*M_PI);
  
    // Нужно проверить, не попадает ли робот в таком случае на установленный куб или например на стену, а если попадает, то сдвинуть его
    // Радиус EPUCK - 3.7 (на всякий случай возьмем 4)
    // Нижеследующие проверки обеспечивают гарантию того, что робот не будет соприкасаться ни со стенами среды, ни с объектами среды
  
    if (xRandomValue >= xSize-5.0) {
      xRandomValue = xRandomValue-5.0;
    }
    if (yRandomValue >= ySize-5.0) {
      yRandomValue = yRandomValue-5.0;
    }
    for (int i=0; i<objectsNumber; i++) {
      if ((xRandomValue >= objectsArray.at(i).x-cubeSize/2.0) && (xRandomValue <= objectsArray.at(i).x+cubeSize/2.0)) {
        xRandomValue = objectsArray.at(i).x-cubeSize/2.0-5.0;
      }
      if ((yRandomValue >= objectsArray.at(i).y-cubeSize/2.0) && (xRandomValue <= objectsArray.at(i).y+cubeSize/2.0)) {
        yRandomValue = objectsArray.at(i).y-cubeSize/2.0-5.0;
      }
    }
  
    ePuckBot->angle = angleRandomValue;
}

int TEnkiEnvironment::forceEnvironment(const std::vector<double>& action) {
  
    // Блок, который вычисляет какое число нужно ставить в массив жизни (в рамках агента)
    int whatToReturn=0;
    for (int i=0; i<objectsNumber; i++) {
      if ((ePuckBot->pos.x>objectsArray[i].x-cubeRadius) && (ePuckBot->pos.x<objectsArray[i].x+cubeRadius)) {
        if ((ePuckBot->pos.y>objectsArray[i].y-cubeRadius) && (ePuckBot->pos.y<objectsArray[i].y+cubeRadius)) {
          if (sqrt((ePuckBot->pos.x-objectsArray[i].x)*(ePuckBot->pos.x-objectsArray[i].x)+(ePuckBot->pos.y-objectsArray[i].y)*(ePuckBot->pos.y-objectsArray[i].y))) {
            // Нужно добавить в некий массив число (i+1), которое как раз и соответствует номеру объекта, в радиусе которого робот сейчас находится
            whatToReturn = i+1;
            break;
          } else {
            whatToReturn = 0;
          }
        }
      }
    }
  
    ePuckBot->leftSpeed = ePuckBot->leftSpeed + action.at(0);
    ePuckBot->rightSpeed = ePuckBot->rightSpeed + action.at(1);
    world->step(worldStep);
    return whatToReturn;
}

std::vector<int> TEnkiEnvironment::testReachingAims(const std::vector< std::vector<double> >& actions, int actionsQuantity) const {
  // Этот метод не будет использован в рамках ENKI
  std::vector<int> someVector;
  someVector.push_back(0);
  return someVector;
}


double TEnkiEnvironment::calculateReward(const std::vector< std::vector<double> >& actions, int actionsQuantity) const {
  //Создаем новый вектор, который в целом будет являться копией старого actions, лишь только с тем изменением, что все нули мы вырежем, так как они определяют лишь границы достижения целей и в подсчете награды никак по сути не участвуют
  std::vector< std::vector<double> > correctedActionsVector;
  
  for (int i = 0; i<actions.size(); i++) {
    if (actions[i][0]!=0) {
      correctedActionsVector.back().push_back(actions[i][0]);
    }
  }
  
	double accumulatedReward = 0.0; // Награда агента
	// Времена последнего достижения цели агентом
	int* achievingTime = new int[goalsNumber];
	// Заполняем времена достижения целей так, чтобы вначале они все имели полную награду
	for (int currentAim = 1; currentAim <= goalsNumber; ++currentAim)
		achievingTime[currentAim - 1] = -rewardRecoveryTime/worldStep - 1;
	// Проходимся по всем действиям агент
	for (int currentActionStep = 1; currentActionStep <= actionsQuantity; ++currentActionStep){
		// Проверяем все цели относительно конкретного шага вермени
		for (int currentAim = 1; currentAim <= goalsNumber; ++currentAim){
			if (goalsArray[currentAim-1].aimComplexity <= currentActionStep){ // Проверяем успел ли бы вообще агент достичь эту цель в начале "жизни" (для экономии времени)
				int achivedFlag = 1; // Признак того, что цель достигнута
				int currentAction = 1; // Текущее проверяемое действие
				// Пока не найдено нарушение последовательности цели или проверка цели закончена
				while (achivedFlag && (currentAction <= goalsArray[currentAim - 1].aimComplexity)){
					// Определение направления изменения и изменяемого бита (с откатыванием времени назад)
					//bool changedDirection = (actions[currentTimeStep - 1 - goalsArray[currentAim - 1].aimComplexity + currentAction][0] > 0);
					//int changedBit = static_cast<int>(fabs(actions[currentTimeStep - 1 - goalsArray[currentAim - 1].aimComplexity + currentAction][0]));
					// Проверяем совпадает ли реальное действие с действием в цели на нужном месте
					//if ((changedBit != goalsArray[currentAim - 1].actionsSequence[currentAction - 1]) ||
          //(changedDirection != goalsArray[currentAim - 1].actionsSequence[currentAction - 1]))
          //achivedFlag = false;
          int visitedObject = actions[currentActionStep - 1 - goalsArray[currentAim-1].aimComplexity + currentAction][0];
          if (visitedObject != goalsArray[currentAim - 1].actionsSequence[currentAction - 1]) {
            achivedFlag = false;
          }
					++currentAction;
				}
				// Если не было нарушения последовательности, то цель достигнута
				if (achivedFlag){
					// Награда линейно восстанавливается до максимального (исходного) уровня за фиксированное кол-во тактов
					accumulatedReward += goalsArray[currentAim - 1].reward * min(1.0, (actions[currentActionStep-1][1] - achievingTime[currentAim - 1])/static_cast<double>(max(1.0, rewardRecoveryTime/worldStep)));
					achievingTime[currentAim - 1] = actions[currentActionStep-1][1];
				}
			} // Конец проверки одной цели
		} // Конец проверки всех целей относительно одного фронта времени
	} // Конец проверки всей "жизни"
	delete []achievingTime;
	return accumulatedReward;
}

#endif
