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


/*************  ��������˵��  **************

�����̻���STC32G����ת�Ӱ壨�����������б�д���ԡ�

��һ��P3.3�������޸�һ��IRC��Ƶ�����ڴ�ӡIRCѡ����š�

��һ��P3.4�������޸�һ��RST�ܽŹ��ܣ����ڴ�ӡ��ǰ��RST�ܽŹ��ܡ�

��һ��P3.5�������޸�һ��LVDS�ż���ѹ���𣬴��ڴ�ӡ��ǰ��LVDS�ż���ѹ����

P2����ˮ���ڲ�ͬIRC��Ƶʱ�л��ٶȲ�ͬ��

���������ʾ������λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
   (�����ϣ����λ����USB����ģʽ�Ļ������ڸ�λ�����ｫ IAP_CONTR ��bit6��0��ѡ��λ���û�������)
   (��Ƶ�޸ĺ�USB HID�豸�ᶪʧ��ͨ��USB HID�����Զ����ع���Ҳ��ʧЧ���������ﲻ��ʾͨ��USB�����ͣ������)

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "../comm/stc32g.h"  //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "stdio.h"

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

/*************  ���س�������    **************/

#define T22M_ADDR   CHIPID11   //22.1184MHz
#define T24M_ADDR   CHIPID12   //24MHz
#define T27M_ADDR   CHIPID13   //27MHz
#define T30M_ADDR   CHIPID14   //30MHz
#define T33M_ADDR   CHIPID15   //33.1776MHz
#define T35M_ADDR   CHIPID16   //35MHz
#define T36M_ADDR   CHIPID17   //36.864MHz
#define T40M_ADDR   CHIPID18   //40MHz
#define T44M_ADDR   CHIPID19   //44.2368MHz
#define T48M_ADDR   CHIPID20   //48MHz

#define VRT6M_ADDR  CHIPID21   //VRTRIM_6M
#define VRT10M_ADDR CHIPID22   //VRTRIM_10M
#define VRT27M_ADDR CHIPID23   //VRTRIM_27M
#define VRT44M_ADDR CHIPID24   //VRTRIM_44M

#define Baudrate      9600L   //������ 115200 ��Ҫ5M����IRCƵ��

/*************  Ƶ�ʶ���    **************/

typedef enum
{
	IRC_500K = 0,		/*0*/
	IRC_1M,
	IRC_2M,
	IRC_3M,
	IRC_5M,
	IRC_6M,
	IRC_8M,
	IRC_10M,
	IRC_11M,
	IRC_12M,
	IRC_15M,
	IRC_18M,
	IRC_20M,
	IRC_22M,
	IRC_24M,
	IRC_27M,
	IRC_30M,
	IRC_33M,
	IRC_35M,
	IRC_36M,
}IRC_IndexType;

typedef enum
{
	IRCBAND_6M = 0,		/*0*/
	IRCBAND_10M,
	IRCBAND_27M,
	IRCBAND_44M,
}IRCBAND_SelType;

/*************  ���ر�������    **************/
u8 index = IRC_24M;
u32 MAIN_Fosc =	24000000L;	//������ʱ��

//P3.2�ڰ�����λ�������
bit Key_Flag;
u16 Key_cnt;

bit Key1_Flag;
u16 Key1_cnt;

bit Key2_Flag;
u16 Key2_cnt;

bit Key3_Flag;
u16 Key3_cnt;

void KeyResetScan(void);
void delay_ms(u8 ms);
void HardwareMarquee(void);
void Timer0_Init(u32 clk);
void SetMCLK(u8 clk);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
	P_SW1 = (P_SW1 & 0x3f) | 0x00;    //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;          //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1 = (65536 -(MAIN_Fosc/Baudrate/4));
	TH1 = (65536 -(MAIN_Fosc/Baudrate/4))>>8;
	TR1 = 1;			//��ʱ��1��ʼ��ʱ
}

