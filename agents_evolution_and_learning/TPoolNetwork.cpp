#include "TPoolNetwork.h"
#include "TPoolConnection.h"
#include "TPoolPredConnection.h"
#include "service.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>


using namespace std;

// Нахождение номера связи в структуре постсинаптического пула - возвращает ноль, если связи нет
int TPoolNetwork::findConnectionNumber(int prePoolNumber, int postPoolNumber){
	for (int currentPoolConnection = 1; currentPoolConnection <= poolsStructure[postPoolNumber - 1]->getInputConnectionsQuantity(); ++currentPoolConnection)
		if (poolsStructure[postPoolNumber - 1]->getConnectionPrePoolID(currentPoolConnection) == prePoolNumber)
			return currentPoolConnection;
	return 0;
}

// Нахождение номера предикторной связи в структуре постсинаптического пула - возвращает ноль, если предикторной связи нет
int TPoolNetwork::findPredConnectionNumber(int prePoolNumber, int postPoolNumber){
	for (int currentPoolPredConnection = 1; currentPoolPredConnection <= poolsStructure[postPoolNumber - 1]->getInputPredConnectionsQuantity(); ++currentPoolPredConnection)
		if (poolsStructure[postPoolNumber - 1]->getPredConnectionPrePoolID(currentPoolPredConnection) == prePoolNumber)
			return currentPoolPredConnection;
	return 0;
}

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
void TPoolNetwork::addPool(int newType, int newLayer, double newBiasMean, double newBiasVariance /*=0*/, int newCapacity /*=1*/){
	if (poolsQuantity >= poolsStructureSize) // Если не хватает места в массиве
		inflatePoolsStructure(INFLATE_POOLS_SIZE);
	poolsStructure[poolsQuantity] = new TNeuralPool(poolsQuantity + 1, newType, newLayer, newBiasMean, newBiasVariance, newCapacity);
	++poolsQuantity;
	// Если пул входной
	if (0 == newType) ++inputResolution;
	// Если пул выходной
	if (2 == newType) ++outputResolution;
	// Смотрим на слой пула
	if (newLayer > layersQuantity) layersQuantity = newLayer;
}

// Удаление пула из сети (с удалением также всех входных и выходных связей из этого пула + корректировка ссылок на все пулы которые идут после него - так как теперь в связях не ссылки а номера - долгая операция !!!)
void TPoolNetwork::deletePool(int poolNumber){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		if (currentPool != poolNumber){ // Если это не стираемый пул (пропускаем просто для экономии времени)
			for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
				if (poolsStructure[currentPool - 1]->getConnectionPrePoolID(currentConnection) == poolNumber){ // Если это связь от стираемого пула
					deleteConnection(currentPool, currentConnection);
					break; // Выходим из цикла, так как у одного пула не может быть более одной связи от другого пула
				}
			for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
				if (poolsStructure[currentPool - 1]->getPredConnectionPrePoolID(currentPredConnection) == poolNumber){ // Если это пред. связь от стираемого пула
					deletePredConnection(currentPool, currentPredConnection);
					break;
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
	// Теперь нужно поправить ссылки на все пулы после удаленного в записях связей
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection){
			if (poolsStructure[currentPool - 1]->getConnectionPrePoolID(currentConnection) > poolNumber)
				poolsStructure[currentPool - 1]->setConnectionPrePoolID(currentConnection, poolsStructure[currentPool - 1]->getConnectionPrePoolID(currentConnection) - 1);
			if (poolsStructure[currentPool - 1]->getConnectionPostPoolID(currentConnection) > poolNumber)
				poolsStructure[currentPool - 1]->setConnectionPostPoolID(currentConnection, poolsStructure[currentPool - 1]->getConnectionPostPoolID(currentConnection) - 1);
		}
		for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection){
			if (poolsStructure[currentPool - 1]->getPredConnectionPrePoolID(currentPredConnection) > poolNumber)
				poolsStructure[currentPool - 1]->setPredConnectionPrePoolID(currentPredConnection, poolsStructure[currentPool - 1]->getPredConnectionPrePoolID(currentPredConnection) - 1);
			if (poolsStructure[currentPool - 1]->getPredConnectionPostPoolID(currentPredConnection) > poolNumber)
				poolsStructure[currentPool - 1]->setPredConnectionPostPoolID(currentPredConnection, poolsStructure[currentPool - 1]->getPredConnectionPostPoolID(currentPredConnection) - 1);
		}
	}
}

