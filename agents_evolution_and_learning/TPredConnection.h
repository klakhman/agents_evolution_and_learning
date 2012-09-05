#ifndef TPREDCONNECTION_H
#define TPREDCONNECTION_H

#include <iostream>
#include <fstream>

class TNeuron;

/* 
Класс предикторной связи нейронной сети
*/
class TPredConnection{
	int ID; // ID предикторной связи
	bool enabled; // Признак экспресии связи
	TNeuron* preNeuron; // Ссылка на пресинаптический нейрон
	TNeuron* postNeuron; // Ссылка на постсинаптический нейрон
public:
	// Конструктор по умолчанию
	TPredConnection(){
		ID = 0;
		enabled = false;
		preNeuron = 0;
		postNeuron = 0;
	}
	// Полный конструктор
	TPredConnection(int newID, bool newEnabled = true, TNeuron* newPreNeuron = 0, TNeuron* newPostNeuron = 0){
		setAll(newID, newEnabled, newPreNeuron, newPostNeuron);
	}
	// Деструктор
	~TPredConnection() {}

	// Геттеры и сеттеры для всех внутренних переменных
	int getID() const { return ID; }
	void setID(int newID) { ID = newID; }
	bool getEnabled() const { return enabled; }
	void setEnabled(bool newEnabled) { enabled = newEnabled; }
	TNeuron* getPreNeuron() const { return preNeuron; }
	void setPreNeuron(TNeuron* newPreNeuron) { preNeuron = newPreNeuron; }
	TNeuron* getPostNeuron() const { return postNeuron; }
	void setPostNeuron(TNeuron* newPostNeuron) { postNeuron = newPostNeuron; }
	// Заполнение всех характеристик связи
	void setAll(int newID, bool newEnabled = true, TNeuron* newPreNeuron = 0, TNeuron* newPostNeuron = 0){
		ID = newID;
		enabled = newEnabled;
		preNeuron = newPreNeuron;
		postNeuron = newPostNeuron;
	}
	// Запись предикторной связи в файл или на экран
	friend std::ostream& operator<<(std::ostream& os, const TPredConnection& poolPredConnection);
};

#endif // TPREDCONNECTION_H
