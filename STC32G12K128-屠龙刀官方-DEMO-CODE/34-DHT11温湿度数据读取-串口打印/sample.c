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

�����̻���STC32G����ת�Ӱ壨�����������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

�ɼ������ߴ�����DHT11����ʪ������

����(115200,N,8,1)��ӡ�ɼ����

����ʱ, Ĭ��ʱ�� 12MHz (�û��������޸�Ƶ��).

******************************************/

#include "STC32G.h"
#include "intrins.h"
#include "stdio.h"

#define MAIN_Fosc        12000000UL

/*****************************************************************************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

sbit DHT=P1^6;//DHT11ģ��

unsigned int Temperature[2];
unsigned int Humidity[2];

unsigned char Data_Receive[5];

void delay_ms(unsigned int ms);
void DHT_Collect();

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;          //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;				//��ʱ��1��ʼ��ʱ

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
#else
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
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
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/*****************************************************************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

	UartInit();

	printf("STC32G DHT11 Test.\r\n");

	while(1)
    {
        DHT_Collect();  //DHT11���ݲɼ�
        printf("Temperature = %u.%u��C\r\n",Temperature[0],Temperature[1]);
        printf("Humidity = %u.%u%%RH\r\n",Humidity[0],Humidity[1]);
        delay_ms(1000); //delay 1s
    }
}
/*****************************************************************************/

void Delay30us()//@12.000MHz
{
	unsigned long i;

	_nop_();
	_nop_();
	_nop_();
	i = 88UL;
	while (i) i--;
}

void Delay20ms()//@12.000MHz
{
	unsigned long i;

	_nop_();
	_nop_();
	_nop_();
	i = 59998UL;
	while (i) i--;
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);   //6T per loop
    }while(--ms);
}

void DHT_Collect()//�¶Ȳɼ�����
{
    unsigned char i,j;//forѭ������
    unsigned int t;//��ʱ�ж�
    unsigned char dat8=0;//һ�ζ�ȡ��8λ���ݣ���Ҫ��5��
    unsigned int CheckValue=0;//У���
    
    DHT=0;//��������ʼ�ź�
    Delay20ms();//����������������18ms
    DHT=1;//������������20~40us
    Delay30us();
    
    t=160;//���ó�ʱ�ȴ�ʱ��
    while(DHT&&t--);//�ȴ�DHT11��������
    if(t==0)//��ʱ
    {
        DHT=1;
    }
    t=600;//���ó�ʱ�ȴ�ʱ��
    while(!DHT&&t--);//�ȴ�DHT11��������
    if(t==0)//��ʱ
    {
        DHT=1;
    }
    t=600;//���ó�ʱ�ȴ�ʱ��
    while(DHT&&t--);//�ȴ�DHT11��������
    if(t==0)//��ʱ
    {
        DHT=1;
    }

    for(j=0;j<5;j++)//5�ζ�ȡ
    {
        for(i=0;i<8;i++)//1��8��λ
        {
            //�ȴ�50us��ʼʱ϶
            t=400;//���ó�ʱ�ȴ�ʱ��
            while(!DHT&&t--);//�ȴ�DHT11��������
            if(t==0)//��ʱ
            {
                DHT=1;
            }
            Delay30us();
            Delay30us();
            dat8<<=1;
            if(DHT) //�ߵ�ƽ����ʱ��ϳ�(70us)
            dat8+=1;//����ֵΪ1
            
            t=300;//���ó�ʱ�ȴ�ʱ��
            while(DHT&&t--);//�ȴ�����¼�ߵ�ƽ����ʱ��
        }
        Data_Receive[j]=dat8;
    }
    
    Delay30us();//�ȴ�DHT11����50us
    Delay30us();
    DHT=1;

    for(i=0;i<4;i++)
    {
        CheckValue+=Data_Receive[i];//����У��ֵ
    }
    if((unsigned char)CheckValue==Data_Receive[4])//�ȶ�
    {
        Humidity[0]=Data_Receive[0];    //ʪ������
        Humidity[1]=Data_Receive[1];    //ʪ��С��
        Temperature[0]=Data_Receive[2]; //�¶�����
        Temperature[1]=Data_Receive[3]%10;  //�¶�С�������λΪ������־
    }
}
