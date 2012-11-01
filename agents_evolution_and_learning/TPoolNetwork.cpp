#include "TPoolNetwork.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"
#include "service.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Корректировка ID пулов (например после удаления)
void TPoolNetwork::fixPoolsIDs(){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		poolsStructure[currentPool - 1]->setID(currentPool);
}

// Корректировка ID связей (например после удаления)
void TPoolNetwork::fixConnectionsIDs(){
	int _connectionsQuantity = 0;
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			poolsStructure[currentPool - 1]->setConnectionID(currentConnection, ++_connectionsQuantity);
}

// Корректировка ID предикторных связей (например после удаления)
void TPoolNetwork::fixPredConnectionsIDs(){
	int _predConnectionsQuantity = 0;
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			poolsStructure[currentPool - 1]->setPredConnectionID(currentPredConnection, ++_predConnectionsQuantity);
}

// Процедура увеличения размера массива пулов
void TPoolNetwork::inflatePoolsStructure(int inflateSize){
	TNeuralPool** newPoolsStructure = new TNeuralPool*[poolsStructureSize + inflateSize];
	memset(newPoolsStructure, 0, (poolsStructureSize + inflateSize) * sizeof(TNeuralPool*));
	memcpy(newPoolsStructure, poolsStructure, poolsStructureSize * sizeof(TNeuralPool*));
	delete []poolsStructure;
	poolsStructure = newPoolsStructure;
	poolsStructureSize += inflateSize;
}

// Декструктор
TPoolNetwork::~TPoolNetwork(){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		delete poolsStructure[currentPool - 1];
	delete []poolsStructure;
}

