/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���STC32G����ת�Ӱ壨�����������б�д���ԣ���Ҫ����LIN�����շ�����·��

USART����LIN1��������LIN2���ӻ����������շ�.

�̰�һ��P32�ڰ���, LIN1������������һ֡����.
�̰�һ��P33�ڰ���, LIN1��������֡ͷ����ȡLIN2�ӻ�Ӧ�����ݣ��ϲ���һ��������֡������USB-CDC���ڴ�ӡ����.

LIN2�ӻ����յ��������͵����ݣ���USB-CDC���ڴ�ӡ����.

USB-CDC���ڴ�ӡ������������һ֡���ݣ�
LIN2 Read: 0x55 0x32 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x5b 

LIN1��������֡ͷ��LIN2�ӻ�Ӧ�����ݣ��ϲ���һ��������֡����
LIN2 Read: 0x55 0xd3 
LIN1 Read: 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0xda 

���������ʾ���ָ�λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
   (�����ϣ����λ����USB����ģʽ�Ļ������ڸ�λ�����ｫ IAP_CONTR ��bit6��0��ѡ��λ���û�������)
2. ͨ�����ء�stc_usb_cdc_32.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�
   (ע�⣺ʹ��CDC�ӿڴ���MCU��λ���Զ����ع��ܣ���Ҫ��ѡ�˿����ã��´�ǿ��ʹ�á�STC USB Writer (HID)������ISP����)

Ĭ�ϴ������ʣ�9600������, �û��������޸�.

����ʱ, Ĭ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#define PRINTF_HID           //printf���ֱ���ض���USB�ӿ�(����usb.hǰ����)

#include "../comm/STC32G.h"  //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "../comm/usb.h"     //USB���Լ���λ����ͷ�ļ�
#include "stdio.h"
#include "intrins.h"

/*****************************************************************************/

#define MAIN_Fosc        24000000UL
#define Baudrate         9600UL
#define Timer0_Reload    (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��
#define TM               (65536UL -(MAIN_Fosc/Baudrate/4))

/*****************************************************************************/

sbit SLP1_N = P7^6;     //0: Sleep
sbit SLP2_N = P7^7;     //0: Sleep

/*************  ���س�������    **************/

#define FRAME_LEN           8    //���ݳ���: 8 �ֽ�
#define UART_BUF_LENGTH     64

/***************  �ṹ����    ***************/

#define	USART1	1
#define	USART2	2

typedef struct
{ 
	u8	id;				//���ں�

	u8	TX_read;		//���Ͷ�ָ��
	u8	TX_write;		//����дָ��
	u8	B_TX_busy;		//æ��־

	u8 	RX_Cnt;			//�����ֽڼ���
	u8	RX_TimeOut;		//���ճ�ʱ
	u8	B_RX_OK;		//���տ����
} COMx_Define; 

COMx_Define	COM1;
COMx_Define	COM2;

/*************  ���ر�������    **************/

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

//P3.2�ڰ�����λ�������
bit Key_Flag;
u16 Key_cnt;

u8 Lin_ID;
u8 TX1_BUF[8];
u8 TX2_BUF[8];

bit B_ULinRX1_Flag;
bit B_ULinRX2_Flag;

bit B_1ms;          //1ms��־

u16 Key1_cnt;
u16 Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;
bit Key2_Short_Flag;
bit Key2_Long_Flag;

bit Key1_Function;
bit Key2_Short_Function;
//bit Key2_Long_Function;

u8  RX1_Buffer[UART_BUF_LENGTH]; //���ջ���
u8  RX2_Buffer[UART_BUF_LENGTH]; //���ջ���

/*************  ���غ�������    **************/

void LinInit();
void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat);
void UsartLinSendHeader(u8 USARTx, u8 lid);
void UsartLinSendData(u8 USARTx, u8 *pdat);
void UsartLinSendChecksum(u8 USARTx, u8 *dat);
void KeyScan(void);
 
