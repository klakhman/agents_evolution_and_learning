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

// Копирующий конструктор (применять только для правильного присваивания с одновременным созданием переменной)
TNeuralNetwork::TNeuralNetwork(const TNeuralNetwork& sourceNetwork){
  synapsesQuantity = 0;
  predConnectionsQuantity = 0;
	layersQuantity = 0;
	inputResolution = 0;
	outputResolution = 0;
  *this = sourceNetwork;
}
// Оператор присваивания (фактически полное копирование сети - создание новых структур)
TNeuralNetwork& TNeuralNetwork::operator=(const TNeuralNetwork& sourceNetwork){
  if (this == &sourceNetwork) return *this;
  eraseNeuralNetwork(); // На всякий случай опустошаем сеть
	// Копируем пулы
	for (int currentNeuron = 1; currentNeuron <= sourceNetwork.getNeuronsQuantity(); ++currentNeuron){
		addNeuron(sourceNetwork.getNeuronType(currentNeuron), sourceNetwork.getNeuronLayer(currentNeuron),
        sourceNetwork.getNeuronBias(currentNeuron), sourceNetwork.getNeuronActive(currentNeuron), sourceNetwork.getNeuronParentPoolID(currentNeuron));
	}
	// Копируем все связи сети
	for (int currentNeuron = 1; currentNeuron <= sourceNetwork.getNeuronsQuantity(); ++currentNeuron){
		for (int currentSynapse = 1; currentSynapse <= sourceNetwork.getNeuronInputSynapsesQuantity(currentNeuron); ++ currentSynapse)
			addSynapse(sourceNetwork.getSynapsePreNeuronID(currentNeuron, currentSynapse), sourceNetwork.getSynapsePostNeuronID(currentNeuron, currentSynapse),
								sourceNetwork.getSynapseWeight(currentNeuron, currentSynapse), sourceNetwork.getSynapseEnabled(currentNeuron, currentSynapse));  
		for (int currentPredConnection = 1; currentPredConnection <= sourceNetwork.getNeuronInputPredConnectionsQuantity(currentNeuron); ++ currentPredConnection)
			addPredConnection(sourceNetwork.getPredConnectionPreNeuronID(currentNeuron, currentPredConnection), sourceNetwork.getPredConnectionPostNeuronID(currentNeuron, currentPredConnection),
								sourceNetwork.getPredConnectionEnabled(currentNeuron, currentPredConnection));
	}
	return *this;
}

// Нахождение номера связи в структуре постсинаптического нейрона - возвращает ноль, если связи нет
int TNeuralNetwork::findSynapseNumber(int preNeuronNumber, int postNeuronNumber){
  for (unsigned int currentSynapse = 1; currentSynapse <= neuronsStructure[postNeuronNumber - 1].inputSynapsesSet.size(); ++currentSynapse)
    if (neuronsStructure[postNeuronNumber - 1].inputSynapsesSet[currentSynapse - 1].preNeuron->ID == preNeuronNumber)
			return currentSynapse;
	return 0;
}

// Нахождение номера предикторной связи в структуре постсинаптического нейрона - возвращает ноль, если предикторной связи нет
int TNeuralNetwork::findPredConnectionNumber(int preNeuronNumber, int postNeuronNumber){
  for (unsigned int currentPredConnection = 1; currentPredConnection <= neuronsStructure[postNeuronNumber - 1].inputPredConnectionsSet.size(); ++currentPredConnection)
    if (neuronsStructure[postNeuronNumber - 1].inputPredConnectionsSet[currentPredConnection - 1].preNeuron->ID == preNeuronNumber)
			return currentPredConnection;
	return 0;
}

// Корректировка ID нейронов (например после удаления)
void TNeuralNetwork::fixNeuronsIDs(){
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
		neuronsStructure[currentNeuron - 1].ID = currentNeuron;
}

// Корректировка ID связей (например после удаления)
void TNeuralNetwork::fixSynapsesIDs(){
	int _synapesQuantity = 0;
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
    for (unsigned int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1].inputSynapsesSet.size(); ++currentSynapse)
      neuronsStructure[currentNeuron - 1].inputSynapsesSet[currentSynapse - 1].ID = ++_synapesQuantity;
}
// Корректировка ID предикторных связей (например после удаления)
void TNeuralNetwork::fixPredConnectionsIDs(){
	int _predConnectionsQuantity = 0;
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
    for (unsigned int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron - 1].inputPredConnectionsSet.size(); ++currentPredConnection)
      neuronsStructure[currentNeuron - 1].inputPredConnectionsSet[currentPredConnection - 1].ID = ++_predConnectionsQuantity;
}