void UartPutc(unsigned char dat)
{
	SBUF = dat; 
	while(TI==0);
	TI = 0;
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/********************** ������ ************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    RSTFLAG |= 0x04;   //����Ӳ����λ����Ҫ���P3.2��״̬ѡ���������򣬷���Ӳ����λ�����USB����ģʽ

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x40;   //����Ϊ׼˫���, P1.6����λ�������
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    Timer0_Init(MAIN_Fosc/1000);	//Timer 0 �ж�Ƶ��, 1000��/��
    UartInit();
    
    MCLKOCR = 0x81; //��ʱ��Ƶ��2��Ƶ�����P1.6��

    EA = 1;     //�������ж�

    //оƬ��λָʾ
    P20 = 0;		//LED On
    delay_ms(250);
    P20 = 1;		//LED Off
    delay_ms(250);
    P20 = 0;		//LED On
    delay_ms(250);
    P20 = 1;		//LED Off
    delay_ms(250);
    printf("����Ӳ�����ܳ�ʼ������.\r\n");

    while(1)
    {
        HardwareMarquee();
    }
} 
/**********************************************/

void HardwareMarquee(void)
{
    P20 = 0;		//LED On
    delay_ms(250);
    P20 = 1;		//LED Off
    P21 = 0;		//LED On
    delay_ms(250);
    P21 = 1;		//LED Off
    P22 = 0;		//LED On
    delay_ms(250);
    P22 = 1;		//LED Off
    P23 = 0;		//LED On
    delay_ms(250);
    P23 = 1;		//LED Off
    P24 = 0;		//LED On
    delay_ms(250);
    P24 = 1;		//LED Off
    P25 = 0;		//LED On
    delay_ms(250);
    P25 = 1;		//LED Off
    P26 = 0;		//LED On
    delay_ms(250);
    P26 = 1;		//LED Off
    P27 = 0;		//LED On
    delay_ms(250);
    P27 = 1;		//LED Off
    P26 = 0;		//LED On
    delay_ms(250);
    P26 = 1;		//LED Off
    P25 = 0;		//LED On
    delay_ms(250);
    P25 = 1;		//LED Off
    P24 = 0;		//LED On
    delay_ms(250);
    P24 = 1;		//LED Off
    P23 = 0;		//LED On
    delay_ms(250);
    P23 = 1;		//LED Off
    P22 = 0;		//LED On
    delay_ms(250);
    P22 = 1;		//LED Off
    P21 = 0;		//LED On
    delay_ms(250);
    P21 = 1;		//LED Off
}

//========================================================================
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-3
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = 2000;   //�̶���12M��Ƶ���㣬��ͬIRC��ʱ�Ͳ�ͬ
        while(--i);   //10T per loop
    }while(--ms);
}

//========================================================================
// ����: void Timer0_Init(u32 clk)
// ����: ��ʱ��0��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-2
// ��ע: 
//========================================================================
void Timer0_Init(u32 clk)
{
    // Timer0��ʼ��
    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)((65536UL - clk) / 256);
    TL0 = (u8)((65536UL - clk) % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// ����: void KeyResetScan(void)
// ����: P3.2�ڰ�������1�봥�������λ������USB����ģʽ��
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void KeyResetScan(void)
{
    u8 temp;
    
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//����1000ms��Ч�������
            {
                Key_Flag = 1;		//���ð���״̬����ֹ�ظ�����

                USBCON = 0x00;      //���USB����
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //���������λ����ISP��ʼִ��
                while (1);
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }

	if(!P33)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt >= 50)		//50ms����
			{
				Key1_Flag = 1;			//���ð���״̬����ֹ�ظ�����

                index++;
                if(index > IRC_36M) index = 0;
                SetMCLK(index);
                printf("IRC index=%bd.\r\n",index);
			}
		}
	}
	else
	{
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	if(!P34)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;
			if(Key2_cnt >= 50)		//50ms����
			{
				Key2_Flag = 1;			//���ð���״̬����ֹ�ظ�����

                if(RSTCFG & 0x10)
                {
                    RSTCFG &= ~0x10;
                    printf("RST�ܽ�����IO��.\r\n");
                }
                else
                {
                    RSTCFG |= 0x10;
                    printf("RST�ܽ�������λ��.\r\n");
                }
			}
		}
	}
	else
	{
		Key2_cnt = 0;
		Key2_Flag = 0;
	}

	if(!P35)
	{
		if(!Key3_Flag)
		{
			Key3_cnt++;
			if(Key3_cnt >= 50)		//50ms����
			{
				Key3_Flag = 1;			//���ð���״̬����ֹ�ظ�����

                temp = (RSTCFG & 0x03);
                temp++;
                RSTCFG &= ~0x03;
                RSTCFG |= (temp & 0x03);
                printf("����LVDS�ż���ѹ����%bd.\r\n",(temp & 0x03));
			}
		}
	}
	else
	{
		Key3_cnt = 0;
		Key3_Flag = 0;
	}
}

