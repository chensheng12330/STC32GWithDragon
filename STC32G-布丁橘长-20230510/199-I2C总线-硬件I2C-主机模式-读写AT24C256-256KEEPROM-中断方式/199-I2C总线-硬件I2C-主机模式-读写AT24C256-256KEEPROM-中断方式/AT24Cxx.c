#include "AT24Cxx.h"

void EEPROM_WriteAbyte(u8 addrH,u8 addrL, u8 dat) 
{
	I2C_Start();							// ��ʼ�ź�
	I2C_WriteAbyte(SLAW);			// ���ʹ��豸��ַ+д
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(addrH);		// д�洢��ַ��8λ
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(addrL);		// д�洢��ַ��8λ
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(dat);			// д1�ֽ�����
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_Stop();								// ���ͽ����ź�
}

u8 EEPROM_ReadAbyte(u8 addrH,u8 addrL)
{
	u8 dat;
	
	I2C_Start();							// ��ʼ�ź�
	I2C_WriteAbyte(SLAW);			// ���ʹ��豸��ַ+д
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(addrH);		// д�洢��ַ��8λ
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(addrL);		// д�洢��ַ��8λ
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
				
	I2C_Start();							// ��ʼ�ź�
	I2C_WriteAbyte(SLAR);			// ���ʹ��豸��ַ+��
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	dat = I2C_ReadAbyte();		// ��1�ֽ�����
	S_NoACK();  							// ���ͷ�Ӧ���ź�
	I2C_Stop();								// ֹͣ�ź�
	return dat;
}

void EEPROM_WriteNbyte(u8 addrH,u8 addrL, u8 *p, u8 number)          /*  WordAddress,1st Data Address,2nd Data Address,Byte lenth   */
                                                            //F0=0,right, F0=1,error
{
    I2C_Start();
    I2C_WriteAbyte(SLAW);
    I2C_Check_ACK();
    if(!F0)
    {
        I2C_WriteAbyte(addrH);
        I2C_Check_ACK();
				I2C_WriteAbyte(addrL);
        I2C_Check_ACK();
        if(!F0)
        {
            do
            {
                I2C_WriteAbyte(*p);     p++;
                I2C_Check_ACK();
                if(F0)  break;
            }
            while(--number);
        }
    }
    I2C_Stop();
}

/****************************/
void EEPROM_ReadNbyte(u8 addrH, u8 addrL, u8 *p, u8 number)       /*  WordAddress,1st Data Address,2nd Data Address,Byte lenth   */
{
    I2C_Start();
    I2C_WriteAbyte(SLAW);
    I2C_Check_ACK();
    if(!F0)
    {
        I2C_WriteAbyte(addrH);
        I2C_Check_ACK();
				I2C_WriteAbyte(addrL);
        I2C_Check_ACK();
        if(!F0)
        {
            I2C_Start();
            I2C_WriteAbyte(SLAR);
            I2C_Check_ACK();
            if(!F0)
            {
                do
                {
                    *p = I2C_ReadAbyte();   p++;
                    if(number != 1)     S_ACK();    //send ACK
                }
                while(--number);
                S_NoACK();          //send no ACK
            }
        }
    }
    I2C_Stop();
}
//===========================д��1�ֽں���===�ú���ֻ������1K 2K EEPROM��AT24C01��AT24C02��============================
/*
void EEPROM_WriteAbyte(u8 addr, u8 dat) 
{
	I2C_Start();							// ��ʼ�ź�
	I2C_WriteAbyte(SLAW);			// ���ʹ��豸��ַ+д
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(addr);			// д�洢��ַ
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(dat);			// д1�ֽ�����
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_Stop();								// ���ͽ����ź�
}
*/
//=====================================================================================================================
//===========================��ȡ1�ֽں���===�ú���ֻ������1K 2K EEPROM��AT24C01��AT24C02��============================
/*
u8 EEPROM_ReadAbyte(u8 addr)
{
	u8 dat;
	
	I2C_Start();							// ��ʼ�ź�
	I2C_WriteAbyte(SLAW);			// ���ʹ��豸��ַ+д
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	I2C_WriteAbyte(addr);		// д�洢��ַ
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
				
	I2C_Start();							// ��ʼ�ź�
	I2C_WriteAbyte(SLAR);			// ���ʹ��豸��ַ+��
	I2C_Check_ACK(); 					// �ȴ�Ӧ��
	dat = I2C_ReadAbyte();		// ��1�ֽ�����
	S_NoACK();  							// ���ͷ�Ӧ���ź�
	I2C_Stop();								// ֹͣ�ź�
	return dat;
}
*/
//=======================================================================================================================
//===========================���ֽ�д�뺯��===�ú���ֻ������1K 2K EEPROM��AT24C01��AT24C02��============================
/*
void EEPROM_WriteNbyte(u8 addr, u8 *p, u8 number)          //  WordAddress,First Data Address,Byte lenth 
                                                            //F0=0,right, F0=1,error
{
    I2C_Start();
    I2C_WriteAbyte(SLAW);
    I2C_Check_ACK();
    if(!F0)
    {
        I2C_WriteAbyte(addr);
        I2C_Check_ACK();
        if(!F0)
        {
            do
            {
                I2C_WriteAbyte(*p);     p++;
                I2C_Check_ACK();
                if(F0)  break;
            }
            while(--number);
        }
    }
    I2C_Stop();
}
*/
//=======================================================================================================================
//============================���ֽڶ�ȡ����===�ú���ֻ������1K 2K EEPROM��AT24C01��AT24C02��============================
/*
void EEPROM_ReadNbyte(u8 addr, u8 *p, u8 number)       //  WordAddress,First Data Address,Byte lenth 
{
    I2C_Start();
    I2C_WriteAbyte(SLAW);
    I2C_Check_ACK();
    if(!F0)
    {
        I2C_WriteAbyte(addr);
        I2C_Check_ACK();
        if(!F0)
        {
            I2C_Start();
            I2C_WriteAbyte(SLAR);
            I2C_Check_ACK();
            if(!F0)
            {
                do
                {
                    *p = I2C_ReadAbyte();   p++;
                    if(number != 1)     S_ACK();    //send ACK
                }
                while(--number);
                S_NoACK();          //send no ACK
            }
        }
    }
    I2C_Stop();
}
*/
//========================================================================================================================