//Добавление нейрона в сеть
void TNeuralNetwork::addNeuron(int newType, int newLayer, double newBias, bool newActive /*=true*/, int newParentNeuronID /*=0*/){
  vector<int> preNeuronsIDsSynapses;
  vector<int> preNeuronsIDsPredConnections;
  // После добавления нового нейрона в сеть из-за реаллокации вектора потенциально могут поехать все адреса нейронов, поэтому надо это учитывать
  // Если связей все еще нет (мы можем как всегда создавать сначала все нейроны), то для быстроты не делаем ничего
  if ((synapsesQuantity != 0) || (predConnectionsQuantity != 0))
    for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron){
      for (int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron-1].getInputSynapsesQuantity(); ++currentSynapse)
        preNeuronsIDsSynapses.push_back(neuronsStructure[currentNeuron-1].getSynapsePreNeuron(currentSynapse)->ID);
      for (int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron-1].getInputPredConnectionsQuantity(); ++currentPredConnection)
        preNeuronsIDsPredConnections.push_back(neuronsStructure[currentNeuron-1].getPredConnectionPreNeuron(currentPredConnection)->ID);
    }
  int previousCapacity = neuronsStructure.capacity();
  
  TNeuron newNeuron(neuronsStructure.size() + 1, newType, newLayer, newBias, newActive, newParentNeuronID);
  neuronsStructure.push_back(newNeuron);
	// Если нейрон входной
  if (INPUT_NEURON == newType) ++inputResolution;
	// Если нейрон выходной
  if (OUTPUT_NEURON == newType) ++outputResolution;
	// Смотрим на слой нейрона
	if (newLayer > layersQuantity) layersQuantity = newLayer;

  // Если capacity изменилась, то есть осуществлялась реаллокация, то надо править ссылки в синапсах
  if ((previousCapacity != neuronsStructure.capacity()) && ((synapsesQuantity != 0) || (predConnectionsQuantity != 0))){
    int synapseNumber = 0;
    int predConnectionNumber = 0;
    for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron){
      for (int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron-1].getInputSynapsesQuantity(); ++currentSynapse){
        setSynapsePreNeuronID(currentNeuron, currentSynapse, preNeuronsIDsSynapses[synapseNumber++]);
        setSynapsePostNeuronID(currentNeuron, currentSynapse, currentNeuron);
      }
      for (int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron-1].getInputPredConnectionsQuantity(); ++currentPredConnection){
        setSynapsePreNeuronID(currentNeuron, currentPredConnection, preNeuronsIDsPredConnections[predConnectionNumber++]);
        setSynapsePostNeuronID(currentNeuron, currentPredConnection, currentNeuron);
      }
    }
  }
}

