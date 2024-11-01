#ifndef __SEG595_H__
#define __SEG595_H__

#include <STC32G.H>		//使用了P6口，需要包含STC32G.h
#include "..\config\config.h"						//config.h			在工程文件夹外的config文件夹中

sbit DS = P6^6;				//串行输入DS(DIO)接P6.6
sbit SH = P6^5;				//串行输入时钟SH(SCK)接P6.5
sbit ST = P6^4;				//锁存时钟ST(RCK)接P6.4

void Write595(u8 dat);					//595写数据函数声明
void Output595();								//595输出显示函数声明
void SEG_Disp(u8 dat,u8 wei);		//数码管显示函数声明

#endif