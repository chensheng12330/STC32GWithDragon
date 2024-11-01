// 	@�����ٳ� 2023/03/13
// 	PWM���벶��ʾ����PWM1P(P1.0)���������ص������أ�������������㷽��Ƶ�ʣ�MAX7219�������ʾ
//	������PWM5(P2.0)������ϵ�Ϊ500Hz�����ذ�����KEY1(P3.2) Ƶ�ʼ���KEY2(P3.3)Ƶ�ʼӣ�4��Ƶ�ʣ�500Hz,1K,2K,5KHz
//  MAX7219�����ģ�����Ŷ��壺CS = P6^5;DIN = P6^6;CLK = P6^4;������MAX7219.h���޸ģ�
//  ʵ�鿪���壺��������.1 @��Ƶ12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"

#define PWMB_PSC (12 - 1)			// 12��Ƶ��ʱ������1us
#define PWMB_PERIOD_500 2000	// ����2���룬500Hz
#define PWMB_PERIOD_1K 1000		// ����1���룬1KHz		
#define PWMB_PERIOD_2K 500		// ����500us��2KHz
#define PWMB_PERIOD_5K 200		// ����200us��5KHz

sbit KEY1 = P3^2;							// ���ذ�����P3.2
sbit KEY2 = P3^3;							// ���ذ�����P3.3

void SYS_Ini();								// STC32��ʼ������
void SEG_Disp(void);					// �������ʾ
void PWM_Config();						// PWM��ʼ������
void Timer0_Init(void);				// ��ʱ��0��ʼ��
void keyscan();								// ����ɨ��

int cap;											// �����ֵ
long int cap_new;							// ��ǰ�������ֵ
int cap_old;									// �ϴβ������ֵ
u16 updateseg;							  // ��������ܱ�־
long int cap_overflow;				// �������ֵ
long int freq;								// ����Ƶ��
u8 segdelay;									// �������ʱ����
bit B_1ms;										// 1ms��ʱ��־
u8 key1delay,key2delay;				// ����1������2��ʱ����
u16 period;										// PWM����
u8 number;										// PWM���ڶ�Ӧ����ֵ��0-3	
bit updateperiod;							// �������ڱ�־

