#include "TTopologyAnalysis.h"

#include <vector>;
#include <map>;

using namespace std;

TTopologyAnalysis::TTopologyAnalysis(void){
}

std::vector<int> TTopologyAnalysis::getPoolId(int id){
	return idArray[id];
}



void TTopologyAnalysis::initializeIdArray(TPoolNetwork *network){
	int currentGeneration = 1;
	int hadGenerationFlagRised;
	int childNumber;
	int currentPool;
	for(int parentId = network->getInputResolution() + network->getOutputResolution() + 1; parentId <= network->getPoolsQuantity(); ++parentId){
		hadGenerationFlagRised = 0;
		childNumber = 0;
		for(currentPool = network->getInputResolution() + network->getOutputResolution() + 1; currentPool <= network->getPoolsQuantity(); ++currentPool){
			if(network->getPoolRootPoolID(currentPool) == parentId){
				if(hadGenerationFlagRised == 0){
					++currentGeneration;
					hadGenerationFlagRised = 1;
				}
				++childNumber;
				vector<int> newId;
				newId.push_back(currentGeneration);
				for(int i = 1; i < currentGeneration - 1; ++i){
					newId.push_back(idArray[parentId].at(i));
				}
				newId.push_back(childNumber);
				idArray.insert(pair<int, vector<int>>(currentPool, newId));
			}
		}
	}
}

TTopologyAnalysis::~TTopologyAnalysis(void){
}
