#ifndef __SEG595_H__
#define __SEG595_H__

#include <STC32G.H>
#include <config.h>

sbit DS = P6^6;				//��������DS(DIO)��P6.6
sbit SH = P6^5;				//��������ʱ��SH(SCK)��P6.5
sbit ST = P6^4;				//����ʱ��ST(RCK)��P6.4

void Write595(u8 dat);					//595д���ݺ�������
void Output595();								//595�����ʾ��������
void SEG_Disp(u8 dat,u8 wei);		//�������ʾ��������

#endif