void main(void)
{
	SYS_Ini();									// STC32��ʼ������
	PWM_Config();							  // PWM��ʼ������
	Timer0_Init();							// ��ʱ��0��ʼ��
	EA = 1;											// ʹ��EA���ж�
	MAX7219_Ini();							// MAX7219��ʼ��
	SEG_Disp();									// �������ʾ
	period = PWMB_PERIOD_500;		// ��ʼPWMƵ��500Hz
	while (1)
	{
		if(B_1ms == 1)						// 1���뵽
		{
			segdelay++;							// �������ʱ+1
			B_1ms = 0;							// ����1�����־
			if(segdelay == 200)			// ÿ200����ˢ��һ�������
			{
				freq = (u16)(MAIN_Fosc / cap)+1;	// ���㷽��Ƶ�ʣ��˴�+1��Ϊ�˼��ϱ�������С������
				SEG_Disp();						// ˢ�������
				segdelay = 0;					// �����������ʱ����
			}
		}
	}
}
void SYS_Ini()								// STC32��ʼ������
{
	EAXFR = 1; 									// ʹ�ܷ��� XFR
	CKCON = 0x00; 							// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00; 								// ���ó������ȴ���������ֵΪ 0 �ɽ� CPU ִ�г�����ٶ�����Ϊ���
	P0M1 = 0x00;P0M0 = 0x00;		// ����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0xFF;P1M0 = 0x00;		// ����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		// ����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		// ����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		// ����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		// ����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		// ����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		// ����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
}
void PWM_ISR() interrupt 26
{
	if(PWMA_SR1 & 0x01)					// �����ж�(PWM���������)
	{
		PWMA_SR1 &= ~0x01;				// ��������жϱ�־
		cap_overflow += 65536;		// ����ֵҪ���ϼ�����һ�����65536
	}
	if (PWMA_SR1 & 0X02)				// �������жϱ�־��1
	{	
		PWMA_SR1 &= ~0X02;				// ���㲶���жϱ�־
		cap_old = cap_new;				// ���µ�ǰ����ֵ
		cap_new =(PWMA_CCR1H << 8) + PWMA_CCR1L;	// ��ȡ����ֵ��8λ,���ϲ�����cap_new
		cap = cap_new + cap_overflow - cap_old;		// �������β����ֵ
		cap_overflow = 0;					// ���㲶���������ֵ
	}
}
void PWM_Config()							// PWM��ʼ������
{
//--------------------------PWMA����-����������----------------------------------	
	PWMA_ENO = 0x00;						// �ر����ʹ��
	PWMA_CCER1 = 0x00;					// �ر�ͨ��
	PWMA_CCMR1 = 0x01; 					// ͨ��ģʽ����Ϊ����
	PWMA_SMCR = 0x56;					  // ����1������ģʽ
	PWMA_CCER1 = 0x01;					// ʹ�ܲ���/�Ƚ�ͨ��1
  PWMA_CCER1 |= 0x00;					// ���ò���/�Ƚ�ͨ��1�����ز���
//	PWMA_CCER1 |= 0x20;					// ���ò���/�Ƚ�ͨ��1�½��ز���

	PWMA_IER = 0x03; 						// ʹ�ܲ����жϡ������ж�
	PWMA_CR1 |= 0x01; 					// ʹ��PWMA������
	
//------------PWMB����-PWM5(P2.0)���һ·PWM��500Hz-5KHz��ռ�ձȣ�50%-------------------
	PWMB_PSCRH = (u16)(PWMB_PSC >> 8);
	PWMB_PSCRL = (u16)(PWMB_PSC);	// Ԥ��Ƶ
	
	PWMB_CCER1 = 0x00;					// �ر�ͨ��
	PWMB_CCMR1 = 0x68; 					// ͨ��ģʽ����Ϊ�����PWMģʽ1
	PWMB_CCER1 = 0x05;					// ʹ�ܲ���/�Ƚ�ͨ��1��ʹ��CCRԤװ��

	PWMB_CCR5H = (u16)(PWMB_PERIOD_500 >> 1 >> 8);
	PWMB_CCR5L = (u16)(PWMB_PERIOD_500 >> 1);	// ����ռ�ձ�50%
	
	PWMB_ARRH = (u16)(PWMB_PERIOD_500 >> 8);
	PWMB_ARRL = (u16)(PWMB_PERIOD_500);	// ��������
	
	PWMB_ENO = 0x01;						// ʹ��PWM5���
	PWMB_BKR = 0x80;						// ʹ��PWMB�����
	PWMB_CR1 |= 0x01; 					// ʹ��PWMB������
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;
	keyscan();
	if(updateperiod == 1)				// �������ڱ�־��1ʱ
	{
		PWMB_ARRH = (u16)(period >> 8);
		PWMB_ARRL = (u16)(period);				// ��������
		PWMB_CCR5H = (u16)(period >> 1 >> 8);
		PWMB_CCR5L = (u16)(period >> 1);	// ����ռ�ձ�
		updateperiod = 0;					// ����������ڱ�־
	}
}
void Timer0_Init(void)				//1����@12.000MHz
{
	AUXR |= 0x80;								//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;								//���ö�ʱ��ģʽ
	TL0 = 0x20;									//���ö�ʱ��ʼֵ
	TH0 = 0xD1;									//���ö�ʱ��ʼֵ
	TF0 = 0;										//���TF0��־
	TR0 = 1;										//��ʱ��0��ʼ��ʱ
	ET0 = 1;										//ʹ�ܶ�ʱ��0�ж�
}
void keyscan()
{
//--------------------KEY1--------------------------------------------------------------
	if(KEY1 == 0)								// ����1����
	{
		if(B_1ms == 1)						// 1���뵽
		{
			key1delay++;						// ����1��ʱ+1
			B_1ms = 0;							// ���㰴��1��ʱ����
		}
		if(KEY1 == 0 && key1delay == 10)	// ����1���£�������ʱ10���뵽
		{
			if(number > 0) number -= 1;			// number-1��number��Ӧ4��Ƶ�ʣ�500Hz��1K��2K��5K
			updateperiod = 1;								// �������ڱ�־��1
		}
	}
	if(KEY1 == 1) key1delay = 0; 				// �ɿ����������㰴��1��ʱ
//--------------------KEY2---------------------------------------------------------------
	if(KEY2 == 0)								// ����2����
	{
		if(B_1ms == 1)						// 1���뵽
		{
			key2delay++;						// ����2��ʱ+1
			B_1ms = 0;							// ���㰴��2��ʱ����
		}
		if(KEY2 == 0 && key2delay == 10)	// ����1���£�������ʱ10���뵽
		{
			if(number < 3) number += 1;			// number+1��number��Ӧ4��Ƶ�ʣ�500Hz��1K��2K��5K
			updateperiod = 1;								// �������ڱ�־��1
		}
	}
	if(KEY2 == 1) key2delay = 0;				// �ɿ�����2�����㰴��2��ʱ
	switch(number)											
	{
		case 0:period = PWMB_PERIOD_500;break;	// number = 0: 500Hz
		case 1:period = PWMB_PERIOD_1K;break;		// number = 1: 1KHz
		case 2:period = PWMB_PERIOD_2K;break;		// number = 2: 2KHz
		case 3:period = PWMB_PERIOD_5K;break;		// number = 3: 5KHz
		default:break;
	}
}
void SEG_Disp(void)													// MAX7219�������ʾ����								
{							
	Write7219(8,15); 													// �����1λϨ��
	Write7219(7,15); 													// �����2λϨ��
	Write7219(6,15); 													// �����3λϨ��
	Write7219(5,15); 													// �����4λϨ��
	Write7219(4,(u16)((freq / 1000)%10)); 		// �����5λ��ʾƵ��ǧλ
	Write7219(3,(u16)((freq / 100)%10)); 			// �����6λ��ʾƵ�ʰ�λ
	Write7219(2,(u16)((freq / 10)%10)); 			// �����7λ��ʾƵ��ʮλ
	Write7219(1,(u16)(freq % 10)); 						// �����8λ��ʾƵ�ʸ�λ
}