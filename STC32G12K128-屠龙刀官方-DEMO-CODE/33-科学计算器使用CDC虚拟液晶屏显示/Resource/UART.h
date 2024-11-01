#ifndef _UART_H_
#define _UART_H_

void 
UART_write_str(char *s);

void
UART_init(void);

void UART_write_byte(unsigned char dat);

#endif