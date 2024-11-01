//	@�����ٳ� 2023/02/24
//	PWMʾ���������1·PWM�����򿪹����Ҽ�����1�������SG92R�������ϼ���ģʽ��PWMģʽ1
//	ʵ�鿪���壺STC32G12K128��������.1 ��Ƶ@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//12��Ƶ
#define PWM_PERIOD 		2000		//����20����
#define PWM_DUTY_0 		50			//ռ�ձ�2.5%
#define PWM_DUTY_45		100		//ռ�ձ�5%	
#define PWM_DUTY_90		150		//ռ�ձ�7.5%
#define PWM_DUTY_135	200		//ռ�ձ�10%
#define PWM_DUTY_180	250		//ռ�ձ�12.5%

sbit KEY_LEFT 	= P7^2;				//�����P7.2
sbit KEY_RIGHT 	= P7^1;				//�Ҽ���P7.1

void PWMA_Config(void);				//PWMA��ʼ�����ú���
void Timer0_Init(void);				//��ʱ��0��ʼ������
void keyscan();								//����ɨ�躯��
void Update_duty();						//����ռ�ձȺ���

bit B_10ms;										//10ms��ʱ��־
u8 keydelay_left,keydelay_right;
u16 duty;
u8 angle;
bit dutychange;

void main(void)
{
	EAXFR = 1; 									// ʹ�ܷ��� XFR
	CKCON = 0x00; 							// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00;								// ���ó������ȴ��������ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����

	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0xFF;		//����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		//����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		//����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		//����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		//����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		//����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	
	duty = PWM_DUTY_0;					//�����ʼ�Ƕ�0
	PWMA_Config();							//PWMA��ʼ������
	Update_duty();							//����ռ�ձ�
	Timer0_Init();							//��ʱ��0��ʼ��
	EA = 1;											//ʹ��EA���ж�

	while (1)
	{
		Update_duty();						//ѭ��ִ��ռ�ձȸ��º���
	}
}
void PWMA_Config(void)
{
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 		//119+1=120Ԥ��Ƶ
	
	PWMA_CCER1 = 0x00; // д CCMRx ǰ���������� CCERx �ر�ͨ��
	PWMA_CCMR1 = 0x60; // ���� CC1 Ϊ PWMA ���ģʽ
	PWMA_CCER1 = 0x01; // ʹ�� CC1 CC2ͨ��
	
	PWMA_CCR1H = (u16)(PWM_DUTY_0 >> 8); // ����ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMA_CCR1L = (u16)(PWM_DUTY_0); 
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // ����PWM����
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x01; // ʹ�� PWM1P �˿����
	PWMA_BKR = 0x80; // ʹ�������
	
	PWMA_CR1 = 0x01; // ����PWM��ʱ��
}
void Timer0_Isr(void) interrupt 1
{
	B_10ms = 1;
	keyscan();
}
void Timer0_Init(void)	//10����@12.000MHz
{
	AUXR &= 0x7F;					//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;					//���ö�ʱ��ģʽ
	TL0 = 0xF0;						//���ö�ʱ��ʼֵ
	TH0 = 0xD8;						//���ö�ʱ��ʼֵ
	TF0 = 0;							//���TF0��־
	TR0 = 1;							//��ʱ��0��ʼ��ʱ
	ET0 = 1;							//ʹ�ܶ�ʱ��0�ж�
}
void keyscan()
{
//--------------------���򿪹�-���-------------------------
	if(KEY_LEFT == 0)			//�������
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			keydelay_left++;	//�����ʱ+1��ÿ��10���룩
		}
		if(KEY_LEFT == 0 && keydelay_left == 2)	//������£����Ҽ�ʱ����2�Σ���ʱȥ��ʱ����10-20����֮�䣩
		{
			if(angle < 4) angle++;		//�Ƕ�+1����ת��
			dutychange = 1;						//����ռ�ձȱ�־��1
		}
	}
	if(KEY_LEFT == 1) keydelay_left = 0;			//�ɿ���������������ʱ����
//--------------------���򿪹�-�Ҽ�-------------------------
	if(KEY_RIGHT == 0 )		//�Ҽ�����
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			keydelay_right++;	//�Ҽ���ʱ+1��ÿ��10���룩
		}
		if(KEY_RIGHT == 0 && keydelay_right == 2)		//�Ҽ����£����Ҽ�ʱ����2�Σ���ʱȥ��ʱ����10-20����֮�䣩
		{
			if(angle > 0) angle--;			//�Ƕ�-����ת��
			dutychange = 1;							//����ռ�ձȱ�־��1
		}
	}
	if(KEY_RIGHT == 1) keydelay_right = 0;		//�ɿ���������������ʱ����
}
void Update_duty()													//����ռ�ձȺ���
{

	if(dutychange == 1)												//����ռ�ձȱ�־Ϊ1ʱ
	{
		switch(angle)
		{
			case 0:duty = PWM_DUTY_0;break;				//װ��0��ռ�ձȵ�duty����
			case 1:duty = PWM_DUTY_45;break;			//װ��45��ռ�ձȵ�duty����
			case 2:duty = PWM_DUTY_90;break;			//װ��90��ռ�ձȵ�duty����
			case 3:duty = PWM_DUTY_135;break;			//װ��135��ռ�ձȵ�duty����
			case 4:duty = PWM_DUTY_180;break;			//װ��180��ռ�ձȵ�duty����
			default:break;
		}
		PWMA_CCR1H = (u8)(duty >> 8); 
		PWMA_CCR1L = duty; 											//��������CC1ͨ��ռ�ձ�

		dutychange = 0;													//ռ�ձȸ��±�־��1
	}
}