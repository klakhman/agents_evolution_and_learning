#ifndef TPOOLPREDCONNECTION_H
#define TPOOLPREDCONNECTION_H

#include <iostream>
#include <fstream>

/* 
Класс предикторной связи сети из пулов
*/
class TPoolPredConnection{
	int ID; // ID предикторной связи
	bool enabled; // Признак экспресии связи
	int disabledStep; // Номер популяции (такта эволюции), в которой связь была выключена (0 - если включена)
	double developPredConnectionProb; // Вероятность развития предикторной связи между нейронами по этой связи
	long innovationNumber; // Номер инновации связи
	int prePoolID; // Ссылка на пресинаптический пул
	int postPoolID; // Ссылка на постсинаптический пул
public:
	// Конструктор по умолчанию
	TPoolPredConnection(){
		ID = 0;
		enabled = false;
		disabledStep = 0;
		developPredConnectionProb = 0;
		innovationNumber = 0;
		prePoolID = 0;
		postPoolID = 0;
	}
	// Полный конструктор
	TPoolPredConnection(int newID, int newPrePoolID, int newPostPoolID, double newDevelopPredConnectionProb = 1, bool newEnabled = true, int newDisabledStep = 0, long newInnovationNumber = 0){
		setAll(newID, newPrePoolID, newPostPoolID, newDevelopPredConnectionProb, newEnabled, newDisabledStep, newInnovationNumber);
	}
	// Деструктор
	~TPoolPredConnection() {}

	// Геттеры и сеттеры для всех внутренних переменных
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	bool getEnabled() const { return enabled; }
	void setEnabled(bool newEnabled) { enabled = newEnabled; }
	int getDisabledStep() const { return disabledStep; }
	void setDisabledStep(int newDisabledStep) { disabledStep = newDisabledStep; }
	double getDevelopPredConnectionProb() const { return developPredConnectionProb; }
	void setDevelopPredConnectionProb(double newDevelopPredConnectionProb) { developPredConnectionProb = newDevelopPredConnectionProb; }
	long getInnovationNumber() const { return innovationNumber; }
	void setInnovationNumber(long newInnovationNumber) { innovationNumber = newInnovationNumber; }
	int getPrePoolID() const { return prePoolID; }
	void setPrePoolID(int newPrePoolID) { prePoolID = newPrePoolID; }
	int getPostPoolID() const { return postPoolID; }
	void setPostPoolID(int newPostPoolID) { postPoolID = newPostPoolID; }
	// Заполнение всех характеристик связи
	void setAll(int newID, int newPrePoolID, int newPostPoolID, double newDevelopPredConnectionProb = 1, bool newEnabled = true, int newDisabledStep = 0, long newInnovationNumber = 0){
		ID = newID;
		enabled = newEnabled;
		disabledStep = newDisabledStep;
		developPredConnectionProb = newDevelopPredConnectionProb;
		innovationNumber = newInnovationNumber;
		prePoolID = newPrePoolID;
		postPoolID = newPostPoolID;
	}
	// Запись предикторной связи в файл или на экран
	friend std::ostream& operator<<(std::ostream& ofs, const TPoolPredConnection& poolPredConnection);
};

#endif // TPOOLPREDCONNECTION_H
