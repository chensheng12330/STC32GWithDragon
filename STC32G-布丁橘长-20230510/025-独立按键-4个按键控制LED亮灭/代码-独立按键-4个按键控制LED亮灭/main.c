//		@�����ٳ� 2023/02/02
//		��������-����KEY1����LED1������KEY2Ϩ��LED1������KEY3����8��LED������KEY4Ϩ��8��LED
//		��������ذ�����KEY1-P3.2��KEY2-P3.3��KEY3-P3.4��KEY4-P3.5
//		���������LED��LED1-P2.0��LED2-P2.1��LEDF3-P2.2��LED4-P2.3��LED5-P2.4��LED6-P2.5��LED7-P2.6��LED8-P2.7
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//������Ƶ35MHz�������ʵ��ʹ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

typedef unsigned char u8;				//unsigned char 		������ u8 ��ʾ
typedef unsigned int u16;				//unsigned int  		������ u16��ʾ
typedef unsigned long int u32;	//unsigned long int ������ u32��ʾ

sbit LED1 = P2^0;		//����LED��LED1-P2.0
sbit KEY1 = P3^2;		//���ذ�����KEY1-P3.2
sbit KEY2 = P3^3;		//���ذ�����KEY2-P3.3
sbit KEY3 = P3^4;		//���ذ�����KEY3-P3.4
sbit KEY4 = P3^5;		//���ذ�����KEY4-P3.5

void delayms(u16 ms);		//��ʱ��������
void keyscan();					//����ɨ�躯��			
	
void main()
{
	EAXFR = 1;				//ʹ��XFR����
	CKCON = 0x00;			//�����ⲿ���������ٶ����
	WTST = 0x00;			//���ó����ȡ�ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����
	
	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0x00;		//����P1��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		//����P3��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	
	while(1)
	{
		keyscan();
	}
}

void  delayms(u16 ms)		//����ʱ����������Ӧ��Ƶ��1����*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}
void keyscan()
{
	if(KEY1 == 0)					//�жϰ���KEY1�Ƿ���
	{	
		delayms(10);				//��ʱ����
		if(KEY1 == 0)				//�ٴ��ж�KEY1�Ƿ���
		{
			LED1 = 0;					//������LED��·���͵�ƽ����������LED1
		}
	}
	if(KEY2 == 0)					//�жϰ���KEY2�Ƿ���
	{
		delayms(10);				//��ʱ����
		if(KEY2 == 0)				//�ٴ��ж�KEY1�Ƿ���
		{
			LED1 = 1;					//������LED��·���ߵ�ƽϨ��Ϩ��LED1
		}
	}
	if(KEY3 == 0)					//�жϰ���KEY3�Ƿ���
	{
		delayms(10);				//��ʱ����
		if(KEY3 == 0)				//�ٴ��ж�KEY1�Ƿ���
		{
			P2 = 0x00;				//������LED��·������P2������LED
		}
	}
	if(KEY4 == 0)					//�жϰ���KEY4�Ƿ���
	{
		delayms(10);				//��ʱ����
		if(KEY4 == 0)				//�ٴ��ж�KEY1�Ƿ���
		{
			P2 = 0xFF;				//������LED��·��Ϩ��P2������LED
		}
	}
}