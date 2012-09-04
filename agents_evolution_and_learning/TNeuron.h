#ifndef TNEURON_H
#define TNEURON_H

#include "TSynapse.h"

/* 
Класс нейрона нейронной сети 
*/
class TNeuron{
	int ID; // ID нейрона
	int type; // Тип нейрона (0 - сенсорный, 1 - скрытый, 2 - выходной)
	double bias; // Смещение нейрона
	int layer; // Номер слоя, в котором находится нейрон
	bool active; // Признак активности нейрона (0 - молчащий нейрон, 1 - активный нейрон)

	TSynapse* inputSynapses; // Массив входных синапсов нейрона
	int inputSynapsesSize; // Текущий размер массива входных синапсов
	int inputSynapsesQuantity; // Количество входных синапсов нейрона

	TNeuralPool* ParentPool; // Ссылка на пул из которого образовался нейрон (не всегда есть)
	int ParentPoolID; // Номер родительского пула

	double currentOut; // Текущий выход нейрона
	double potential; // Текущий потенциал нейрона
	double previousOut; // Предыдущий выход нейрона

	static const double EMPTY_OUT; // Признак неозначенного выхода нейрона
	static const double ACTIVITY_TRESHOLD; // Порог активности, при котором сигнал передается по выходным связям
	static const int INFLATE_SYNAPSES_SIZE = 10; // Размер увеличения массива с входным связями в случае переполнения
}

#endif // TNEURON_H