#include "service.h"

#include <cmath>
#include <cstdlib>
#include <string>
#include <ctime>
#include "config.h"

using namespace std;

// Функция генерации псевдослучайного равномерно распределенного дискретной величины (куски интервалов включаются)
int service::uniformDiscreteDistribution(int A, int B)
{
   //Вариант из книги Кенига-Му (Accelerated C++)
   int range = B - A + 1;
   const int bucket_size = RAND_MAX / range;
   int r;

   do r = rand() / bucket_size;
   while (r>=range);

   return A + r;

	// Вариант с использованием библиотеки boost - работает значительно медленее, но зато статистически более верно и диапазон генерируемых чисел значительно выше
	//boost::random::uniform_int_distribution<> uniformDiscreteDistr(A, B);
	//return uniformDiscreteDistr(randomGenerator);
}

// Если пользователь хочет использовать более мощные функции библиотеки boost::random, то необходимо ее установить и отменить директиву NOT_USE_BOOST_LIBRARIES в файле config.h
#ifndef NOT_USE_BOOST_LIBRARIES

  #include <boost/random/mersenne_twister.hpp>
  #include <boost/random/uniform_int_distribution.hpp>
  #include <boost/random/uniform_real_distribution.hpp>
  #include <boost/random/normal_distribution.hpp>
  boost::random::mt11213b randomGenerator(static_cast<unsigned int>(time(0)));
  boost::random::normal_distribution<> normalDistr(0, 1);
  boost::random::uniform_real_distribution<> uniformRealDistr(0, 1);

  // Функция генерации псеводослучайного СТРОГО равномерно распределенного числа на интервале [min;max)
  // ВНИМАНИЕ! Правый конец интервала не включается
  double service::uniformDistribution(double A, double B)
  {
	  // Вариант с использованием библиотеки boost::random
	  return uniformRealDistr(randomGenerator)*(B-A) + A;
  }

  // Функция генерация псевдослучайного нормально распределенного числа
  double service::normalDistribution(double mean, double variance)
  {
	  // Вариант с использованием библиотеки boost::random
    return normalDistr(randomGenerator)*variance + mean; 
  }

#else

  // Функция генерации псеводослучайного СТРОГО равномерно распределенного числа на интервале [min;max)
  // ВНИМАНИЕ! Правый конец интервала не включается
  double service::uniformDistribution(double A, double B)
  {
    const int SCALE = 10000;
    return (uniformDiscreteDistribution(0, SCALE - 1)/static_cast<double>(SCALE)) * (B - A) + A;
  }

  // Функция генерация псевдослучайного нормально распределенного числа
  double service::normalDistribution(double mean, double variance)
  {
     /* Генерация методом Бокса-Мюллера - необходимы две СВ распределенные равномерно (0; 1] */
    const int SCALE = 10000;
    double firstValue = uniformDiscreteDistribution(1, SCALE)/static_cast<double>(SCALE);
    double secondValue = uniformDiscreteDistribution(1, SCALE)/static_cast<double>(SCALE);
    return variance*sqrt(-2*log(firstValue))*cos(2*PI*secondValue) + mean;
  }

#endif

// Процедура перевода десятичного числа в двоичное (старшие разряды в начале массива)
void service::decToBin(int decNumber, bool binNumber[], int binResolution)
{
   //Algorithm from "Thinking in C++" by Bruce Eckel (vol.1 p.127 in russian edition)
   for (int currentPosition = binResolution; currentPosition > 0; --currentPosition)
      binNumber[binResolution - currentPosition] = (decNumber & (1 << (currentPosition-1))) ? true : false;
}

// Процедура перевода двоичного числа в десятичное (старшие разряды должны быть в начале массива)
int service::binToDec(bool binNumber[], int binResolution)
{
   int _decNumber = 0;
   for (int currentPosition = 1; currentPosition <= binResolution; ++currentPosition)
      _decNumber += static_cast<int>(pow(2.0, binResolution - currentPosition)) * binNumber[currentPosition - 1];

   return _decNumber;
}

// Процедура конвертирования десятичного числа в шестнадцатеричное - символьное
// По умолчанию кодируется service::MAX_HEX_RESOLUTION разрядов, так что в начале могут быть нули, а большие числа будут обрезаться
void service::decToHex(int decNumber, string& hexNumber, int hexResolution /* = MAX_HEX_RESOLUTION*/)
{
	hexNumber = "";
	for (int currentPosition = 1; currentPosition <= hexResolution; ++currentPosition)
   {
      int integralPart = decNumber / static_cast<int>(pow(16.0, hexResolution-currentPosition)); 
		if (integralPart > 9) // Если это символ
			hexNumber += ('A' + integralPart - 10);
      else // Если это цифра
			hexNumber += ('0' + integralPart);
      decNumber = decNumber % 16;
   }
}

// Перевод окраски из формата HSV в RGB (ссылка http://www.cs.rit.edu/~ncs/color/t_convert.html)
void service::HSVtoRGB(int& R, int& G, int& B, double H, double S, double V)
{
	double _R, _G, _B;
	if(S == 0)
		_R = _G = _B = V; // achromatic (grey)
  else{
	  double f, p, q, t;
	  H /= 60;			// sector 0 to 5
	  int i = static_cast<int>(floor(H));
	  f = H - i;			// factorial part of h
	  p = V * ( 1 - S );
	  q = V * ( 1 - S * f );
	  t = V * ( 1 - S * ( 1 - f ) );
	  switch( i ) {
		  case 0:
			  _R = V; _G = t; _B = p;
			  break;
		  case 1:
			  _R = q; _G = V; _B = p;
			  break;
		  case 2:
			  _R = p; _G = V; _B = t;
			  break;
		  case 3:
			  _R = p; _G = q; _B = V;
			  break;
		  case 4:
			  _R = t; _G = p; _B = V;
			  break;
		  default:		// case 5:
			  _R = V; _G = p; _B = q;
			  break;
	  }
  }
  R = static_cast<int>(255*_R);
  G = static_cast<int>(255*_G);
  B = static_cast<int>(255*_B);
}