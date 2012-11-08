#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <cmath>


namespace service{

	const double PI = 3.14159265;

	// Максимальное число разрядов шестиричного числа, которое может быть представлено по умолчанию (без конкретного указания) 
	const int MAX_HEX_RESOLUTION = 6; 

	// Логистическая функция (всегда положительная) - передается наклон кривой у нуля, который всегда должен быть положителен
	inline double tansig(double x, double slop = 2) { return ( 1/ (1 + exp(-slop*x)) ); }

	// Стандартная логистическая функция (от -1 до 1) - передается наклон кривой у нуля, который всегда должен быть положителен
	inline double tansig_standart(double x, double slop = 2) { return (2 / (1 + exp(-slop*x)) - 1); }
	
	// Функция генерации псевдослучайного равномерно распределенного дискретной величины (куски интервалов включаются)
	int uniformDiscreteDistribution(int A, int B);

	// Функция генерации псеводослучайного СТРОГО равномерно распределенного числа (сами куски интервала включаются, если не указано иного) - РАБОТАЕТ МЕДЛЕННО
	double uniformDistribution(double A, double B, bool include_start = true, bool include_end = true);

	// Функция генерации псеводослучайного СЛАБО равномерно распределенного числа (сами куски интервала включаются, если не указано иного) - РАБОТАЕТ БЫСТРО
	//double uniformDistributionWeak(double A, double B, bool include_start = true, bool include_end = true);

	// Функция генерация псевдослучайного нормально распределенного числа
	double normalDistribution(double mean, double variance);

	// Процедура перевода десятичного числа в двоичное
	void decToBin(int decNumber, bool binNumber[], int binResolution);

	// Процедура перевода двоичного числа в десятичное (старшие разряды должны быть в начале массива)
	int binToDec(bool binNumber[], int binResolution);

	// Процедура конвертирования десятичного числа в шестнадцатеричное - символьное
	// По умолчанию кодируется service::MAX_HEX_RESOLUTION разрядов, так что в начале могут быть нули, а большие числа будут обрезаться
	void decToHex(int decNumber, std::string& hexNumber, int hexResolution = MAX_HEX_RESOLUTION);

};

#endif // SERVICE_H