// Удаление нейрона из сети (с удалением также всех входных и выходных связей из этого нейрона)
void TNeuralNetwork::deleteNeuron(unsigned int neuronNumber){
  // ПРОБЛЕМА - после стирания нейрона у всех нейронов могут измениться адреса в векторе => надо править все ссылки в синапсах и предикторных связях
  /*struct SConnectionNeuronsIDs{
    int preNeuronID;
    int postNeuronID;
    SConnectionNeuronsIDs(int _preNeuronID, int _postNeuronID) : preNeuronID(_preNeuronID), postNeuronID(_postNeuronID) {};
  };*/
  vector<int> synapsesPreNeuron; // Позиция связи в векторе - это номер в _ОБЩЕЙ_ последовательности синапсов
  vector<int> predConnectionsPreNeuron;
  // Проходим по всей сети и стираем синапсы и пред.связи у которых удаляемый нейрон пресинаптический 
  // Также записываем информацию об ID пре- и пост-синаптических нейронов для всех остальных связей (кроме связей удаляемого нейрона)
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
		if (currentNeuron != neuronNumber){ // Если это не стираемый нейрон
      for (unsigned int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1].inputSynapsesSet.size(); ++currentSynapse)
        if (neuronsStructure[currentNeuron - 1].inputSynapsesSet[currentSynapse - 1].preNeuron->ID == neuronNumber){ // Если это синапс от стираемого нейрона
					deleteSynapse(currentNeuron, currentSynapse);
          --currentSynapse;
        }
        else{
          unsigned int preNeuronID = neuronsStructure[currentNeuron - 1].inputSynapsesSet[currentSynapse - 1].preNeuron->ID;
          // Сразу сдвигаем номер ID нейрона, если он идет после удаляемого
          synapsesPreNeuron.push_back((preNeuronID < neuronNumber) ? preNeuronID : preNeuronID - 1);
        }

      for (unsigned int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron - 1].inputPredConnectionsSet.size(); ++currentPredConnection)
        if (neuronsStructure[currentNeuron - 1].inputPredConnectionsSet[currentPredConnection - 1].preNeuron->ID == neuronNumber){ // Если это пред. связь от стираемого нейрона
					deletePredConnection(currentNeuron, currentPredConnection);
          --currentPredConnection;
				}
        else{
          unsigned int preNeuronID = neuronsStructure[currentNeuron - 1].inputPredConnectionsSet[currentPredConnection - 1].preNeuron->ID;
          predConnectionsPreNeuron.push_back((preNeuronID < neuronNumber) ? preNeuronID : preNeuronID - 1);
        }
		}
	// Если нейрон входной
  if (INPUT_NEURON == neuronsStructure[neuronNumber - 1].type) --inputResolution;
	// Если нейрон выходной
  if (OUTPUT_NEURON == neuronsStructure[neuronNumber - 1].type) --outputResolution;
	// Стираем нейрон
  synapsesQuantity -= neuronsStructure[neuronNumber - 1].inputSynapsesSet.size();
  predConnectionsQuantity -= neuronsStructure[neuronNumber - 1].inputPredConnectionsSet.size();
  neuronsStructure.erase(neuronsStructure.begin() + neuronNumber - 1);
	fixNeuronsIDs();
  int overallSynapseNumber = 0;
  int overallPredConnectionNumber = 0;
  // Теперь правим все ссылки в связях
  for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron){
    for (unsigned int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1].inputSynapsesSet.size(); ++currentSynapse){
      setSynapsePreNeuronID(currentNeuron, currentSynapse, synapsesPreNeuron[overallSynapseNumber]);
      setSynapsePostNeuronID(currentNeuron, currentSynapse, currentNeuron);
      ++overallSynapseNumber;
    }
    for (unsigned int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron].inputPredConnectionsSet.size(); ++currentPredConnection){
      setPredConnectionPreNeuronID(currentNeuron, currentPredConnection, predConnectionsPreNeuron[overallPredConnectionNumber]);
      setPredConnectionPostNeuronID(currentNeuron, currentPredConnection, currentNeuron);
      ++overallPredConnectionNumber;
    }
  }
  fixSynapsesIDs();
  fixPredConnectionsIDs();
}

//Удаление связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
void TNeuralNetwork::deleteSynapse(unsigned int neuronNumber,unsigned int synapseNumber, bool fixIDs /*=true*/){
	neuronsStructure[neuronNumber-1].deleteSynapse(synapseNumber);
	if (fixIDs){
    for (unsigned int currentSynapse = synapseNumber; currentSynapse <= neuronsStructure[neuronNumber - 1].inputSynapsesSet.size(); ++currentSynapse)
      neuronsStructure[neuronNumber - 1].inputSynapsesSet[currentSynapse - 1].ID -= 1;
    for (unsigned int currentNeuron = neuronNumber + 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
      for (unsigned int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1].inputSynapsesSet.size(); ++currentSynapse)
				neuronsStructure[currentNeuron - 1].inputSynapsesSet[currentSynapse - 1].ID -= 1;
	}
	--synapsesQuantity;
}

// Удаление предикторной связи из сети (fixIDs - признак того, чтобы мы корректировали все ID после удаления связи - если подряд идет несколько операций удаления, то дешевле отключать эту операцию и потом в конце проводить корректировку с помощью отдельного метода)
void TNeuralNetwork::deletePredConnection(unsigned int neuronNumber,unsigned int predConnectionNumber, bool fixIDs /*=true*/){
	neuronsStructure[neuronNumber-1].deletePredConnection(predConnectionNumber);
	if (fixIDs){
    for (unsigned int currentPredConnection = predConnectionNumber; currentPredConnection <= neuronsStructure[neuronNumber - 1].inputPredConnectionsSet.size(); ++currentPredConnection)
      neuronsStructure[neuronNumber - 1].inputPredConnectionsSet[currentPredConnection - 1].ID -= 1;
		for (unsigned int currentNeuron = neuronNumber + 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
      for (unsigned int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron - 1].inputPredConnectionsSet.size(); ++currentPredConnection)
				neuronsStructure[currentNeuron - 1].inputPredConnectionsSet[currentPredConnection - 1].ID -= 1;
	}
	--predConnectionsQuantity;
}

// Стирание сети
void TNeuralNetwork::eraseNeuralNetwork(){
	if (neuronsStructure.size() != 0) 
    neuronsStructure.resize(0);
	synapsesQuantity = 0;
	predConnectionsQuantity = 0;
	layersQuantity = 0;
	inputResolution = 0;
	outputResolution = 0;
}

