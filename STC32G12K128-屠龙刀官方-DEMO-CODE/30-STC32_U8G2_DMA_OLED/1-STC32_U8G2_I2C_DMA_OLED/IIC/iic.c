#include "iic.h"
#include "system.h"

uint8_t xdata DmaTxBuffer[32];

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

//ע�⣺OLEDģ��ʹ��I2Cģʽʱ�轫SSD1306��D1��D2�������������̽�R8���裬�����ղ���ACK���ᵼ��DMAģʽ�����쳣��
#define USE_I2C_DMA   1        //1:ʹ�� I2C DMA; 0:ʹ��Ӳ�� I2C

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;				//��ʱ��1��ʼ��ʱ

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
#else
	P_SW2 |= 1;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
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
    I2CMSCR = 0x01;                         //����START����
    Wait();
}

void SendData(char dat)
{
    I2CTXD = dat;                           //д���ݵ����ݻ�����
    I2CMSCR = 0x02;                         //����SEND����
    Wait();
}

void RecvACK()
{
    I2CMSCR = 0x03;                         //���Ͷ�ACK����
    Wait();
}

char RecvData()
{
    I2CMSCR = 0x04;                         //����RECV����
    Wait();
    return I2CRXD;
}

void SendACK()
{
    I2CMSST = 0x00;                         //����ACK�ź�
    I2CMSCR = 0x05;                         //����ACK����
    Wait();
}

void SendNAK()
{
    I2CMSST = 0x01;                         //����NAK�ź�
    I2CMSCR = 0x05;                         //����ACK����
    Wait();
}

void Stop()
{
    I2CMSCR = 0x06;                         //����STOP����
    Wait();
}
#endif

void HAL_I2C_Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
//void WriteNbyte(u8 addr, u8 *p, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
	MemAddSize = 0;
	Timeout = 0;
    
#if(USE_I2C_DMA == 0)

    Start();                                //������ʼ����
    SendData((uint8_t)DevAddress);          //�����豸��ַ+д����
    RecvACK();
    if(MemAddSize != 0)
    {
        SendData((uint8_t)MemAddress);      //���ʹ洢��ַ
        RecvACK();
    }
    do
    {
        SendData(*pData++);
        RecvACK();
    }
    while(--Size);
    Stop();                                 //����ֹͣ����

#else
    
    //ע�⣺OLEDģ��ʹ��I2Cģʽʱ�轫SSD1306��D1��D2�������������̽�R8���裬�����ղ���ACK���ᵼ��DMAģʽ�����쳣��
    
    MemAddress = 0;
    DmaTxBuffer[0] = (uint8_t)DevAddress;
//    DmaTxBuffer[1] = (uint8_t)MemAddress;
    memcpy(&DmaTxBuffer[1],pData,Size);

    while(I2CMSST & 0x80);    //���I2C������æµ״̬

    I2CMSST = 0x00;
    I2CMSCR = 0x09;             //set cmd //write_start_combo
    DMA_I2C_CR = 0x01;
    DMA_I2CT_AMT = Size;	    //���ô������ֽ���(��8λ)��Size + �豸��ַ
    DMA_I2CT_AMTH = 0x00;		//���ô������ֽ���(��8λ)
    DMA_I2C_ST1 = Size;	    	//������Ҫ�����ֽ���(��8λ)��Size + �豸��ַ
    DMA_I2C_ST2 = 0x00;			//������Ҫ�����ֽ���(��8λ)
    DMA_I2CT_CR |= 0x40;		//bit7 1:ʹ�� I2CT_DMA, bit6 1:��ʼ I2CT_DMA

    while((DMA_I2CT_STA & 0x01)==0);  //DMAæ���
    DMA_I2CT_STA &= ~0x01;  //�����־λ
    DMA_I2C_CR = 0x00;

#endif
}

//void ReadNbyte(u8 addr, u8 *p, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
//{
//    Start();                                //������ʼ����
//    SendData(0);                         //�����豸��ַ+д����
//    RecvACK();
//    SendData(addr);                         //���ʹ洢��ַ
//    RecvACK();
//    Start();                                //������ʼ����
//    SendData(0);                         //�����豸��ַ+������
//    RecvACK();
//    do
//    {
//        *p = RecvData();
//        p++;
//        if(number != 1) SendACK();          //send ACK
//    }
//    while(--number);
//    SendNAK();                              //send no ACK	
//    Stop();                                 //����ֹͣ����
//}

void HAL_I2C_Init()
{
    I2C_S1 =0;   //I2C���ܽ�ѡ��00:P1.5,P1.4; 01:P2.5,P2.4; 11:P3.2,P3.3
    I2C_S0 =1;
    I2CCFG = 0xe0;              //ʹ��I2C����ģʽ
    I2CMSST = 0x00;

    EA = 1;     //�����ж�
}

void DMA_Config(void)
{
#if(USE_I2C_DMA == 1)
	DMA_I2CT_STA = 0x00;
	DMA_I2CT_CFG = 0x00;	//bit7 1:Enable Interrupt
	DMA_I2CT_AMT = 0x1f;	//���ô������ֽ���(��8λ)��n+1
	DMA_I2CT_AMTH = 0x00;	//���ô������ֽ���(��8λ)��n+1
	DMA_I2CT_TXAH = (u8)((u16)&DmaTxBuffer >> 8);	//I2C�������ݴ洢��ַ
	DMA_I2CT_TXAL = (u8)((u16)&DmaTxBuffer);
	DMA_I2CT_CR = 0x80;		//bit7 1:ʹ�� I2CT_DMA, bit6 1:��ʼ I2CT_DMA

//	DMA_I2CR_STA = 0x00;
//	DMA_I2CR_CFG = 0x80;	//bit7 1:Enable Interrupt
//	DMA_I2CR_AMT = 0x1f;	//���ô������ֽ���(��8λ)��n+1
//	DMA_I2CR_AMTH = 0x00;	//���ô������ֽ���(��8λ)��n+1
//	DMA_I2CR_RXAH = (u8)((u16)&DmaRxBuffer >> 8);	//I2C�������ݴ洢��ַ
//	DMA_I2CR_RXAL = (u8)((u16)&DmaRxBuffer);
//	DMA_I2CR_CR = 0x81;		//bit7 1:ʹ�� I2CT_DMA, bit5 1:��ʼ I2CT_DMA, bit0 1:��� FIFO

	DMA_I2C_ST1 = 0x1f;		//������Ҫ�����ֽ���(��8λ)��n+1
	DMA_I2C_ST2 = 0x00;		//������Ҫ�����ֽ���(��8λ)��n+1
#endif
}
