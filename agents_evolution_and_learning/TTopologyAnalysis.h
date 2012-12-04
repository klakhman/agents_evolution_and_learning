#ifndef TTopologyAnalysis_H
#define TTopologyAnalysis_H

#include "TPoolNetwork.h"

#include <vector>
#include <map>

class TTopologyAnalysis{
	std::map<int, std::vector<int>> idArray;
public:
	TTopologyAnalysis(void);

	std::vector<int> getPoolId(int id);

	void initializeIdArray(TPoolNetwork* network);

	~TTopologyAnalysis(void);
};

#endif