#ifndef __MAX7219_H__
#define __MAX7219_H__

#include <STC32G.h>

sbit CS = P6^5;          		//MAX7219片选CS          	
sbit DIN = P6^6;           	//MAX7219串行数据DIN     
sbit CLK = P6^4;           	//MAX7219串行时钟CLK    
 
#define DECODE_MODE  0x09   //译码控制寄存器
#define INTENSITY    0x0A   //亮度控制寄存器
#define SCAN_LIMIT   0x0B   //扫描界限寄存器
#define SHUT_DOWN    0x0C   //关断模式寄存器
#define DISPLAY_TEST 0x0F   //测试控制寄存器     
#define DP 0x80								// 小数点位

void Write7219(unsigned char address,unsigned char dat);
void MAX7219_Ini(void);

#endif