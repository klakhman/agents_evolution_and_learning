#ifndef TPOOLPREDCONNECTION_H
#define TPOOLPREDCONNECTION_H

#include <iostream>
#include <fstream>

class TNeuralPool;

/* 
Класс предикторной связи сети из пулов
*/
class TPoolPredConnection{
	int ID; // ID предикторной связи
	bool enabled; // Признак экспресии связи
	int disabledStep; // Номер популяции (такта эволюции), в которой связь была выключена (0 - если включена)
	double developPredConnectionProb; // Вероятность развития предикторной связи между нейронами по этой связи
	long innovationNumber; // Номер инновации связи
	TNeuralPool* prePool; // Ссылка на пресинаптический пул
	TNeuralPool* postPool; // Ссылка на постсинаптический пул
public:
	// Конструктор по умолчанию
	TPoolPredConnection(){
		ID = 0;
		enabled = false;
		disabledStep = 0;
		developPredConnectionProb = 0;
		innovationNumber = 0;
		prePool = 0;
		postPool = 0;
	}
	// Полный конструктор
	TPoolPredConnection(int newID, bool newEnabled = true, int newDisabledStep = 0, double newDevelopPredConnectionProb = 1, long newInnovationNumber = 0, TNeuralPool* newPrePool = 0, TNeuralPool* newPostPool = 0){
		setAll(newID, newEnabled, newDisabledStep, newDevelopPredConnectionProb, newInnovationNumber, newPrePool, newPostPool);
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
	TNeuralPool* getPrePool() const { return prePool; }
	void setPrePool(TNeuralPool* newPrePool) { prePool = newPrePool; }
	TNeuralPool* getPostPool() const { return postPool; }
	void setPostPool(TNeuralPool* newPostPool) { postPool = newPostPool; }
	// Заполнение всех характеристик связи
	void setAll(int newID, bool newEnabled = true, int newDisabledStep = 0, double newDevelopPredConnectionProb = 1, long newInnovationNumber = 0, TNeuralPool* newPrePool = 0, TNeuralPool* newPostPool = 0){
		ID = newID;
		enabled = newEnabled;
		disabledStep = newDisabledStep;
		developPredConnectionProb = newDevelopPredConnectionProb;
		innovationNumber = newInnovationNumber;
		prePool = newPrePool;
		postPool = newPostPool;
	}
	// Запись предикторной связи в файл или на экран
	friend std::ostream& operator<<(std::ostream& ofs, const TPoolPredConnection& poolPredConnection);
};

#endif // TPOOLPREDCONNECTION_H
