#include "TNeuralNetwork.h"
#include "TSynapse.h"
#include "TPredConnection.h"
#include "service.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

// Нахождение номера связи в структуре постсинаптического нейрона - возвращает ноль, если связи нет
int TNeuralNetwork::findSynapseNumber(int preNeuronNumber, int postNeuronNumber){
	for (int currentSynapse = 1; currentSynapse <= neuronsStructure[postNeuronNumber - 1]->getInputSynapsesQuantity(); ++currentSynapse)
		if (neuronsStructure[postNeuronNumber - 1]->getSynapsePreNeuron(currentSynapse)->getID() == preNeuronNumber)
			return currentSynapse;
	return 0;
}

// Нахождение номера предикторной связи в структуре постсинаптического нейрона - возвращает ноль, если предикторной связи нет
int TNeuralNetwork::findPredConnectionNumber(int preNeuronNumber, int postNeuronNumber){
	for (int currentPredConnection = 1; currentPredConnection <= neuronsStructure[postNeuronNumber - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
		if (neuronsStructure[postNeuronNumber - 1]->getPredConnectionPreNeuron(currentPredConnection)->getID() == preNeuronNumber)
			return currentPredConnection;
	return 0;
}

// Корректировка ID нейронов (например после удаления)
void TNeuralNetwork::fixNeuronsIDs(){
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		neuronsStructure[currentNeuron - 1]->setID(currentNeuron);
}

// Процедура увеличения размера массива нейронов
void TNeuralNetwork::inflateNeuronsStructure(int inflateSize){
	TNeuron** newNeuronsStructure = new TNeuron*[neuronsStructureSize + inflateSize];
	memset(newNeuronsStructure, 0, (neuronsStructureSize + inflateSize) * sizeof(TNeuron*));
	memcpy(newNeuronsStructure, neuronsStructure, neuronsStructureSize * sizeof(TNeuron*));
	delete []neuronsStructure;
	neuronsStructure = newNeuronsStructure;
	neuronsStructureSize += inflateSize;
}

// Декструктор
TNeuralNetwork::~TNeuralNetwork(){
	if (neuronsStructure){
		for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			delete neuronsStructure[currentNeuron - 1];
		delete []neuronsStructure;
	}
}

//Добавление нейрона в сеть
void TNeuralNetwork::addNeuron(int newID, int newType, double newBias, int newLayer, bool newActive /*=true*/, int newParentneuralID /*=0*/){
	if (neuronsQuantity >= neuronsStructureSize) // Если не хватает места в массиве
		inflateNeuronsStructure(INFLATE_NEURONS_SIZE);
	neuronsStructure[neuronsQuantity++] = new TNeuron(newID, newType, newBias, newLayer, newActive, newParentneuralID);
	// Если нейрон входной
	if (0 == newType) ++inputResolution;
	// Если нейрон выходной
	if (2 == newType) ++outputResolution;
	// Смотрим на слой нейрона
	if (newLayer > layersQuantity) layersQuantity = newLayer;
}

// Удаление нейрона из сети (с удалением также всех входных и выходных связей из этого нейрона)
void TNeuralNetwork::deleteNeuron(int neuronNumber){
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		if (currentNeuron != neuronNumber){ // Если это не стираемый нейрон (пропускаем просто для экономии времени)
			for (int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1]->getInputSynapsesQuantity(); ++currentSynapse)
				if (neuronsStructure[currentNeuron - 1]->getSynapsePreNeuron(currentSynapse)->getID() == neuronNumber){ // Если это синапс от стираемого нейрона
					deleteSynapse(currentNeuron, currentSynapse);
					break; // Выходим из цикла, так как у одного нейрона не может быть более одной связи от другого нейрона
				}
			for (int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
				if (neuronsStructure[currentNeuron - 1]->getPredConnectionPreNeuron(currentPredConnection)->getID() == neuronNumber){ // Если это пред. связь от стираемого нейрона
					deletePredConnection(currentNeuron, currentPredConnection);
					break;
				}
		}
	// Если нейрон входной
	if (0 == neuronsStructure[neuronNumber - 1]->getType()) --inputResolution;
	// Если нейрон выходной
	if (2 == neuronsStructure[neuronNumber - 1]->getType()) --outputResolution;
	// Стираем нейрон и сдвигаем массив
	synapsesQuantity -= neuronsStructure[neuronNumber - 1]->getInputSynapsesQuantity();
	predConnectionsQuantity -= neuronsStructure[neuronNumber - 1]->getInputPredConnectionsQuantity();
	delete neuronsStructure[neuronNumber - 1];
	for (int currentNeuron = neuronNumber - 1; currentNeuron < neuronsQuantity - 1; ++currentNeuron)
		neuronsStructure[currentNeuron] = neuronsStructure[currentNeuron + 1];
	neuronsStructure[neuronsQuantity - 1] = 0;
	--neuronsQuantity;
	fixNeuronsIDs();
}

// Стирание сети
void TNeuralNetwork::eraseNeuralNetwork(){
	if (neuronsStructure) {
		for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			delete neuronsStructure[currentNeuron - 1];
		delete []neuronsStructure;
		neuronsStructure = 0;
	}
	neuronsStructureSize = 0;
	neuronsQuantity = 0;
	synapsesQuantity = 0;
	predConnectionsQuantity = 0;
	layersQuantity = 0;
	inputResolution = 0;
	outputResolution = 0;
}

// "Перезагрузка" сети
void TNeuralNetwork::reset(){
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		neuronsStructure[currentNeuron - 1]->reset();
}

// Обсчет одного такта работы сети
void TNeuralNetwork::calculateNetwork(double inputVector[]){
	// Сначала подготавливаем все нейроны
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		neuronsStructure[currentNeuron - 1]->prepare();
	// Заполняем входные нейроны
	for (int currentBit = 1; currentBit <= inputResolution; ++currentBit)
		neuronsStructure[currentBit - 1]->setCurrentOut(inputVector[currentBit - 1]);
	// Проходимся по нейронам по слоям (начинаем со второго)
	for (int currentLayer = 2; currentLayer <= layersQuantity; ++currentLayer)
		for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			if (neuronsStructure[currentNeuron - 1]->getLayer() == currentLayer)
				neuronsStructure[currentNeuron - 1]->calculateOut();
}

// Обсчет одного такта работы сети, в условиях спонтанной активации нейронов
void TNeuralNetwork::calculateSpontaneousNetwork(double spontaneousActivityProb){
	// Сначала подготавливаем все нейроны
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		neuronsStructure[currentNeuron - 1]->prepare();
	// Отдельно обсчитываем входные нейроны
	for (int currentNeuron = 1; currentNeuron <= inputResolution; ++currentNeuron)
		// Если нейрон спотанно активируется
		if (service::uniformDistribution(0, 1, true, false) < spontaneousActivityProb)
			neuronsStructure[currentNeuron - 1]->setCurrentOut(service::uniformDistribution(0.5, 1, false, false));
		else
			neuronsStructure[currentNeuron - 1]->setCurrentOut(0.0);
	// Проходимся по нейронам по слоям (начинаем со второго)
	for (int currentLayer = 2; currentLayer <= layersQuantity; ++currentLayer)
		for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			if (neuronsStructure[currentNeuron - 1]->getLayer() == currentLayer){
				neuronsStructure[currentNeuron - 1]->calculateOut();
				// Если нейрон спонтанно активируется, то просто заменяем выход на спонтанный (при этом оставляем потенциал, таким какой был в результате подсчета)
				if (service::uniformDistribution(0, 1, true, false) < spontaneousActivityProb)
					neuronsStructure[currentNeuron - 1]->setCurrentOut(service::uniformDistribution(0.5, 1, false, false));
			}
}

// Получение текущего выходного вектора сети
void TNeuralNetwork::getOutputVector(double outputVector[]){
	for (int currentBit = 1; currentBit <= outputResolution; ++currentBit)
		outputVector[currentBit - 1] = neuronsStructure[currentBit - 1 + inputResolution]->getCurrentOut(); 
}

// Вывод сети в файл как графа (с использованием сторонней утилиты dot.exe из пакета GraphViz) 
// Для корректной работы необходимо чтобы путь к dot.exe был прописан в $PATH
void TNeuralNetwork::printGraphNetwork(string graphFilename, bool printWeights /*=false*/) const{
	ofstream hDotGraphFile;
	hDotGraphFile.open((graphFilename + ".dot").c_str());
	// Инициализируем и указываем, что слои должны следовать слево на право
	hDotGraphFile << "digraph G { \n\trankdir=LR;\n";
	// Подсчитываем кол-во пулов сети
	int poolsQuantity = 0;
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		if (neuronsStructure[currentNeuron - 1]->getParentPoolID() > poolsQuantity)
			poolsQuantity = neuronsStructure[currentNeuron - 1]->getParentPoolID();
	// Записываем нейроны послойно
	for (int currentLayer = 1; currentLayer <= layersQuantity; ++ currentLayer){
		// Указываем, что это один слой и нейроны должны идти в столбец
		hDotGraphFile << "\t{ rank = same; \n";
		for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			// !! Не отображаем неактивные нейроны
			if ((neuronsStructure[currentNeuron - 1]->getLayer() == currentLayer) && (neuronsStructure[currentNeuron - 1]->getActive())){
				// Определяем насыщенность цвета нейрона (отражает принадлежность к пулу)
				string hex;
				service::decToHex((poolsQuantity - neuronsStructure[currentNeuron-1]->getParentPoolID() + 1) * (255 - 30) / poolsQuantity + 30, hex, 2);
				string color = hex + hex + hex;
				hDotGraphFile << "\t\tsubgraph cluster" << neuronsStructure[currentNeuron - 1]->getParentPoolID() << "{\n" << "\t\t\tstyle=dashed;\n\t\t\tcolor=lightgrey;\n" <<
					"\t\t\t\"" << neuronsStructure[currentNeuron - 1]->getID() << "\"[label=\"" << neuronsStructure[currentNeuron - 1]->getID() << "; " << neuronsStructure[currentNeuron - 1]->getParentPoolID() <<
					"\", shape=\"circle\", style=filled, fillcolor=\"#" << color << "\"];\n\t\t}\n";
			}
		hDotGraphFile << "\t}\n"; // Заканчиваем запись слоя
	}
	// Записываем синапсы
	double maxWeightValue = 1.0;
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		for (int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1]->getInputSynapsesQuantity(); ++currentSynapse)
			//Отображаем связи только между активными нейронами
			if ((neuronsStructure[currentNeuron - 1]->getSynapseEnabled(currentSynapse)) &&
					(neuronsStructure[currentNeuron - 1]->getSynapsePreNeuron(currentSynapse)->getActive()) && 
						(neuronsStructure[currentNeuron - 1]->getSynapsePreNeuron(currentSynapse)->getActive())){
				string hex;
				service::decToHex(static_cast<int>(min(fabs(255 * neuronsStructure[currentNeuron - 1]->getSynapseWeight(currentSynapse) / maxWeightValue), 255.0)), hex, 2);
				string color;
				if (neuronsStructure[currentNeuron - 1]->getSynapseWeight(currentSynapse) < 0)
					color = "0000" + hex; // Оттенок синего
				else
					color = hex + "0000"; // Оттенок красного
				hDotGraphFile << "\t\"" << neuronsStructure[currentNeuron - 1]->getSynapsePreNeuron(currentSynapse)->getID() << "\" -> \"" <<
					neuronsStructure[currentNeuron - 1]->getSynapsePostNeuron(currentSynapse)->getID() << "\" [ ";
				if (printWeights) // Если надо напечатать веса
					hDotGraphFile << "label=\"" << neuronsStructure[currentNeuron - 1]->getSynapseWeight(currentSynapse) << "\", ";
				hDotGraphFile << "arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";
			}
	// Записываем предикторные связи
	for (int currentNeuron = 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
		for (int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			if ((neuronsStructure[currentNeuron - 1]->getPredConnectionEnabled(currentPredConnection)) &&
					(neuronsStructure[currentNeuron - 1]->getPredConnectionPreNeuron(currentPredConnection)->getActive()) &&
						(neuronsStructure[currentNeuron - 1]->getPredConnectionPostNeuron(currentPredConnection)->getActive()))
				hDotGraphFile << "\t\"" << neuronsStructure[currentNeuron - 1]->getPredConnectionPreNeuron(currentPredConnection)->getID() << "\" -> \"" <<
					neuronsStructure[currentNeuron - 1]->getPredConnectionPostNeuron(currentPredConnection)->getID() << "\" [style=dashed, arrowsize=0.7, color=\"#000000\", penwidth=2.0];\n";
	hDotGraphFile << "}";
	hDotGraphFile.close();
	system(("dot -Tjpg " + graphFilename + ".dot -o " + graphFilename).c_str());
}

//Печать сети в файл или на экран
ostream& operator<<(ostream& os, const TNeuralNetwork& neuralNetwork){
	for (int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsQuantity; ++currentNeuron)
		os << *(neuralNetwork.neuronsStructure[currentNeuron - 1]);
	os << "|\n"; // Записываем разделитель между нейронами и синапсами
	for (int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsQuantity; ++currentNeuron)
		for (int currentSynapse = 1; currentSynapse <= neuralNetwork.neuronsStructure[currentNeuron - 1]->getInputSynapsesQuantity(); ++currentSynapse)
			os << *(neuralNetwork.neuronsStructure[currentNeuron-1]->inputSynapsesSet[currentSynapse-1]); 
	os << "||\n"; // Записываем разделитель между синапсами и предикторными связями
	for (int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsQuantity; ++currentNeuron)
		for (int currentPredConnection = 1; currentPredConnection <= neuralNetwork.neuronsStructure[currentNeuron - 1]->getInputPredConnectionsQuantity(); ++currentPredConnection)
			os << *(neuralNetwork.neuronsStructure[currentNeuron-1]->inputPredConnectionsSet[currentPredConnection-1]); 
	os << "|||\n"; // Записываем разделитель между сетями
	return os;
}

//Считывание сети из файла или экрана
istream& operator>>(istream& is, TNeuralNetwork& neuralNetwork){
	neuralNetwork.eraseNeuralNetwork(); // На всякий случай опустошаем сеть
	string tmp_string;
	// Создаем все нейроны сети
	is >> tmp_string; // Считываем тип нейрона
	while (tmp_string != "|"){ // Считываем до разделителя между пулами и связями
		int newType = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем смещение нейрона
		double newBias = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем слой нейрона
		int newLayer = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем признак активности нейрона
		bool newActive = (atoi(tmp_string.c_str()) != 0);
		is >> tmp_string; // Считываем номер родительского пула
		int newParentPoolID = atoi(tmp_string.c_str());
		neuralNetwork.addNeuron(neuralNetwork.getNeuronsQuantity() + 1, newType, newBias, newLayer, newActive, newParentPoolID);
		is >> tmp_string; // Считываем типа нейрона
	}
	// Создаем все синапсы между пулами
	is >> tmp_string; // Считываем номер пресинаптического нейрона
	while (tmp_string != "||"){ // Считываем до разделителя между синапсами и предикторными связями
		int preNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем номер постсинаптического нейрона
		int postNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем вес синапса
		double newWeight = atof(tmp_string.c_str());
		is >> tmp_string; // Считываем признак экспресии синапса
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		neuralNetwork.addSynapse(preNeuronNumber, postNeuronNumber, neuralNetwork.getSynapsesQuantity() + 1, newWeight, newEnabled);
		is >> tmp_string; // Считываем номер пресинаптического нейрона
	}
	// Создаем все предикторные связи между нейронами
	is >> tmp_string; // Считываем номер пресинаптического нейрона
	while (tmp_string != "|||"){ // Считываем до разделителя между предикторными связями и концом сети
		int preNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем номер постсинаптического нейрона
		int postNeuronNumber = atoi(tmp_string.c_str());
		is >> tmp_string; // Считываем признак экспресии связи
		bool newEnabled = (atoi(tmp_string.c_str()) != 0);
		neuralNetwork.addPredConnection(preNeuronNumber, postNeuronNumber, neuralNetwork.getPredConnectionsQuantity() + 1, newEnabled);
		is >> tmp_string; // Считываем номер пресинаптического нейрона
	}

	return is;
}
