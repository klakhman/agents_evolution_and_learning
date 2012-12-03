#include "TPoolPredConnection.h"

#include <iostream>
#include <fstream>

using namespace std;

// Запись предикторной связи в файл или на экран
ostream& operator<<(ostream& ofs, const TPoolPredConnection& poolPredConnection){
	ofs << poolPredConnection.prePoolID << "\t" << poolPredConnection.postPoolID 
		<< "\t" << poolPredConnection.enabled << "\t" << poolPredConnection.disabledStep
		<< "\t" << poolPredConnection.developPredConnectionProb << "\t" << poolPredConnection.innovationNumber << endl;
	return ofs;
}
