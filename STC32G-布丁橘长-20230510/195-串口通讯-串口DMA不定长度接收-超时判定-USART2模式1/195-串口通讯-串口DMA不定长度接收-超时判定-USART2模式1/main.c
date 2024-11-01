//	@布丁橘长 2023/04/26
// 	串口2DMA接收不定长度数据示例：利用串口2中断，进行超时判定，实现串口2DMA接收不定长度数据
//	实验效果：PC端发送不定长度数据给MCU，MCU接收完成后，原样返回给PC（代码缓冲区设置为256字节，可以根据需要更改）
//  程序设置了2种发送模式， 实现DMA不定长度数据超时接收，printf串口打印该长度数据，以及固定长度串口DMA发送
//											  	发送模式1：接收数据长度小于256字节，使用超时判定，接收不定长度数据，然后返回该长度数据给PC端
//											  	发送模式2：接收数据长度等于256字节，使用串口DMA，将数据原样返回给PC
//  串口2使用默认引脚P1.0(RxD) P1.1(TxD)
//	实验开发板：STC32G12K128屠龙刀三.1 主频@22.1184MHz

#include <STC32G.H>
#include <stdio.h>
#include "config.h"

#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算
																										// 波特率115200
#define DMA_AMT_LEN 255 			// DMA传输总字节（AMT+1） 255+1=256字节
																																																	
u8 xdata DMABuffer[256];			// 数据存放在XRAM(XDATA区域)，需要使用关键字xdata
bit	DmaTxFlag;								// 发送完成标志
bit	DmaRxFlag;								// 接收完成标志
bit B_1ms;										// 1毫秒标志
bit busy;											// 串口忙标志
u16 Rx_cnt;										// Rx接收计数
u8 RX_TimeOut;								// 串口接收超时计数	
u8 i; 

void sysini(void);						// STC32初始化设置
void Blink();									// P2口LED闪烁
void Uart2Init();							// UART2初始化
void DMA_Config();						// DMA初始化
void Timer0_Init(void);				// 定时器0初始化
void UartPutc(u8 dat);				// 串口发送字符函数
char putchar(char c);					// 重构的putchar函数，用于printf函数串口打印

