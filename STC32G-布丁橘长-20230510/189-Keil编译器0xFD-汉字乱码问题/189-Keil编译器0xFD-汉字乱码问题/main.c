//	@布丁橘长 2023/04/23
//  Keil编译器0xFD编码，导致汉字乱码问题
//  USART1-模式1，主频22.1184MHz,波特率115200,8位数据方式，1位起始位，无校验位，1位停止位，
//  串口1使用默认引脚P3.0(RxD) P3.1(TxD)
//	按单片机P3.2按键，printf串口打印到电脑端
//	实验开发板：STC32G12K128屠龙刀三.1 主频@22.1184MHz

#include <STC32G.H>
#include <stdio.h>
#include "config.h"
#include "delay.h"

#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算
																										// 波特率115200																							
sbit KEY1 = P3^2;							// 板载P3.2按键
																									
bit busy;											// 串口忙标志
bit B_1ms;										// 1毫秒标志
bit key1flag;									// 按键1按下标志
char key1delay;								// 按键1计时

void sysini(void);						// STC32初始化设置
void Timer0_Init(void);				// 1毫秒@11.0592MHz
void Uart1Init();							// UART1初始化
void UartPutc(u8 dat);				// 串口发送字符函数
char putchar(char c);					// putchar函数，用于串口打印

void main(void)
{
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	Uart1Init();								// 串口1初始化
	ES = 1;											// 使能串口1中断
	EA = 1;											// 使能EA总中断
	
	while (1)
	{
		if(KEY1 == 0)							// P3.2按键按下
		{
			if(key1flag == 0)				// 当按下标志为0时
			{																			
				if(B_1ms == 1)				// 1毫秒到
				{	
					B_1ms = 0;					// 清零1毫秒标志
					key1delay++;				// 按键延时,每1毫秒+1
				}					
				if(key1delay >=10)		// 按下时间大于10毫秒
				{
					key1delay = 0;			// 清零按键延时
					key1flag = 1;				// 按下标志置1
					printf("打印开始\r\n");
					printf("例\xFD如\r\n");		// 使用\xFD，在包含0xFD的汉字后面，插入0xFD，补全数据
					printf("数\xFD量\r\n");		// 使用\xFD，在包含0xFD的汉字后面，插入0xFD，补全数据
					printf("三\xFD个\r\n");		// 使用\xFD，在包含0xFD的汉字后面，插入0xFD，补全数据
					printf("她\xFD的\r\n");		// 使用\xFD，在包含0xFD的汉字后面，插入0xFD，补全数据
					printf("听\xFD说\r\n");		// 使用\xFD，在包含0xFD的汉字后面，插入0xFD，补全数据
					printf("打印结束\r\n");
				}
			}
		}
		if(KEY1 == 1)							// 按键松开
		{														
			key1flag = 0;						// 清零按键按下标志
			key1delay = 0;					// 清零按键延时
		}
	}
}
void sysini()
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 1毫秒标志
}

void Timer0_Init(void)				// 1毫秒@11.0592MHz
{
	AUXR |= 0x80;								// 定时器时钟1T模式
	TMOD &= 0xF0;								// 设置定时器模式
	TL0 = 0xCD;									// 设置定时初始值
	TH0 = 0xD4;									// 设置定时初始值
	TF0 = 0;										// 清除TF0标志
	TR0 = 1;										// 定时器0开始计时
	ET0 = 1;										// 使能定时器0中断
}

void Uart1Isr() interrupt 4
{
	if (TI)											// 发送中断标志置1时
	{
		TI = 0;										// 清零发送中断标志
		busy = 0;									// 清零忙状态
	}
	
	if (RI)											// 接收中断标志置1时
	{
		RI = 0;										// 清零接收中断标志
	}
}
void Uart1Init()							// UART1初始化
{
	/*
	S1_S1 = 0;
	S1_S0 = 1;									// 串口1切换到P3.6(RxD) P3.7(TxD)
	*/
	SCON = 0x50;								// 模式1（8位数据）、接收使能
	T2L = BRT;						
	T2H = BRT >> 8;							// 波特率对应的重装载值
	S1BRT = 1;									// 定时器2做波特率发生器
	T2x12 = 1;									// 1T模式
	T2R = 1;										// 启动定时器2
	busy = 0;										// 清零忙标志
}
void UartPutc(u8 dat)					// 串口发送字符函数
{
	SBUF = dat;									// 发送字符dat
	while(!TI);									// 等待发送完成
	TI = 0;											// 清零发送完成标志
}
char putchar(char c)					// printf需要调用putchar函数，这里构造一个使用串口发送字符的putchar函数
{															
	UartPutc(c);								// 串口发送字符
	return c;										// 返回字符
}