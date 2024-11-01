/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "uart.h"
#include "usb.h"
#include "usb_req_class.h"

BOOL UartBusy;

void uart_init()
{
    S1_S1 = 1;  //UART1 switch to, 00: P3.0 P3.1, 01: P3.6 P3.7, 10: P1.6 P1.7, 11: P4.3 P4.4
    S1_S0 = 0;
	SCON = (SCON & 0x3f) | 0x40; 
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TL1  = BR(115200);
	TH1  = BR(115200) >> 8;
	TR1 = 1;				//定时器1开始计时
    ES  = 1;    //允许中断
    REN = 1;    //允许接收

//    S2_S = 1;    //选择串口2通道  0: P1.0, P1.1;  1: P4.6, P4.7

//    S2CON = 0x50;
//    T2L = BR(115200);
//    T2H = BR(115200) >> 8;
//    T2x12 = 1;
//    T2R = 1;
//    ES2 = 1;

    LineCoding.dwDTERate = 0x00c20100;  //115200
    LineCoding.bCharFormat = 0;
    LineCoding.bParityType = 0;
    LineCoding.bDataBits = 8;
}

void uart1_isr (void) interrupt UART1_VECTOR
{
    if(RI)
    {
        RI = 0;
        TxBuffer[TxWptr++] = SBUF;
    }

    if(TI)
    {
        TI = 0;
        UartBusy = 0;
    }
}

//void uart2_isr() interrupt UART2_VECTOR
//{
//    if (S2TI)
//    {
//        S2TI = 0;
//        UartBusy = 0;
//    }

//    if (S2RI)
//    {
//        S2RI = 0;
//        TxBuffer[TxWptr++] = S2BUF;
//    }
//}

void uart_set_parity(BYTE parity)
{
    switch (parity)
    {
    case NONE_PARITY:
        SCON = 0x50;
        break;
    case ODD_PARITY:
    case EVEN_PARITY:
    case MARK_PARITY:
        SCON = 0xda;
        break;
    case SPACE_PARITY:
        SCON = 0xd2;
        break;
    }
}

void uart_set_baud(DWORD baud)
{
    WORD temp;

    switch (baud)
    {
    case 300:
        TL1 = BR(300);
        TH1 = BR(300) >> 8;
        break;
    case 600:
        TL1 = BR(600);
        TH1 = BR(600) >> 8;
        break;
    case 1200:
        TL1 = BR(1200);
        TH1 = BR(1200) >> 8;
        break;
    case 2400:
        TL1 = BR(2400);
        TH1 = BR(2400) >> 8;
        break;
    case 4800:
        TL1 = BR(4800);
        TH1 = BR(4800) >> 8;
        break;
    case 9600:
        TL1 = BR(9600);
        TH1 = BR(9600) >> 8;
        break;
    case 14400:
        TL1 = BR(14400);
        TH1 = BR(14400) >> 8;
        break;
    case 19200:
        TL1 = BR(19200);
        TH1 = BR(19200) >> 8;
        break;
    case 28800:
        TL1 = BR(28800);
        TH1 = BR(28800) >> 8;
        break;
    case 38400:
        TL1 = BR(38400);
        TH1 = BR(38400) >> 8;
        break;
    case 57600:
        TL1 = BR(57600);
        TH1 = BR(57600) >> 8;
        break;
    case 115200:
        TL1 = BR(115200);
        TH1 = BR(115200) >> 8;
        break;
    case 230400:
        TL1 = BR(230400);
        TH1 = BR(230400) >> 8;
        break;
    case 460800:
        TL1 = BR(460800);
        TH1 = BR(460800) >> 8;
        break;
    default:
        temp = (WORD)BR(baud);
        TL1 = temp;
        TH1 = temp >> 8;
        break;
    }
}

void uart_polling()
{
    BYTE dat;
    BYTE cnt;

    if (DeviceState != DEVSTATE_CONFIGURED)  //如果USB配置没有完成，就直接退出
        return;

    if (!UsbInBusy && (TxRptr != TxWptr))
    {
        EUSB = 0;
        UsbInBusy = 1;
        usb_write_reg(INDEX, 1);
        cnt = 0;
        while (TxRptr != TxWptr)
        {
            usb_write_reg(FIFO1, TxBuffer[TxRptr++]);
            cnt++;
            if (cnt == EP1IN_SIZE) break;
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }

    if (!UartBusy && (RxRptr != RxWptr))
    {
        dat = RxBuffer[RxRptr++];
        UartBusy = 1;
        switch (LineCoding.bParityType)
        {
        case NONE_PARITY:
        case SPACE_PARITY:
            TB8 = 0;
            break;
        case ODD_PARITY:
            ACC = dat;
            TB8 = !P;
            break;
        case EVEN_PARITY:
            ACC = dat;
            TB8 = P;
            break;
        case MARK_PARITY:
            TB8 = 1;
            break;
        }
        SBUF = dat;

        while (UartBusy);
    }

    if (UsbOutBusy)
    {
        EUSB = 0;
        if (RxWptr - RxRptr < 256 - EP1OUT_SIZE)
        {
            UsbOutBusy = 0;
            usb_write_reg(INDEX, 1);
            usb_write_reg(OUTCSR1, 0);
        }
        EUSB = 1;
    }
}
