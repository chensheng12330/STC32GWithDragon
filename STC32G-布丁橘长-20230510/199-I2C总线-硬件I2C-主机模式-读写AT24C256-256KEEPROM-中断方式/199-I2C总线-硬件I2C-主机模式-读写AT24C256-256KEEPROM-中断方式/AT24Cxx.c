#include "AT24Cxx.h"

void EEPROM_WriteAbyte(u8 addrH,u8 addrL, u8 dat) 
{
	I2C_Start();							// 起始信号
	I2C_WriteAbyte(SLAW);			// 发送从设备地址+写
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(addrH);		// 写存储地址高8位
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(addrL);		// 写存储地址低8位
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(dat);			// 写1字节数据
	I2C_Check_ACK(); 					// 等待应答
	I2C_Stop();								// 发送结束信号
}

u8 EEPROM_ReadAbyte(u8 addrH,u8 addrL)
{
	u8 dat;
	
	I2C_Start();							// 起始信号
	I2C_WriteAbyte(SLAW);			// 发送从设备地址+写
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(addrH);		// 写存储地址高8位
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(addrL);		// 写存储地址低8位
	I2C_Check_ACK(); 					// 等待应答
				
	I2C_Start();							// 起始信号
	I2C_WriteAbyte(SLAR);			// 发送从设备地址+读
	I2C_Check_ACK(); 					// 等待应答
	dat = I2C_ReadAbyte();		// 读1字节数据
	S_NoACK();  							// 发送非应答信号
	I2C_Stop();								// 停止信号
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
//===========================写入1字节函数===该函数只适用于1K 2K EEPROM（AT24C01、AT24C02）============================
/*
void EEPROM_WriteAbyte(u8 addr, u8 dat) 
{
	I2C_Start();							// 起始信号
	I2C_WriteAbyte(SLAW);			// 发送从设备地址+写
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(addr);			// 写存储地址
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(dat);			// 写1字节数据
	I2C_Check_ACK(); 					// 等待应答
	I2C_Stop();								// 发送结束信号
}
*/
//=====================================================================================================================
//===========================读取1字节函数===该函数只适用于1K 2K EEPROM（AT24C01、AT24C02）============================
/*
u8 EEPROM_ReadAbyte(u8 addr)
{
	u8 dat;
	
	I2C_Start();							// 起始信号
	I2C_WriteAbyte(SLAW);			// 发送从设备地址+写
	I2C_Check_ACK(); 					// 等待应答
	I2C_WriteAbyte(addr);		// 写存储地址
	I2C_Check_ACK(); 					// 等待应答
				
	I2C_Start();							// 起始信号
	I2C_WriteAbyte(SLAR);			// 发送从设备地址+读
	I2C_Check_ACK(); 					// 等待应答
	dat = I2C_ReadAbyte();		// 读1字节数据
	S_NoACK();  							// 发送非应答信号
	I2C_Stop();								// 停止信号
	return dat;
}
*/
//=======================================================================================================================
//===========================多字节写入函数===该函数只适用于1K 2K EEPROM（AT24C01、AT24C02）============================
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
//============================多字节读取函数===该函数只适用于1K 2K EEPROM（AT24C01、AT24C02）============================
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