void main(void)
{
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	Uart2Init();								// 串口2初始化
	DMA_Config();
	EA = 1;											// 使能EA总中断
	
	DmaTxFlag = 0;							// 清零发送完成标志
	DmaRxFlag = 0;							// 清零接收完成标志
	while (1)
	{
		if((DmaTxFlag) && (DmaRxFlag))	// 当发送和接收完成标志均为1时，表示空闲
		{
			Rx_cnt = 0;							// 清零接收计数
			RX_TimeOut = 0;				// 清零接收超时计数
			DmaTxFlag = 0;					// 清零发送完成标志
			DMA_UR2T_CR = 0xc0;			// bit7 1:使能 UART2_DMA, bit6 1:开始 UART2_DMA 自动发送
			DmaRxFlag = 0;					// 清零接收完成标志
			DMA_UR2R_CR = 0xa1;			// bit7 1:使能 UART2_DMA, bit5 1:开始 UART2_DMA 自动接收, bit0 1:清除 FIFO
		}
		if(B_1ms) 								//1ms 到
		{
			B_1ms = 0;
			if(RX_TimeOut > 0)			// 超时计数
			{
				if(--RX_TimeOut == 0)					// 接收超时计数
				{
					DMA_UR2R_CR = 0x00; 					// 关闭 UART2_DMA
					printf("\r\n接收超时，以下是已接收到的数\xFD据：\r\n"); 	// UART2 发送 一个字符串,\xFD用于补全汉字‘数’的编码，防止出现乱码
					for(i=0;i<Rx_cnt;i++) printf("%bc",DMABuffer[i]);		// 将接收到的数据，发送给PC端
					printf("\r\n");								// 数据发送完成后，发送回车、换行符
					Rx_cnt = 0;										// 清零接收计数
					DMA_UR2R_CR = 0xa1; 					//bit7 1: 使能UART2_DMA，bit5 1: 开始 UART2_DMA 自动接收，bit0 1: 清除 FIFO
				}
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
void Timer0_Init(void)				//1毫秒@22.1184MHz
{
	AUXR |= 0x80;								//定时器时钟1T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0x9A;									//设置定时初始值
	TH0 = 0xA9;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}
void Uart2Init()							// UART2初始化
{
	S2CFG = 0x01;								// 串口2此位必须为1，否则可能产生不可预期后果

	S2CON = 0x50;								// 模式1（8位数据）、接收使能
	T2L = BRT;						
	T2H = BRT >> 8;							// 波特率对应的重装载值
	T2x12 = 1;									// 1T模式
	T2R = 1;										// 启动定时器2
	ES2 = 1;										// 使能串口2中断
}
void UartPutc(u8 dat)					// 串口发送字符函数
{
	busy = 1;										// 发送前，将忙标志busy置1
	S2BUF = dat;									// 需要发送的数据dat送入SBUF
	while(busy);								// 等待发送完成
}
char putchar(char c)					// 重构的putchar函数，用于printf函数串口打印
{
	UartPutc(c);								// 调用UartPutc串口发送字符函数
	return c;
}
void UART2_Isr (void) interrupt 8
{
	if(S2RI)										// 接收完成标志置1时
	{
		S2RI = 0;									// 清零接收完成标志
		Rx_cnt++;									// 接收计数+1
		if(Rx_cnt > DMA_AMT_LEN) Rx_cnt = 0;		// 接收计数大于等于DMA缓冲区长度，清零接收计数
		RX_TimeOut = 5; 					// 如果 5ms 没收到新的数据，判定一串数据接收完毕
	}
	if(S2TI)										// 发送标志置1时
	{
		S2TI = 0;									// 清零发送完成标志
		busy = 0;									// 清零串口忙标志
	}
}
void DMA_Config(void)
{
	DMA_UR2T_CFG = 0x80;				// bit7 1:使能串口2DMA发送中断
	DMA_UR2T_STA = 0x00;				// 清零串口2DMA发送完成中断标志、清零数据覆盖中断标志
	DMA_UR2T_AMT =  DMA_AMT_LEN;								// 设置传输总字节数(低8位)：n+1
	DMA_UR2T_AMTH = DMA_AMT_LEN >> 8;						// 设置传输总字节数(高8位)：n+1
	DMA_UR2T_TXAH = (u8)((u16)&DMABuffer >> 8);	// 设置传输数据的源地址，高8位
	DMA_UR2T_TXAL = (u8)((u16)&DMABuffer);			// 设置传输数据的源地址，低8位
	DMA_UR2T_CR = 0xc0;					// bit7 1:使能串口2DMA发送, bit6 1:开始DMA自动发送

	DMA_UR2R_CFG = 0x80;				// bit7 1:使能串口2DMA接收中断
	DMA_UR2R_STA = 0x00;				// 清零串口2DMA接收完成中断标志、清零数据丢弃中断标志 
	DMA_UR2R_AMT =  DMA_AMT_LEN;								// 设置传输总字节数(低8位)：n+1
	DMA_UR2R_AMTH = DMA_AMT_LEN >> 8;						// 设置传输总字节数(高8位)：n+1
	DMA_UR2R_RXAH = (u8)((u16)&DMABuffer >> 8);	// 设置传输数据的目标地址，高8位
	DMA_UR2R_RXAL = (u8)((u16)&DMABuffer);			// 设置传输数据的目标地址，低8位
	DMA_UR2R_CR = 0xa1;					//bit7 1:使能串口2DMA接收, bit5 1:开始DMA自动接收, bit0 1:清除 FIFO
}
void UART2_DMA_Interrupt(void) interrupt 13		// 串口DMA中断号大于31，借用13号保留中断中转
{																							// 详情参照布丁橘长-STC32系列视频第36期，或STC32手册第5.9章节
	if (DMA_UR2T_STA & 0x01)		// 发送完成中断标志为1时
	{
		DMA_UR2T_STA &= ~0x01;		// 清零发送完成中断标志
		DmaTxFlag = 1;						// 发送完成标志置1
	}
	if (DMA_UR2T_STA & 0x04)		// 数据覆盖中断标志为1时
	{
		DMA_UR2T_STA &= ~0x04;		// 清零数据覆盖中断标志
	}
	if (DMA_UR2R_STA & 0x01)		// 接收完成中断标志为1时
	{
		DMA_UR2R_STA &= ~0x01;		// 清零接收完成中断标志
		DmaRxFlag = 1;						// 接收完成标志置1
	}
	if (DMA_UR2R_STA & 0x02)		// 数据丢弃中断标志为1时
	{
		DMA_UR2R_STA &= ~0x02;		// 清零数据丢弃中断标志
	}
}