//Удаление связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
void TPoolNetwork::deleteConnection(int poolNumber, int connectionNumber, bool fixIDs /*= true*/){
	poolsStructure[poolNumber-1]->deleteConnection(connectionNumber);
	if (fixIDs){
		for (int currentPoolConnection = connectionNumber; currentPoolConnection <= poolsStructure[poolNumber - 1]->getInputConnectionsQuantity(); ++currentPoolConnection)
			poolsStructure[poolNumber - 1]->setConnectionID(currentPoolConnection, poolsStructure[poolNumber - 1]->getConnectionID(currentPoolConnection) - 1);
		for (int currentPool = poolNumber + 1; currentPool <= poolsQuantity; ++currentPool)
			for (int currentPoolConnection = 1; currentPoolConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentPoolConnection)
				poolsStructure[currentPool - 1]->setConnectionID(currentPoolConnection, poolsStructure[currentPool - 1]->getConnectionID(currentPoolConnection) - 1);
	}
	--connectionsQuantity;
}

// Удаление предикторной связи из сети
void TPoolNetwork::deletePredConnection(int poolNumber, int predConnectionNumber, bool fixIDs /*= true*/){
	poolsStructure[poolNumber-1]->deletePredConnection(predConnectionNumber);
	if (fixIDs){
		for (int currentPoolPredConnection = predConnectionNumber; currentPoolPredConnection <= poolsStructure[poolNumber - 1]->getInputPredConnectionsQuantity(); ++currentPoolPredConnection)
			poolsStructure[poolNumber - 1]->setPredConnectionID(currentPoolPredConnection, poolsStructure[poolNumber - 1]->getPredConnectionID(currentPoolPredConnection) - 1);
		for (int currentPool = poolNumber + 1; currentPool <= poolsQuantity; ++currentPool)
			for (int currentPoolPredConnection = 1; currentPoolPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPoolPredConnection)
				poolsStructure[currentPool - 1]->setPredConnectionID(currentPoolPredConnection, poolsStructure[currentPool - 1]->getPredConnectionID(currentPoolPredConnection) - 1);
	}
	--predConnectionsQuantity;
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
	for (int currentPool = 1; currentPool <= sourcePoolNetwork.getPoolsQuantity(); ++currentPool){
		addPool(sourcePoolNetwork.getPoolType(currentPool), sourcePoolNetwork.getPoolLayer(currentPool),
					sourcePoolNetwork.getPoolBiasMean(currentPool), sourcePoolNetwork.getPoolBiasVariance(currentPool), sourcePoolNetwork.getPoolCapacity(currentPool));
		setPoolRootPoolID(currentPool, sourcePoolNetwork.getPoolRootPoolID(currentPool));
		setPoolAppearenceEvolutionTime(currentPool, sourcePoolNetwork.getPoolAppearenceEvolutionTime(currentPool));
	}
	// Копируем все связи сети
	for (int currentPool = 1; currentPool <= sourcePoolNetwork.getPoolsQuantity(); ++currentPool){
		for (int currentPoolConnection = 1; currentPoolConnection <= sourcePoolNetwork.getPoolInputConnectionsQuantity(currentPool); ++ currentPoolConnection)
			addConnection(sourcePoolNetwork.getConnectionPrePoolID(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionPostPoolID(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionWeightMean(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionWeightVariance(currentPool, currentPoolConnection),  
								sourcePoolNetwork.getConnectionDevelopSynapseProb(currentPool, currentPoolConnection), 
								sourcePoolNetwork.getConnectionEnabled(currentPool, currentPoolConnection), sourcePoolNetwork.getConnectionDisabledStep(currentPool, currentPoolConnection),
								sourcePoolNetwork.getConnectionInnovationNumber(currentPool, currentPoolConnection));
		for (int currentPoolPredConnection = 1; currentPoolPredConnection <= sourcePoolNetwork.getPoolInputPredConnectionsQuantity(currentPool); ++ currentPoolPredConnection)
			addPredConnection(sourcePoolNetwork.getPredConnectionPrePoolID(currentPool, currentPoolPredConnection), sourcePoolNetwork.getPredConnectionPostPoolID(currentPool, currentPoolPredConnection),
								sourcePoolNetwork.getDevelopPredConnectionProb(currentPool, currentPoolPredConnection), 
								sourcePoolNetwork.getPredConnectionEnabled(currentPool, currentPoolPredConnection), sourcePoolNetwork.getPredConnectionDisabledStep(currentPool, currentPoolPredConnection),
								sourcePoolNetwork.getPredConnectionInnovationNumber(currentPool, currentPoolPredConnection));
	}
	return *this;
}

// Вывод сети в файл как графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
void TPoolNetwork::printGraphNetwork(string graphFilename, bool printWeights /*=false*/) const{
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
					color = "0000FF" + hex; // Оттенок синего
				else
					color = "FF0000" + hex; // Оттенок красного
				hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getConnectionPrePoolID(currentConnection) << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getConnectionPostPoolID(currentConnection) << "\" [ ";
				if (printWeights) //  Если надо напечатать веса
					hDotGraphFile << "label=\"" << poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) << "\", ";
				hDotGraphFile <<"arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";
			}
	// Записываем предикторные связи
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			if (poolsStructure[currentPool - 1]->getPredConnectionEnabled(currentPredConnection))
				hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getPredConnectionPrePoolID(currentPredConnection) << "\" -> \"" <<
					poolsStructure[currentPool - 1]->getPredConnectionPostPoolID(currentPredConnection) << "\" [style=dashed, arrowsize=0.7, color=\"#000000\", penwidth=2.0];\n";
	hDotGraphFile << "}";
	hDotGraphFile.close();
	system(("dot -Tjpg " + graphFilename + ".dot -o " + graphFilename).c_str());
}

// Вывод сети в файл как супер-графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
void TPoolNetwork::printGraphNetworkAlternative(string graphFilename, int scale, int genealogy) const{
	ofstream hDotGraphFile;
	hDotGraphFile.open((graphFilename + ".dot").c_str());
	// Инициализируем
	hDotGraphFile << "digraph G { \n\t\n";

	// строка вида: x,y
	std::vector<string> nodePosition;

	string nodeShape;

	// массивы с координатами пулов, индексы в массиве соответствуют Id пула
	std::vector<int> nodeXPosition;			
	std::vector<int> nodeYPosition;

	// костыль: чтобы индексы в массивах соответствовали id пулов, нужно их заполнять последовательно. Т.к. внутренний идет последним по id, его actualCurrentType = 2
	int actualCurrentType;					

	// Записываем пулы по типам (входной, скрытый, выходной)
	for (int currentType = 0; currentType <= 2; ++currentType){
//		hDotGraphFile << "\t{ \n";

		// Задаем форму для текущего типа, и присваиваем значение костылю
		// Входной
		if(currentType == 0){
			actualCurrentType = 0;
			nodeShape = "triangle";
		}
		// Скрытый
		if(currentType == 1){
			actualCurrentType = 2;
			nodeShape = "invtriangle";
		}
		// Выходной
		if(currentType == 2){
			actualCurrentType = 1;
			nodeShape = "circle";
		}
		
		// Обнуляем текущее расстояние по изменяемой координате
		int distance = 0;

		for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
			// помни про костыль - обход упорядочен по id
			if (poolsStructure[currentPool - 1]->getType() == actualCurrentType){
				// На входном или выходном слое мы меняем только X-координату
				if(actualCurrentType == 0){
					nodeXPosition.push_back(distance += (int)1.5*scale);
					nodeYPosition.push_back(0);
//					nodePosition.push_back(std::to_string(nodeXPosition[currentPool - 1]) + "," + std::to_string(nodeYPosition[currentPool - 1]));
//					hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "\" [pos = \"" + nodePosition[currentPool - 1] + "!\", label=\"" << poolsStructure[currentPool - 1]->getID() << "\", shape = \"" + nodeShape + "\"];\n" ;
				}
				if(actualCurrentType == 2){
					nodeXPosition.push_back(distance += (int)1.5*scale);
					nodeYPosition.push_back(scale * (poolsQuantity - inputResolution - outputResolution + 1) + 1000);
//					nodePosition.push_back(std::to_string(nodeXPosition[currentPool - 1]) + "," + std::to_string(nodeYPosition[currentPool - 1]));
//					hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "\" [pos = \"" + nodePosition[currentPool - 1] + "!\", label=\"" << poolsStructure[currentPool - 1]->getID() << "\", shape = \"" + nodeShape + "\"];\n" ;
				}
				// На скрытом - только Y-координату
				if(actualCurrentType == 1){
					nodeYPosition.push_back(500 + (distance += scale));
					nodeXPosition.push_back(8 * scale + scale * (inputResolution > outputResolution ? inputResolution + 1 : outputResolution + 1));
//					nodePosition.push_back(std::to_string(nodeXPosition[currentPool - 1]) + "," + std::to_string(nodeYPosition[currentPool - 1]));
//					hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "\" [pos = \"" + nodePosition[currentPool - 1] + "!\", label=\"" << poolsStructure[currentPool - 1]->getID() << "\", shape = \"" + nodeShape + "\"];\n" ;
				}
			}
			
//		hDotGraphFile << "\t}\n"; // Заканчиваем запись слоя
	}

	// если печатаем генеалогию
	if(genealogy == 1){
		int evolutionStep;

		// слой фиктивных вершин для построения генеалогии
		hDotGraphFile << "\t{ \n";

		// координаты фиктивных вершин, аналогично обычным
		vector<int> genealogyNodesXPosition;
		vector<int> genealogyNodesYPosition;
		vector<string> genealogyNodePosition;

		// массив отсортированных нейронов внутреннего слоя
		vector<int> nodesSequence;

		// координата X внутреннего слоя
		int XPos = (int) 8 * scale + scale * (inputResolution > outputResolution ? inputResolution + 1 : outputResolution + 1);

		// сортировка внутреннего слоя
		nodesSequence.push_back(inputResolution + outputResolution + 1);
		for(evolutionStep = 1; evolutionStep <= 5000; evolutionStep++){
			for (int currentPool = inputResolution + outputResolution + 1; currentPool <= poolsQuantity; ++currentPool){
				if(poolsStructure[currentPool - 1]->getAppearenceEvolutionTime() == evolutionStep){
					int flag = 0;
					for(vector<int>::iterator iterator = nodesSequence.begin(); iterator != nodesSequence.end(); iterator++){
						if(*iterator == poolsStructure[currentPool - 1]->getRootPoolID()){
							nodesSequence.insert(iterator + 1, 1, currentPool);
							flag = 1;
							break;
						}
					}
					if(flag == 0)
						nodesSequence.push_back(currentPool);
				}
			}
		}

		// корректировка значений координат внутренних пулов в соответствии с сортировкой
		int distance = 0;
		for(vector<int>::iterator  iterator = nodesSequence.begin(); iterator != nodesSequence.end(); iterator++){
			nodeYPosition[*iterator - 1] = 500 + (distance += scale);
		}

		// запись пулов в файл
		for(int currentPool = 1; currentPool <= poolsQuantity; currentPool++){
			nodePosition.push_back(std::to_string(nodeXPosition[currentPool - 1]) + "," + std::to_string(nodeYPosition[currentPool - 1]));
			hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "\" [pos = \"" + nodePosition[currentPool - 1] + "!\", label=\"" << poolsStructure[currentPool - 1]->getID() << "\", shape = \"" + nodeShape + "\"];\n" ;
		}


/*		// здесь были полезные сведения, возможно они мне еще пригодятся, не надо удалять (Василий)
		vector<int>::iterator childrenIterator;
		for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
			cout << "Current pool: " << currentPool << endl;
			int i = 1;
			if(!idChildrenMap[currentPool].empty()){
				nodeYPosition[currentPool] = nodeYPosition[currentPool] + idChildrenMap[currentPool].size() * scale + scale;
				for(childrenIterator = idChildrenMap[currentPool].begin(); childrenIterator < idChildrenMap[currentPool].end(); childrenIterator++, i++){
					cout << "Child: " << *childrenIterator << endl;
//					cout << nodeYPosition[*childrenIterator - 1] << endl;
//					nodeYPosition[*childrenIterator - 1] = nodeYPosition[currentPool - 1] + i * scale;
//					cout << nodeYPosition[*childrenIterator - 1] << endl;
				}
			}
		}
*/				
//		printGraphNetworkSort(16, 0, 0, scale);
		
		// Вычисление и запись фиктивных вершин
		nodeShape = "diamond";
		evolutionStep = 0;
		genealogyNodesXPosition.push_back(XPos);
		genealogyNodesYPosition.push_back(nodeYPosition[inputResolution + outputResolution]);
		genealogyNodePosition.push_back(std::to_string(genealogyNodesXPosition.back()) + "," + std::to_string(genealogyNodesYPosition.back()));
		hDotGraphFile << "\t\t\""<< poolsStructure[inputResolution + outputResolution]->getID() << "1gen" << "\" [pos = \"" + genealogyNodePosition.back() + "!\", label=\"" << "\", shape = \"none\", height=.1, width=.1];\n" ;

		genealogyNodesXPosition.push_back(XPos + 5000 - evolutionStep);
		genealogyNodesYPosition.push_back(nodeYPosition[inputResolution + outputResolution]);
		genealogyNodePosition.push_back(std::to_string(genealogyNodesXPosition.back()) + "," + std::to_string(genealogyNodesYPosition.back()));
		hDotGraphFile << "\t\t\""<< poolsStructure[inputResolution + outputResolution]->getID() << "2gen" << "\" [pos = \"" + genealogyNodePosition.back() + "!\", label=\"" << "\", shape = \"" + nodeShape + "\", height=.1, width=.1];\n" ;
		
		for(evolutionStep = 1; evolutionStep <= 5000; evolutionStep++){
			for (int currentPool = inputResolution + outputResolution + 1; currentPool <= poolsQuantity; ++currentPool){
				if(poolsStructure[currentPool - 1]->getAppearenceEvolutionTime() == evolutionStep){
					genealogyNodesXPosition.push_back(XPos);
					genealogyNodesYPosition.push_back(nodeYPosition[currentPool - 1]);
					genealogyNodePosition.push_back(std::to_string(genealogyNodesXPosition.back()) + "," + std::to_string(genealogyNodesYPosition.back()));
					hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "1gen" << "\" [pos = \"" + genealogyNodePosition.back() + "!\", label=\"" << "\", shape = \"none\", height=.1, width=.1];\n" ;

					genealogyNodesXPosition.push_back(XPos + 5000 - evolutionStep);
					genealogyNodesYPosition.push_back(nodeYPosition[currentPool - 1]);
					genealogyNodePosition.push_back(std::to_string(genealogyNodesXPosition.back()) + "," + std::to_string(genealogyNodesYPosition.back()));
					hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "2gen" << "\" [pos = \"" + genealogyNodePosition.back() + "!\", label=\"" << "\", shape = \"" + nodeShape + "\", height=.1, width=.1];\n" ;

					genealogyNodesXPosition.push_back(XPos + 5000 - evolutionStep);
					genealogyNodesYPosition.push_back(nodeYPosition[poolsStructure[currentPool - 1]->getRootPoolID() - 1]);
					genealogyNodePosition.push_back(std::to_string(genealogyNodesXPosition.back()) + "," + std::to_string(genealogyNodesYPosition.back()));
					hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "3gen" << "\" [pos = \"" + genealogyNodePosition.back() + "!\", label=\"" << "\", shape = \"" + nodeShape + "\", height=.1, width=.1];\n" ;
				}
			}
		}

		hDotGraphFile << "\t}\n"; // Заканчиваем запись слоя

		// запись генеалогических связей
		int prePoolId = inputResolution + outputResolution + 1;
		hDotGraphFile << "\t\"" << prePoolId << "1gen" << "\" -> \"" <<
		prePoolId << "2gen" << "\" [ ";
		hDotGraphFile << "dir=none, penwidth=2.0];\n";

		for(evolutionStep = 1; evolutionStep <= 5000; evolutionStep++){
			for (int currentPool = inputResolution + outputResolution + 1; currentPool <= poolsQuantity; ++currentPool){
				if(poolsStructure[currentPool - 1]->getAppearenceEvolutionTime() == evolutionStep && poolsStructure[currentPool - 1]->getRootPoolID()){
					int prePoolId = currentPool;
					int postPoolId = poolsStructure[currentPool - 1]->getRootPoolID();
					hDotGraphFile << "\t\"" << prePoolId << "1gen" << "\" -> \"" <<
					prePoolId << "2gen" << "\" [ ";
					hDotGraphFile << "dir=none, penwidth=2.0];\n";
					hDotGraphFile << "\t\"" << prePoolId << "2gen" << "\" -> \"" <<
					prePoolId << "3gen" << "\" [ ";
					hDotGraphFile << "dir=none, penwidth=2.0];\n";
					hDotGraphFile << "\t\"" << prePoolId << "3gen" << "\" -> \"" <<
					postPoolId << "2gen" << "\" [ ";
					hDotGraphFile << "dir=none, penwidth=2.0];\n";
				}
			}
		}
	} else {
		// запись пулов в файл
		for(int currentPool = 1; currentPool <= poolsQuantity; currentPool++){
			nodePosition.push_back(std::to_string(nodeXPosition[currentPool - 1]) + "," + std::to_string(nodeYPosition[currentPool - 1]));
			hDotGraphFile << "\t\t\""<< poolsStructure[currentPool - 1]->getID() << "\" [pos = \"" + nodePosition[currentPool - 1] + "!\", label=\"" << poolsStructure[currentPool - 1]->getID() << "\", shape = \"" + nodeShape + "\"];\n" ;
		}
	}
	
	// ищем максимальный вес связи для нормализации
	double maxWeightValue = 0;
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool){
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection){
			if(poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) > maxWeightValue){
				maxWeightValue = poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection);
			}
		}
	}

	// геометрический центр области ограниченной внутренним слоем справа и внешними слоями сверху и снизу
	int geometricCenterX = 1500;
	// геометрический центр области ограниченной внутренним слоем справа и внешними слоями сверху и снизу
	int geometricCenterY = (scale * (poolsQuantity - inputResolution - outputResolution + 1) + 500)/2;

	string splineControlPoints;
	int prePoolId;
	int postPoolId;

	// Записываем связи
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			if (poolsStructure[currentPool - 1]->getConnectionEnabled(currentConnection)){
				string hex;
				service::decToHex(static_cast<int>(fabs(255 * poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) / maxWeightValue)), hex, 2);
				string color;
				if (poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) < 0)
					color = "0000FF" + hex; // Оттенок синего
				else
					color = "FF0000" + hex; // Оттенок красного
				prePoolId = poolsStructure[currentPool - 1]->getConnectionPrePoolID(currentConnection);
				postPoolId = poolsStructure[currentPool - 1]->getConnectionPostPoolID(currentConnection);
				hDotGraphFile << "\t\"" << prePoolId << "\" -> \"" <<
					postPoolId << "\" [ ";
