#ifndef __ADFLITER_H__
#define __ADFLITER_H__

#include "config.h"

#define Sample 5								// ��������

u32 Fliter_range();							// ADC�޷��˲��㷨
u32 Fliter_middle();						// ADC��λֵ�˲��㷨������Sample�Σ�ð������ȡ�м�ֵ����������Sample���԰�������Ķ�,Sampleֵ��Ϊ������
u32 Fliter_average();						// ADC����ƽ���˲���

#endif
