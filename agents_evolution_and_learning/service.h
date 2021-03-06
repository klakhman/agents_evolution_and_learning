﻿#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <cmath>
#include <vector>
#include <fstream>


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

  // Функция генерации псеводослучайного СТРОГО равномерно распределенного числа на интервале [min;max)
  // ВНИМАНИЕ! Правый конец интервала не включается
  double uniformDistribution(double A, double B);

	// Функция генерация псевдослучайного нормально распределенного числа
	double normalDistribution(double mean, double variance);

	// Процедура перевода десятичного числа в двоичное
	void decToBin(int decNumber, bool binNumber[], int binResolution);

	// Процедура перевода двоичного числа в десятичное (старшие разряды должны быть в начале массива)
	int binToDec(bool binNumber[], int binResolution);

	// Процедура конвертирования десятичного числа в шестнадцатеричное - символьное
	// По умолчанию кодируется service::MAX_HEX_RESOLUTION разрядов, так что в начале могут быть нули, а большие числа будут обрезаться
	void decToHex(int decNumber, std::string& hexNumber, int hexResolution = MAX_HEX_RESOLUTION);

  inline std::string decToHex(int decNumber, int hexResolution = MAX_HEX_RESOLUTION){
    std::string tmpHex;
    decToHex(decNumber, tmpHex, hexResolution);
    return tmpHex;
  }

  // Перевод окраски из формата HSV в RGB (ссылка http://www.cs.rit.edu/~ncs/color/t_convert.html)
  void HSVtoRGB(int& R, int& G, int& B, double H, double S, double V);
  
  /// Формирование имени файла среды
  std::string envName(unsigned int envNumber);

  /// Формирование пути к файлу среды
  std::string envPath(const std::string& environmentDirectory, unsigned int envNumber);

  /// Формирование имени файла лучшей популяции по аттрибутам запуска
  std::string bestPopName(unsigned int envNumber, unsigned int tryNumber, const std::string& runSign);

  /// Формирование пути к файлу с лучшей популяцией по аттрибутам запуска 
  std::string bestPopPath(const std::string& resultsDirectory, unsigned int envNumber, unsigned int tryNumber, const std::string& runSign);

  // Печать вектора в файл (шаблонная функция)
  template<class T>
  void printVectorToFile(const std::vector<T>& _vector, std::string outputFilename); 
};

// Печать вектора в файл (шаблонная функция)
template<class T>
void service::printVectorToFile(const std::vector<T>& _vector, std::string outputFilename){
  std::ofstream outputFile;
  outputFile.open(outputFilename.c_str());
  for (unsigned int i = 0; i < _vector.size(); ++i)
    outputFile << _vector[i] << std::endl;
  outputFile.close();
}

#endif // SERVICE_H