//				if (printWeights) //  Если надо напечатать веса
//					hDotGraphFile << "label=\"" << poolsStructure[currentPool - 1]->getConnectionWeightMean(currentConnection) << "\", ";
				// для связи между внутренними и внешними пулами
				if((poolsStructure[prePoolId - 1]->getType() == 0 || poolsStructure[prePoolId - 1]->getType() == 2 || poolsStructure[postPoolId - 1]->getType() == 0 || poolsStructure[postPoolId - 1]->getType() == 2)
					&& !(poolsStructure[prePoolId - 1]->getType() == 0 && poolsStructure[postPoolId - 1]->getType() == 2)){
						// если связь идет к/от входного слоя
						if(poolsStructure[prePoolId - 1]->getType() == 0 || poolsStructure[postPoolId - 1]->getType() == 0)
							splineControlPoints = "pos = \"" + nodePosition[prePoolId - 1] + " " + std::to_string(geometricCenterX - 300) + "," + std::to_string(geometricCenterY - 100) + " " + std::to_string(geometricCenterX - 330) + "," + std::to_string(geometricCenterY - 200) + " " + nodePosition[postPoolId - 1] + "\", ";
						else
							splineControlPoints = "pos = \"" + nodePosition[prePoolId - 1] + " " + std::to_string(geometricCenterX - 200) + "," + std::to_string(geometricCenterY + 100) + " " + std::to_string(geometricCenterX - 220) + "," + std::to_string(geometricCenterY + 200) + " " + nodePosition[postPoolId - 1] + "\", ";

				} else {
					// для связи между внешними пулами
					if(poolsStructure[prePoolId - 1]->getType() == 0 && poolsStructure[postPoolId - 1]->getType() == 2){
						splineControlPoints = "pos = \"" + nodePosition[prePoolId - 1] + " " + std::to_string(geometricCenterX - 1400) + "," + std::to_string(geometricCenterY - 700) + " " + std::to_string(geometricCenterX - 1400) + "," + std::to_string(geometricCenterY + 700) + " " + nodePosition[postPoolId - 1] + "\", ";
					} else {
						// для связи между внутренними пулами
						if(!genealogy){
							if((poolsStructure[prePoolId - 1]->getType() == 1 && poolsStructure[postPoolId - 1]->getType() == 1) && (prePoolId != postPoolId)){
								if(nodeYPosition[prePoolId - 1] < nodeYPosition[postPoolId - 1]){
									splineControlPoints = "pos = \"" + nodePosition[prePoolId - 1] + " " + std::to_string(3*geometricCenterX + 0.7*abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId - 1])) + "," + std::to_string(min(nodeYPosition[prePoolId - 1], nodeYPosition[postPoolId - 1]) + (abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId - 1]))/2 - 100) + " " + std::to_string(3*geometricCenterX + 0.7*abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId - 1])) + "," + std::to_string(min(nodeYPosition[prePoolId - 1], nodeYPosition[postPoolId - 1]) + (abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId -1]))/2 + 100) + " " + nodePosition[postPoolId - 1] + "\", ";
								} else {
									splineControlPoints = "pos = \"" + nodePosition[prePoolId - 1] + " " + std::to_string(3*geometricCenterX + 0.7*abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId - 1])) + "," + std::to_string(min(nodeYPosition[prePoolId - 1], nodeYPosition[postPoolId - 1]) + (abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId - 1]))/2 + 100) + " " + std::to_string(3*geometricCenterX + 0.7*abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId - 1])) + "," + std::to_string(min(nodeYPosition[prePoolId - 1], nodeYPosition[postPoolId - 1]) + (abs(nodeYPosition[prePoolId - 1] - nodeYPosition[postPoolId -1]))/2 - 100) + " " + nodePosition[postPoolId - 1] + "\", ";
								}
							} else {
								splineControlPoints = "";
							}
						} else {
							splineControlPoints = "";
							color = "00000000";
						}
					}
				}

				hDotGraphFile << splineControlPoints << "arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";
			}

	

	// Записываем предикторные связи
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
			for (int currentPredConnection = 1; currentPredConnection <= poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
				if (poolsStructure[currentPool - 1]->getPredConnectionEnabled(currentPredConnection)){
					prePoolId = poolsStructure[currentPool - 1]->getPredConnectionPrePoolID(currentPredConnection);
					postPoolId = poolsStructure[currentPool - 1]->getPredConnectionPostPoolID(currentPredConnection);
					hDotGraphFile << "\t\"" << poolsStructure[currentPool - 1]->getPredConnectionPrePoolID(currentPredConnection) << "\" -> \"" <<
						poolsStructure[currentPool - 1]->getPredConnectionPostPoolID(currentPredConnection) << "\" [style=dashed, arrowsize=0.7, color=\"#000000\", penwidth=2.0];\n";
				}
	hDotGraphFile << "}";
	hDotGraphFile.close();
	system(("dot -Kneato -n2 -Tgif " + graphFilename + ".dot -o " + graphFilename + ".gif").c_str());
}