// Обсчет одного такта работы сети
void TNeuralNetwork::calculateNetwork(double inputVector[]){
	// Сначала подготавливаем все нейроны
  for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
		neuronsStructure[currentNeuron - 1].prepare();
	// Заполняем входные нейроны
	for (int currentBit = 1; currentBit <= inputResolution; ++currentBit)
		neuronsStructure[currentBit - 1].currentOut = inputVector[currentBit - 1];
  unsigned int neuronsQuantity = neuronsStructure.size();
	// Проходимся по нейронам по слоям (начинаем со второго)
	for (int currentLayer = 2; currentLayer <= layersQuantity; ++currentLayer)
    for (unsigned int currentNeuron = inputResolution + 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			if (neuronsStructure[currentNeuron - 1].layer == currentLayer)
				neuronsStructure[currentNeuron - 1].calculateOut();
}

// Обсчет одного такта работы сети, в условиях спонтанной активации нейронов
void TNeuralNetwork::calculateSpontaneousNetwork(double spontaneousActivityProb){
	// Сначала подготавливаем все нейроны
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
		neuronsStructure[currentNeuron - 1].prepare();
	// Отдельно обсчитываем входные нейроны
	for (int currentNeuron = 1; currentNeuron <= inputResolution; ++currentNeuron)
		// Если нейрон спотанно активируется
		if (service::uniformDistribution(0, 1, true, false) < spontaneousActivityProb)
      neuronsStructure[currentNeuron - 1].currentOut = service::uniformDistribution(0.5, 1, false, false);
		else
			neuronsStructure[currentNeuron - 1].currentOut = 0.0;
  unsigned int neuronsQuantity = neuronsStructure.size();
	// Проходимся по нейронам по слоям (начинаем со второго)
	for (int currentLayer = 2; currentLayer <= layersQuantity; ++currentLayer)
    for (unsigned int currentNeuron = inputResolution + 1; currentNeuron <= neuronsQuantity; ++currentNeuron)
			if (neuronsStructure[currentNeuron - 1].layer == currentLayer)
      {
				neuronsStructure[currentNeuron - 1].calculateOut();
				// Если нейрон спонтанно активируется, то просто заменяем выход на спонтанный (при этом оставляем потенциал, таким какой был в результате подсчета)
				if (service::uniformDistribution(0, 1, true, false) < spontaneousActivityProb)
					neuronsStructure[currentNeuron - 1].currentOut = service::uniformDistribution(0.5, 1, false, false);
			}
}

// Получение текущего выходного вектора сети
void TNeuralNetwork::getOutputVector(double outputVector[]){
	for (int currentBit = 1; currentBit <= outputResolution; ++currentBit)
		outputVector[currentBit - 1] = neuronsStructure[currentBit - 1 + inputResolution].currentOut; 
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
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
		if (neuronsStructure[currentNeuron - 1].getParentPoolID() > poolsQuantity)
			poolsQuantity = neuronsStructure[currentNeuron - 1].getParentPoolID();
	// Записываем нейроны послойно
	for (int currentLayer = 1; currentLayer <= layersQuantity; ++ currentLayer){
		// Указываем, что это один слой и нейроны должны идти в столбец
		hDotGraphFile << "\t{ rank = same; \n";
		for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
			// !! Не отображаем неактивные нейроны
			if ((neuronsStructure[currentNeuron - 1].getLayer() == currentLayer) && (neuronsStructure[currentNeuron - 1].getActive())){
				// Определяем насыщенность цвета нейрона (отражает принадлежность к пулу)
				string hex;
				service::decToHex((poolsQuantity - neuronsStructure[currentNeuron-1].getParentPoolID() + 1) * (255 - 30) / poolsQuantity + 30, hex, 2);
				string color = hex + hex + hex;
				hDotGraphFile << "\t\tsubgraph cluster" << neuronsStructure[currentNeuron - 1].getParentPoolID() << "{\n" << "\t\t\tstyle=dashed;\n\t\t\tcolor=lightgrey;\n" <<
					"\t\t\t\"" << neuronsStructure[currentNeuron - 1].getID() << "\"[label=\"" << neuronsStructure[currentNeuron - 1].getID() << "; " << neuronsStructure[currentNeuron - 1].getParentPoolID() <<
					"\", shape=\"circle\", style=filled, fillcolor=\"#" << color << "\"];\n\t\t}\n";
			}
		hDotGraphFile << "\t}\n"; // Заканчиваем запись слоя
	}
	// Записываем синапсы
	double maxWeightValue = 1.0;
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
		for (int currentSynapse = 1; currentSynapse <= neuronsStructure[currentNeuron - 1].getInputSynapsesQuantity(); ++currentSynapse)
			//Отображаем связи только между активными нейронами
			if ((neuronsStructure[currentNeuron - 1].getSynapseEnabled(currentSynapse)) &&
					(neuronsStructure[currentNeuron - 1].getSynapsePreNeuron(currentSynapse)->getActive()) && 
						(neuronsStructure[currentNeuron - 1].getSynapsePostNeuron(currentSynapse)->getActive())){
				string hex;
				service::decToHex(static_cast<int>(min(fabs(255 * neuronsStructure[currentNeuron - 1].getSynapseWeight(currentSynapse) / maxWeightValue), 255.0)), hex, 2);
				string color;
				if (neuronsStructure[currentNeuron - 1].getSynapseWeight(currentSynapse) < 0)
					color = "0000FF" + hex; // Оттенок синего
				else
					color = "FF0000" + hex; // Оттенок красного
				hDotGraphFile << "\t\"" << neuronsStructure[currentNeuron - 1].getSynapsePreNeuron(currentSynapse)->getID() << "\" -> \"" <<
					neuronsStructure[currentNeuron - 1].getSynapsePostNeuron(currentSynapse)->getID() << "\" [ ";
				if (printWeights) // Если надо напечатать веса
					hDotGraphFile << "label=\"" << neuronsStructure[currentNeuron - 1].getSynapseWeight(currentSynapse) << "\", ";
				hDotGraphFile << "arrowsize=0.7, color=\"#" << color << "\", penwidth=2.0];\n";
			}
	// Записываем предикторные связи
	for (unsigned int currentNeuron = 1; currentNeuron <= neuronsStructure.size(); ++currentNeuron)
		for (int currentPredConnection = 1; currentPredConnection <= neuronsStructure[currentNeuron - 1].getInputPredConnectionsQuantity(); ++currentPredConnection)
			if ((neuronsStructure[currentNeuron - 1].getPredConnectionEnabled(currentPredConnection)) &&
					(neuronsStructure[currentNeuron - 1].getPredConnectionPreNeuron(currentPredConnection)->getActive()) &&
						(neuronsStructure[currentNeuron - 1].getPredConnectionPostNeuron(currentPredConnection)->getActive()))
				hDotGraphFile << "\t\"" << neuronsStructure[currentNeuron - 1].getPredConnectionPreNeuron(currentPredConnection)->getID() << "\" -> \"" <<
					neuronsStructure[currentNeuron - 1].getPredConnectionPostNeuron(currentPredConnection)->getID() << "\" [style=dashed, arrowsize=0.7, color=\"#000000\", penwidth=2.0];\n";
	hDotGraphFile << "}";
	hDotGraphFile.close();
	system(("dot -Tjpg " + graphFilename + ".dot -o " + graphFilename).c_str());
}

//Печать сети в файл или на экран
ostream& operator<<(ostream& os, const TNeuralNetwork& neuralNetwork){
	for (unsigned int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsStructure.size(); ++currentNeuron)
		os << neuralNetwork.neuronsStructure[currentNeuron - 1];
	os << "|\n"; // Записываем разделитель между нейронами и синапсами
	for (unsigned int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsStructure.size(); ++currentNeuron)
		for (int currentSynapse = 1; currentSynapse <= neuralNetwork.neuronsStructure[currentNeuron - 1].getInputSynapsesQuantity(); ++currentSynapse)
			os << neuralNetwork.neuronsStructure[currentNeuron-1].inputSynapsesSet[currentSynapse-1]; 
	os << "||\n"; // Записываем разделитель между синапсами и предикторными связями
	for (unsigned int currentNeuron = 1; currentNeuron <= neuralNetwork.neuronsStructure.size(); ++currentNeuron)
		for (int currentPredConnection = 1; currentPredConnection <= neuralNetwork.neuronsStructure[currentNeuron - 1].getInputPredConnectionsQuantity(); ++currentPredConnection)
			os << neuralNetwork.neuronsStructure[currentNeuron-1].inputPredConnectionsSet[currentPredConnection-1]; 
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
		neuralNetwork.addNeuron(newType, newLayer, newBias, newActive, newParentPoolID);
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
		neuralNetwork.addSynapse(preNeuronNumber, postNeuronNumber, newWeight, newEnabled);
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
		neuralNetwork.addPredConnection(preNeuronNumber, postNeuronNumber, newEnabled);
		is >> tmp_string; // Считываем номер пресинаптического нейрона
	}

	return is;
}
