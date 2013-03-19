#include "TTopologyAnalysis.h"

#include <vector>
#include <map>
#include <cstdlib>
#include <string>

using namespace std;

TTopologyAnalysis::TTopologyAnalysis(void){
}

/*std::vector<int> TTopologyAnalysis::getPoolId(int id){
	return idArray[id];
}*/



map< int, vector<int> > TTopologyAnalysis::initializeIdArray(TPoolNetwork *network){
	// номер поколения текущего пула
	int currentGeneration = 1;
	// порядковый номер потомка (как считает родитель)
	int childNumber;
	int currentPool;
	int parentId;
	vector<int> newId;
	// добавляем id для первого пула
	newId.push_back(currentGeneration);
	std::map< int, std::vector<int> > idArray;
	idArray.insert(pair< int, vector<int> >(network->getInputResolution() + network->getOutputResolution() + 1, newId));
	// цикл по внутренним пулам
	for(parentId = network->getInputResolution() + network->getOutputResolution() + 1; parentId <= network->getPoolsQuantity(); ++parentId){
		childNumber = 0;
		// цикл поиска пулов, родителем которых является текущий пул
		for(currentPool = network->getInputResolution() + network->getOutputResolution() + 1; currentPool <= network->getPoolsQuantity(); ++currentPool){
			if(network->getPoolRootPoolID(currentPool) == parentId){
				++childNumber;
				newId.clear();
				currentGeneration = idArray[parentId][0] + 1;
				// указываем поколение потомка
				newId.push_back(currentGeneration);
				// указываем id родителя (все, кроме его поколения)
				for(int i = 1; i < currentGeneration - 1; ++i){
					newId.push_back(idArray[parentId].at(i));
				}
				cout << "new ID: " << currentPool << ", ";
				// указываем порядковый номер потомка
				newId.push_back(childNumber);
				for(unsigned int j = 0; j < newId.size(); j++)
					cout << newId.at(j) << " ";
				cout << endl;
				// добавляем id для текущего пула в массив id сети
				idArray.insert(pair< int, vector<int> >(currentPool, newId));
			}
		}
	}
	return idArray;

}

void printGraphNetworkMGraal(TPoolNetwork *network, map< int, vector<int> > idArray, string graphFilename){
	ofstream hGraalGraphFile;
	hGraalGraphFile.open((graphFilename + ".txt").c_str());
	// Записываем связи
	for (int currentPool = 1; currentPool <= network->getPoolsQuantity(); ++currentPool)
		for (int currentConnection = 1; currentConnection <= network->getPoolInputConnectionsQuantity(currentPool); ++currentConnection)
			if (network->getConnectionEnabled(currentPool, currentConnection)){
				// работает только если у кажого из нейронов менее 10 потомков (одноразрядное число)
				for(unsigned int i = 0; i < idArray[network->getConnectionPrePoolID(currentPool, currentConnection)].size(); ++i){
					hGraalGraphFile << idArray[network->getConnectionPrePoolID(currentPool, currentConnection)][i];
				}
				hGraalGraphFile << " ";
				for(unsigned int i = 0; i < idArray[network->getConnectionPostPoolID(currentPool, currentConnection)].size(); ++i){
					hGraalGraphFile << idArray[network->getConnectionPostPoolID(currentPool, currentConnection)][i];
				}
				hGraalGraphFile << "\n";
			}
	hGraalGraphFile.close();
//	system(("list2leda " + graphFilename + ".txt" + graphFilename + "leda.gw").c_str());
}

TTopologyAnalysis::~TTopologyAnalysis(void){
}
