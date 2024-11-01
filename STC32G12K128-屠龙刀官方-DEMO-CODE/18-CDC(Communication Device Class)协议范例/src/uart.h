#ifndef __UART_H__
#define __UART_H__

#define BR(n)                   (65536 - ((FOSC) / (n) + 2)/ 4)

void uart_init();
void uart_set_parity(BYTE parity);
void uart_set_baud(DWORD baud);
void uart_polling();

#endif
