#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <cmath>

namespace service{

	const double PI = 3.14159265;

	// ћаксимальное число разр€дов шестиричного числа, которое может быть представлено по умолчанию (без конкретного указани€) 
	const int MAX_HEX_RESOLUTION = 6; 

	// Ћогистическа€ функци€ (всегда положительна€) - передаетс€ наклон кривой у нул€, который всегда должен быть положителен
	inline double tansig(double x, double slop = 2) { return ( 1/ (1 + exp(-slop*x)) ); }

	// —тандартна€ логистическа€ функци€ (от -1 до 1) - передаетс€ наклон кривой у нул€, который всегда должен быть положителен
	inline double tansig_standart(double x, double slop = 2) { return (2 / (1 + exp(-slop*x)) - 1); }
	
	// ‘ункци€ генерации псевдослучайного равномерно распределенного дискретной величины (куски интервалов включаютс€)
	int uniformDiscreteDistribution(int A, int B);

	// ‘ункци€ генерации псеводослучайного равномерно распределенного числа (сами куски интервала включаютс€, если не указано иного)
	double uniformDistribution(double A, double B, bool include_start = true, bool include_end = true);

	// ‘ункци€ генераци€ псевдослучайного нормально распределенного числа
	double normalDistribution(double mean, double variance);

	// ѕроцедура перевода дес€тичного числа в двоичное
	void decToBin(int decNumber, bool binNumber[], int binResolution);

	// ѕроцедура перевода двоичного числа в дес€тичное (старшие разр€ды должны быть в начале массива)
	int binToDec(bool binNumber[], int binResolution);

	// ѕроцедура конвертировани€ дес€тичного числа в шестнадцатеричное - символьное
	// ѕо умолчанию кодируетс€ service::MAX_HEX_RESOLUTION разр€дов, так что в начале могут быть нули, а большие числа будут обрезатьс€
	void decToHex(int decNumber, std::string& hexNumber, int hexResolution = MAX_HEX_RESOLUTION);

};

#endif // SERVICE_H