/********************* ������ *************************/
void main(void)
{
	u8 i;
	
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    RSTFLAG |= 0x04;   //����Ӳ����λ����Ҫ���P3.2��״̬ѡ���������򣬷���Ӳ����λ�����USB����ģʽ

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    usb_init();

    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run

	Lin_ID = 0x32;
	LinInit();
    EUSB = 1;   //IE2��ص��ж�ʹ�ܺ���Ҫ��������EUSB
	EA = 1;     //�����ж�

	SLP1_N = 1;
	SLP2_N = 1;
	TX1_BUF[0] = 0x11;
	TX1_BUF[1] = 0x12;
	TX1_BUF[2] = 0x13;
	TX1_BUF[3] = 0x14;
	TX1_BUF[4] = 0x15;
	TX1_BUF[5] = 0x16;
	TX1_BUF[6] = 0x17;
	TX1_BUF[7] = 0x18;
	
	TX2_BUF[0] = 0x21;
	TX2_BUF[1] = 0x22;
	TX2_BUF[2] = 0x23;
	TX2_BUF[3] = 0x24;
	TX2_BUF[4] = 0x25;
	TX2_BUF[5] = 0x26;
	TX2_BUF[6] = 0x27;
	TX2_BUF[7] = 0x28;
	
    printf("STC32G USART LIN1-LIN2 Test.\r\n");
    
	while(1)
	{
        if (bUsbOutReady) //USB���Լ���λ�������
        {
            usb_OUT_done();
        }

        if((B_ULinRX2_Flag) && (COM2.RX_Cnt >= 2))
        {
            B_ULinRX2_Flag = 0;

            //LIN1��������֡ͷ��LIN2�ӻ��ж� ID=0x13 ����Ӧ��������У����
            if((RX2_Buffer[0] == 0x55) && ((RX2_Buffer[1] & 0x3f) == 0x13)) //PID -> ID
            {
                UsartLinSendData(USART2,TX2_BUF);
                UsartLinSendChecksum(USART2,TX2_BUF);
            }
        }

		if(B_1ms)   //1ms��
		{
			B_1ms = 0;
			KeyScan();
			if(Key1_Function)
			{
				Key1_Function = 0;
				UsartLinSendHeader(USART1,0x13);  //����֡ͷ����ȡ����֡�����һ��������֡
			}
			if(Key2_Short_Function)
			{
				Key2_Short_Function = 0;
				UsartLinSendFrame(USART1,Lin_ID, TX1_BUF);  //����һ����������
			}

			if(COM2.RX_TimeOut > 0)     //��ʱ����
			{
				if(--COM2.RX_TimeOut == 0)
				{
					printf("LIN2 Read: ");
					for(i=0; i<COM2.RX_Cnt; i++)    printf("0x%02x ",RX2_Buffer[i]);    //�Ӵ�������յ�������
					COM2.RX_Cnt  = 0;   //����ֽ���
					printf("\r\n");
				}
			}
            
			if(COM1.RX_TimeOut > 0)     //��ʱ����
			{
				if(--COM1.RX_TimeOut == 0)
				{
					printf("LIN1 Read: ");
					for(i=0; i<COM1.RX_Cnt; i++)    printf("0x%02x ",RX1_Buffer[i]);    //�Ӵ�������յ�������
					COM1.RX_Cnt  = 0;   //����ֽ���
					printf("\r\n");
				}
			}
		}
	}
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}

//========================================================================
// ����: void UsartLinSendByte(u8 USARTx, u8 dat)
// ����: ����һ���ֽں�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinSendByte(u8 USARTx, u8 dat)
{
	if(USARTx == USART1)
	{
		COM1.B_TX_busy = 1;
		SBUF = dat;
		while(COM1.B_TX_busy);
	}
	else if(USARTx == USART2)
	{
		COM2.B_TX_busy = 1;
		S2BUF = dat;
		while(COM2.B_TX_busy);
	}
}

//========================================================================
// ����: void UsartLinSendData(u8 USARTx, u8 *pdat)
// ����: Lin�������ݺ�����
// ����: *pdat: �������ݻ�����.
// ����: Lin ID.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinSendData(u8 USARTx, u8 *pdat)
{
	u8 i;

	for(i=0;i<FRAME_LEN;i++)
	{
		UsartLinSendByte(USARTx,pdat[i]);
	}
}

