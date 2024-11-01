/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于STC32G核心转接板（屠龙刀）进行编写测试，需要焊接LIN总线收发器电路。

USART类型LIN1做主机，LIN2做从机进行主从收发.

短按一下P32口按键, LIN1主机发送完整一帧数据.
短按一下P33口按键, LIN1主机发送帧头并获取LIN2从机应答数据（合并成一串完整的帧），从USB-CDC串口打印出来.

LIN2从机接收到主机发送的数据，从USB-CDC串口打印出来.

USB-CDC串口打印主机发送完整一帧数据：
LIN2 Read: 0x55 0x32 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x5b 

LIN1主机发送帧头，LIN2从机应答数据（合并成一串完整的帧）：
LIN2 Read: 0x55 0xd3 
LIN1 Read: 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0xda 

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_cdc_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。
   (注意：使用CDC接口触发MCU复位并自动下载功能，需要勾选端口设置：下次强制使用”STC USB Writer (HID)”进行ISP下载)

默认传输速率：9600波特率, 用户可自行修改.

下载时, 默认时钟 24MHz (用户可自行修改频率).

******************************************/

#define PRINTF_HID           //printf输出直接重定向到USB接口(包含usb.h前定义)

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件
#include "stdio.h"
#include "intrins.h"

/*****************************************************************************/

#define MAIN_Fosc        24000000UL
#define Baudrate         9600UL
#define Timer0_Reload    (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒
#define TM               (65536UL -(MAIN_Fosc/Baudrate/4))

/*****************************************************************************/

sbit SLP1_N = P7^6;     //0: Sleep
sbit SLP2_N = P7^7;     //0: Sleep

/*************  本地常量声明    **************/

#define FRAME_LEN           8    //数据长度: 8 字节
#define UART_BUF_LENGTH     64

/***************  结构声明    ***************/

#define	USART1	1
#define	USART2	2

typedef struct
{ 
	u8	id;				//串口号

	u8	TX_read;		//发送读指针
	u8	TX_write;		//发送写指针
	u8	B_TX_busy;		//忙标志

	u8 	RX_Cnt;			//接收字节计数
	u8	RX_TimeOut;		//接收超时
	u8	B_RX_OK;		//接收块完成
} COMx_Define; 

COMx_Define	COM1;
COMx_Define	COM2;

/*************  本地变量声明    **************/

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

u8 Lin_ID;
u8 TX1_BUF[8];
u8 TX2_BUF[8];

bit B_ULinRX1_Flag;
bit B_ULinRX2_Flag;

bit B_1ms;          //1ms标志

u16 Key1_cnt;
u16 Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;
bit Key2_Short_Flag;
bit Key2_Long_Flag;

bit Key1_Function;
bit Key2_Short_Function;
//bit Key2_Long_Function;

u8  RX1_Buffer[UART_BUF_LENGTH]; //接收缓冲
u8  RX2_Buffer[UART_BUF_LENGTH]; //接收缓冲

/*************  本地函数声明    **************/

void LinInit();
void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat);
void UsartLinSendHeader(u8 USARTx, u8 lid);
void UsartLinSendData(u8 USARTx, u8 *pdat);
void UsartLinSendChecksum(u8 USARTx, u8 *dat);
void KeyScan(void);
 
/********************* 主函数 *************************/
void main(void)
{
	u8 i;
	
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    RSTFLAG |= 0x04;   //设置硬件复位后需要检测P3.2的状态选择运行区域，否则硬件复位后进入USB下载模式

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    usb_init();

    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run

	Lin_ID = 0x32;
	LinInit();
    EUSB = 1;   //IE2相关的中断使能后，需要重新设置EUSB
	EA = 1;     //打开总中断

	SLP1_N = 1;
	SLP2_N = 1;
	TX1_BUF[0] = 0x11;
	TX1_BUF[1] = 0x12;
	TX1_BUF[2] = 0x13;
	TX1_BUF[3] = 0x14;
	TX1_BUF[4] = 0x15;
	TX1_BUF[5] = 0x16;
	TX1_BUF[6] = 0x17;
	TX1_BUF[7] = 0x18;
	
	TX2_BUF[0] = 0x21;
	TX2_BUF[1] = 0x22;
	TX2_BUF[2] = 0x23;
	TX2_BUF[3] = 0x24;
	TX2_BUF[4] = 0x25;
	TX2_BUF[5] = 0x26;
	TX2_BUF[6] = 0x27;
	TX2_BUF[7] = 0x28;
	
    printf("STC32G USART LIN1-LIN2 Test.\r\n");
    
	while(1)
	{
        if (bUsbOutReady) //USB调试及复位所需代码
        {
            usb_OUT_done();
        }

        if((B_ULinRX2_Flag) && (COM2.RX_Cnt >= 2))
        {
            B_ULinRX2_Flag = 0;

            //LIN1主机发送帧头，LIN2从机判断 ID=0x13 则发送应答数据与校验码
            if((RX2_Buffer[0] == 0x55) && ((RX2_Buffer[1] & 0x3f) == 0x13)) //PID -> ID
            {
                UsartLinSendData(USART2,TX2_BUF);
                UsartLinSendChecksum(USART2,TX2_BUF);
            }
        }

		if(B_1ms)   //1ms到
		{
			B_1ms = 0;
			KeyScan();
			if(Key1_Function)
			{
				Key1_Function = 0;
				UsartLinSendHeader(USART1,0x13);  //发送帧头，获取数据帧，组成一个完整的帧
			}
			if(Key2_Short_Function)
			{
				Key2_Short_Function = 0;
				UsartLinSendFrame(USART1,Lin_ID, TX1_BUF);  //发送一串完整数据
			}

			if(COM2.RX_TimeOut > 0)     //超时计数
			{
				if(--COM2.RX_TimeOut == 0)
				{
					printf("LIN2 Read: ");
					for(i=0; i<COM2.RX_Cnt; i++)    printf("0x%02x ",RX2_Buffer[i]);    //从串口输出收到的数据
					COM2.RX_Cnt  = 0;   //清除字节数
					printf("\r\n");
				}
			}
            
			if(COM1.RX_TimeOut > 0)     //超时计数
			{
				if(--COM1.RX_TimeOut == 0)
				{
					printf("LIN1 Read: ");
					for(i=0; i<COM1.RX_Cnt; i++)    printf("0x%02x ",RX1_Buffer[i]);    //从串口输出收到的数据
					COM1.RX_Cnt  = 0;   //清除字节数
					printf("\r\n");
				}
			}
		}
	}
}

