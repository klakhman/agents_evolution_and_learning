#include "TNeuron.h"
#include "TSynapse.h"
#include "TPredConnection.h"
#include "service.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

const double TNeuron::EMPTY_OUT = -10.0; // Признак неозначенного выхода нейрона
const double TNeuron::ACTIVITY_TRESHOLD = 0.5; // Порог активности, при котором сигнал передается по выходным связям

// Рассчет выхода нейрона
void TNeuron::calculateOut(){
	potential = 0; // На всякий случай
	potential += bias; // Добавляем смещение
	for (unsigned int currentSynapse = 1; currentSynapse <= inputSynapsesSet.size(); ++currentSynapse)
	{
    if (inputSynapsesSet[currentSynapse - 1].preNeuron->active){ // Если нейрон активен, то проводим суммирование, иначе просто пропускаем для улучшения производительности
      // Определяем какая связь - рекуррентная или прямая
		  double preNeuronOut = (layer > inputSynapsesSet[currentSynapse - 1].preNeuron->layer) ? 
													  inputSynapsesSet[currentSynapse - 1].preNeuron->currentOut:
													  inputSynapsesSet[currentSynapse - 1].preNeuron->previousOut;
		  // Если выход не означен, то что-то пошло не так (эта строчка больше для отладки)
		  //if (preNeuronOut == EMPTY_OUT) exit(2);
		  if (preNeuronOut > ACTIVITY_TRESHOLD) // Если пресинаптический сигнал проходит по синапсу
			  potential += inputSynapsesSet[currentSynapse - 1].weight * preNeuronOut;
    }
	}
	// Если нейрон активен, то вычисляем выход
	if (active)
		currentOut = service::tansig(potential);
	else 
		// Иначе нулевой выход
		currentOut = 0;
}

// Печать сведений о нейроне в файл или на экран
std::ostream& operator<<(std::ostream& os, const TNeuron& neuron){
	os << neuron.type << "\t" << neuron.bias << "\t" << neuron.layer 
		<< "\t" << neuron.active << "\t" << neuron.parentPoolID << std::endl;
	return os;
}
