#ifndef __ADFLITER_H__
#define __ADFLITER_H__

#include "config.h"

#define Sample 21								// ��������

u16 Fliter_range();							// ADC�޷��˲��㷨
u16 Fliter_middle();						// ADC��λֵ�˲��㷨������Sample�Σ�ð������ȡ�м�ֵ����������Sample���԰�������Ķ�,Sampleֵ��Ϊ������
u16 Fliter_average();						// ADC����ƽ���˲���

#endif
