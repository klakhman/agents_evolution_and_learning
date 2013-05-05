//
//  TEnkiAgent.cpp
//  enki
//
//  Created by Сергей Муратов on 03.03.13.
//
//

#include "TEnkiAgent.h"

using namespace std;

static const double deltaSpeedForWheels = 1.0;

vector<double> TEnkiAgent::decodeAction(double outputVector[]) {
  vector<double> actionVector;
  actionVector.push_back((outputVector[0]-outputVector[1])*deltaSpeedForWheels);
  actionVector.push_back((outputVector[2]-outputVector[3])*deltaSpeedForWheels);
  return actionVector;
}

void TEnkiAgent::life(TEnvironment& environment, int agentLifeTime, bool rewardCalculate /*= true*/){
	double* environmentVector = new double[neuralController->getInputResolution()];
	double* outputVector = new double[neuralController->getOutputResolution()];
  vector<double> actionVector(getActionResolution());
  
  agentLife.erase(agentLife.begin(), agentLife.end());
  vector<double> firstVector;
  firstVector.push_back(0);
  firstVector.push_back(0);
  agentLife.push_back(firstVector);
  
	neuralController->reset();
  for (int agentLifeStep = 1; agentLifeStep <= agentLifeTime; ++agentLifeStep) {
    environment.getCurrentEnvironmentVector(environmentVector);
    neuralController->calculateNetwork(environmentVector);
    neuralController->getOutputVector(outputVector);
    actionVector = decodeAction(outputVector);
    // Действуем на среду, проверяем, успешно ли действие, а также проверяем, что это действие новое, так как в противном случае это действие не несет никакой новой информации
    int actionSuccess = environment.forceEnvironment(actionVector);
    double previousActionSuccess = agentLife[agentLife.size()-1][0];

    if (static_cast<int>(previousActionSuccess) != actionSuccess) {      
      vector<double> vectorWithSuccessAndTime;
      vectorWithSuccessAndTime.push_back(actionSuccess);
      vectorWithSuccessAndTime.push_back(agentLifeStep);
      agentLife.push_back(vectorWithSuccessAndTime);
    }
    //agentLife[agentLifeTime - 1][0] = actionSuccess;
    // Проводим процедуру обучения (если такой режим)
    if (1 == learningSettings.learningMode) learning();
    // !!!! Случайное обучение - только для контроля качества обучения !!!!!
    else if (2 == learningSettings.learningMode) randomLearning();
  }
  
	if (rewardCalculate)
		reward = environment.calculateReward(agentLife, static_cast<int>(agentLife.size()));
	else
		reward = 0;
	delete []outputVector;
	delete []environmentVector;
}
