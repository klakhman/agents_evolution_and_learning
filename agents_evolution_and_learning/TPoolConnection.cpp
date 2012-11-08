#include "TPoolConnection.h"
#include "TNeuralPool.h"

#include <iostream>
#include <fstream>

using namespace std;

// Запись связи в файл
ostream& operator<<(ostream& ofs, const TPoolConnection& poolConnection){
	ofs << poolConnection.prePool->getID() << "\t" << poolConnection.postPool->getID() 
		<< "\t" << poolConnection.weightMean << "\t" << poolConnection.weightVariance
		<< "\t" << poolConnection.enabled << "\t" << poolConnection.disabledStep
		<< "\t" << poolConnection.developSynapseProb << "\t" << poolConnection.innovationNumber << endl;
	return ofs;
}
