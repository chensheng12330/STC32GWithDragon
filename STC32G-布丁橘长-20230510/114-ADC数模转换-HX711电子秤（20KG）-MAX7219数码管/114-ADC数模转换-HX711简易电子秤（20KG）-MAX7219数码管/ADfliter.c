#include "ADfliter.h"
#include "STC32G.H"
#include "config.h"
#include "HX711.h"

extern u32 adc_result;					//	ADCת�����;
static u32 value;

u32 Fliter_average()						//	����ƽ���˲��㷨
{
	static u32 count1;
	u32 sum = 0;
	for(count1 = 0;count1 < Sample;count1++)
	{
		sum += HX711_Read();
	}
	return (u32)(sum / Sample);
}

u32 Fliter_range()							//	AD�޷��˲��㷨
{
	static u32 new_value;
	value = new_value;
	new_value = HX711_Read();
	if((new_value - value > 500)||(value - new_value > 500)) return value;//�������0.3V�������β���ֵ
	else return new_value;
}
u32 Fliter_middle()						//	AD��λֵ�˲��㷨
//	����sanmple�Σ�ð������ȡ�м�ֵ����������Sample���԰�������Ķ�,Sampleֵ��Ϊ����
{
	static u32 value_buf[Sample];
	static u32 count,i,j,temp;
	for(count = 0;count < Sample;count++)				//	��������sanple��
	{
		value_buf[count] =  HX711_Read();
	}
	for(j = 0;j < Sample-1;j++)									//	ð�ݷ�����
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
	return value_buf[(Sample-1)/2];							//�����м�ֵ
}