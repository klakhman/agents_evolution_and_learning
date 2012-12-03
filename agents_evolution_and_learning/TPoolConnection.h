#ifndef TPOOLCONNECTION_H
#define TPOOLCONNECTION_H

#include <iostream>
#include <fstream>

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
	int prePoolID; // Ссылка на пресинаптический пул
	int postPoolID; // Ссылка на постсинаптический пул
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
		prePoolID = 0;
		postPoolID = 0;
	}
	// Полный конструктор
	TPoolConnection(int newID,  int newPrePoolID, int newPostPoolID, double newWeightMean, double newWeightVariance = 0, double newDevelopSynapseProb = 1, bool newEnabled = true, int newDisabledStep = 0, long newInnovationNumber = 0){
		setAll(newID,  newPrePoolID, newPostPoolID, newWeightMean, newWeightVariance, newDevelopSynapseProb, newEnabled, newDisabledStep, newInnovationNumber);
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
	int getPrePoolID() const { return prePoolID; }
	void setPrePoolID(int newPrePoolID) { prePoolID = newPrePoolID; }
	int getPostPoolID() const { return postPoolID; }
	void setPostPoolID(int newPostPoolID) { postPoolID = newPostPoolID; }
	// Заполнение всех характеристик связи
	void setAll(int newID,  int newPrePoolID, int newPostPoolID, double newWeightMean, double newWeightVariance = 0, double newDevelopSynapseProb = 1, bool newEnabled = true, int newDisabledStep = 0, long newInnovationNumber = 0){
		ID = newID;
		weightMean = newWeightMean;
		weightVariance = newWeightVariance;
		enabled = newEnabled;
		disabledStep = newDisabledStep;
		developSynapseProb = newDevelopSynapseProb;
		innovationNumber = newInnovationNumber;
		prePoolID = newPrePoolID;
		postPoolID = newPostPoolID;
	};
	// Запись связи в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TPoolConnection& poolConnection);
};

#endif // TPOOLCONNECTION_H
