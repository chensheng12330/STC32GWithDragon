//	@布丁橘长 2023/04/18
//	单片机双机通信示例，单片机1和单片机2均烧录此代码，波特率115200，主频11.0592MHz，1位起始位、8位数据、1位停止位、无校验位
//	UART4：模式0-8位数据可变波特率模式，使用默认引脚P0.2(RxD) P0.3(TxD)
//	单片机1串口和单片机2串口，交叉连接：单片机1P0.2连单片机2P0.3，单片机1P0.3连单片机2P0.2
//  实验效果：单片机1-P3.2按键按下，发送字符'1'给单片机2，单片机2点亮P2.0LED
// 						单片机1-P3.3按键按下，发送字符'2'给单片机2，单片机2点亮P2.1LED
//						单片机1-P3.4按键按下，发送字符'3'给单片机2，单片机2点亮P2.2LED
//						单片机1-P3.5按键按下，发送字符'4'给单片机2，单片机2点亮P2.3LED
//						同理，反过来，单片机2按键，发送对应的字符给单片机1，单片机1点亮对应的LED
//	实验开发板：STC32G12K128屠龙刀三.1 主频@11.0592MHz

#include <STC32G.H>
#include "config.h"

#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算
																										// 波特率115200
																									
sbit KEY1 = P3^2;							// 板载P3.2按键
sbit KEY2 = P3^3;							// 板载P3.3按键
sbit KEY3 = P3^4;							// 板载P3.4按键
sbit KEY4 = P3^5;							// 板载P3.5按键
																									
bit busy;											// 串口忙标志
char wptr;										// 写指针
char rptr;										// 读指针
char buffer[16];							// 接收缓存，长度16
bit B_1ms;										// 1毫秒标志
bit key1flag,key2flag,key3flag,key4flag;				// 按键1按下标志
char key1delay,key2delay,key3delay,key4delay;		// 按键1计时
bit blinkflag;								// 闪烁标志


void sysini(void);						// STC32初始化设置
void Blink();									// P2口流水灯
void Timer0_Init(void);				// 1毫秒@11.0592MHz
void UartSend(char dat);			// 串口发送字符
void UartSendStr(char *p);		// 串口发送字符串
void Uart4Init();							// UART4初始化
void Keyscan();								// 按键扫描函数

void main(void)
{
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	Uart4Init();								// 串口4初始化
	ES4 = 1;										// 使能串口4中断
	EA = 1;											// 使能EA总中断
	
	while (1)
	{
		Keyscan();								// 按键扫描函数
		if(blinkflag == 1) 				// 当LED闪烁标志置1时
		{
			switch (buffer[0])						// 判断接收的字符
			{
				case '1':P2 = 0xFE;break;		// 字符‘1’，点亮P2.0LED
				case '2':P2 = 0xFD;break;		// 字符‘2’，点亮P2.1LED
				case '3':P2 = 0xFB;break;		// 字符‘3’，点亮P2.2LED
				case '4':P2 = 0xF7;break;		// 字符‘4’，点亮P2.3LED
				default:break;
			}
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

void Uart4Isr() interrupt 18
{
	if (S4TI)										// 发送中断标志置1时
	{
		S4TI = 0;									// 清零发送中断标志
		busy = 0;									// 清零忙状态
	}
	if (S4RI)										// 接收中断标志置1时
	{
		S4RI = 0;									// 清零接收中断标志
//		buffer[wptr++] = S4BUF;	// 接收长度16
//		wptr &= 0x0f;						// 接收长度16
		
		buffer[0] = S4BUF;				// 接收长度1
		blinkflag = 1;						// 闪烁标志置1
	}
}
void Uart4Init()							// UART4初始化
{
	S4CON = 0x50;								// 模式0（8位数据）、接收使能，定时器4做波特率发生器
	T4L = BRT;						
	T4H = BRT >> 8;							// 波特率对应的重装载值
	T4x12 = 1;									// 1T模式
	T4R = 1;										// 启动定时器4
	wptr = 0x00;								// 清零写指针
	rptr = 0x00;								// 清零读指针
	busy = 0;										// 清零忙标志
}
void UartSend(char dat)				// 发送字符函数
{
	while (busy);								// 当串口忙时，等待
	busy = 1;										// 忙标志置1
	S4BUF = dat;								// 发送数据dat
}
void UartSendStr(char *p)			// 发送字符串函数
{
	while (*p)									// 字符串结束标志‘\0’前循环
	{
		UartSend(*p++);						// 逐个发送字符串的字符
	}
}
void Keyscan()
{
//-----------------------KEY1：P3.2按键，发送字符'1'-----------------------------------
	if(KEY1 == 0)														// P3.2按键按下
	{
		if(key1flag == 0)											// 当按下标志为0时
		{																			
			if(B_1ms == 1) key1delay++;					// 按键延时,每1毫秒+1
			if(key1delay >=10)									// 按下时间大于10毫秒
			{
				key1delay = 0;										// 清零按键延时
				key1flag = 1;											// 按下标志置1
				UartSend('1');										// 发送字符串'1'
			}
		}
	}
	if(KEY1 == 1)							// 按键松开
	{														
		key1flag = 0;						// 清零按键按下标志
		key1delay = 0;					// 清零按键延时
	}
//-----------------------KEY2：P3.3按键，发送字符'2'-----------------------------------
	if(KEY2 == 0)														// P3.3按键按下
	{
		if(key2flag == 0)											// 当按下标志为0时
		{																			
			if(B_1ms == 1) key2delay++;					// 按键延时,每1毫秒+1
			if(key2delay >=10)									// 按下时间大于10毫秒
			{
				key2delay = 0;										// 清零按键延时
				key2flag = 1;											// 按下标志置1
				UartSend('2');										// 发送字符'2'
			}
		}
	}
	if(KEY2 == 1)							// 按键松开
	{														
		key2flag = 0;						// 清零按键按下标志
		key2delay = 0;					// 清零按键延时
	}	
//-----------------------KEY3：P3.4按键，发送字符'3'-----------------------------------
	if(KEY3 == 0)														// P3.4按键按下
	{
		if(key3flag == 0)											// 当按下标志为0时
		{																			
			if(B_1ms == 1) key3delay++;					// 按键延时,每1毫秒+1
			if(key3delay >=10)									// 按下时间大于10毫秒
			{
				key3delay = 0;										// 清零按键延时
				key3flag = 1;											// 按下标志置1
				UartSend('3');										// 发送字符'3'
			}
		}
	}
	if(KEY3 == 1)							// 按键松开
	{														
		key3flag = 0;						// 清零按键按下标志
		key3delay = 0;					// 清零按键延时
	}	
//-----------------------KEY4：P3.5按键，发送字符'4'-----------------------------------
	if(KEY4 == 0)														// P3.5按键按下
	{
		if(key4flag == 0)											// 当按下标志为0时
		{																			
			if(B_1ms == 1) key4delay++;					// 按键延时,每1毫秒+1
			if(key4delay >=10)									// 按下时间大于10毫秒
			{
				key4delay = 0;										// 清零按键延时
				key4flag = 1;											// 按下标志置1
				UartSend('4');										// 发送字符'4'
			}
		}
	}
	if(KEY4 == 1)							// 按键松开
	{														
		key4flag = 0;						// 清零按键按下标志
		key4delay = 0;					// 清零按键延时
	}	
}