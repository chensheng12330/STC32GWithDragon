//	@�����ٳ� 2023/02/24
//	PWMʾ���������8·PWM����������8��SG92R���ת����ϵͳʱ��12MHz��PWM��120Ԥ��Ƶ
//	������ƣ�PWM����20���룬0��ռ�ձ�2.5%,45��ռ�ձ�5%,90��ռ�ձ�7.5%,135��ռ�ձ�10%,180��ռ�ձ�12.5%,
//	���1-4��PWMAͨ��PWM1P/2P/3P/4P���ƣ�PWMA�����ϼ���ģʽ��PWMģʽ1
//	���5-6��PWMBͨ��PWM5��6��7��8 ���ƣ�PWMB�����¼���ģʽ��PWMģʽ2
//	���򿪹�-���Ҽ������ƶ��1����ת�����򿪹�-���¼������ƶ��2����ת��
//	���ذ�����P3.2��P3.3�������ƶ��3��P3.4��P3.5�������ƶ��4��
//	����������KEY1��KEY2���ƶ��5��KEY3��KEY4���ƶ��6��KEY5��KEY6���ƶ��7��KEY7��KEY8���ƶ��8��
//	���򿪹�-�м���������/ż���Σ�����8�����ת��0��/90��
//	����ͺ�SG92R�����Ŷ��壺P1.0 P5.4 P1.4 P1.6 P0.0 P0.1 P0.2 P0.3 P0.4
//	���򿪹����Ŷ��壺�ϼ���P7.4 �¼���P7.3 �����P7.2 �Ҽ���P7.1 �м���P7.0
//	���ذ������Ŷ��壺P3.2 P3.3 P3.4 P3.5
//	8�����������Ŷ��壺����1��P2.0 ����2��P2.1 ����3��P2.2 ����4��P2.3 ����5��P2.4 ����6��P2.5 ����7��P2.6 ����8��P2.7
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

sbit KEY_UP 		= P7^4;				//�ϼ���P7.4
sbit KEY_DOWN 	= P7^3;				//�¼���P7.3
sbit KEY_LEFT 	= P7^2;				//�����P7.2
sbit KEY_RIGHT 	= P7^1;				//�Ҽ���P7.1
sbit KEY_MIDDLE = P7^0;				//�м���P7.0

sbit KEY1 = P2^0;							//����1��P2.0
sbit KEY2 = P2^1;							//����2��P2.1
sbit KEY3 = P2^2;							//����3��P2.2
sbit KEY4 = P2^3;							//����4��P2.3
sbit KEY5 = P2^4;							//����5��P2.4
sbit KEY6 = P2^5;							//����6��P2.5
sbit KEY7 = P2^6;							//����7��P2.6
sbit KEY8 = P2^7;							//����8��P2.7

void PWM_Config(void);				//PWM��ʼ�����ú���
void Timer0_Init(void);				//��ʱ��0��ʼ������
void keyscan();								//����ɨ�躯��
void Update_Duty();						//����ռ�ձȺ���

bit B_10ms;										//10ms��ʱ��־
u8 keydelay_left,keydelay_right,keydelay_up,keydelay_down,keydelay_middle;					//���򿪹���ʱ����
u8 key1delay,key2delay,key3delay,key4delay,key5delay,key6delay,key7delay,key8delay;	//����������ʱ����
u8 P32delay,P33delay,P34delay,P35delay;												//P3.2-P3.5������ʱ����
u16 duty1,duty2,duty3,duty4,duty5,duty6,duty7,duty8;					//8�������Ӧ��ռ�ձȱ���
u8 angle1,angle2,angle3,angle4,angle5,angle6,angle7,angle8;		//8�������Ӧ�ĽǶ�λ�ñ���
bit dutyupdate;								//����ռ�ձȱ�־
u8 middlekeynums;							//���򿪹��м�����������

