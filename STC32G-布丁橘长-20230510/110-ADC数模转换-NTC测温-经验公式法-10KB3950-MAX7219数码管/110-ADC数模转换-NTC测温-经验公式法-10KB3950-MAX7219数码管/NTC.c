#include "NTC.h"
#include "config.h"
#include <math.h>

#define     Vref     2.5

float CalculationTemperature(u16 adc)
{
    float Temper=0.0;												// 温度
    float Rt=0.0;														// NTC热敏电阻阻值
    float R25=10000.0;											// NTC热敏电阻25℃时的阻值：10K
    float T2=298.15;												// 273.15+25;
		float B=3950.0;													// 热敏指数3950
    float K=273.15;													// 绝对温度
    float RtV=0.0;													// NTC热敏电阻电压

    RtV = (adc*(Vref/4096));    						// NTC电压
    Rt = (RtV*R25)/(Vref-RtV);							// NTC阻值
    Temper = 1.0/(log(Rt/R25)/B + 1.0/T2)-K;	// 经验公式计算温度
    return Temper;													// 返回温度值
}