/*void TPoolNetwork::printGraphNetworkSort(int childPool, int parentPool, int childNumber, int scale) const{
	cout << "Child pool: " << childPool << endl;
	if(parentPool == 0)
		cout << "recursion..." << endl;
//		nodeYPosition[childPool - 1] = nodeYPosition[childPool - 1];
	else
		if(childNumber == 1)
			nodeYPosition[childPool - 1] = nodeYPosition[parentPool - 1] + scale;
		else
			nodeYPosition[childPool - 1] = nodeYPosition[parentPool - 1] + scale * idChildrenMap[idChildrenMap[parentPool].at(childNumber - 2)].size() + scale + scale;
	for(int i = 1; i <= idChildrenMap[childPool].size(); i++)
		printGraphNetworkSort(idChildrenMap[childPool].at(i - 1), childPool, i, scale);
	return;
}
*/

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

//Печать сети со всеми сведений о пулах в файл или на экран (вместе с номером пула родителя и временем появления в эволюции)
ostream& TPoolNetwork::printNetworkExtra(ostream& os){
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		poolsStructure[currentPool - 1]->printPoolExtra(os);
	os << "|\n"; // Записываем разделитель между пулами и связями
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentConnection = 1; currentConnection <= poolsStructure[currentPool - 1]->getInputConnectionsQuantity(); ++currentConnection)
			os << *(poolsStructure[currentPool-1]->connectednessSet[currentConnection-1]); 
	os << "||\n"; // Записываем разделитель между связями и предикторными связями
	for (int currentPool = 1; currentPool <= poolsQuantity; ++currentPool)
		for (int currentPredConnection = 1; currentPredConnection <=poolsStructure[currentPool - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			os << *(poolsStructure[currentPool-1]->predConnectednessSet[currentPredConnection-1]); 
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
		poolNetwork.addPool(newType, newLayer, newBiasMean, newBiasVariance, newCapacity);
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
		poolNetwork.addConnection(prePoolNumber, postPoolNumber, newWeightMean, newWeightVariance, newDevelopSynapseProb, newEnabled, newDisabledStep, newInnovationNumber);
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
		poolNetwork.addPredConnection(prePoolNumber, postPoolNumber, newDevelopPredConnectionProb, newEnabled, newDisabledStep, newInnovationNumber);
		is >> tmp_string; // Считываем номер пресинаптического пула
	}

	return is;
}

//Считывание сети со всеми сведениями о пулах из файла или на экрана (вместе с номером пула родителя и временем появления в эволюции)
istream& TPoolNetwork::readNetworkExtra(istream& is){
	erasePoolNetwork(); // На всякий случай опустошаем сеть
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
		is >> tmp_string; // Считываем номер родительского пула
		int rootPoolID = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем время появления в эволюции
		int appearenceEvolutionTime = atoi(tmp_string.c_str());
		addPool(newType, newLayer, newBiasMean, newBiasVariance, newCapacity);
		poolsStructure[getPoolsQuantity() - 1]->setRootPoolID(rootPoolID);
		poolsStructure[getPoolsQuantity() - 1]->setAppearenceEvolutionTime(appearenceEvolutionTime);
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
		addConnection(prePoolNumber, postPoolNumber, newWeightMean, newWeightVariance, newDevelopSynapseProb, newEnabled, newDisabledStep, newInnovationNumber);
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
		addPredConnection(prePoolNumber, postPoolNumber, newDevelopPredConnectionProb, newEnabled, newDisabledStep, newInnovationNumber);
		is >> tmp_string; // Считываем номер пресинаптического пула
	}

	return is;
}


