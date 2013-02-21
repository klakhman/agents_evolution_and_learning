#include "service.h"

#include <cmath>
#include <cstdlib>
#include <string>
#include <ctime>

//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int_distribution.hpp>
//#include <boost/random/uniform_real_distribution.hpp>
//#include <boost/random/normal_distribution.hpp>
//boost::random::mt11213b randomGenerator(static_cast<unsigned int>(time(0)));
//boost::random::normal_distribution<> normalDistr(0, 1);
//boost::random::uniform_real_distribution<> uniformRealDistr(0, 1);


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

// Функция генерации псеводослучайного СТРОГО равномерно распределенного числа на интервале [min;max)
// ВНИМАНИЕ! Правый конец интервала не включается
double service::uniformDistribution(double A, double B)
{
  const int SCALE = 10000;
  return (uniformDiscreteDistribution(0, SCALE - 1)/static_cast<double>(SCALE)) * (B - A) + A;

	// Вариант с использованием библиотеки boost
	// Однако может генерировать только одну ситуацию включения концов: [min; max)
	//return uniformRealDistr(randomGenerator)*(B-A) + A;
}

// Функция генерация псевдослучайного нормально распределенного числа
double service::normalDistribution(double mean, double variance)
{
   /* Генерация с использованием центральной предельной теоремы
   int RandomNumbers = 12; // Количество случайных чисел, которое используется для генерации одного нормально распределенного
   double RandomSum = 0.0; // Сумма последовательности случайных чисел
   for (int i=0; i<RandomNumbers; i++)
      RandomSum += rand()%10001 / 10000.0;
   return Variance * (RandomSum - 6) + Mean; // Variance * sqrt(12.0/RandomNumbers) * (RandomSum - RandomNumbers/6.0) + Mean;*/

   /* Генерация методом Бокса-Мюллера - необходимы две СВ распределенные равномерно (0; 1] */
  const int SCALE = 10000;
  double firstValue = uniformDiscreteDistribution(1, SCALE)/static_cast<double>(SCALE);
  double secondValue = uniformDiscreteDistribution(1, SCALE)/static_cast<double>(SCALE);
  return variance*sqrt(-2*log(firstValue))*cos(2*PI*secondValue) + mean;

	// Вариант с использованием библиотеки boost
  //return normalDistr(randomGenerator)*variance + mean; 
}

// Процедура перевода десятичного числа в двоичное (старшие разряды в начале массива)
void service::decToBin(int decNumber, bool binNumber[], int binResolution)
{
   /*for (int CurrentPosition = BinResolution; CurrentPosition>0; --CurrentPosition)
   {
      if (DecNumber >= (int) trunc(exp((CurrentPosition-1)*log(2))+0.5))
      {
         BinNumber[BinResolution - CurrentPosition] = 1;
         DecNumber -= (int) trunc(exp((CurrentPosition-1)*log(2))+0.5);
      }
      else
         BinNumber[BinResolution - CurrentPosition] = 0;
   }*/
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