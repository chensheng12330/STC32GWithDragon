//	@�����ٳ� 2023/03/31
//	ADCʾ����̫���ܰ�Ѱ��ϵͳ��2������̨�����򿪹��������ҿ����ֶ�������̨ת�������򿪹��м��Զ�Ѱ��
// 	��Ƭ���ϵ��Զ�Ѱ��һ�Σ�Ѱ�⶯����1����̨��λ�����Ҳ̫࣬���ܰ�б����λ�ã�
//																		2: ��̨��ת���ף�̫���ܰ����ϵ���һ����
//																	  3: ��̨��ת���ף�̫���ܰ����ϵ���һ����
//																		4���ظ�2-3����3��
//																		5: ����Ѱ�⣬��̨ת�����������λ��
//  �������򿪹أ��Ҽ���P7.0 �����P7.2 �Ҽ���P7.1 �����P7.3 �Ҽ���P7.4
//  ����ͺ�SG92R�����1��P1.0�����2��P5.4
//  ADC�������ţ�P0.0
//  ̫���ܰ�5V
//	ʵ�鿪���壺STC32G12K128��������.1 ��Ƶ@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//120��Ƶ
#define PWM_PERIOD 		2000		//����20����
#define PWM_DUTY_0 		50			//ռ�ձ�2.5%
#define PWM_DUTY_45		100			//ռ�ձ�5%	
#define PWM_DUTY_90		150			//ռ�ձ�7.5%
#define PWM_DUTY_135	200			//ռ�ձ�10%
#define PWM_DUTY_180	250			//ռ�ձ�12.5%

sbit KEY_LEFT 	= P7^2;				//�����P7.2
sbit KEY_RIGHT 	= P7^1;				//�Ҽ���P7.1
sbit KEY_UP	= P7^3;						//�����P7.3
sbit KEY_DOWN = P7^4;					//�Ҽ���P7.4
sbit KEY_MIDDLE = P7^0;				//�м���P7.0
void ADC_Config();						// ADC��ʼ������
void PWMA_Config(void);				//PWMA��ʼ�����ú���
void Timer0_Init(void);				//��ʱ��0��ʼ������
void keyscan();								//����ɨ�躯��
void Update_duty();						//����ռ�ձȺ���
u16 ADC_Read();

u8 keydelay_left,keydelay_right,keydelay_up,keydelay_down;	// ������ʱ
u16 duty1,duty2;							// ���ռ�ձ�1��2
bit updateduty1,updateduty2;	// ����ռ�ձȱ�־1��2
u16 adc_result;								// ADCת�����
bit B_10ms;										// 10ms��־ 
u16 searchdelay;							// Ѱ����ʱ		
u16 postion1,postion2;				// ���λ��1��2
u16 adc_result_new;						// ADC�������
u8 searchnums;								// ת������
bit start;										// ��ʼ��־
bit dir;											// ת�������־
bit searchflag;								// Ѱ���־

