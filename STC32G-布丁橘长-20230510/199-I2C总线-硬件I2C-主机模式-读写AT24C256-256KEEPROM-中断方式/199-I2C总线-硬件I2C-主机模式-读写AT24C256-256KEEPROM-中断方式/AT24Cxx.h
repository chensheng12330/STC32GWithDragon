#ifndef __AT24CXX_H__
#define __AT24CXX_H__

#include <STC32G.h>
#include "I2C.h"
#include "config.h"

#define SLAW    0xA0					// EEPROM��ַ+д 1010 0000 	 0xA0 (A2=0,A1=0,A0=0)
#define SLAR    0xA1					// EEPROM��ַ+�� 1010 0001	 0xA1	(A2=0,A1=0,A0=0)

u8 EEPROM_ReadAbyte(u8 addrH,u8 addrL);												// ��1���ֽ�
void EEPROM_WriteAbyte(u8 addrH,u8 addrL, u8 dat); 						// д1���ֽ�
void EEPROM_WriteNbyte(u8 addrH,u8 addrL, u8 *p, u8 number);  /*  WordAddress,1st Data Address,2nd Data Address,Byte lenth   */
void EEPROM_ReadNbyte(u8 addrH, u8 addrL, u8 *p, u8 number);  /*  WordAddress,1st Data Address,2nd Data Address,Byte lenth   */

//void EEPROM_WriteAbyte(u8 addr, u8 dat); 							// 1�ֽ�д�뺯����AT24C01��AT24C02 ��1���ֽڵ�ַEEPROM����
//u8 EEPROM_ReadAbyte(u8 addr);												// 1�ֽڶ�ȡ������AT24C01��AT24C02 ��1���ֽڵ�ַEEPROM����
//void EEPROM_ReadNbyte(u8 addr, u8 *p, u8 number); 		// ���ֽڶ�ȡ������AT24C01��AT24C02 ��1���ֽڵ�ַEEPROM����
//void EEPROM_WriteNbyte(u8 addr, u8 *p, u8 number);		// ���ֽ�д�뺯����AT24C01��AT24C02 ��1���ֽڵ�ַEEPROM���� 	

#endif