#ifndef __I2C_H__
#define __I2C_H__

#include <STC32G.h>

void I2C_Start();							// 起始信号
void I2C_Stop();							// 结束信号
void I2C_WriteAbyte(unsigned char dat); 	// 发送1字节函数
char I2C_ReadAbyte(void);			// 接收1字节函数
void S_ACK(void); 						// 发送应答信号
void S_NoACK(void); 					// 发送非应答信号
void I2C_Check_ACK(void); 		// 接收应答信号
	
#endif