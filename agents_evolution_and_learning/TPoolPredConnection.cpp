#include "TPoolPredConnection.h"
#include "TNeuralPool.h"

#include <iostream>
#include <fstream>

using namespace std;

// ������ ������������ ����� � ���� ��� �� �����
ostream& operator<<(ostream& ofs, const TPoolPredConnection& poolPredConnection){
	ofs << poolPredConnection.prePool->getID() << "\t" << poolPredConnection.postPool->getID() 
		<< "\t" << poolPredConnection.enabled << "\t" << poolPredConnection.disabledStep
		<< "\t" << poolPredConnection.developPredConnectionProb << "\t" << poolPredConnection.innovationNumber << endl;
	return ofs;
}
