#include "TTopologyAnalysis.h"

#include <vector>
#include <map>

using namespace std;

TTopologyAnalysis::TTopologyAnalysis(void){
}

std::vector<int> TTopologyAnalysis::getPoolId(int id){
	return idArray[id];
}



void TTopologyAnalysis::initializeIdArray(TPoolNetwork *network){
	// ����� ��������� �������� ����
	int currentGeneration = 1;
	// ���������� ����� ������� (��� ������� ��������)
	int childNumber;
	int currentPool;
	int parentId;
	vector<int> newId;
	// ��������� id ��� ������� ����
	newId.push_back(currentGeneration);
	idArray.insert(pair<int, vector<int>>(network->getInputResolution() + network->getOutputResolution() + 1, newId));
	// ���� �� ���������� �����
	for(parentId = network->getInputResolution() + network->getOutputResolution() + 1; parentId <= network->getPoolsQuantity(); ++parentId){
		childNumber = 0;
		// ���� ������ �����, ��������� ������� �������� ������� ���
		for(currentPool = network->getInputResolution() + network->getOutputResolution() + 1; currentPool <= network->getPoolsQuantity(); ++currentPool){
			if(network->getPoolRootPoolID(currentPool) == parentId){
				++childNumber;
				newId.clear();
				currentGeneration = idArray.at(parentId)[0] + 1;
				// ��������� ��������� �������
				newId.push_back(currentGeneration);
				// ��������� id �������� (���, ����� ��� ���������)
				for(int i = 1; i < currentGeneration - 1; ++i){
					newId.push_back(idArray[parentId].at(i));
				}
				cout << "new ID: " << currentPool << ", ";
				// ��������� ���������� ����� �������
				newId.push_back(childNumber);
				for(unsigned int j = 0; j < newId.size(); j++)
					cout << newId.at(j) << " ";
				cout << endl;
				// ��������� id ��� �������� ���� � ������ id ����
				idArray.insert(pair<int, vector<int>>(currentPool, newId));
			}
		}
	}
}

TTopologyAnalysis::~TTopologyAnalysis(void){
}
