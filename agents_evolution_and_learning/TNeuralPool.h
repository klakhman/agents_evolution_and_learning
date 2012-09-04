#ifndef TNEURALPOOL_H
#define TNEURALPOOL_H

#include <iostream>
#include <fstream>

class TPoolNetwork;
class TPoolConnection;
class TPoolPredConnection;

/* 
����� ������������� ����
*/
class TNeuralPool{
	int ID; // ID ����
	int type; // ��� ���� (0 - ���������, 1 - �������, 2 - ��������)
	int capacity; // ����� ����
	double biasMean; // ������� �������� �������� ����
	double biasVariance; // ��������� �������� �������� ����
	int layer; // ����� ���� �������� ����
	
	TPoolConnection** connectednessSet; // ������ ������� ������ ����
	int connectednessSetSize; // ������� ������ ������� ������� ������
	int inputConnectionsQuantity; // ���������� ������� ������ ����

	TPoolPredConnection** predConnectednessSet; // ������ ������� ������������ ������ ����
	int predConnectednessSetSize; // ������� ������ ������� ������� ������������ ������
	int inputPredConnectionsQuantity; // ���������� ������� ������������ ������ ����

	static const int INFLATE_CONNECTIONS_SIZE = 10; // ������ ���������� ������� � ������� ������� � ������ ������������
	static const int INFLATE_PRED_CONNECTIONS_SIZE = 10; // ������ ���������� ������� � ������� ������������� ������� � ������ ������������
	
	// ��������� ���������� ������� ������� ������� ������
	void inflateConnectednessSet(int inflateSize);
	// ��������� ���������� ������� ������� ������� ������������ ������
	void inflatePredConnectednessSet(int inflateSize);

public:
	// ����������� �� ���������
	TNeuralPool(){
		ID = 0;
		type = 0;
		capacity = 0;
		biasMean = 0;
		biasVariance = 0;
		layer = 0;
		connectednessSet = 0;
		connectednessSetSize = 0;
		inputConnectionsQuantity = 0;
		predConnectednessSet = 0;
		predConnectednessSetSize = 0;
		inputPredConnectionsQuantity = 0;
	}
	// ������ �����������
	TNeuralPool(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer){
		setAll(newID, newType, newCapacity, newBiasMean, newBiasVariance, newLayer);
		connectednessSet = 0;
		connectednessSetSize = 0;
		inputConnectionsQuantity = 0;
		predConnectednessSet = 0;
		predConnectednessSetSize = 0;
		inputPredConnectionsQuantity = 0;
	}
	// ����������
	~TNeuralPool();

	// ������� � ������� ��� ���� ���������� ����������
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	int getType() const { return type; }
	void setType(int newType) { type = newType; }
	int getCapacity() const { return capacity; }
	void setCapacity(int newCapacity) { capacity = newCapacity; }
	double getBiasMean() const { return biasMean; }
	void setBiasMean(double newBiasMean) { biasMean = newBiasMean; }
	double getBiasVariance() const { return biasVariance; }
	void setBiasVariance(double newBiasVariance) { biasVariance = newBiasVariance; }
	int getLayer() const { return layer; }
	void setLayer(int newLayer) { layer = newLayer; }
	// ��������� ���������� ������� ������ ����
	int getInputConnectionsQuantity() const { return inputConnectionsQuantity; }
	// ��������� ���������� ������� ������������ ������ ����
	int getInputPredConnectionsQuantity() const { return inputPredConnectionsQuantity; }
	// ���������� ���� ������������� ����
	void setAll(int newID, int newType, int newCapacity, double newBiasMean, double newBiasVariance, int newLayer){
		ID = newID;
		type = newType;
		capacity = newCapacity;
		biasMean = newBiasMean;
		biasVariance = newBiasVariance;
		layer = newLayer;
	}

	// ������� � ������� ��� ������ ������� ���� (�� ���� ������� ���������� ����� ����� � ������� ������)
	int getConnectionID(int connectionNumber) const;
	void setConnectionID(int connectionNumber, int newID);
	double getConnectionWeightMean(int connectionNumber) const;
	void setConnectionWeightMean(int connectionNumber, double newWeightMean);
	double getConnectionWeightVariance(int connectionNumber) const;
	void setConnectionWeightVariance(int connectionNumber, double newWeightVariance);
	bool getConnectionEnabled(int connectionNumber) const;
	void setConnectionEnabled(int connectionNumber, bool newEnabled);
	int getConnectionDisabledStep(int connectionNumber) const;
	void setConnectionDisabledStep(int connectionNumber, int newDisabledStep);
	double getConnectionDevelopSynapseProb(int connectionNumber) const;
	void setConnectionDevelopSynapseProb(int connectionNumber, double newDevelopSynapseProb);
	long int getConnectionInnovationNumber(int connectionNumber) const;
	void setConnectionInnovationNumber(int connectionNumber, long int newInnovationNumber);
	TNeuralPool* getConnectionPrePool(int connectionNumber) const;
	void setConnectionPrePool(int connectionNumber, TNeuralPool* newPrePool);
	TNeuralPool* getConnectionPostPool(int connectionNumber) const;
	void setConnectionPostPool(int connectionNumber, TNeuralPool* newPostPool);

	// ������� � ������� ��� ������������ ������ ������� ���� (�� ���� ������� ���������� ����� ����� � ������� ������)
	int getPredConnectionID(int predConnectionNumber) const;
	void setPredConnectionID(int predConnectionNumber, int newID);
	bool getPredConnectionEnabled(int predConnectionNumber) const;
	void setPredConnectionEnabled(int predConnectionNumber, bool newEnabled);
	int getPredConnectionDisabledStep(int predConnectionNumber) const;
	void setPredConnectionDisabledStep(int predConnectionNumber, int newDisabledStep);
	double getDevelopPredConnectionProb(int predConnectionNumber) const;
	void setDevelopPredConnectionProb(int predConnectionNumber, double newDevelopPredConnectionProb);
	long int getPredConnectionInnovationNumber(int predConnectionNumber) const;
	void setPredConnectionInnovationNumber(int predConnectionNumber, long int newInnovationNumber);
	TNeuralPool* getPredConnectionPrePool(int predConnectionNumber) const;
	void setPredConnectionPrePool(int predConnectionNumber, TNeuralPool* newPrePool);
	TNeuralPool* getPredConnectionPostPool(int predConnectionNumber) const;
	void setPredConnectionPostPool(int predConnectionNumber, TNeuralPool* newPostPool);

	// ���������� ������� ����� � ���
	void addConnection(int newID, double newWeightMean, double newWeightVariance, bool newEnabled = true, int newDisabledStep = 0, double newDevelopSynapseProb = 1, long int newInnovationNumber = 0, TNeuralPool* newPrePool = 0);
	// �������� ����� �� ����
	void deleteConnection(int connectionNumber);

	// ���������� ������� ������������ ����� � ���
	void addPredConnection(int newID, bool newEnabled = true, int newDisabledStep = 0, double newDevelopPredConnectionProb = 1, long int newInnovationNumber = 0, TNeuralPool* newPrePool = 0);
	// �������� ������������ ����� �� ����
	void deletePredConnection(int predConnectionNumber);

	// ������ �������� � ���� � ����
	friend std::ostream& operator<<(std::ostream& ofs, const TNeuralPool& neuralPool);

	//������ ���� � ����
	friend std::ostream& operator<<(std::ostream& ofs, const TPoolNetwork& PoolNetwork); // ������� ������ ���� ��������� �������������, ����� ��� ����� ������ ������ � ������ ������ ����
	//friend int main(int argc, char** argv);
};

#endif // TNEURALPOOL_H