//========================================================================
// ����: void UsartLinSendChecksum(u8 USARTx, u8 *dat)
// ����: ����У���벢���͡�
// ����: ���ݳ����������.
// ����: none.
// �汾: VER1.0
// ����: 2020-12-2
// ��ע: 
//========================================================================
void UsartLinSendChecksum(u8 USARTx, u8 *dat)
{
    u16 sum = 0;
    u8 i;

    for(i = 0; i < FRAME_LEN; i++)
    {
        sum += dat[i];
        if(sum & 0xFF00)
        {
            sum = (sum & 0x00FF) + 1;
        }
    }
    sum ^= 0x00FF;
    UsartLinSendByte(USARTx,(u8)sum);
}

//========================================================================
// ����: void UsartSendBreak(void)
// ����: ��ģʽ����Lin����Break������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartSendBreak(u8 USARTx)
{
	if(USARTx == USART1)
	{
		USARTCR5 |= 0x04;		//��ģʽ Send Break
	}
	else if(USARTx == USART2)
	{
		USART2CR5 |= 0x04;		//��ģʽ Send Break
	}
	UsartLinSendByte(USARTx,0x00);
}

//========================================================================
// ����: void UsartLinSendPID(u8 id)
// ����: ID�����У�����ת��PID�벢���͡�
// ����: ID��.
// ����: none.
// �汾: VER1.0
// ����: 2020-12-2
// ��ע: 
//========================================================================
void UsartLinSendPID(u8 USARTx, u8 id)
{
	u8 P0 ;
	u8 P1 ;
	
	P0 = (((id)^(id>>1)^(id>>2)^(id>>4))&0x01)<<6 ;
	P1 = ((~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&0x01)<<7 ;
	
	UsartLinSendByte(USARTx,id|P0|P1);
}

//========================================================================
// ����: void UsartLinSendHeader(u8 lid)
// ����: Lin��������֡ͷ������
// ����: ID��.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinSendHeader(u8 USARTx, u8 lid)
{
	UsartSendBreak(USARTx);				//Send Break
	UsartLinSendByte(USARTx,0x55);		//Send Sync Field
	UsartLinSendPID(USARTx,lid);		//��������ID
}

//========================================================================
// ����: void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat)
// ����: Lin������������֡������
// ����: lid: Lin ID; *pdat: �������ݻ�����.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat)
{
	UsartSendBreak(USARTx);				//Send Break
	UsartLinSendByte(USARTx,0x55);		//Send Sync Field

	UsartLinSendPID(USARTx,lid);		//��������ID
	UsartLinSendData(USARTx,pdat);
	UsartLinSendChecksum(USARTx,pdat);
}

//========================================================================
// ����: void UsartLinBaudrate(u8 USARTx, u16 brt)
// ����: Lin���߲��������ú�����
// ����: brt: ������.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinBaudrate(u8 USARTx, u16 brt)
{
	u16 tmp;
	
	tmp = (MAIN_Fosc >> 4) / brt;
	if(USARTx == USART1)
	{
		USARTBRH = (u8)(tmp>>8);
		USARTBRL = (u8)tmp;
	}
	else if(USARTx == USART2)
	{
		USART2BRH = (u8)(tmp>>8);
		USART2BRL = (u8)tmp;
	}
}

//========================================================================
// ����: void LinInit()
// ����: Lin��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void LinInit()
{
    P_SW1 = (P_SW1 & 0x3f) | 0xc0;  //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

	SCON = (SCON & 0x3f) | 0x40;    //USART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
	TI = 0;
	REN = 1;    //�������
	ES  = 1;    //�����ж�

	UsartLinBaudrate(USART1,Baudrate);		//���ò�����
	SMOD = 1;				//����1�����ʷ���
	USARTCR1 |= 0x80;		//LIN Mode Enable
	USARTCR5 &= ~0x20;		//LIN Master Mode
	
	//====== USART2 config =======
	S2_S  = 1;      //USART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

	S2CON = (S2CON & 0x3f) | 0x50;  //8λ����,�ɱ䲨����, �������
	AUXR |= 0x14;   //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
	ES2 = 1;        //�����ж�

	UsartLinBaudrate(USART2,Baudrate);	//���ò�����
	S2CFG |= 0x80;			//S2MOD = 1
	USART2CR1 |= 0x80;		//LIN Mode Enable
	USART2CR5 |= 0x20;		//LIN Slave Mode

	USART2CR5 |= 0x10;		//AutoSync
}

//========================================================================
// ����: UART1_ISR_Handler
// ����: UART1�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2022-06-23
//========================================================================
void UART1_ISR_Handler (void) interrupt UART1_VECTOR
{
	u8 Status;

	if(RI)
	{
		RI = 0;

		//--------USART LIN---------------
		Status = USARTCR5;
		if(Status & 0x02)     //if LIN header is detected
		{
			B_ULinRX1_Flag = 1;
		}

		if(Status & 0xc0)     //if LIN break is detected / LIN header error is detected
		{
			COM1.RX_Cnt = 0;
		}
		USARTCR5 &= ~0xcb;    //Clear flag
		//--------------------------------
		
		if(COM1.B_RX_OK == 0)
		{
			if(COM1.RX_Cnt >= UART_BUF_LENGTH)	COM1.RX_Cnt = 0;
			RX1_Buffer[COM1.RX_Cnt++] = SBUF;
			COM1.RX_TimeOut = 5;
		}
	}

	if(TI)
	{
		TI = 0;
		COM1.B_TX_busy = 0;
	}
}

//========================================================================
// ����: UART2_ISR_Handler
// ����: UART2�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2022-06-23
//========================================================================
void UART2_ISR_Handler (void) interrupt UART2_VECTOR
{
	u8 Status;

	if(S2RI)
	{
		S2RI = 0;

		//--------USART LIN---------------
		Status = USART2CR5;
		if(Status & 0x02)     //if LIN header is detected
		{
			B_ULinRX2_Flag = 1;
		}

		if(Status & 0xc0)     //if LIN break is detected / LIN header error is detected
		{
			COM2.RX_Cnt = 0;
		}
		USART2CR5 &= ~0xcb;   //Clear flag
		//--------------------------------
		
		if(COM2.B_RX_OK == 0)
		{
			if(COM2.RX_Cnt >= UART_BUF_LENGTH)	COM2.RX_Cnt = 0;
			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;
			COM2.RX_TimeOut = 5;
		}
	}

	if(S2TI)
	{
		S2TI = 0;
		COM2.B_TX_busy = 0;
	}
}

//========================================================================
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
	u16 i;
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i);
	}while(--ms);
}

