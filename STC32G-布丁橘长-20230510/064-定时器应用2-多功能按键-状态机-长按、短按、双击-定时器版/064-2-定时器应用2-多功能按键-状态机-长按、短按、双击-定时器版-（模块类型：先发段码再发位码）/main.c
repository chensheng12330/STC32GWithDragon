//		@�����ٳ� 2023/02/14
//		�๦�ܰ���-��ʱ����-״̬��-�������̰���˫��
//		ռ�ö�ʱ��4����Ҫ���������жԶ�ʱ��4��ʼ����Timer4_Init();��ʹ��EA=1
//		KEY1�̰���LED1������ KEY1������LED1Ϩ��KEY1˫����LED1��LED2�������ٴ�˫����LED1��LED2Ϩ��
//		KEY2�̰���LED3������ KEY2������LED3Ϩ��KEY2˫����LED3��LED4�������ٴ�˫����LED3��LED4Ϩ��
//		KEY3�̰���LED5������ KEY3������LED5Ϩ��KEY3˫����LED5��LED6�������ٴ�˫����LED5��LED6Ϩ��
//		KEY4�̰���LED7������ KEY4������LED7Ϩ��KEY4˫����LED7��LED8�������ٴ�˫����LED7��LED8Ϩ��
//		�������ʾ��Ӧ��ֵ
//		8λ������LEDģ�飬��8��LED���������ӣ�ÿ��LED��һ���������衣�����˽�VCC���͵�ƽ������
//		����4������������ÿ������һ�˽�GND����һ�˴�һ��300ŷ����󣬽�P32-P35
//		LED���Ŷ��壺	P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		�����ģ�����Ŷ��壺DS(DIO):P6.6 SH(SCK):P6.5 ST(RCK):P6.4
//		�������Ŷ��壺P32->KEY1 P33->KEY2 P34->KEY3 P35->KEY4����key_multi.h�ж���
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1 35MHz 

#include <STC32G.H>
#include "key_multi_timer.h"
#include "config.h"
#include "seg595.h"

#define ON 	0				//����0 Ϊ ����������LED��·��
#define	OFF	1				//����1 Ϊ �أ�������LED��·��

sbit LED1 = P2^0;		//���Ŷ��壺LED1->P20
sbit LED2 = P2^1;		//���Ŷ��壺LED2->P21
sbit LED3 = P2^2;		//���Ŷ��壺LED3->P22
sbit LED4 = P2^3;		//���Ŷ��壺LED4->P23
sbit LED5 = P2^4;		//���Ŷ��壺LED5->P24
sbit LED6 = P2^5;		//���Ŷ��壺LED6->P25
sbit LED7 = P2^6;		//���Ŷ��壺LED7->P26
sbit LED8 = P2^7;		//���Ŷ��壺LED8->P27

void Timer0_Init(void);