/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: void UsartLinSendByte(u8 USARTx, u8 dat)
// 描述: 发送一个字节函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinSendByte(u8 USARTx, u8 dat)
{
	if(USARTx == USART1)
	{
		COM1.B_TX_busy = 1;
		SBUF = dat;
		while(COM1.B_TX_busy);
	}
	else if(USARTx == USART2)
	{
		COM2.B_TX_busy = 1;
		S2BUF = dat;
		while(COM2.B_TX_busy);
	}
}

//========================================================================
// 函数: void UsartLinSendData(u8 USARTx, u8 *pdat)
// 描述: Lin发送数据函数。
// 参数: *pdat: 设置数据缓冲区.
// 返回: Lin ID.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinSendData(u8 USARTx, u8 *pdat)
{
	u8 i;

	for(i=0;i<FRAME_LEN;i++)
	{
		UsartLinSendByte(USARTx,pdat[i]);
	}
}

//========================================================================
// 函数: void UsartLinSendChecksum(u8 USARTx, u8 *dat)
// 描述: 计算校验码并发送。
// 参数: 数据场传输的数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-12-2
// 备注: 
//========================================================================
void UsartLinSendChecksum(u8 USARTx, u8 *dat)
{
    u16 sum = 0;
    u8 i;

    for(i = 0; i < FRAME_LEN; i++)
    {
        sum += dat[i];
        if(sum & 0xFF00)
        {
            sum = (sum & 0x00FF) + 1;
        }
    }
    sum ^= 0x00FF;
    UsartLinSendByte(USARTx,(u8)sum);
}

//========================================================================
// 函数: void UsartSendBreak(void)
// 描述: 主模式发送Lin总线Break函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartSendBreak(u8 USARTx)
{
	if(USARTx == USART1)
	{
		USARTCR5 |= 0x04;		//主模式 Send Break
	}
	else if(USARTx == USART2)
	{
		USART2CR5 |= 0x04;		//主模式 Send Break
	}
	UsartLinSendByte(USARTx,0x00);
}

