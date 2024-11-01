#include "ADfliter.h"
#include "STC32G.H"
#include "config.h"

extern u16 adc_result;					//	ADC转换结果;
static u16 value;

u16 Fliter_average()						//	算术平均滤波算法
{
 u16 count1;
	u32 sum = 0;
	for(count1 = 0;count1 < Sample;count1++)
	{
		while(!ADC_FLAG);
		sum += ADC_RES*256 + ADC_RESL;
		ADC_FLAG = 0;
		ADC_START = 1;
	}
	return (u16)(sum / Sample);
}

u16 Fliter_range()							//	AD限幅滤波算法
{
	static u16 new_value;
	value = new_value;
	while(!ADC_FLAG);
		new_value = ADC_RES*256 + ADC_RESL;
		ADC_FLAG = 0;
		ADC_START = 1;
	if((new_value - value > 500)||(value - new_value > 500)) return value;//跳变大于0.3V放弃本次采样值
	else return new_value;
}
u16 Fliter_middle()						//	AD中位值滤波算法
//	采样sanmple次，冒泡排序，取中间值。采样次数Sample可以按照需求改动,Sample值需为奇数
{
	static u16 value_buf[Sample];
	static u16 count,i,j,temp;
	for(count = 0;count < Sample;count++)				//	连续采样sanple次
	{
		while(!ADC_FLAG);
		value_buf[count] =  ADC_RES*256 + ADC_RESL;
		ADC_FLAG = 0;
		ADC_START = 1;
	}
	for(j = 0;j < Sample-1;j++)									//	冒泡法排序
	{
		for(i = 0;i < Sample-1-j;i++)
		{
			if(value_buf[i] > value_buf[i+1])
			{
				temp = value_buf[i];
				value_buf[i] = value_buf[i+1];
				value_buf[i+1] = temp;
			}
		}
	}
	return value_buf[(Sample-1)/2];							//返回中间值
}