#include "service.h"

#include <cmath>
#include <cstdlib>
#include <string>

using namespace std;

// ������� ��������� ���������������� ���������� ��������������� ���������� �������� (����� ���������� ����������)
int service::uniformDiscreteDistribution(int A, int B)
{
   // ������� �� ����� ������-�� (Accelerated C++)
   int range = B - A + 1;
   const int bucket_size = RAND_MAX / range;
   int r;

   do r = rand() / bucket_size;
   while (r>=range);

   return A + r;
}

// ������� ��������� ����������������� ������ ���������� ��������������� ����� (���� ����� ��������� ����������, ���� �� ������� �����) - �������� ��������
double service::uniformDistribution(double A, double B, bool include_start/* = true*/, bool include_end /*= true*/)
{
   const int SCALE = 10000;
   // ������������ ��������, ����� ������������� �������� ��������� ��� �� ��������� ������
   //A += (!include_start) * 1.0F/static_cast<double>(scale);
   //B -= (!include_end) * 1.0F/static_cast<double>(scale);
	/* ����� ��������, �� ������������� ����� �����������! */
   return (uniformDiscreteDistribution(0 + 1 * (!include_start), SCALE - 1 * (!include_end) )/static_cast<double>(SCALE)) * (B - A) + A;
}

// ������� ��������� ����������������� ����� ���������� ��������������� ����� (���� ����� ��������� ����������, ���� �� ������� �����) - �������� ������
// ���� �������� ��� ����� ���������, �� ������������� ���������� "��������" ����������� - ���������� ������� �������
double service::uniformDistributionWeak(double A, double B, bool include_start/* = true*/, bool include_end /*= true*/){
	return (rand() % ( RAND_MAX + 1 - 1 * (!include_start) - 1 * (!include_end)) + 1 * (!include_start)) / static_cast<double>(RAND_MAX) * (B - A) + A;
}

// ������� ��������� ���������������� ��������� ��������������� �����
double service::normalDistribution(double mean, double variance)
{
   /* ��������� � �������������� ����������� ���������� �������
   int RandomNumbers = 12; // ���������� ��������� �����, ������� ������������ ��� ��������� ������ ��������� ���������������
   double RandomSum = 0.0; // ����� ������������������ ��������� �����
   for (int i=0; i<RandomNumbers; i++)
      RandomSum += rand()%10001 / 10000.0;

   return Variance * (RandomSum - 6) + Mean; // Variance * sqrt(12.0/RandomNumbers) * (RandomSum - RandomNumbers/6.0) + Mean;*/

   /* ��������� ������� �����-������� */
   return variance*sqrt(-2*log(uniformDistribution(0, 1, false)))*cos(2*PI*uniformDistribution(0, 1, false)) + mean;
}

// ��������� �������� ����������� ����� � �������� (������� ������� � ������ �������)
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

// ��������� �������� ��������� ����� � ���������� (������� ������� ������ ���� � ������ �������)
int service::binToDec(bool binNumber[], int binResolution)
{
   int _decNumber = 0;
   for (int currentPosition = 1; currentPosition <= binResolution; ++currentPosition)
      _decNumber += static_cast<int>(pow(2.0, binResolution - currentPosition)) * binNumber[currentPosition - 1];

   return _decNumber;
}

// ��������� ��������������� ����������� ����� � ����������������� - ����������
// �� ��������� ���������� service::MAX_HEX_RESOLUTION ��������, ��� ��� � ������ ����� ���� ����, � ������� ����� ����� ����������
void service::decToHex(int decNumber, string& hexNumber, int hexResolution /* = MAX_HEX_RESOLUTION*/)
{
	hexNumber = "";
	for (int currentPosition = 1; currentPosition <= hexResolution; ++currentPosition)
   {
      int integralPart = decNumber / static_cast<int>(pow(16.0, hexResolution-currentPosition)); 
		if (integralPart > 9) // ���� ��� ������
			hexNumber += ('A' + integralPart - 10);
      else // ���� ��� �����
			hexNumber += ('0' + integralPart);
      decNumber = decNumber % 16;
   }
}