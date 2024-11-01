//	@布丁橘长 2023/04/22
//  printf串口打印-双机通信示例：USART1-模式1，主频22.1184MHz,波特率115200,8位数据方式，1位起始位，无校验位，1位停止位，
//	单片机1和单片机2均烧录此代码，
//	每按一次P3.2按键，向对向单片机发送的字符+1，对向单片机相应LED闪烁3次
//  串口1引脚切换到P3.6(RxD) P3.7(TxD)使用
//	实验开发板：STC32G12K128屠龙刀三.1 主频@22.1184MHz

#include <STC32G.H>
#include <stdio.h>
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
u8 LEDnums;										// LED序号，用于P2口第几个LED闪烁

u8 number1;										// 测试变量，发送次数 unsigned char 类型

void sysini(void);						// STC32初始化设置
void Blink(u8 nums);					// P2口第nums个LED闪烁，nums取值1-8
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
					number1++;					// 每次发送后number1+1
					if(number1 > 8) number1 = 1;	// 防溢出，number取值1-8
// Keil 扩展了 b h l，对应字节宽度，b-8位，h-16位(默认)，l-32位
// printf中有符号对应 %d，无符号对应%u 或 %x， 浮点型：%f，字符串 %s
// u8  对应 %bu 或 %bx，	char 对应 %bd
// u16 对应 %hu	或 %hx，	int  对应 %d 或 %hd
// u32 对应 %lu 或 %lx，  long 对应 %ld
					printf("%bu",number1);	// 发送unsigned char类型，number1是	u8	类型，需要使用 %bu 或 %bx 
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
			blinkflag = 0; 					// LED闪烁标志置1
			if(buffer[0] > 48 && buffer[0] < 57)	LEDnums = buffer[0] - 48;	
			// LED序号等于接收到的字符减48，比如接收到字符‘1’，LED序号 = ‘1’-48 = 49 -48 = 1;
			Blink(LEDnums);
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
		blinkflag = 1;						// 闪烁标志置1
	}
}
void Uart1Init()							// UART1初始化
{
	S1_S1 = 0;
	S1_S0 = 1;									// 串口1切换到P3.6(RxD) P3.7(TxD)
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
void UartPutc(u8 dat)					// 串口发送字符函数
{
	SBUF = dat;									// 发送字符dat
	while(!TI);									// 等待发送完成
	TI = 0;											// 清零发送完成标志
}
char putchar(char c)					// printf需要调用putchar函数，这里构造一个使用串口发送字符的putchar函数
{															// 也可以不重构putchar，直接使用keil自带的，
															// 但使用printf前需要查询TI状态，TI为1才能发送，发送完成也需要清零TI
	UartPutc(c);								// 串口发送字符
	return c;										// 返回字符
}
void Blink(u8 nums)						// P2口第nums个LED闪烁，nums取值1-8
{
	u8 i;
	for(i = 0;i < 3;i++)				// 闪烁3次
	{
		P2 = ~(0x01 << (nums-1));	// 点亮第nums个LED					
		delayms(200);							// 延时200ms
		P2 = 0xFF;								// 熄灭所有			
		delayms(200);							// 延时200ms
	}
}