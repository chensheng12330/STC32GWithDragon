#include "I2C.h"

bit busy;

void I2C_Isr() interrupt 24
{
	if (I2CMSST & 0x40)
	{
		I2CMSST &= ~0x40; // ���жϱ�־
		busy = 0;
	}
}
void I2C_Start(void) 
{
	busy = 1;
	I2CMSCR = 0x81; // ���� START ����
	while (busy);
}
void I2C_WriteAbyte(unsigned char dat)
{
	I2CTXD = dat; // д���ݵ����ݻ�����
	busy = 1;
	I2CMSCR = 0x82; // ���� SEND ����
	while (busy);
}
void I2C_Check_ACK(void)
{
	busy = 1;
	I2CMSCR = 0x83; // ���Ͷ� ACK ����
	while (busy);
}
char I2C_ReadAbyte(void) 
{
	busy = 1;
	I2CMSCR = 0x84; // ���� RECV ����
	while (busy);
	return I2CRXD;
}
void S_ACK(void) 
{
	I2CMSST = 0x00; // ���� ACK �ź�
	busy = 1;
	I2CMSCR = 0x85; // ���� ACK ����
	while (busy);
}
void S_NoACK(void)
{
	I2CMSST = 0x01; // ���� NAK �ź�
	busy = 1;
	I2CMSCR = 0x85; // ���� ACK ����
	while (busy);
}
void I2C_Stop(void)
{
	busy = 1;
	I2CMSCR = 0x86; // ���� STOP ����
	while (busy);
}