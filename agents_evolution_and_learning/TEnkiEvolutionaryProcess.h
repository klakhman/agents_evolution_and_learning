//
//  TEnkiEvolutionaryProcess.h
//  Agents Evolution And Learning ENKI
//
//  Created by Сергей Муратов on 10.03.13.
//  Copyright (c) 2013 Laboratory. All rights reserved.
//

#ifndef TENKIEVOLUTIONARYPROCESS_H
#define TENKIEVOLUTIONARYPROCESS_H

#include <iostream>
#include "TEvolutionaryProcess.h"
#include "TEnkiAgent.h"
#include "TEnkiEnvironment.h"

class TEnkiEvolutionaryProcess:public TEvolutionaryProcess
{
  public:
    TPopulation<TEnkiAgent> *agentsPopulation;
  
    void createMainResultsFile(std::ofstream& resultsFile, unsigned int randomSeed);
    void start(unsigned int randomSeed = 0);
  	void makeLogNote(std::ostream& outputConsole, std::ostream& bestAgentsConsole, TPopulation<TEnkiAgent>* bestPopulation, int currentEvolutionStep /*=0*/);
};

#endif /* defined(__Agents_Evolution_And_Learning_ENKI__TEnkiEvolutionaryProcess__) */
