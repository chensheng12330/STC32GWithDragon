//	@布丁橘长 2023/04/17
//  单片机和电脑串口通信示例：USART1-模式1，波特率115200,8位数据方式，1位起始位，无校验位，1位停止位，
//	按单片机P3.2按键，发送字符串“HellWorld”给电脑端，电脑端发送数字1，单片机P2口LED闪烁3次
//	实验开发板：STC32G12K128屠龙刀三.1 主频@11.0592MHz

#include <STC32G.H>
#include "config.h"
#include "delay.h"

#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算
																										// 波特率115200
																									
sbit KEY1 = P3^2;							// 板载P3.2按键
																									
bit busy;											// 串口忙标志
char wptr;										// 写指针
char rptr;										// 读指针
char buffer[16];							// 接收缓存，长度16
bit B_1ms;										// 1毫秒标志
bit key1flag;									// 按键1按下标志
bit blinkflag;								// 闪烁标志
char key1delay;								// 按键1计时

void sysini(void);						// STC32初始化设置
void Blink();									// P2口流水灯
void Timer0_Init(void);				// 1毫秒@11.0592MHz
void UartSend(char dat);			// 串口发送字符
void UartSendStr(char *p);		// 串口发送字符串
void Uart1Init();							// UART1初始化


void main(void)
{
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	Uart1Init();								// 串口1初始化
	ES = 1;											// 使能串口1中断
	EA = 1;											// 使能EA总中断
	
	while (1)
	{
		if(KEY1 == 0)														// P3.2按键按下
		{
			if(key1flag == 0)											// 当按下标志为0时
			{																			
				if(B_1ms == 1) key1delay++;					// 按键延时,每1毫秒+1
				if(key1delay >=10)									// 按下时间大于10毫秒
				{
					key1delay = 0;										// 清零按键延时
					key1flag = 1;											// 按下标志置1
					UartSendStr("HelloWorld!\r\n");		// 发送字符串“HelloWorld!”
				}
			}
		}
		if(KEY1 == 1)							// 按键松开
		{														
			key1flag = 0;						// 清零按键按下标志
			key1delay = 0;					// 清零按键延时
		}
		if(blinkflag == 1) 				// 当LED闪烁标志置1时
		{
			blinkflag = 0;					// 清零闪烁标志							
			Blink();								// P2口LED闪烁3次
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
//		buffer[wptr++] = SBUF;	// 接收长度16
//		wptr &= 0x0f;						// 接收长度16
		
		buffer[0] = SBUF;					// 接收长度1
		if(buffer[0] == '1') 			// 当接收到的字符是‘1’时
		{
			blinkflag = 1; 					// LED闪烁标志置1
		}
	}
}
void Uart1Init()							// UART1初始化
{
	SCON = 0x50;								// 模式1（8位数据）、接收使能
	T2L = BRT;						
	T2H = BRT >> 8;							// 波特率对应的重装载值
	S1BRT = 1;									// 定时器2做波特率发生器
	T2x12 = 1;									// 1T模式
	T2R = 1;										// 启动定时器2
	wptr = 0x00;								// 清零写指针
	rptr = 0x00;								// 清零读指针
	busy = 0;										// 清零忙标志
}
void UartSend(char dat)				// 发送字符函数
{
	while (busy);								// 当串口忙时，等待
	busy = 1;										// 忙标志置1
	SBUF = dat;									// 发送数据dat
}
void UartSendStr(char *p)			// 发送字符串函数
{
	while (*p)									// 字符串结束标志‘\0’前循环
	{
		UartSend(*p++);						// 逐个发送字符串的字符
	}
}
void Blink()									// P2口流水灯
{
	u8 i;
	for(i = 0;i < 6;i++)				// 闪烁3次（即亮灭6个过程）
	{
		P2 = ~P2;									// 取反实现亮灭
		delayms(200);							// 延时200ms
	}
}