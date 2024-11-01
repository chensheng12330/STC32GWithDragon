//	@�����ٳ� 2023/03/01
//	PWMʾ������PWM1P�л���P2.0����PWM1P_2�����򿪹����Ҽ�������/��СP2.0LED������
//	ʵ�鿪���壺STC32G12K128��������.1 ��Ƶ@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//120��Ƶ
#define PWM_PERIOD 		1000	//����10����

sbit KEY_UP 		= P7^4;				//�ϼ���P7.4
sbit KEY_DOWN 	= P7^3;				//�¼���P7.3
sbit KEY_LEFT 	= P7^2;				//�����P7.2
sbit KEY_RIGHT 	= P7^1;				//�Ҽ���P7.1
sbit KEY_MIDDLE = P7^0;				//�м���P7.0

void PWMA_Config(void);				//PWMA��ʼ�����ú���
void Update_duty();						//����ռ�ձȺ���
void keyscan();								//����ɨ�躯��
void Timer0_Init(void);				//��ʱ��0��ʼ������

u16 duty;											//ռ�ձȱ���
bit dutyupdate;								//ռ�ձȸ��±�־
bit B_10ms;										//10�����ʱ��־
u8 keydelay_left;							//�����ʱ����
u8 keydelay_right;						//�Ҽ���ʱ����

void main(void)
{
	EAXFR = 1; 									// ʹ�ܷ��� XFR
	CKCON = 0x00; 							// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00;								// ���ó������ȴ��������ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����

	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0x00;		//����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		//����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		//����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		//����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		//����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		//����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	
	PWMA_Config();							//PWMA��ʼ������
	Update_duty();							//����ռ�ձ�
	Timer0_Init();							//��ʱ��0��ʼ��
	EA = 1;											//ʹ��EA���ж�
	duty = PWM_PERIOD;					//��ʼռ�ձ�100%��LEDϨ��
	while (1)
	{
		Update_duty();						//ѭ��ִ��ռ�ձȸ��º���
	}
}
void Timer0_Isr(void) interrupt 1
{
	B_10ms = 1;									//10�����ʱ��־
	keyscan();									//ÿ10���룬ִ��һ�ΰ���ɨ��
}
void Timer0_Init(void)				//10����@12.000MHz
{
	AUXR &= 0x7F;								//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;								//���ö�ʱ��ģʽ
	TL0 = 0xF0;									//���ö�ʱ��ʼֵ
	TH0 = 0xD8;									//���ö�ʱ��ʼֵ
	TF0 = 0;										//���TF0��־
	TR0 = 1;										//��ʱ��0��ʼ��ʱ
	ET0 = 1;										//ʹ�ܶ�ʱ��0�ж�
}
void PWMA_Config(void)
{
	PWMA_PS = 0x01;											//PWM1P�����л���P2.0
	
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 				//119+1=120Ԥ��Ƶ
	
	PWMA_CCER1 = 0x00; 									// д CCMRx ǰ���������� CCERx �ر�ͨ��
	PWMA_CCMR1 = 0x60; 									// ���� CC1 Ϊ PWMA ���ģʽ��PWMģʽ1
	PWMA_CCER1 = 0x01; 									// ʹ�� CC1 CC2ͨ��
	
	PWMA_CCR1H = (u16)(PWM_PERIOD >> 8);// ���ó�ʼռ�ձ�100
	PWMA_CCR1L = (u16)(PWM_PERIOD); 
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // ����PWM����
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x01; 										// ʹ�� PWM1P �˿����
	PWMA_BKR = 0x80;										// ʹ�������
	
	PWMA_CR1 = 0x01; 										// ����PWM��ʱ��
}
void Update_duty()										//����ռ�ձȺ���
{
	PWMA_CCR1H = (u8)(duty >> 8); 
	PWMA_CCR1L = duty; 									//����CC1ͨ��ռ�ձ�
	dutyupdate = 0;											//����ռ�ձȱ�־����
}
void keyscan()
{
//--------------------���򿪹�-���-------------------------
	if(KEY_LEFT == 0)
	{
		if(B_10ms == 1)										//10���뵽
		{
			B_10ms = 0;											//����10�����־
			keydelay_left++;								//���򿪹��������ʱ+1��ÿ��10���룩
		}
		if(KEY_LEFT == 0 && keydelay_left == 2)		//������£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(duty < 1000) duty += 100;
			dutyupdate = 1;									//ռ�ձȸ��±�־��1
		}
	}
	if(KEY_LEFT == 1) keydelay_left = 0;//�ɿ����������������ʱ����
//--------------------���򿪹�-�Ҽ�-------------------------
	if(KEY_RIGHT == 0 )			
	{
		if(B_10ms == 1)										//10���뵽
		{
			B_10ms = 0;											//����10�����־
			keydelay_right++;								//���򿪹��Ҽ�����ʱ+1��ÿ��10���룩
		}
		if(KEY_RIGHT == 0 && keydelay_right == 2)		//������£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(duty > 0) duty -= 100;
			dutyupdate = 1;														//ռ�ձȸ��±�־��1
		}
	}
	if(KEY_RIGHT == 1) keydelay_right = 0;				//�ɿ������������Ҽ���ʱ����
}