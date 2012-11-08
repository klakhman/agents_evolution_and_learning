#include "TPredConnection.h"
#include "TNeuron.h"

#include <iostream>
#include <fstream>

using namespace std;

// Запись предикторной связи в файл или на экран
ostream& operator<<(ostream& os, const TPredConnection& predConnection){
	os << predConnection.preNeuron->getID() << "\t" << predConnection.postNeuron->getID() << "\t" << predConnection.enabled  << endl;
	return os;
}