void main(void)
{
	EAXFR = 1; 									// ʹ�ܷ��� XFR
	CKCON = 0x00; 							// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00;								// ���ó������ȴ��������ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����

	P0M1 = 0xFF;P0M0 = 0x00;		// ����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0xFF;		// ����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		// ����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		// ����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		// ����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		// ����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		// ����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		// ����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	
	duty1 = PWM_DUTY_0;					// ���1��ʼ�Ƕ�0
	duty2 = PWM_DUTY_135;				// ���2��ʼ�Ƕ�135
	PWMA_Config();							// PWMA��ʼ������
	ADC_Config();								// ADC��ʼ������
	Update_duty();							// ����ռ�ձ�
	Timer0_Init();							// ��ʱ��0��ʼ��
	EA = 1;											// ʹ��EA���ж�
	start = 1;
	while (1)
	{
		if(searchflag == 1)				// Ѱ���־��1ʱ
		{
			duty1 = PWM_DUTY_0;			// ���1��ʼ�Ƕ�0
			duty2 = PWM_DUTY_135;		// ���2��ʼ�Ƕ�135
			searchflag = 0;					// ����Ѱ���־
			updateduty1 = 1;				// ����ռ�ձȱ�־1��1
			updateduty2 = 1;				// ����ռ�ձȱ�־2��1
			Update_duty();					// ����ռ�ձ�
			adc_result = 0;					// ����������
			start = 1;							// ��ʼѰ���־��1
		}
		if(start == 1)						
		{
			if(B_10ms == 1)			// 1���뵽
			{
				B_10ms = 0;				// ����1�����־
				searchdelay++;		// Ѱ����ʱ+1
			}
			if(searchdelay == 1)									// 1���뵽
			{
				if(searchnums%2 == 1) dir = 1;			// ת����������������1
				else if(searchnums%2 == 0)dir = 0;	// ת������ż��������0
				if(dir == 0)												// ����0ʱ
				{
					if(duty1<=250)duty1++;						// ���1��ת
				}
				else if(dir == 1)										// ����0ʱ
				{
					if(duty1>= 50)duty1--;						// ���1��ת
				}
				searchdelay = 0;										// ����Ѱ����ʱ
				updateduty1 = 1;										// ����ռ�ձȱ�־1��1
				Update_duty();											// ִ��ռ�ձȸ��º���
				adc_result_new = ADC_Read();				// ��ȡADCת�����
				if(adc_result_new > adc_result) 		// ��ǰ����ֵ������һ�β���ֵ
				{
					adc_result = adc_result_new;			// ��ǰ����ֵ����ֵ����һ�β���ֵ
					postion1 = duty1;									// ������1λ��
					postion2 = duty2;									// ������2λ��
				}
			}
		if(duty1 > 250 || duty1 < 50)						// ���1ת����0�Ȼ�180��λ��
		{
			if(duty1 > 250) duty1 = 250;					// ��ֹ�ظ�����if��
			if(duty1 < 50) duty1 = 50;						// ��ֹ�ظ�����if��
			searchnums++;													// ת������+1
			duty2 -= 30;													// ���2��̫���ܰ壩���ϵ���һ��
			updateduty1 = 1;											// ����ռ�ձȱ�־1��1
			updateduty2 = 1;											// ����ռ�ձȱ�־2��1
			Update_duty();												// ִ��ռ�ձȸ��º���
			if(searchnums == 6)										// ת��6�κ�
			{
				start = 0;													// ���㿪ʼ��־
				searchnums = 0;											// ����ת������
				duty1 = postion1;										// �����λ��1��ֵ�����1ռ�ձ�
				duty2 = postion2;										// �����λ��2��ֵ�����2ռ�ձ�
				updateduty1 = 1;										// ����ռ�ձȱ�־1��1
				updateduty2 = 1;										// ����ռ�ձȱ�־2��1
				Update_duty();											//ִ��ռ�ձȸ��º���
			}
		}
		}
	if(start == 0)	Update_duty();						// ִ��ռ�ձȸ��º���
	}
}
void PWMA_Config(void)
{
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); //Ԥ��Ƶ
	
	PWMA_CCER1 = 0x00;					// д CCMRx ǰ���������� CCERx �ر�ͨ��
	PWMA_CCMR1 = 0x60; 					// ���� CC1Ϊ PWMA ���ģʽ,PWMģʽ1
	PWMA_CCMR2 = 0x60; 					// ���� CC1Ϊ PWMA ���ģʽ,PWMģʽ1
	PWMA_CCER1 = 0x11;					// ʹ�� CC1 CC2ͨ��
	
	PWMA_CCR1H = (u16)(PWM_DUTY_0 >> 8); 
	PWMA_CCR1L = (u16)(PWM_DUTY_0); 		//���ó�ʼռ�ձ�
	
	PWMA_CCR2H = (u16)(PWM_DUTY_135 >> 8); 
	PWMA_CCR2L = (u16)(PWM_DUTY_135); 		//���ó�ʼռ�ձ�
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // ����PWM����
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x05; 						// ʹ�� PWM1P PWM2P�˿����
	PWMA_BKR = 0x80; 						// ʹ�������
	
	PWMA_CR1 = 0x01; 						// ����PWM��ʱ��
}
void ADC_Config()
{
	P0M1 = 0x01;P0M0 = 0x00;		// P0.0����Ϊ��������ģʽ
	ADC_CONTR &= 0xF0;							
	ADC_CONTR |= 0x08;					// ����ADCͨ��ΪP0.0
	ADCTIM = 0x3F;							// ADCʱ�����ã�ͨ��ѡ��ʱ�䣺1��ʱ�ӣ�ͨ������ʱ�䣺4��ʱ�ӣ�����ʱ�䣺32��ʱ��
//	ADCCFG = 0x0F;						// ADCƵ�����ã�ϵͳʱ��/2/16�����������
	ADCCFG = 0x2F;							// ADCƵ�����ã�ϵͳʱ��/2/16�������Ҷ���
	ADC_POWER = 1;							// ʹ��ADC
}
u16 ADC_Read()
{
	u16 adc_res;
	ADC_START = 1;											// ����ADCת��
	while(!ADC_FLAG);										// �ȴ�ת�����
	adc_res = ADC_RES*256 + ADC_RESL;		// ��ȡ����������Ҷ��뷽ʽ��
	ADC_FLAG = 0;												// ����ADCת����ɱ�־
	return adc_res;											// ����ADCת�����
}
void Timer0_Isr(void) interrupt 1
{
	keyscan();									// ����ɨ��
	B_10ms = 1;									// 10�����־
}
void Timer0_Init(void)				//10����@12.000MHz
{
	TM0PS = 0x00;								//���ö�ʱ��ʱ��Ԥ��Ƶ ( ע��:��������ϵ�ж��д˼Ĵ���,������鿴�����ֲ� )
	AUXR &= 0x7F;								//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;								//���ö�ʱ��ģʽ
	TL0 = 0xF0;									//���ö�ʱ��ʼֵ
	TH0 = 0xD8;									//���ö�ʱ��ʼֵ
	TF0 = 0;										//���TF0��־
	TR0 = 1;										//��ʱ��0��ʼ��ʱ
	ET0 = 1;										//ʹ�ܶ�ʱ��0�ж�
}
void keyscan()								//��ʱ��10���룬ɨ��1�ΰ��������ﲻȥ������
{
//--------------------���򿪹�-���-------------------------
	if(KEY_LEFT == 0)						//�������
	{
		if(duty1 < 250) duty1 += 1;	//�Ƕ�+1�����1��ת��
		updateduty1= 1;						//����ռ�ձȱ�־��1
	}
//--------------------���򿪹�-�Ҽ�-------------------------
	if(KEY_RIGHT == 0 )					//�Ҽ�����
	{
		if(duty1 >  50) duty1 -= 1 ;	//�Ƕ�-1�����1��ת��
		updateduty1 = 1;						//����ռ�ձȱ�־��1
	}
//--------------------���򿪹�-�ϼ�-------------------------
	if(KEY_UP == 0)						//�ϼ�����
	{
		if(duty2 < 250) duty2 += 1;	//�Ƕ�+1�����2��ת��
		updateduty2 = 1;						//����ռ�ձȱ�־��1
	}
//--------------------���򿪹�-�¼�-------------------------
	if(KEY_DOWN == 0 )					//�¼�����
	{
		if(duty2 >  50) duty2 -= 1 ;	//�Ƕ�-1�����2��ת��
		updateduty2 = 1;						//����ռ�ձȱ�־��1
	}
//--------------------���򿪹�-�м�-------------------------
	if(KEY_MIDDLE == 0 )					//�м�����
	{
		searchflag = 1;
	}
}
void Update_duty()						//����ռ�ձȺ���
{
	if(updateduty1 == 1)				//����ռ�ձȱ�־Ϊ1ʱ
	{
		PWMA_CCR1H = (u8)(duty1 >> 8); 
		PWMA_CCR1L = duty1; 			//��������CC1ͨ��ռ�ձ�
		updateduty1 = 0;					//����ռ�ձȸ��±�־
	}
		if(updateduty2 == 1)			//����ռ�ձȱ�־Ϊ1ʱ
	{
		PWMA_CCR2H = (u8)(duty2 >> 8); 
		PWMA_CCR2L = duty2; 			//��������CC2ͨ��ռ�ձ�
		updateduty2 = 0;					//����ռ�ձȸ��±�־
	}
}