#ifndef TTopologyAnalysis_H
#define TTopologyAnalysis_H

#include "TPoolNetwork.h"

#include <vector>
#include <map>

// класс анализа топологии агента
class TTopologyAnalysis{
	
public:
	TTopologyAnalysis(void);

	// получить новый id для пула (необходимо предварительно инициализировать массив idArray)
//	std::vector<int> getPoolId(int id);

	// инициализация массива новых id для сети
	static std::map< int, std::vector<int> > initializeIdArray(TPoolNetwork* network);

  static void printGraphNetworkMGraal(TPoolNetwork *network, std::map< int, std::vector<int> > idArray, std::string graphFilename);

	~TTopologyAnalysis(void);
};

#endif