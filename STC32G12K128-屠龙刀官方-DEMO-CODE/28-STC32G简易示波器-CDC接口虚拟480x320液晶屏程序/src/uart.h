#ifndef __UART_H__
#define __UART_H__

void uart_init();
void uart_recv_done();
void uart_send(int size);

extern BOOL RxFlag;
extern BYTE RxCount;
extern BYTE edata RxBuffer[64];
extern BYTE edata TxBuffer[1024];


#endif
