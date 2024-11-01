/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "uart.h"
#include "config_stc.h"

BYTE xdata UartTxBuffer[64];
BYTE xdata UartRxBuffer[64];
BYTE bUartRxWptr;
BYTE bUartRxStage;
BOOL bUartSendBusy;
BOOL bUartRecvReady;

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
    PS = 1;  //提高串口中断优先级，这样才能在其它中断函数里进行串口数据收发
    
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
        }
    }
}