//========================================================================
// 函数: void UsartLinSendPID(u8 id)
// 描述: ID码加上校验符，转成PID码并发送。
// 参数: ID码.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-12-2
// 备注: 
//========================================================================
void UsartLinSendPID(u8 USARTx, u8 id)
{
	u8 P0 ;
	u8 P1 ;
	
	P0 = (((id)^(id>>1)^(id>>2)^(id>>4))&0x01)<<6 ;
	P1 = ((~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&0x01)<<7 ;
	
	UsartLinSendByte(USARTx,id|P0|P1);
}

//========================================================================
// 函数: void UsartLinSendHeader(u8 lid)
// 描述: Lin主机发送帧头函数。
// 参数: ID码.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinSendHeader(u8 USARTx, u8 lid)
{
	UsartSendBreak(USARTx);				//Send Break
	UsartLinSendByte(USARTx,0x55);		//Send Sync Field
	UsartLinSendPID(USARTx,lid);		//设置总线ID
}

//========================================================================
// 函数: void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat)
// 描述: Lin主机发送完整帧函数。
// 参数: lid: Lin ID; *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat)
{
	UsartSendBreak(USARTx);				//Send Break
	UsartLinSendByte(USARTx,0x55);		//Send Sync Field

	UsartLinSendPID(USARTx,lid);		//设置总线ID
	UsartLinSendData(USARTx,pdat);
	UsartLinSendChecksum(USARTx,pdat);
}

//========================================================================
// 函数: void UsartLinBaudrate(u8 USARTx, u16 brt)
// 描述: Lin总线波特率设置函数。
// 参数: brt: 波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinBaudrate(u8 USARTx, u16 brt)
{
	u16 tmp;
	
	tmp = (MAIN_Fosc >> 4) / brt;
	if(USARTx == USART1)
	{
		USARTBRH = (u8)(tmp>>8);
		USARTBRL = (u8)tmp;
	}
	else if(USARTx == USART2)
	{
		USART2BRH = (u8)(tmp>>8);
		USART2BRL = (u8)tmp;
	}
}

//========================================================================
// 函数: void LinInit()
// 描述: Lin初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void LinInit()
{
    P_SW1 = (P_SW1 & 0x3f) | 0xc0;  //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

	SCON = (SCON & 0x3f) | 0x40;    //USART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
	TI = 0;
	REN = 1;    //允许接收
	ES  = 1;    //允许中断

	UsartLinBaudrate(USART1,Baudrate);		//设置波特率
	SMOD = 1;				//串口1波特率翻倍
	USARTCR1 |= 0x80;		//LIN Mode Enable
	USARTCR5 &= ~0x20;		//LIN Master Mode
	
	//====== USART2 config =======
	S2_S  = 1;      //USART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

	S2CON = (S2CON & 0x3f) | 0x50;  //8位数据,可变波特率, 允许接收
	AUXR |= 0x14;   //定时器2时钟1T模式,开始计时
	ES2 = 1;        //允许中断

	UsartLinBaudrate(USART2,Baudrate);	//设置波特率
	S2CFG |= 0x80;			//S2MOD = 1
	USART2CR1 |= 0x80;		//LIN Mode Enable
	USART2CR5 |= 0x20;		//LIN Slave Mode

	USART2CR5 |= 0x10;		//AutoSync
}

//========================================================================
// 函数: UART1_ISR_Handler
// 描述: UART1中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2022-06-23
//========================================================================
void UART1_ISR_Handler (void) interrupt UART1_VECTOR
{
	u8 Status;

	if(RI)
	{
		RI = 0;

		//--------USART LIN---------------
		Status = USARTCR5;
		if(Status & 0x02)     //if LIN header is detected
		{
			B_ULinRX1_Flag = 1;
		}

		if(Status & 0xc0)     //if LIN break is detected / LIN header error is detected
		{
			COM1.RX_Cnt = 0;
		}
		USARTCR5 &= ~0xcb;    //Clear flag
		//--------------------------------
		
		if(COM1.B_RX_OK == 0)
		{
			if(COM1.RX_Cnt >= UART_BUF_LENGTH)	COM1.RX_Cnt = 0;
			RX1_Buffer[COM1.RX_Cnt++] = SBUF;
			COM1.RX_TimeOut = 5;
		}
	}

	if(TI)
	{
		TI = 0;
		COM1.B_TX_busy = 0;
	}
}

//========================================================================
// 函数: UART2_ISR_Handler
// 描述: UART2中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2022-06-23
//========================================================================
void UART2_ISR_Handler (void) interrupt UART2_VECTOR
{
	u8 Status;

	if(S2RI)
	{
		S2RI = 0;

		//--------USART LIN---------------
		Status = USART2CR5;
		if(Status & 0x02)     //if LIN header is detected
		{
			B_ULinRX2_Flag = 1;
		}

		if(Status & 0xc0)     //if LIN break is detected / LIN header error is detected
		{
			COM2.RX_Cnt = 0;
		}
		USART2CR5 &= ~0xcb;   //Clear flag
		//--------------------------------
		
		if(COM2.B_RX_OK == 0)
		{
			if(COM2.RX_Cnt >= UART_BUF_LENGTH)	COM2.RX_Cnt = 0;
			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;
			COM2.RX_TimeOut = 5;
		}
	}

	if(S2TI)
	{
		S2TI = 0;
		COM2.B_TX_busy = 0;
	}
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
	u16 i;
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i);
	}while(--ms);
}

//========================================================================
// 函数: void KeyScan(void)
// 描述: 按键扫描程序。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyScan(void)
{
	//单纯短按按键
	if(!P33)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt >= 50)		//50ms防抖
			{
				Key1_Flag = 1;			//设置按键状态，防止重复触发
				Key1_Function = 1;
			}
		}
	}
	else
	{
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	//长按短按按键
	if(!P32)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;
			if(Key2_cnt >= 1000)		//长按1s
			{
				Key2_Short_Flag = 0;	//清除短按标志
				Key2_Long_Flag = 1;		//设置长按标志
				Key2_Flag = 1;			//设置按键状态，防止重复触发
//				Key2_Long_Function = 1;

                //P3.2口按键长按1秒触发软件复位，进入USB下载模式。
                USBCON = 0x00;      //清除USB设置
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
                while (1);
			}
			else if(Key2_cnt >= 50)		//50ms防抖
			{
				Key2_Short_Flag = 1;	//设置短按标志
			}
		}
	}
	else
	{
		if(Key2_Short_Flag)			//判断是否短按
		{
			Key2_Short_Flag = 0;	//清除短按标志
			Key2_Short_Function = 1;
		}
		Key2_cnt = 0;
		Key2_Flag = 0;	//按键释放
	}
}
