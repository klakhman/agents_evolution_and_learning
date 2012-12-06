#ifndef TTopologyAnalysis_H
#define TTopologyAnalysis_H

#include "TPoolNetwork.h"

#include <vector>
#include <map>

// класс анализа топологии агента
class TTopologyAnalysis{
	std::map< int, std::vector<int> > idArray;
public:
	TTopologyAnalysis(void);

	// получить новый id для пула (необходимо предварительно инициализировать массив idArray)
	std::vector<int> getPoolId(int id);

	// инициализация массива новых id для сети
	void initializeIdArray(TPoolNetwork* network);

	~TTopologyAnalysis(void);
};

#endif