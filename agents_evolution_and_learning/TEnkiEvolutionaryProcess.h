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
#include <string>

class TEnkiEvolutionaryProcess:public TEvolutionaryProcess
{
  public:
    TPopulation<TEnkiAgent> *agentsPopulation;

	TEnkiEvolutionaryProcess(){agentsPopulation = 0;}
  
    void createMainResultsFile(std::ofstream& resultsFile, unsigned int randomSeed);
    void start(unsigned int randomSeed = 0, const std::string initPopulationFilename = "");
  	void makeLogNote(std::ostream& outputConsole, std::ostream& bestAgentsConsole, TPopulation<TEnkiAgent>* bestPopulation, int currentEvolutionStep /*=0*/);

	static void makeResultsOverview(const std::string& dir, unsigned int evolutionDuration, const std::string& maxRewardFilename, const std::string& averageRewardFilename);
};

#endif /* defined(__Agents_Evolution_And_Learning_ENKI__TEnkiEvolutionaryProcess__) */