//========================================================================
// ����: void KeyScan(void)
// ����: ����ɨ�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void KeyScan(void)
{
	//�����̰�����
	if(!P33)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt >= 50)		//50ms����
			{
				Key1_Flag = 1;			//���ð���״̬����ֹ�ظ�����
				Key1_Function = 1;
			}
		}
	}
	else
	{
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	//�����̰�����
	if(!P32)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;
			if(Key2_cnt >= 1000)		//����1s
			{
				Key2_Short_Flag = 0;	//����̰���־
				Key2_Long_Flag = 1;		//���ó�����־
				Key2_Flag = 1;			//���ð���״̬����ֹ�ظ�����
//				Key2_Long_Function = 1;

                //P3.2�ڰ�������1�봥�������λ������USB����ģʽ��
                USBCON = 0x00;      //���USB����
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //���������λ����ISP��ʼִ��
                while (1);
			}
			else if(Key2_cnt >= 50)		//50ms����
			{
				Key2_Short_Flag = 1;	//���ö̰���־
			}
		}
	}
	else
	{
		if(Key2_Short_Flag)			//�ж��Ƿ�̰�
		{
			Key2_Short_Flag = 0;	//����̰���־
			Key2_Short_Function = 1;
		}
		Key2_cnt = 0;
		Key2_Flag = 0;	//�����ͷ�
	}
}
