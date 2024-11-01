#include "I2C.h"

bit busy;

void I2C_Isr() interrupt 24
{
	if (I2CMSST & 0x40)
	{
		I2CMSST &= ~0x40; // 清中断标志
		busy = 0;
	}
}
void I2C_Start(void) 
{
	busy = 1;
	I2CMSCR = 0x81; // 发送 START 命令
	while (busy);
}
void I2C_WriteAbyte(unsigned char dat)
{
	I2CTXD = dat; // 写数据到数据缓冲区
	busy = 1;
	I2CMSCR = 0x82; // 发送 SEND 命令
	while (busy);
}
void I2C_Check_ACK(void)
{
	busy = 1;
	I2CMSCR = 0x83; // 发送读 ACK 命令
	while (busy);
}
char I2C_ReadAbyte(void) 
{
	busy = 1;
	I2CMSCR = 0x84; // 发送 RECV 命令
	while (busy);
	return I2CRXD;
}
void S_ACK(void) 
{
	I2CMSST = 0x00; // 设置 ACK 信号
	busy = 1;
	I2CMSCR = 0x85; // 发送 ACK 命令
	while (busy);
}
void S_NoACK(void)
{
	I2CMSST = 0x01; // 设置 NAK 信号
	busy = 1;
	I2CMSCR = 0x85; // 发送 ACK 命令
	while (busy);
}
void I2C_Stop(void)
{
	busy = 1;
	I2CMSCR = 0x86; // 发送 STOP 命令
	while (busy);
}