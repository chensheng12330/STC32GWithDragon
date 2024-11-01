//	@布丁橘长 2023/04/24
//  串口3DMA自动收发数据示例：UART3-模式0，主频22.1184MHz,波特率115200,8位数据方式，1位起始位，无校验位，1位停止位，
//	串口3DMA循环发送10000字节数据给PC端显示，同时P2口LED闪烁
//  串口3使用默认引脚P0.0(RxD) P0.1(TxD)
//	实验开发板：STC32G12K128屠龙刀三.1 主频@22.1184MHz

#include <STC32G.H>
#include "config.h"
#include "delay.h"

#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算
																										// 波特率115200
#define DMA_AMT_LEN 9999 			// DMA传输总字节（AMT+1） 9999+1=10000字节
																																																	
u8 xdata DmaBuffer[10000];		// 数据存放在XRAM(XDATA区域)，需要使用关键字xdata
bit	DmaTxFlag;								// 发送完成标志
bit	DmaRxFlag;								// 接收完成标志

volatile u16 i; 
void sysini(void);						// STC32初始化设置
void Blink();									// P2口LED闪烁
void Uart3Init();							// UART3初始化
void DMA_Config();						// DMA初始化

void main(void)
{
	sysini();										// STC32初始化设置
	Uart3Init();								// 串口3初始化
	DMA_Config();
	EA = 1;											// 使能EA总中断
	
	DmaTxFlag = 0;							// 清零发送完成标志
	DmaRxFlag = 0;							// 清零接收完成标志
	for(i = 0 ;i < 10000;i++)		// 10000个字符
	{
		DmaBuffer[i] = i%128;			// 10000个字符，为128个ASCII字符
	}
	while (1)
	{
		DmaTxFlag = 1;
		DmaRxFlag = 1;
		if((DmaTxFlag) && (DmaRxFlag))	// 当发送和接收完成标志均为1时，表示空闲
		{
			DmaTxFlag = 0;					// 清零发送完成标志
			DMA_UR3T_CR = 0xc0;			// bit7 1:使能 UART3_DMA, bit6 1:开始 UART3_DMA 自动发送
			DmaRxFlag = 0;					// 清零接收完成标志
			DMA_UR3R_CR = 0xa1;			// bit7 1:使能 UART3_DMA, bit5 1:开始 UART3_DMA 自动接收, bit0 1:清除 FIFO
		}
		Blink();
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
void Uart3Init()							// UART3初始化
{
	S3CON = 0x50;								// 模式0（8位数据）、接收使能，定时器3做波特率发生器
	T3L = BRT;						
	T3H = BRT >> 8;							// 波特率对应的重装载值
	T3x12 = 1;									// 1T模式
	T3R = 1;										// 启动定时器3
}
void Blink()									// P2口LED闪烁
{
	u8 i;
	for(i = 0;i < 6;i++)				// 闪烁3次（即亮灭6个过程）
	{
		P2 = ~P2;									// 取反实现亮灭
		delayms(100);							// 延时200ms
	}
}
void DMA_Config(void)
{
	DMA_UR3T_CFG = 0x80;				// bit7 1:使能串口3DMA发送中断
	DMA_UR3T_STA = 0x00;				// 清零串口3DMA发送完成中断标志、清零数据覆盖中断标志
	DMA_UR3T_AMT =  DMA_AMT_LEN;								// 设置传输总字节数(低8位)：n+1
	DMA_UR3T_AMTH = DMA_AMT_LEN >> 8;						// 设置传输总字节数(高8位)：n+1
	DMA_UR3T_TXAH = (u8)((u16)&DmaBuffer >> 8);	// 设置传输数据的源地址，高8位
	DMA_UR3T_TXAL = (u8)((u16)&DmaBuffer);			// 设置传输数据的源地址，低8位
	DMA_UR3T_CR = 0xc0;					// bit7 1:使能串口3DMA发送, bit6 1:开始DMA自动发送

	DMA_UR3R_CFG = 0x80;				// bit7 1:使能串口3DMA接收中断
	DMA_UR3R_STA = 0x00;				// 清零串口3DMA接收完成中断标志、清零数据丢弃中断标志 
	DMA_UR3R_AMT =  DMA_AMT_LEN;								// 设置传输总字节数(低8位)：n+1
	DMA_UR3R_AMTH = DMA_AMT_LEN >> 8;						// 设置传输总字节数(高8位)：n+1
	DMA_UR3R_RXAH = (u8)((u16)&DmaBuffer >> 8);	// 设置传输数据的目标地址，高8位
	DMA_UR3R_RXAL = (u8)((u16)&DmaBuffer);			// 设置传输数据的目标地址，低8位
	DMA_UR3R_CR = 0xa1;					//bit7 1:使能串口3DMA接收, bit5 1:开始DMA自动接收, bit0 1:清除 FIFO
}
void UART3_DMA_Interrupt(void) interrupt 13		// 串口DMA中断号大于31，借用13号保留中断中转
{																							// 详情参照布丁橘长-STC32系列视频第36期，或STC32手册第5.9章节
	if (DMA_UR3T_STA & 0x01)		// 发送完成中断标志为1时
	{
		DMA_UR3T_STA &= ~0x01;		// 清零发送完成中断标志
		DmaTxFlag = 1;						// 发送完成标志置1
	}
	if (DMA_UR3T_STA & 0x04)		// 数据覆盖中断标志为1时
	{
		DMA_UR3T_STA &= ~0x04;		// 清零数据覆盖中断标志
	}
	if (DMA_UR3R_STA & 0x01)		// 接收完成中断标志为1时
	{
		DMA_UR3R_STA &= ~0x01;		// 清零接收完成中断标志
		DmaRxFlag = 1;						// 接收完成标志置1
	}
	if (DMA_UR3R_STA & 0x02)		// 数据丢弃中断标志为1时
	{
		DMA_UR3R_STA &= ~0x02;		// 清零数据丢弃中断标志
	}
}