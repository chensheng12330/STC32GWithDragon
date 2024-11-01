//#include "UART.h"
//#include "STC12C5A60S2.h"
//
//#define uchar unsigned char
//#define uint unsigned int
//
//#define FOSC 11059200L      //System frequency
//#define BAUD 9600           //UART baudrate
//
//bit busy;
//extern char UART_sbuf[20];
//extern char count;
//
//void UART_init(void)
//{
//	PCON &= 0x7F;			//波特率不倍速
//    SCON = 0x50;            //8-bit variable UART
//	AUXR &= 0xBF;			//定时器1时钟为Fosc/12,即12T
//	AUXR &= 0xFE;			//串口1选择定时器1为波特率发生器
//    TMOD &= 0x0F;			//清除定时器1模式位
//	TMOD |= 0x20;			//设定定时器1为8位自动重装方式
//    TH1 = TL1 = -(FOSC/12/32/BAUD); //Set auto-reload vaule
//	ET1 = 0;				//禁止定时器1中断
//    TR1 = 1;                //启动定时器1
//    ES = 1;                 //打开串口中断
//    EA = 1;                 //打开中断允许
//}
//
//
///*----------------------------
//UART interrupt service routine
//----------------------------*/
//void Uart_Isr() interrupt 4
//{
//    if (RI)
//    {
//        ES=0;					//禁止中断
//		count += 1;
//		UART_sbuf[count] = SBUF;
// 		RI=0; 					//清中断标志
// 		ES=1;					//允许串口中断
//    }
//    if (TI)
//    {
//        TI = 0;             //Clear transmit interrupt flag
//        busy = 0;           //Clear transmit busy flag
//    }
//}
//
///*----------------------------
//Send a byte data to UART
//Input: dat (data to be sent)
//Output:None
//----------------------------*/
//void UART_write_byte(uchar dat)
//{
//    while (busy);           //Wait for the completion of the previous data is sent
//    busy = 1;
//    SBUF = dat;             //Send data to UART buffer
//}
//
///*----------------------------
//Send a string to UART
//Input: s (address of string)
//Output:None
//----------------------------*/
//void UART_write_str(char *s)
//{
//    while (*s)              //Check the end of the string
//    {
//        UART_write_byte(*s++);     //Send current char and increment string ptr
//    }
//}
//
