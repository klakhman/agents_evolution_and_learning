#include "TSynapse.h"
#include "TNeuron.h"

#include <iostream>
#include <fstream>

using namespace std;

// ������ ����� � ����
ostream& operator<<(ostream& os, const TSynapse& synapse){
	os << synapse.preNeuron->getID() << "\t" << synapse.postNeuron->getID() 
		<< "\t" << synapse.weight << "\t" << synapse.enabled << endl;
	return os;
}
