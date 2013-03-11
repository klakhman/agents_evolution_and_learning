//
//  TEnkiAgent.h
//  enki
//
//  Created by Сергей Муратов on 03.03.13.
//
//

#ifndef TENKIAGENT_H
#define TENKIAGENT_H

#include <iostream>
#include "TAgent.h"
#include "config.h"
#include <vector>

class TEnkiAgent:public TAgent {
  public:
  int getActionResolution() const {
    return 2;
  }
  int calculateOutputResolution(int inputResolution) const {
    return 4;
  }
  std::vector<double> decodeAction(double outputVector[]);
  void life(TEnvironment& environment, int agentLifeTime, bool rewardCalculate = true);
};


#endif /* defined(__enki__TEnkiAgent__) */
