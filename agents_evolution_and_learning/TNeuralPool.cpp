#include "TNeuralPool.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"

#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;

// Печать сведений о пуле в файл или на экран
ostream& operator<<(ostream& os, const TNeuralPool& neuralPool){
	os << neuralPool.type << "\t" << neuralPool.capacity << "\t" << neuralPool.biasMean
		<<	"\t" << neuralPool.biasVariance << "\t" << neuralPool.layer << endl;
	return os;
}

// Печать всех сведений о пуле в файл или на экран (вместе с номером пула родителя и временем появления в эволюции)
std::ostream& TNeuralPool::printPoolExtra(std::ostream& os) const{
	os << type << "\t" << capacity << "\t" << biasMean
		<<	"\t" << biasVariance << "\t" << layer << "\t" << rootPoolID << "\t" << appearenceEvolutionTime << endl;
	return os;
}