//========================================================================
// ����: void timer0_int(void)
// ����: Timer0 1ms�жϺ�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void timer0_int(void) interrupt 1  //1ms �жϺ���
{
    KeyResetScan();   //P3.2�ڰ������������λ������USB����ģʽ������Ҫ�˹��ܿ�ɾ�����д���
}

//========================================================================
// ����: void SetMCLK(u8 clk)
// ����: ������Ƶ
// ����: clk: ��Ƶ���
// ����: none.
// �汾: VER1.0
// ����: 2022-10-14
// ��ע: 
//========================================================================
void SetMCLK(u8 clk) //������Ƶ
{
	if(clk > IRC_36M) return;	//Error
	switch(clk)
	{
	case IRC_500K:
		//ѡ��500KHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 48;
		MAIN_Fosc =	500000L;	//������ʱ��
		break;

	case IRC_1M:
		//ѡ��1MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 24;
		MAIN_Fosc =	1000000L;	//������ʱ��
		break;

	case IRC_2M:
		//ѡ��2MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 12;
		MAIN_Fosc =	2000000L;	//������ʱ��
		break;

	case IRC_3M:
		//ѡ��3MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 8;
		MAIN_Fosc =	3000000L;	//������ʱ��
		break;

	case IRC_5M:
		//ѡ��5.5296MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 4;
		MAIN_Fosc =	5529600L;	//������ʱ��
		break;

	case IRC_6M:
		//ѡ��6MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 4;
		MAIN_Fosc =	6000000L;	//������ʱ��
		break;

	case IRC_8M:
		//ѡ��8MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 3;
		MAIN_Fosc =	8000000L;	//������ʱ��
		break;

	case IRC_10M:
		//ѡ��10MHz
		CLKDIV = 0x04;
		IRTRIM = T40M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 4;
		MAIN_Fosc =	10000000L;	//������ʱ��
		break;

	case IRC_11M:
		//ѡ��11.0592MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 2;
		MAIN_Fosc =	11059200L;	//������ʱ��
		break;

	case IRC_12M:
		//ѡ��12MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 2;
		MAIN_Fosc =	12000000L;	//������ʱ��
		break;

	case IRC_15M:
		//ѡ��15MHz
		CLKDIV = 0x04;
		IRTRIM = T30M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 2;
		MAIN_Fosc =	15000000L;	//������ʱ��
		break;

	case IRC_18M:
		//ѡ��18.432MHz
		CLKDIV = 0x04;
		IRTRIM = T36M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 2;
		MAIN_Fosc =	18432000L;	//������ʱ��
		break;

	case IRC_20M:
		//ѡ��20MHz
		CLKDIV = 0x04;
		IRTRIM = T40M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 2;
		MAIN_Fosc =	20000000L;	//������ʱ��
		break;

	case IRC_24M:
		//ѡ��24MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	24000000L;	//������ʱ��
		break;

	case IRC_27M:
		//ѡ��27MHz
		CLKDIV = 0x04;
		IRTRIM = T27M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	27000000L;	//������ʱ��
		break;

	case IRC_30M:
		//ѡ��30MHz
		CLKDIV = 0x04;
		IRTRIM = T30M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	30000000L;	//������ʱ��
		break;

	case IRC_33M:
		//ѡ��33.1176MHz
		CLKDIV = 0x04;
		IRTRIM = T33M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	33117600L;	//������ʱ��
		break;

	case IRC_35M:
		//ѡ��35MHz
		CLKDIV = 0x04;
		IRTRIM = T35M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 1;
		MAIN_Fosc =	35000000L;	//������ʱ��
		break;

	case IRC_36M:
		//ѡ��36.864MHz
		CLKDIV = 0x04;
		IRTRIM = T36M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 1;
		MAIN_Fosc =	36864000L;	//������ʱ��
		break;

	default:
		//ѡ��22.1184MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	22118400L;	//������ʱ��
		break;
	}
	Timer0_Init(MAIN_Fosc/1000);
	UartInit();
}
