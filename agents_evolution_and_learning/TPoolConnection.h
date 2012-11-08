#ifndef TPOOLCONNECTION_H
#define TPOOLCONNECTION_H

#include <iostream>
#include <fstream>

class TNeuralPool;

/* 
Класс связи сети из пулов
*/
class TPoolConnection{
	int ID; // ID связи
	double weightMean; // Среднее веса синапса, который развивается по этой связи
	double weightVariance; // Дисперсия веса синапса, который развивается по этой связи
	bool enabled; // Признак экспресии связи
	int disabledStep; // Номер популяции (такта эволюции), в которой связь была выключена (0 - если включена)
	double developSynapseProb; // Вероятность развития синапса по этой связи
	long innovationNumber; // Номер инновации связи
	TNeuralPool* prePool; // Ссылка на пресинаптический пул
	TNeuralPool* postPool; // Ссылка на постсинаптический пул
public:
	// Конструктор по умолчанию
	TPoolConnection(){
		ID = 0;
		weightMean = 0;
		weightVariance = 0;
		enabled = false;
		disabledStep = 0;
		developSynapseProb = 0;
		innovationNumber = 0;
		prePool = 0;
		postPool = 0;
	}
	// Полный конструктор
	TPoolConnection(int newID, double newWeightMean, double newWeightVariance, bool newEnabled = true, int newDisabledStep = 0, double newDevelopSynapseProb = 1, long newInnovationNumber = 0, TNeuralPool* newPrePool = 0, TNeuralPool* newPostPool = 0){
		setAll(newID, newWeightMean, newWeightVariance, newEnabled, newDisabledStep, newDevelopSynapseProb, newInnovationNumber, newPrePool, newPostPool);
	}
	// Деструктор
	~TPoolConnection() {}

	// Геттеры и сеттеры для всех внутренних переменных
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	double getWeightMean() const { return weightMean; }
	void setWeightMean(double newWeightMean) { weightMean = newWeightMean; }
	double getWeightVariance() const { return weightVariance; }
	void setWeightVariance(double newWeightVariance) { weightVariance = newWeightVariance; } 
	bool getEnabled() const { return enabled; }
	void setEnabled(bool newEnabled) { enabled = newEnabled; }
	int getDisabledStep() const { return disabledStep; }
	void setDisabledStep(int newDisabledStep) { disabledStep = newDisabledStep; }
	double getDevelopSynapseProb() const { return developSynapseProb; }
	void setDevelopSynapseProb(double newDevelopSynapseProb) { developSynapseProb = newDevelopSynapseProb; }
	long getInnovationNumber() const { return innovationNumber; }
	void setInnovationNumber(long newInnovationNumber) { innovationNumber = newInnovationNumber; }
	TNeuralPool* getPrePool() const { return prePool; }
	void setPrePool(TNeuralPool* newPrePool) { prePool = newPrePool; }
	TNeuralPool* getPostPool() const { return postPool; }
	void setPostPool(TNeuralPool* newPostPool) { postPool = newPostPool; }
	// Заполнение всех характеристик связи
	void setAll(int newID, double newWeightMean, double newWeightVariance, bool newEnabled = true, int newDisabledStep = 0, double newDevelopSynapseProb = 1, long newInnovationNumber = 0, TNeuralPool* newPrePool = 0, TNeuralPool* newPostPool = 0){
		ID = newID;
		weightMean = newWeightMean;
		weightVariance = newWeightVariance;
		enabled = newEnabled;
		disabledStep = newDisabledStep;
		developSynapseProb = newDevelopSynapseProb;
		innovationNumber = newInnovationNumber;
		prePool = newPrePool;
		postPool = newPostPool;
	};
	// Запись связи в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TPoolConnection& poolConnection);
};

#endif // TPOOLCONNECTION_H
