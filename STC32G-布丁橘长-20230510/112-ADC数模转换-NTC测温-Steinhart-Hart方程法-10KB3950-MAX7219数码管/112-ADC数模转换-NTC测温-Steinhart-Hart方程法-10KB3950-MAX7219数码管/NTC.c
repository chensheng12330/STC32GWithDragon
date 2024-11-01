#include "NTC.h"
#include "config.h"
#include "math.h"

#define Vref 2.5

float CalculationTemperature(u16 adc)
{
	float A = 1.14E-3,B = 2.34E-4,C = 7.54E-8;
	float Ka = 273.15;
  float temper;
	float Rt = 0.0;
	float RtV = 0.0;
	float R25 = 10000;
	
	RtV = (adc*(Vref/4096));    						// NTC电压
  Rt = (RtV*R25)/(Vref-RtV);								// NTC阻值
	temper = 1/(A + B*log(Rt) + C*pow(log(Rt),3))-Ka; // Steinhart-Hart方程
  return temper;
}