#include "TAgent.h"
#include "watchers.h"
#include <fstream>

using namespace std;

void watchers::ActNeuronsWatcher::watch(const TAgent& agent, const TEnvironment& env){
  unsigned int activeNeuronsQ = 0;
  TNeuralNetwork* controller = agent.getPointerToAgentController();
  for (unsigned int neuronN = 1; neuronN <= controller->getNeuronsQuantity(); ++neuronN)
    controller += controller->getNeuronActive(neuronN);
  actNeuronsQ.push_back(activeNeuronsQ);
}

void watchers::ActNeuronsWatcher::print(const string& filename){
  ofstream output(filename.c_str());
  for (unsigned int step = 0; step < actNeuronsQ.size(); ++step)
    output << step << "\t" << actNeuronsQ[step] << endl;
  output.close();
}

void watchers::ActNeuronsWatcher::clear(){
  actNeuronsQ.clear();
}