#ifndef TSYNAPSE_H
#define TSYNAPSE_H

#include <iostream>
#include <fstream>

class TNeuron;

/* 
Класс синапса нейронной сети 
*/
class TSynapse{
	int ID; // ID синапса
	double weight; // Вес синапса (синаптический коэффициент)
	bool enabled; // Признак экспресии синапса
	TNeuron* preNeuron; // Ссылка на пресинаптический нейрон
	TNeuron* postNeuron; // Ссылка на постсинаптический нейрон

public:
	// Конструктор по умолчанию
	TSynapse(){
		ID = 0;
		weight = 0;
		enabled = false;
		preNeuron = NULL;
		postNeuron = NULL;
	}
	// Полный конструктор
	TSynapse(int newID, double newWeight, bool newEnabled = true, TNeuron* newPreNeuron = NULL, TNeuron* newPostNeuron = NULL){
		setAll(newID, newWeight, newEnabled, newPreNeuron, newPostNeuron);
	}
	// Деструктор
	~TSynapse() {}

	// Геттеры и сеттеры для всех внутренних переменных
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	double getWeight() const { return weight; }
	void setWeight(double newWeight) { weight = newWeight; }
	bool getEnabled() const { return enabled; }
	void setEnabled(bool newEnabled) { enabled = newEnabled; }
	TNeuron* getPreNeuron() const { return preNeuron; }
	void setPreNeuron(TNeuron* newPreNeuron) { preNeuron = newPreNeuron; }
	TNeuron* getPostNeuron() const { return postNeuron; }
	void setPostNeuron(TNeuron* newPostNeuron) { postNeuron = newPostNeuron; }
	// Заполнение всех характеристик синапса
	void setAll(int newID, double newWeight, bool newEnabled = true, TNeuron* newPreNeuron = NULL, TNeuron* newPostNeuron = NULL){
		ID = newID;
		weight = newWeight;
		enabled = newEnabled;
		preNeuron = newPreNeuron;
		postNeuron = newPostNeuron;
	}
	// Запись синапса в файл или на экран
	friend std::ostream& operator<<(std::ostream& ofs, const TSynapse& synapse);
};

#endif // TSYNAPSE_H