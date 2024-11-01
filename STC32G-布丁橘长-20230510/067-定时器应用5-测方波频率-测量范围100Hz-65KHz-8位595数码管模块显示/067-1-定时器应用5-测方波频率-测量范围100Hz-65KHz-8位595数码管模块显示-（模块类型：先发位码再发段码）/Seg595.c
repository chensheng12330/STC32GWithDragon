#include "Seg595.h"

//共阳极段码数组		0(0)	1(1)	2(2)	3(3)	4(4)	5(5)	6(6)	7(7)	8(8)	9(9)
u8 SEG_Code[26] = {	0xc0,	0xf9,	0xa4,	0xb0,	0x99,	0x92,	0x82,	0xf8,	0x80,	0x90,
//									A(10)	b(11)	C(12)	d(13) E(14) F(15) H(16) L(17) o(18 P(19)									 
										0x88,	0x83,	0xc6,	0xa1,	0x86,	0x8e,	0x89,	0xc7,	0xa3,	0x8c,
//	               	q(20)	U(21)	灭(22) -(23) 亮(24)
										0x98,	0xc1,	0xff,	0xbf,	0x00}; 
//共阳极位码数组-4位数码管
u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//共阳极位码数组，COM1接595Q0，COM2接Q1...即数码管左边第一位接Q0...第八位接Q7

void Write595(u8 dat)			//595写数据函数
{
	u16 i;
	for(i = 0;i < 8;i++)		
	{
		SH = 0;								//拉低串行输入时钟SCLK
		DS = dat & 0x80;			//取最高位
		SH = 1;								//SCKL时钟上升沿，数据存移位寄存器
	  dat <<= 1;						//每次一位
	}
}
void Output595()					//595输出函数
{
	ST = 0;									//拉低ST时钟
	ST = 1;									//ST时钟上升沿，更新输出存储器数据
}

void SEG_Disp(u8 dat,u8 wei)			//数码管显示函数
{
	u8 datcode,weicode;
	datcode = SEG_Code[dat];				//取十进制dat对应的段码
	weicode = WEI_Code[wei-1];			//取十进制wei对应的位码
	
	Write595(weicode);							//先发位码
	Write595(datcode);							//再发段码
	Output595();										//595输出显示
}