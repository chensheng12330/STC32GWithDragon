#ifndef __I2C_H__
#define __I2C_H__

#include <STC32G.h>

void I2C_Start();							// ��ʼ�ź�
void I2C_Stop();							// �����ź�
void I2C_WriteAbyte(unsigned char dat); 	// ����1�ֽں���
char I2C_ReadAbyte(void);			// ����1�ֽں���
void S_ACK(void); 						// ����Ӧ���ź�
void S_NoACK(void); 					// ���ͷ�Ӧ���ź�
void I2C_Check_ACK(void); 		// ����Ӧ���ź�
	
#endif