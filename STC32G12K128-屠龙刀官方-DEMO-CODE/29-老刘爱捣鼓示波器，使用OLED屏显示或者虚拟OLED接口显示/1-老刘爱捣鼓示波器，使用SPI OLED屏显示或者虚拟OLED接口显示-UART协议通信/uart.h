#ifndef __UART_H__
#define __UART_H__

void uart_init();
void uart_OUT_done();

void uart_SendByte(BYTE dat);
void uart_SendBlock(BYTE cnt);


extern BYTE xdata UartTxBuffer[64];
extern BYTE xdata UartRxBuffer[64];
extern BOOL bUartSendBusy;
extern BOOL bUartRecvReady;

#endif


