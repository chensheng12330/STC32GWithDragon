#include "iic.h"
#include "system.h"

uint8_t xdata DmaTxBuffer[32];

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf 使用 UART1; 2:printf 使用 UART2

//注意：OLED模块使用I2C模式时需将SSD1306的D1与D2脚连起来，即短接R8电阻，否则收不到ACK，会导致DMA模式发送异常。
#define USE_I2C_DMA   1        //1:使用 I2C DMA; 0:使用硬件 I2C

/******************** 串口打印函数 ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;				//定时器1开始计时

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //串口1选择定时器2为波特率发生器
#else
	P_SW2 |= 1;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //定时器2时钟1T模式,开始计时
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
	TI = 0;
#else
	S2BUF  = dat; 
	while((S2CON & 2) == 0);
	S2CON &= ~2;    //Clear Tx flag
#endif
}


#if(USE_I2C_DMA == 0)
void Wait()
{
    while (!(I2CMSST & 0x40));
    I2CMSST &= ~0x40;
}

void Start()
{
    I2CMSCR = 0x01;                         //发送START命令
    Wait();
}

void SendData(char dat)
{
    I2CTXD = dat;                           //写数据到数据缓冲区
    I2CMSCR = 0x02;                         //发送SEND命令
    Wait();
}

void RecvACK()
{
    I2CMSCR = 0x03;                         //发送读ACK命令
    Wait();
}

char RecvData()
{
    I2CMSCR = 0x04;                         //发送RECV命令
    Wait();
    return I2CRXD;
}

void SendACK()
{
    I2CMSST = 0x00;                         //设置ACK信号
    I2CMSCR = 0x05;                         //发送ACK命令
    Wait();
}

void SendNAK()
{
    I2CMSST = 0x01;                         //设置NAK信号
    I2CMSCR = 0x05;                         //发送ACK命令
    Wait();
}

void Stop()
{
    I2CMSCR = 0x06;                         //发送STOP命令
    Wait();
}
#endif

void HAL_I2C_Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
//void WriteNbyte(u8 addr, u8 *p, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
	MemAddSize = 0;
	Timeout = 0;
    
#if(USE_I2C_DMA == 0)

    Start();                                //发送起始命令
    SendData((uint8_t)DevAddress);          //发送设备地址+写命令
    RecvACK();
    if(MemAddSize != 0)
    {
        SendData((uint8_t)MemAddress);      //发送存储地址
        RecvACK();
    }
    do
    {
        SendData(*pData++);
        RecvACK();
    }
    while(--Size);
    Stop();                                 //发送停止命令

#else
    
    //注意：OLED模块使用I2C模式时需将SSD1306的D1与D2脚连起来，即短接R8电阻，否则收不到ACK，会导致DMA模式发送异常。
    
    MemAddress = 0;
    DmaTxBuffer[0] = (uint8_t)DevAddress;
//    DmaTxBuffer[1] = (uint8_t)MemAddress;
    memcpy(&DmaTxBuffer[1],pData,Size);

    while(I2CMSST & 0x80);    //检查I2C控制器忙碌状态

    I2CMSST = 0x00;
    I2CMSCR = 0x09;             //set cmd //write_start_combo
    DMA_I2C_CR = 0x01;
    DMA_I2CT_AMT = Size;	    //设置传输总字节数(低8位)：Size + 设备地址
    DMA_I2CT_AMTH = 0x00;		//设置传输总字节数(高8位)
    DMA_I2C_ST1 = Size;	    	//设置需要传输字节数(低8位)：Size + 设备地址
    DMA_I2C_ST2 = 0x00;			//设置需要传输字节数(高8位)
    DMA_I2CT_CR |= 0x40;		//bit7 1:使能 I2CT_DMA, bit6 1:开始 I2CT_DMA

    while((DMA_I2CT_STA & 0x01)==0);  //DMA忙检测
    DMA_I2CT_STA &= ~0x01;  //清除标志位
    DMA_I2C_CR = 0x00;

#endif
}

//void ReadNbyte(u8 addr, u8 *p, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
//{
//    Start();                                //发送起始命令
//    SendData(0);                         //发送设备地址+写命令
//    RecvACK();
//    SendData(addr);                         //发送存储地址
//    RecvACK();
//    Start();                                //发送起始命令
//    SendData(0);                         //发送设备地址+读命令
//    RecvACK();
//    do
//    {
//        *p = RecvData();
//        p++;
//        if(number != 1) SendACK();          //send ACK
//    }
//    while(--number);
//    SendNAK();                              //send no ACK	
//    Stop();                                 //发送停止命令
//}

void HAL_I2C_Init()
{
    I2C_S1 =0;   //I2C功能脚选择，00:P1.5,P1.4; 01:P2.5,P2.4; 11:P3.2,P3.3
    I2C_S0 =1;
    I2CCFG = 0xe0;              //使能I2C主机模式
    I2CMSST = 0x00;

    EA = 1;     //打开总中断
}

void DMA_Config(void)
{
#if(USE_I2C_DMA == 1)
	DMA_I2CT_STA = 0x00;
	DMA_I2CT_CFG = 0x00;	//bit7 1:Enable Interrupt
	DMA_I2CT_AMT = 0x1f;	//设置传输总字节数(低8位)：n+1
	DMA_I2CT_AMTH = 0x00;	//设置传输总字节数(高8位)：n+1
	DMA_I2CT_TXAH = (u8)((u16)&DmaTxBuffer >> 8);	//I2C发送数据存储地址
	DMA_I2CT_TXAL = (u8)((u16)&DmaTxBuffer);
	DMA_I2CT_CR = 0x80;		//bit7 1:使能 I2CT_DMA, bit6 1:开始 I2CT_DMA

//	DMA_I2CR_STA = 0x00;
//	DMA_I2CR_CFG = 0x80;	//bit7 1:Enable Interrupt
//	DMA_I2CR_AMT = 0x1f;	//设置传输总字节数(低8位)：n+1
//	DMA_I2CR_AMTH = 0x00;	//设置传输总字节数(高8位)：n+1
//	DMA_I2CR_RXAH = (u8)((u16)&DmaRxBuffer >> 8);	//I2C接收数据存储地址
//	DMA_I2CR_RXAL = (u8)((u16)&DmaRxBuffer);
//	DMA_I2CR_CR = 0x81;		//bit7 1:使能 I2CT_DMA, bit5 1:开始 I2CT_DMA, bit0 1:清除 FIFO

	DMA_I2C_ST1 = 0x1f;		//设置需要传输字节数(低8位)：n+1
	DMA_I2C_ST2 = 0x00;		//设置需要传输字节数(高8位)：n+1
#endif
}
