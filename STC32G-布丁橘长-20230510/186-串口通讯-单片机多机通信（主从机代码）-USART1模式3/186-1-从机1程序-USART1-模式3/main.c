//	@布丁橘长 2023/04/20
//	单片机多机通信示例，从机1程序，从机1地址：0x01
//	波特率115200，主频11.0592MHz，1位起始位、9位数据、1位校验位、1位停止位
//	USART1：模式3-9位数据可变波特率模式，为了方便调试，切换串口1引脚到P3.6(RxD)、P3.7(TxD)
//	各从机RXD和主机TXD相连，各从机TXD和主机RXD相连，主机和各从机GND相连
//  实验效果：主机-P3.2按键按下，发送字符'1'给从机1，	 从机1点亮P2.0 LED1，再次发送熄灭
// 						主机-P3.3按键按下，发送字符'2'给从机2，	 从机2点亮P2.1 LED2，再次发送熄灭
//						主机-P3.4按键按下，发送字符'3'给从机3，	 从机3点亮P2.2 LED3，再次发送熄灭
//						主机-P3.5按键按下，发送字符'4'给从机1-3，从机1-3点亮P2.0-P2.3 LED1-LED4，再次发送熄灭
//						从机1-P3.2按键按下，发送字符‘1’给主机，  主机点亮P2.0 LED1，再次发送熄灭
//						从机2-P3.2按键按下，发送字符‘2’给主机，  主机点亮P2.1 LED2，再次发送熄灭
//						从机3-P3.2按键按下，发送字符‘3’给主机，  主机点亮P2.2 LED3，再次发送熄灭
//	该程序只设计了主机和各从机的通信，从机之间不能通信
//	实验开发板：STC32G12K128屠龙刀三.1 主频@11.0592MHz

#include <STC32G.H>
#include "config.h"

#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算
																										// 波特率11520
#define RECEIVING 1						// 接收数据状态
#define READY	0								// 就绪状态

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
u8 state;											// 程序状态：READY：就绪   RECEIVING：接收中
u8 LED1nums,LEDAllnums;				// LED亮灭次数

void sysini(void);						// STC32初始化设置
void Timer0_Init(void);				// 1毫秒@11.0592MHz
void UartSend(char dat);			// 串口发送字符
void UartSendStr(char *p);		// 串口发送字符串
void Uart1Init();							// UART1初始化
void Keyscan();								// 按键扫描函数

void main(void)
{
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	Uart1Init();								// 串口1初始化
	ES = 1;											// 使能串口1中断
	EA = 1;											// 使能EA总中断
	
	while (1)
	{
		Keyscan();								// 按键扫描函数
		if(blinkflag == 1) 				// 当LED闪烁标志置1时
		{
			blinkflag = 0;					// 清零闪烁标志
			if(buffer[0] == '1')		// 当接收到的字符是'1'
			{
				LED1nums++;						// LED1nums+1
				LED1nums &= 0x0F;			// 保留低4位数据，用于防溢出，当LED1nums大于15，清零
				if(LED1nums & 0x01) P2 = 0xFE;	// LED1nums为奇数时，点亮P2.0 LED1
				else P2 = 0xFF;				// LED1nums为偶数时，熄灭LED
			}	
			if(buffer[0] == '4')		// 当接收到的字符是‘4’
			{
				LEDAllnums++;					// LEDALLnums+1
				LEDAllnums &= 0x0F;		// 保留低4位数据，用于防溢出，当LEDALLnums大于15，清零
				if(LEDAllnums & 0x01) P2 = 0x00;	// LEDALLnums为奇数时，点亮P2口8个LED
				else P2 = 0xFF;				// LEDALLnums为偶数时，熄灭LED
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
		if(RB8)										// 地址接收标志置1时
		{
			SM2 = 0;								// 退出多机通信状态，准备接收数据
			state = RECEIVING;			// 状态转为 接收中	
		}
		if(!RB8 && state == RECEIVING)	// 当RB8为0时并且从机是接收状态
		{
			state = READY;					// 状态转为 就绪
			buffer[0] = SBUF;				// 读取接收到的数据
			SM2 = 1;								// 进入多机通信状态，地址监测
			blinkflag = 1;					// 闪烁标志置1
		}
	}
}
void Uart1Init()							// UART1初始化
{
	S1_S1 = 0;	
	S1_S0 = 1;									// 切换串口1引脚到P3.6(RxD) P3.7(TxD)
	
	SCON = 0xD0;								// 模式3（9位数据）、接收使能、禁止多机通信
	T2L = BRT;						
	T2H = BRT >> 8;							// 波特率对应的重装载值
	S1BRT = 1;									// 定时器2做波特率发生器
	T2x12 = 1;									// 1T模式
	T2R = 1;										// 启动定时器2
	
// SADDR和SADEN用于自动地址识别，当主机发送的地址和从机地址匹配，自动中断
	SADDR = 0x01;								// 从机1地址：0x01
	SADEN = 0xFF;								// 自动地址识别，地址8位均识别
	
	wptr = 0x00;								// 清零写指针
	rptr = 0x00;								// 清零读指针
	busy = 0;										// 清零忙标志
}
void UartSend(char dat)				// 发送字符函数
{
	while (busy);								// 当串口忙时，等待
	busy = 1;										// 忙标志置1
	TB8 = 0;										// 数据帧标志
	SBUF = dat;									// 发送数据dat
}
void UartSendAddr(char addr)	// 发送地址函数
{
	while (busy);								// 当串口忙时，等待
	busy = 1;										// 忙标志置1
	TB8 = 1;										// 地址帧标志
	SBUF = addr;								// 发送数据dat
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
	if(KEY1 == 0)								// P3.2按键按下
	{
		if(key1flag == 0)					// 当按下标志为0时
		{																			
			if(B_1ms == 1) 					// 1毫秒到		
			{
				B_1ms = 0;						// 清零1毫秒标志
				key1delay++;					// 按键延时,每1毫秒+1
			}			
			if(key1delay >=10)			// 按下时间大于10毫秒
			{
				key1delay = 0;				// 清零按键延时
				key1flag = 1;					// 按下标志置1
				UartSend('1');				// 发送字符'1'
			}
		}
	}
	if(KEY1 == 1)								// 按键松开
	{														
		key1flag = 0;							// 清零按键按下标志
		key1delay = 0;						// 清零按键延时
	}
}