//Добавление нейронального пула в сеть
void TPoolNetwork::addPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer){
	if (poolsQuantity >= poolsStructureSize) // Если не хватает места в массиве
		inflatePoolsStructure(INFLATE_POOLS_SIZE);
	poolsStructure[poolsQuantity++] = new TNeuralPool(newID, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
	// Если пул входной
	if (0 == newType) ++inputResolution;
	// Если пул выходной
	if (2 == newType) ++outputResolution;
	// Смотрим на слой пула
	if (newLayer > layersQuantity) layersQuantity = newLayer;
}

// Удаление пула из сети (с удалением также всех входных и выходных связей из этого пула)
void TPoolNetwork::deletePool(int poolNumber){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		if (currentPool != poolNumber){ // Если это не стираемый пул (пропускаем просто для экономии времени)
			for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
				if (poolsStructure[currentPool - 1]->getConnectionPrePool(currentConnection)->getID() == poolNumber){ // Если это связь от стираемого пула
					deleteConnection(currentPool, currentConnection);
					--currentConnection; // Чтобы мы остались на той же самой позиции
				}
			for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
				if (poolsStructure[currentPool - 1]->getPredConnectionPrePool(currentPredConnection)->getID() == poolNumber){ // Если это пред. связь от стираемого пула
					deletePredConnection(currentPool, currentPredConnection);
					--currentPredConnection;
				}
		}
	// Если пул входной
	if (0 == poolsStructure[poolNumber - 1]->getType()) --inputResolution;
	// Если пул выходной
	if (2 == poolsStructure[poolNumber - 1]->getType()) --outputResolution;
	// Стираем пул и сдвигаем массив
	connectionsQuantity -= poolsStructure[poolNumber - 1]->getInputConnectionsQuantity();
	predConnectionsQuantity -= poolsStructure[poolNumber - 1]->getInputPredConnectionsQuantity();
	delete poolsStructure[poolNumber - 1];
	for (int currentPool = poolNumber - 1; currentPool < poolsQuantity - 1; ++currentPool)
		poolsStructure[currentPool] = poolsStructure[currentPool + 1];
	poolsStructure[poolsQuantity - 1] = 0;
	--poolsQuantity;
	fixPoolsIDs();
}

// Проверка существования связи в сети
bool TPoolNetwork::checkConnectionExistance(int prePoolNumber, int postPoolNumber){
	bool check = false;
	for (int currentPoolConnection = 1; currentPoolConnection <= poolsStructure[postPoolNumber - 1]->getInputConnectionsQuantity(); ++currentPoolConnection)
		if (poolsStructure[postPoolNumber - 1]->getConnectionPrePool(currentPoolConnection)->getID() == prePoolNumber)
			return true;

	return false;
}

// Проверка существования предикторной связи в сети
bool TPoolNetwork::checkPredConnectionExistance(int prePoolNumber, int postPoolNumber){
	for (int currentPoolPredConnection = 1; currentPoolPredConnection <= poolsStructure[postPoolNumber - 1]->getInputPredConnectionsQuantity(); ++currentPoolPredConnection)
		if (poolsStructure[postPoolNumber - 1]->getPredConnectionPrePool(currentPoolPredConnection)->getID() == prePoolNumber)
			return true;

	return false;
}

// Стирание сети
void TPoolNetwork::erasePoolNetwork(){
	if (poolsStructure){
		for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
			delete poolsStructure[currentPool - 1];
		delete []poolsStructure;
		poolsStructure = 0;
		poolsStructureSize = 0;
	}
	poolsQuantity = 0;
	connectionsQuantity = 0;
	predConnectionsQuantity = 0;
	layersQuantity = 0;
	inputResolution = 0;
	outputResolution = 0;
}

// Оператор присваивания (фактически полное копирование сети - создание новых структур)
TPoolNetwork& TPoolNetwork::operator=(const TPoolNetwork& sourcePoolNetwork){
	erasePoolNetwork(); // На всякий случай опустошаем сеть
	// Копируем пулы
	for (int currentPool = 1; currentPool <= sourcePoolNetwork.getPoolsQuantity(); ++currentPool)
		addPool(sourcePoolNetwork.getPoolID(currentPool), sourcePoolNetwork.getPoolType(currentPool), sourcePoolNetwork.getPoolCapacity(currentPool),
					sourcePoolNetwork.getPoolBiasMean(currentPool), sourcePoolNetwork.getPoolBiasVariance(currentPool), sourcePoolNetwork.getPoolLayer(currentPool));
	// Копируем все связи сети
	for (int currentPool = 1; currentPool <= sourcePoolNetwork.getPoolsQuantity(); ++currentPool){
		for (int currentPoolConnection = 1; currentPoolConnection <= sourcePoolNetwork.getPoolInputConnectionsQuantity(currentPool); ++ currentPoolConnection)
			addConnection(sourcePoolNetwork.getConnectionPrePool(currentPool, currentPoolConnection)->getID(), sourcePoolNetwork.getConnectionPostPool(currentPool, currentPoolConnection)->getID(),
								sourcePoolNetwork.getConnectionID(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionWeightMean(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionWeightVariance(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionEnabled(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionDisabledStep(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionDevelopSynapseProb(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionInnovationNumber(currentPool, currentPoolConnection));
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= sourcePoolNetwork.getPoolInputPredConnectionsQuantity(currentPool); ++ currentPoolPredConnection)
			addPredConnection(sourcePoolNetwork.getPredConnectionPrePool(currentPool, currentPoolPredConnection)->getID(), sourcePoolNetwork.getPredConnectionPostPool(currentPool, currentPoolPredConnection)->getID(),
								sourcePoolNetwork.getPredConnectionID(currentPool, currentPoolPredConnection), sourcePoolNetwork.getPredConnectionEnabled(currentPool, currentPoolPredConnection),
								sourcePoolNetwork.getPredConnectionDisabledStep(currentPool, currentPoolPredConnection), sourcePoolNetwork.getDevelopPredConnectionProb(currentPool, currentPoolPredConnection),
								sourcePoolNetwork.getPredConnectionInnovationNumber(currentPool, currentPoolPredConnection));
	}
	return *this;
}

// Вывод сети в файл как графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
void TPoolNetwork::printGraphNetwork(string graphFilename){
	ofstream hDotGraphFile;
	hDotGraphFile.open((graphFilename + ".dot").c_str());
	// Инициализируем и указываем, что слои должны следовать слево на право
	hDotGraphFile << "digraph G { \n\trankdir=LR;\n";
	// Записываем пулы послойно
	for (int currentLayer = 1; currentLayer <= layersQuantity; ++ currentLayer){
		// Указываем, что это один слой и пулы должны идти в столбец
		hDotGraphFile << "\t{ rank = same; \n";
		for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
			if (poolsStructure[currentPool - 1]->getLayer() == currentLayer)
				hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "\" [label=\"" << poolsStructure[currentPool - 1]->getID() << "\", shape = \"circle\"];\n" ;
		hDotGraphFile << "\t}\n"; // Заканчиваем запись слоя
	}
	// Записываем связи
	double maxWeightValue = 1.0;
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			if (poolsStructure[currentPool - 1]->getConnectionEnabled(currentConnection)){
				string hex;
				service::decToHex(static_cast<int>(min(fabs(255 * poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) / maxWeightValue), 255.0)), hex, 2);
				string color;
				if (poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) < 0)
					color = "0000" + hex; // Оттенок синего
				else
					color = hex + "0000"; // Оттенок красного
				/*hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getConnectionPrePool(currentConnection)->getID() << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getConnectionPostPool(currentConnection)->getID() << "\" [label=\"" << poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) << 
					"\", arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";*/
				hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getConnectionPrePool(currentConnection)->getID() << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getConnectionPostPool(currentConnection)->getID() << "\" [ arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";
			}
	// Записываем предикторные связи
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			if (poolsStructure[currentPool - 1]->getPredConnectionEnabled(currentPredConnection))
				hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getPredConnectionPrePool(currentPredConnection)->getID() << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getPredConnectionPostPool(currentPredConnection)->getID() << "\" [style=dashed, arrowsize=0.7, color=\"#000000\", penwidth=2.0];\n";
	hDotGraphFile << "}";
	hDotGraphFile.close();
	system(("dot -Tjpg " + graphFilename + ".dot -o " + graphFilename).c_str());
}

//Печать сети в файл или на экран
ostream& operator<<(ostream& os, const TPoolNetwork& poolNetwork){
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		os << *(poolNetwork.poolsStructure[currentPool - 1]);
	os << "|\n"; // Записываем разделитель между пулами и связями
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			os << *(poolNetwork.poolsStructure[currentPool-1]->connectednessSet[currentConnection-1]); 
	os << "||\n"; // Записываем разделитель между связями и предикторными связями
	for (int currentPool = 1; currentPool <= poolNetwork.poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolNetwork.poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			os << *(poolNetwork.poolsStructure[currentPool-1]->predConnectednessSet[currentPredConnection-1]); 
	os << "|||\n"; // Записываем разделитель между сетями
	return os;
}

//Считывание сети из файла или экрана
istream& operator>>(istream& is, TPoolNetwork& poolNetwork){
	poolNetwork.erasePoolNetwork(); // На всякий случай опустошаем сеть
	string tmp_string;
	// Создаем все пулы сети
	is >> tmp_string; // Считываем тип пула
	while (tmp_string != "|"){ // Считываем до разделителя между пулами и связями
		int newType = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем объем пула
		int newCapacity = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем среднее смещения нейронов пула
		double newBiasMean = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем дисперсию смещения нейронов пула
		double newBiasVariance = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем слой пула
		int newLayer = atoi(tmp_string.c_str());
		poolNetwork.addPool(poolNetwork.getPoolsQuantity() + 1, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
		is >> tmp_string; // Считываем типа пула
	}
	// Создаем все связи между пулами
	is >> tmp_string; // Считываем номер пресинаптического пула
	while (tmp_string != "||"){ // Считываем до разделителя между связями и предикторными связями
		int prePoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем номер постсинаптического пула
		int postPoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем среднее веса связи
		double newWeightMean = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем дисперсию веса связи
		double newWeightVariance = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем признак экспресии связи
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		is >> tmp_string; // Считываем так выключения связи
		int newDisabledStep = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем вероятность образования синапса по этой связи
		double newDevelopSynapseProb = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем номер инновации этой связи
		long newInnovationNumber = atoi(tmp_string.c_str());
		poolNetwork.addConnection(prePoolNumber, postPoolNumber, poolNetwork.getConnectionsQuantity() + 1, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber);
		is >> tmp_string; // Считываем номер пресинаптического пула
	}
	// Создаем все предикторные связи между пулами
	is >> tmp_string; // Считываем номер пресинаптического пула
	while (tmp_string != "|||"){ // Считываем до разделителя между предикторными связями и концом сети
		int prePoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем номер постсинаптического пула
		int postPoolNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем признак экспресии связи
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		is >> tmp_string; // Считываем так выключения связи
		int newDisabledStep = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем вероятность образования предикторной связи по этой связи
		double newDevelopPredConnectionProb = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем номер инновации этой связи
		long newInnovationNumber = atoi(tmp_string.c_str());
		poolNetwork.addPredConnection(prePoolNumber, postPoolNumber, poolNetwork.getPredConnectionsQuantity() + 1, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber);
		is >> tmp_string; // Считываем номер пресинаптического пула
	}

	return is;
}

