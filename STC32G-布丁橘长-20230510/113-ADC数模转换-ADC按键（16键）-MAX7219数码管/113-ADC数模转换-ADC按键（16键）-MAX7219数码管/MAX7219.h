#ifndef __MAX7219_H__
#define __MAX7219_H__

#include <STC32G.h>

sbit CS = P6^5;          		//MAX7219ƬѡCS          	
sbit DIN = P6^6;           	//MAX7219��������DIN     
sbit CLK = P6^4;           	//MAX7219����ʱ��CLK    
 
#define DECODE_MODE  0x09   //������ƼĴ���
#define INTENSITY    0x0A   //���ȿ��ƼĴ���
#define SCAN_LIMIT   0x0B   //ɨ����޼Ĵ���
#define SHUT_DOWN    0x0C   //�ض�ģʽ�Ĵ���
#define DISPLAY_TEST 0x0F   //���Կ��ƼĴ���     
#define DP 0x80								// С����λ

void Write7219(unsigned char address,unsigned char dat);
void MAX7219_Ini(void);

#endif