void main(void)
{
	EAXFR = 1; 									// ʹ�ܷ��� XFR
	CKCON = 0x00; 							// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00;								// ���ó������ȴ��������ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����

	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0xFF;		//����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0xFF;		//����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		//����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		//����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		//����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		//����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		//����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	
	duty1 = duty2 = duty3 = duty4 = PWM_DUTY_0;								//���1-4����ʼλ��0��
	duty5 = duty6 = duty7 = duty8 = PWM_PERIOD - PWM_DUTY_0;	//���5-8����ʼλ��0�ȣ�PWMģʽ2�����¼���ģʽ��
	PWM_Config();							//PWM��ʼ�����ú���
	Timer0_Init();						//��ʱ��0��ʼ������
	EA = 1;										//ʹ�����ж�

	while (1)
	{
		Update_Duty();						//ѭ��ִ�и���ռ�ձȺ���
	}
}
void PWM_Config(void)
{
	PWMB_PS = 0xAA;							//PWMB:P0.0-P0.3 
	
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 				//119+1=120Ԥ��Ƶ
	
	PWMB_PSCRH = (u16)(PWM_PSC >> 8);
	PWMB_PSCRL = (u16)(PWM_PSC); 				//119+1=120Ԥ��Ƶ
	
	PWMA_CCER1 = 0x00; // д CCMRx ǰ���������� CCERx �ر�ͨ��
	PWMA_CCER2 = 0x00; 
	PWMA_CCMR1 = 0x60; // ���� CC1 Ϊ PWMģʽ1
	PWMA_CCMR2 = 0x60; // ���� CC2 Ϊ PWMģʽ1
	PWMA_CCMR3 = 0x60; // ���� CC3 Ϊ PWMģʽ1
	PWMA_CCMR4 = 0x60; // ���� CC4 Ϊ PWMģʽ1
	PWMA_CCER1 = 0x11; // ʹ�� CC1 CC2ͨ��
	PWMA_CCER2 = 0x11; // ʹ�� CC3 CC4ͨ��
	
	PWMB_CCER1 = 0x00; // д CCMRx ǰ���������� CCERx �ر�ͨ��
	PWMB_CCER2 = 0x00; 
	PWMB_CCMR1 = 0x70; // ���� CC5 Ϊ PWMģʽ2
	PWMB_CCMR2 = 0x70; // ���� CC6 Ϊ PWMģʽ2
	PWMB_CCMR3 = 0x70; // ���� CC7 Ϊ PWMģʽ2
	PWMB_CCMR4 = 0x70; // ���� CC8 Ϊ PWMģʽ2
	PWMB_CCER1 = 0x11; // ʹ�� CC5 CC6ͨ��
	PWMB_CCER2 = 0x11; // ʹ�� CC7 CC8ͨ��
	
	PWMA_CCR1H = (u8)(PWM_DUTY_0 >> 8); // ����CC1ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMA_CCR1L = (u8)(PWM_DUTY_0); 
	
	PWMA_CCR2H = (u8)(PWM_DUTY_0 >> 8); // ����CC2ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMA_CCR2L = (u8)(PWM_DUTY_0); 
	
	PWMA_CCR3H = (u8)(PWM_DUTY_0 >> 8); // ����CC3ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMA_CCR3L = (u8)(PWM_DUTY_0); 
	
	PWMA_CCR4H = (u8)(PWM_DUTY_0 >> 8); // ����CC4ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMA_CCR4L = (u8)(PWM_DUTY_0); 
	
	PWMB_CCR5H = (u8)((PWM_PERIOD - PWM_DUTY_0) >> 8); // ����CC5ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMB_CCR5L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMB_CCR6H = (u8)((PWM_PERIOD) - PWM_DUTY_0 >> 8); // ����CC6ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMB_CCR6L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMB_CCR7H = (u8)((PWM_PERIOD - PWM_DUTY_0) >> 8); // ����CC7ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMB_CCR7L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMB_CCR8H = (u8)((PWM_PERIOD - PWM_DUTY_0) >> 8); // ����CC8ռ�ձ�ʱ�� ռ�ձ�2.5%
	PWMB_CCR8L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMA_ARRH = (u8)(PWM_PERIOD >> 8); // ����PWMA����20����
	PWMA_ARRL = (u8)(PWM_PERIOD); 
	
	PWMB_ARRH = (u8)(PWM_PERIOD >> 8); // ����PWMB����20����
	PWMB_ARRL = (u8)(PWM_PERIOD); 
	
	PWMA_ENO = 0x55; // ʹ�� PWM1P 2P 3P 4P�˿����
	PWMA_BKR = 0x80; // PWMAʹ�������
	
	PWMB_ENO = 0x55; // ʹ�� PWM5 6 7 8 �˿����
	PWMB_BKR = 0x80; // PWMBʹ�������
	
	PWMA_CR1 = 0x01; // ����PWMA��ʱ�������ϼ���ģʽ
	PWMB_CR1 = 0x11; // ����PWMB��ʱ�������¼���ģʽ
}
void Timer0_Isr(void) interrupt 1
{
	B_10ms = 1;							//10�����ʱ��־
	keyscan();							//ÿ10���룬ִ��һ�ΰ���ɨ��
}
void Timer0_Init(void)		//10����@12.000MHz
{
	AUXR &= 0x7F;						//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;						//���ö�ʱ��ģʽ
	TL0 = 0xF0;							//���ö�ʱ��ʼֵ
	TH0 = 0xD8;							//���ö�ʱ��ʼֵ
	TF0 = 0;								//���TF0��־
	TR0 = 1;								//��ʱ��0��ʼ��ʱ
	ET0 = 1;								//ʹ�ܶ�ʱ��0�ж�
}
void keyscan()						//����ɨ�躯��
{
//--------------------���򿪹�-���-------------------------
	if(KEY_LEFT == 0)
	{
		if(B_10ms == 1)				//10���뵽
		{
			B_10ms = 0;					//����10�����־
			keydelay_left++;		//���򿪹��������ʱ+1��ÿ��10���룩
		}
		if(KEY_LEFT == 0 && keydelay_left == 2)		//������£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle1 < 4) angle1++;								//���1�Ƕ�+1����ת��
			dutyupdate = 1;													//ռ�ձȸ��±�־��1
		}
	}
	if(KEY_LEFT == 1) keydelay_left = 0;				//�ɿ����������������ʱ����
//--------------------���򿪹�-�Ҽ�-------------------------
	if(KEY_RIGHT == 0 )			
	{
		if(B_10ms == 1)				//10���뵽
		{
			B_10ms = 0;					//����10�����־
			keydelay_right++;		//���򿪹��Ҽ�����ʱ+1��ÿ��10���룩
		}
		if(KEY_RIGHT == 0 && keydelay_right == 2)	//������£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle1 > 0) angle1--;								//���1�Ƕ�-1����ת��
			dutyupdate = 1;													//ռ�ձȸ��±�־��1
		}
	}
	if(KEY_RIGHT == 1) keydelay_right = 0;			//�ɿ������������Ҽ���ʱ����
	//--------------------���򿪹�-�м�-------------------------
	if(KEY_MIDDLE == 0 )
	{
		if(B_10ms == 1)				//10���뵽
		{
			B_10ms = 0;					//����10�����־
			keydelay_middle++;	//���򿪹��м�����ʱ+1��ÿ��10���룩
		}
		if(KEY_MIDDLE == 0 && keydelay_middle == 2)//�м����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			middlekeynums++;		//���򿪹��м�����ʱ+1��ÿ��10���룩
			if(middlekeynums % 2 == 1)								//�м����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
			{
				angle1 = angle2 = angle3 = angle4 = 2;	//�����ΰ��£����ת��90��λ��
				angle5 = angle6 = angle7 = angle8 = 2;	
			}
			else if(middlekeynums % 2 == 0)						//ż���ΰ��£����ת��0��λ��
			{
				angle1 = angle2 = angle3 = angle4 = 0;
				angle5 = angle6 = angle7 = angle8 = 0;
			}
			dutyupdate = 1;														//����ռ�ձȱ�־��1
			if(middlekeynums > 99) middlekeynums = 0;	//�������������������99������
		}
	}
	if(KEY_MIDDLE == 1) keydelay_middle = 0;			//�ɿ��м��������м���ʱ����
	//--------------------���򿪹�-�ϼ�-------------------------
	if(KEY_UP == 0)				
	{
		if(B_10ms == 1)				//10���뵽
		{
			B_10ms = 0;					//����10�����־
			keydelay_up++;			//���򿪹��ϼ�����ʱ+1��ÿ��10���룩
		}
		if(KEY_UP == 0 && keydelay_up == 2)					//�ϼ����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle2 < 4) angle2++;			//���2�Ƕ�+1����ת��		
			dutyupdate = 1;								//����ռ�ձȱ�־��1
		}
	}
	if(KEY_UP == 1) keydelay_up = 0;	//�ɿ��ϼ��������ϼ���ʱ����
//--------------------���򿪹�-�¼�-------------------------
	if(KEY_DOWN == 0 )
	{
		if(B_10ms == 1)			//10���뵽
		{													
			B_10ms = 0;				//����10�����־
			keydelay_down++;	//�¼���ʱ+1
		}
		if(KEY_DOWN == 0 && keydelay_down == 2)		//�¼����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle2 > 0) angle2--;					//���2�Ƕ�-1����ת��
			dutyupdate = 1;										//����ռ�ձȱ�־��1
		}
	}
	if(KEY_DOWN == 1) keydelay_down= 0;		//�ɿ��ϼ��������ϼ���ʱ����
		//--------------------P3.2��-------------------------
	if(P32 == 0)
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			P32delay++;				//P3.2������ʱ+1��ÿ��10���룩
		}
		if(P32 == 0 && P32delay == 2)			//P3.2�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩	
		{
			if(angle3 < 4) angle3++;				//���3�Ƕ�+1����ת��	
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(P32 == 1) P32delay = 0;					//�ɿ�P3.2���������ϼ���ʱ����
//--------------------P3.3��-------------------------
	if(P33 == 0 )
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			P33delay++;				//P3.3������ʱ+1��ÿ��10���룩
		}
		if(P33 == 0 && P33delay == 2)			//P3.3�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle3 > 0) angle3--;				//���3�Ƕ�-1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(P33 == 1) P33delay = 0;					//�ɿ�P3.3���������ϼ���ʱ����
		//--------------------P3.4��-------------------------
	if(P34 == 0)
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			P34delay++;				//P3.4������ʱ+1��ÿ��10���룩
		}
		if(P34 == 0 && P34delay == 2)			//P3.4�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩	
		{
			if(angle4 < 4) angle4++;				//���4�Ƕ�+1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(P34 == 1) P34delay = 0;					//�ɿ�P3.4���������ϼ���ʱ����
//--------------------P3.5��-------------------------
	if(P35 == 0 )			
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			P35delay++;				//P3.5������ʱ+1��ÿ��10���룩
		}
		if(P35 == 0 && P35delay == 2)			//P3.5�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle4 > 0) angle4--;				//���4�Ƕ�-1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(P35 == 1) P35delay = 0;					//�ɿ�P3.5���������ϼ���ʱ����
//--------------------KEY1��-------------------------
	if(KEY1 == 0)
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key1delay++;			//KEY1������ʱ+1��ÿ��10���룩
		}
		if(KEY1 == 0 && key1delay == 2)		//KEY1�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle5 < 4) angle5++;				//���5�Ƕ�+1����ת��	
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(KEY1 == 1) key1delay = 0;				//�ɿ�KEY1���������ϼ���ʱ����
//--------------------KEY2��-------------------------
	if(KEY2 == 0 )
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key2delay++;			//KEY2������ʱ+1��ÿ��10���룩
		}
		if(KEY2 == 0 && key2delay == 2)		//KEY2�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle5 > 0) angle5--;				//���5�Ƕ�-1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(KEY2 == 1) key2delay = 0;				//�ɿ�KEY2���������ϼ���ʱ����
	//--------------------KEY3��-------------------------
	if(KEY3 == 0)
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key3delay++;			//KEY3������ʱ+1��ÿ��10���룩
		}
		if(KEY3 == 0 && key3delay == 2)		//KEY3�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩	
		{
			if(angle6 < 4) angle6++;				//���6�Ƕ�+1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(KEY3 == 1) key3delay = 0;				//�ɿ�KEY3���������ϼ���ʱ����
//--------------------KEY4��-------------------------
	if(KEY4 == 0 )
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key4delay++;			//KEY4������ʱ+1��ÿ��10���룩
		}
		if(KEY4 == 0 && key4delay == 2)		//KEY4�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle6 > 0) angle6--;				//���6�Ƕ�-1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(KEY4 == 1) key4delay = 0;				//�ɿ�KEY4���������ϼ���ʱ����
//--------------------KEY5��-------------------------
	if(KEY5 == 0)
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key5delay++;			//KEY5������ʱ+1��ÿ��10���룩
		}
		if(KEY5 == 0 && key5delay == 2)		//KEY5�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle7 < 4) angle7++;				//���7�Ƕ�+1����ת��	
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(KEY5 == 1) key5delay = 0;				//�ɿ�KEY5���������ϼ���ʱ����
//--------------------KEY6��-------------------------
	if(KEY6 == 0 )
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key6delay++;			//KEY6������ʱ+1��ÿ��10���룩
		}
		if(KEY6 == 0 && key6delay == 2)	//KEY6�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle7 > 0) angle7--;			//���7�Ƕ�-1����ת��
			dutyupdate = 1;								//����ռ�ձȱ�־��1
		}
	}
	if(KEY6 == 1) key6delay = 0;			//�ɿ�KEY6���������ϼ���ʱ����
	//--------------------KEY7��-------------------------
	if(KEY7 == 0)
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key7delay++;			//KEY7������ʱ+1��ÿ��10���룩
		}
		if(KEY7 == 0 && key7delay == 2)		//KEY7�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{	
			if(angle8 < 4) angle8++;				//���8�Ƕ�+1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(KEY7 == 1) key7delay = 0;				//�ɿ�KEY7���������ϼ���ʱ����
//--------------------KEY8��-------------------------
	if(KEY8 == 0 )
	{
		if(B_10ms == 1)			//10���뵽
		{
			B_10ms = 0;				//����10�����־
			key8delay++;			//KEY8������ʱ+1��ÿ��10���룩
		}
		if(KEY8 == 0 && key8delay == 2)		//KEY8�����£�����ʱ����2�Σ�����ȥ��ʱ����10-20����֮�䣩
		{
			if(angle8 > 0) angle8--;				//���8�Ƕ�-1����ת��
			dutyupdate = 1;									//����ռ�ձȱ�־��1
		}
	}
	if(KEY8 == 1) key8delay = 0;				//�ɿ�KEY8���������ϼ���ʱ����
}
void Update_Duty()
{
//--------------------����ռ�ձ�-------------------------
	if(dutyupdate == 1)
	{
		switch(angle1)
		{
			case 0:duty1 = PWM_DUTY_0;break;			//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty1 = PWM_DUTY_45;break;			//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty1 = PWM_DUTY_90;break;			//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty1 = PWM_DUTY_135;break;		//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty1 = PWM_DUTY_180;break;		//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		switch(angle2)
		{
			case 0:duty2 = PWM_DUTY_0;break;			//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty2 = PWM_DUTY_45;break;			//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty2 = PWM_DUTY_90;break;			//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty2 = PWM_DUTY_135;break;		//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty2 = PWM_DUTY_180;break;		//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		switch(angle3)
		{
			case 0:duty3 = PWM_DUTY_0;break;			//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty3 = PWM_DUTY_45;break;			//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty3 = PWM_DUTY_90;break;			//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty3 = PWM_DUTY_135;break;		//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty3 = PWM_DUTY_180;break;		//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		switch(angle4)
		{
			case 0:duty4 = PWM_DUTY_0;break;		//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty4 = PWM_DUTY_45;break;		//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty4 = PWM_DUTY_90;break;		//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty4 = PWM_DUTY_135;break;	//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty4 = PWM_DUTY_180;break;	//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		switch(angle5)
		{
			case 0:duty5 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty5 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty5 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty5 = PWM_PERIOD -PWM_DUTY_135;break;		//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty5 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		switch(angle6)
		{
			case 0:duty6 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty6 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty6 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty6 = PWM_PERIOD - PWM_DUTY_135;break;		//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty6 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		switch(angle7)
		{
			case 0:duty7 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty7 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty7 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty7 = PWM_PERIOD - PWM_DUTY_135;break;		//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty7 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		switch(angle8)
		{
			case 0:duty8 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0��װ��0��ռ�ձȵ�duty����
			case 1:duty8 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1��װ��45��ռ�ձȵ�duty����
			case 2:duty8 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2��װ��90��ռ�ձȵ�duty����
			case 3:duty8 = PWM_PERIOD - PWM_DUTY_135;break;		//angle=3��װ��135��ռ�ձȵ�duty����
			case 4:duty8 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4��װ��180��ռ�ձȵ�duty����
			default:break;
		}
		PWMA_CCR1H = (u8)(duty1 >> 8); 
		PWMA_CCR1L = duty1; 						//����CC1ͨ��ռ�ձ�
		
		PWMA_CCR2H = (u8)(duty2 >> 8); 
		PWMA_CCR2L = duty2; 						//����CC2ͨ��ռ�ձ�
		
		PWMA_CCR3H = (u8)(duty3 >> 8); 
		PWMA_CCR3L = duty3; 						//����CC3ͨ��ռ�ձ�
		
		PWMA_CCR4H = (u8)(duty4 >> 8); 
		PWMA_CCR4L = duty4; 						//����CC4ͨ��ռ�ձ�
		
		PWMB_CCR5H = (u8)(duty5 >> 8); 
		PWMB_CCR5L = duty5;							//����CC5ͨ��ռ�ձ�
		
		PWMB_CCR6H = (u8)(duty6 >> 8); 
		PWMB_CCR6L = duty6; 						//����CC6ͨ��ռ�ձ�
		
		PWMB_CCR7H = (u8)(duty7 >> 8); 
		PWMB_CCR7L = duty7; 						//����CC7ͨ��ռ�ձ�
		
		PWMB_CCR8H = (u8)(duty8 >> 8); 
		PWMB_CCR8L = duty8; 						//����CC8ͨ��ռ�ձ�
		dutyupdate = 0;									//����ռ�ձȸ��±�־
	}
}