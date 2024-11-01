//	@�����ٳ� 2023/03/01
//	PWMʾ������ʹ��PWM1P���PWMʵ�ֺ�����Ч��������PWM1P�л���P2.0����PWM1P_2
//	ʵ�鿪���壺STC32G12K128��������.1 ��Ƶ@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//120��Ƶ
#define PWM_PERIOD 		1000		//����10����

void PWMA_Config(void);				//PWMA��ʼ�����ú���
void Update_duty();						//����ռ�ձȺ���

u16 duty;											//ռ�ձȱ���
bit updateduty;								//ռ�ձȸ��±�־
bit blinkflag;								//������״̬��־

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
	EA = 1;											//ʹ��EA���ж�
	duty = PWM_PERIOD;					//��ʼռ�ձ�100%��LEDϨ��
	while (1)
	{
		Update_duty();						//ѭ��ִ��ռ�ձȸ��º���
	}
}
void PWM_ISR() interrupt 26
{
	if(PWMA_SR1 & 0x01)											//PWM�жϺ󣬲�ѯ�жϱ�־λ�������жϱ�־�Ƿ���1
	{
		PWMA_SR1 &= ~0x01;										//��������жϱ�־λ
		if(blinkflag == 0) duty -= 10;				//�ɰ��������̣�duty-10
		else if(blinkflag == 1) duty += 10;		//�����������̣�duty+10
		if(duty >= 1000) blinkflag = 0;				//���������1000
		else if(duty <= 0) blinkflag = 1;			//�����
		updateduty = 1;												//ռ�ձȸ��±�־��1
	}
}
void PWMA_Config(void)
{
	PWMA_PS = 0x01;													//PWM1P�����л���P2.0	
	
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 						//119+1=120Ԥ��Ƶ
	
	PWMA_CCER1 = 0x00; 											// д CCMRx ǰ���������� CCERx �ر�ͨ��
	PWMA_CCMR1 = 0x60; 											// ���� CC1 Ϊ PWMA ���ģʽ��PWMģʽ1
	PWMA_CCER1 = 0x01; 											// ʹ�� CC1 ͨ��
	
	PWMA_CCR1H = (u16)(PWM_PERIOD >> 8); 		// ���ó�ʼռ�ձ�100%��LEDϨ��
	PWMA_CCR1L = (u16)(PWM_PERIOD); 
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8);			// ����PWM����
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x01; 												// ʹ�� PWM1P �˿����
	PWMA_BKR = 0x80;												// ʹ�������
	
	PWMA_IER = 0x01;												// ʹ�ܸ����ж�
	PWMA_CR1 = 0x01; 												// ����PWM��ʱ��
}
void Update_duty()												//����ռ�ձȺ���
{
	PWMA_CCR1H = (u8)(duty >> 8); 
	PWMA_CCR1L = duty; 											//����CC1ͨ��ռ�ձ�
	updateduty = 0;													//ռ�ձȸ��±�־����
}