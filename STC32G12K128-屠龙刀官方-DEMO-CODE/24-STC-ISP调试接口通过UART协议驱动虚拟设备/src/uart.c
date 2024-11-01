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

#include "stc.h"
#include "uart.h"

BYTE xdata UartTxBuffer[64];
BYTE xdata UartRxBuffer[64];
BYTE bUartRxWptr;
BYTE bUartRxStage;
BOOL bUartSendBusy;
BOOL bUartRecvReady;

void delay_ms(u8 ms);

void uart_init()
{
    S1_S0 = 0;
    S1_S1 = 0;
    SCON = 0x50;
    S1BRT = 1;
    T2x12 = 1;
    T2L = BAUD;
    T2H = BAUD >> 8;
    T2R = 1;
    ES = 1;
    
    bUartSendBusy = 0;
    bUartRecvReady = 0;
    bUartRxWptr = 0;
    bUartRxStage = 0;
}

void uart_SendBlock(BYTE cnt)
{
    BYTE i;
    
    for (i=0; i<cnt; i++)
        uart_SendByte(UartTxBuffer[i]);
}

void uart_OUT_done()
{
    bUartRecvReady = 0;
    bUartRxWptr = 0;
    bUartRxStage = 0;
}

void uart_SendByte(BYTE dat)
{
    while (bUartSendBusy);
    SBUF = dat;
    bUartSendBusy = 1;
}

void uart_isr() interrupt 4
{
    BYTE dat;

    if (TI)
    {
        TI = 0;
        bUartSendBusy = 0;
    }
    
    if (RI)
    {
        RI = 0;
        
        if (bUartRecvReady)
            return;
        
        dat = SBUF;
        switch (bUartRxStage)
        {
        case 0:
L_CheckHeader:
            bUartRxWptr = 0;
            bUartRxStage = 0;
            if (dat == 'K')
                bUartRxStage = 1;
            else if (dat == '@')
                bUartRxStage = 9;
            break;
        case 1:
            if (dat != 'E')
                goto L_CheckHeader;
            bUartRxStage = 2;
            break;
        case 2:
            if (dat != 'Y')
                goto L_CheckHeader;
            bUartRxStage = 3;
            break;
        case 3:
            if (dat != 'P')
                goto L_CheckHeader;
            bUartRxStage = 4;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            UartRxBuffer[bUartRxWptr++] = dat;
            bUartRxStage++;
            if (bUartRxStage == 8)
                bUartRecvReady = 1;
            break;

        case 9:
            if (dat != 'S')
                goto L_CheckHeader;
            bUartRxStage = 10;
            break;
        case 10:
            if (dat != 'T')
                goto L_CheckHeader;
            bUartRxStage = 11;
            break;
        case 11:
            if (dat != 'C')
                goto L_CheckHeader;
            bUartRxStage = 12;
            break;
        case 12:
            if (dat != 'I')
                goto L_CheckHeader;
            bUartRxStage = 13;
            break;
        case 13:
            if (dat != 'S')
                goto L_CheckHeader;
            bUartRxStage = 14;
            break;
        case 14:
            if (dat != 'P')
                goto L_CheckHeader;
            bUartRxStage = 15;
            break;
        case 15:
            if (dat != '#')
                goto L_CheckHeader;
            bUartRxStage = 0;
            delay_ms(200);
            IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
            while (1);
            break;
        }
    }
}

void delay_ms(u8 ms)
{
     u16 i;
     do{
          i = FOSC / 6000;
          while(--i);
     }while(--ms);
}