u8 Key1_nums,Key2_nums,Key3_nums,Key4_nums;		//KEY1˫��������KEY2˫��������KEY3˫��������KEY4˫������
u8 keyvalue;		//��ֵ
u8 segvalue;
void main()
{
	EAXFR = 1; 			// ʹ�ܷ��� XFR
	CKCON = 0x00; 	// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00;		// ���ó������ȴ��������ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����
	
	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0x00;		//����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		//����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		//����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		//����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		//����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		//����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	
	Timer4_Init();					//��ʱ��4��ʼ��
	EA = 1;									//ʹ��EA���ж�
	while(1)
	{
		keyvalue = key_multi();		//��ȡ��ֵ����ֵ�ͳ����̰�˫���ĸ��ϼ�ֵ��
		
		if(keyvalue == Key1_Single_Click)
		{
			LED1 = ON;			//KEY1�̰�������LED1
			segvalue = Key1_Single_Click;
		}
		if(keyvalue == Key1_Long_Click) 
		{
			LED1 = OFF;			//KEY1������Ϩ��LED1
			segvalue = Key1_Long_Click;
		}
		if(keyvalue == Key1_Double_Click) 								//KEY1˫��������LED1��LED2���ٴ�˫��Ϩ��LED1��LED2
		{
			Key1_nums++;		//KEY1˫������
			segvalue = Key1_Double_Click;
			if(Key1_nums%2 == 1)		//������˫��������LED1��LED2
			{
				LED1 = ON;
				LED2 = ON;
			}
			if(Key1_nums%2 == 0)		//ż����˫����Ϩ��LED1��LED2
			{
				LED1 = OFF;
				LED2 = OFF;
			}
			if(Key1_nums >= 99) Key1_nums = 0;		//��ֹKEY1˫���������������
		}
		
		if(keyvalue == Key2_Single_Click)
		{
			LED3 = ON;			//KEY2�̰�������LED3
			segvalue = Key2_Single_Click;
		}
		if(keyvalue == Key2_Long_Click) 	
		{
			LED3 = OFF;			//KEY2������Ϩ��LED3
			segvalue = Key2_Long_Click;
		}
		if(keyvalue == Key2_Double_Click) 								//KEY3˫��������LED3��LED4���ٴ�˫����Ϩ��LED3��LED4
		{
			Key2_nums++;		//KEY2˫������
			segvalue = Key2_Double_Click;
			if(Key2_nums%2 == 1)		//������˫��������LED3��LED4
			{
				LED3 = ON;
				LED4 = ON;
			}
			if(Key2_nums%2 == 0)		//ż����˫����Ϩ��LED3��LED4
			{
				LED3 = OFF;
				LED4 = OFF;
			}
			if(Key2_nums >= 99) Key2_nums = 0;	//��ֹKEY2˫���������������
		}
		
		if(keyvalue == Key3_Single_Click)
		{	
			LED5 = ON;			//KEY3�̰�������LED5
			segvalue = Key3_Single_Click;
		}
		if(keyvalue == Key3_Long_Click)
		{
			LED5 = OFF;			//KEY3������Ϩ��LED5
			segvalue = Key3_Long_Click;
		}
		if(keyvalue == Key3_Double_Click) 								//KEY3˫��������LED5��LED6���ٴ�˫����Ϩ��LED5��LED6
		{
			Key3_nums++;		//KEY3˫������
			segvalue = Key3_Double_Click;
			if(Key3_nums%2 == 1)		//������˫��������LED5��LED6
			{
				LED5 = ON;
				LED6 = ON;
			}
			if(Key3_nums%2 == 0)		//ż����˫����Ϩ��LED5��LED6
			{
				LED5 = OFF;
				LED6 = OFF;
			}
			if(Key3_nums >= 99) Key3_nums = 0;		//��ֹKEY3˫���������������
		}
		
		if(keyvalue == Key4_Single_Click) 
		{
			LED7 = ON;			//KEY4�̰�������LED7
			segvalue = Key4_Single_Click;
		}
		if(keyvalue == Key4_Long_Click) 	
		{
			LED7 = OFF;			//KEY4������Ϩ��LED7
			segvalue = Key4_Long_Click;
		}
		if(keyvalue == Key4_Double_Click) 								//KEY4˫��������LED7��LED8���ٴ�˫����Ϩ��LED7��LED8
		{
			Key4_nums++;		//KEY4˫������
			segvalue = Key4_Double_Click;
			if(Key4_nums%2 == 1)		//������˫��������LED7��LED8	
			{
				LED7 = ON;
				LED8 = ON;
			}
			if(Key4_nums%2 == 0)		//ż����˫����Ϩ��LED7��LED8
			{
				LED7 = OFF;
				LED8 = OFF;
			}
			if(Key4_nums >= 99) Key4_nums = 0;		//��ֹKEY4˫���������������
		}
		SEG_Disp((u8)(segvalue/10),7);
		SEG_Disp((u8)(segvalue%10),8);
	}
}