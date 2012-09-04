#include "TPoolNetwork.h"

#include <iostream>
#include <fstream>

int test(){
	TPoolNetwork PoolNetwork;
	/*PoolNetwork.addPool(1, 0, 1, 0.2, 0.003, 1);
	PoolNetwork.addPool(2, 2, 1, 0.4, 0.004, 3);
	PoolNetwork.addConnection(1, 2, 1, 0.1, 0, 1, 0, 0.5, 1);
	PoolNetwork.addPredConnection(2, 1, 1, 1, 0, 0.6, 1);
	PoolNetwork.addPool(3, 1, 1, 0.8, 0.1, 2);
	printf("%i\t%i\t%i\n", PoolNetwork.getPoolsQuantity(), PoolNetwork.getConnectionsQuantity(), PoolNetwork.getPredConnectionsQuantity());
	printf("%i\t%i\n", PoolNetwork.getPoolID(1), PoolNetwork.getPoolID(2));*/
	//std::cout << PoolNetwork;
	std::ifstream ifs("C:/Runs/net.txt");
	ifs >> PoolNetwork;
	ifs.close();

	std::ofstream ofs("C:/Runs/net1.txt");
	ofs << PoolNetwork;
	ofs.close();
	return 0;
}

int main(int argc, char** argv){
	test();
}
