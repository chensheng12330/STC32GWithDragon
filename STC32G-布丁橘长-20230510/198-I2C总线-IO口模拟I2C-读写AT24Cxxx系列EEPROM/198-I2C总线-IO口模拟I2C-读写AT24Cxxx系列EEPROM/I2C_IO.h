#ifndef __I2C_IO_H__
#define __I2C_IO_H__

#include <STC32G.h>
#include "config.h"

sbit SDA = P7^0;
sbit SCL = P7^1;

void I2C_Delay(void); 				//for normal MCS51, delay (2 * dly + 4) T, for STC12Cxxxx delay (4 * dly + 10) T
void I2C_Start(void);         //start the I2C, SDA High-to-low when SCL is high
void I2C_Stop(void);          //STOP the I2C, SDA Low-to-high when SCL is high
void S_ACK(void);             //Send ACK (LOW)
void S_NoACK(void);           //Send No ACK (High)
void I2C_Check_ACK(void);     //Check ACK, If F0=0, then right, if F0=1, then error
void I2C_WriteAbyte(u8 dat);  //write a byte to I2C
u8 I2C_ReadAbyte(void);       //read A byte from I2C
void I2C_Delay(void);

#endif