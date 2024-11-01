
/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Tel: 86-0513-55012928,55012929 ---------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/* ������Ϊ����Ӧ�ó���, �Ѳ���ͨ��, �û������������, ���ṩ����֧��  */
/*---------------------------------------------------------------------*/

/*************	����˵��	**************

�����̻���STC32GΪ����оƬ���б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

��ʾ��������

�����ļ�:
EEPROM.c
OSCOPE-V1-12bit-CDC.c	12bit����


	STC 32λ8051ȫ���ѧ�ƻ�
	������-STC32G12K128��Դʾ����ר��
	2��CAN��USB��32λ8051��
	STC32G12K128���߾�׼12λADC��
	DMA֧�֣�TFT������ADC��4�鴮�ڣ�SPI��I2C��
	STC32G12K128����;����
	0513-55012928��55012929
	������www.STCMCUDATA.com

ʾ��������:
������ѹ: 5.0V��
MCU:      STC32G12K128��
��Ƶ:     35MHz��
ADC:      ��߲���800KHz 12λ����ʾ����ʹ����߲�����500KHz��
ģ�����: 100KHz.
��ʾ��:   ������ʾ480x320��
          ˮƽ16��һ��25�㣬һ��400�㣬�洢���4000�㡣
          ��ֱ10��ÿ��25�㣬һ��250�㡣
ʱ��:     1-2-5������50us 100us 200us 500us 1ms 2ms 5ms 10ms 20ms 50ms
                     100ms 200ms 500ms 1s 2s 5s 10s 20s 50s
��ֱ����: 250mV  500mV   1V  2.5V/DIV��
����ģʽ: �����ش������½��ش�����
������ʽ: �Զ�����׼�����Ρ�

����ѡ��2�������ź�:
1:  ѡ��P1.7����, 2.5V REF ͨ��3.9K+3.9K��ѹ��P1.7, P1.7����18K������ⲿ�����ѹ(���+-12.5V), P1.7�Եؽ�һ��470pF����. �����Ƽ��ķ�ʽ. 
                  ����ͬ���ź�ʹ�õ���IO�ж�, ���������źŸ���Ҫ����+5V�Ż�ͬ��.
2:  ѡ��P1.4����, P1.0ͨ��SPWM���һ��400Hz�����Ҳ�, ��������RC�˲�����P1.4. RC����Ϊ3.3K 10nF, �������Ļ������Ϊ10K����ݲ���22nF.
    ѡ��P1.4����ʱ, ��ʾ�ĵ�ѹ����Ҫ��СΪ1/10.

******************************************/


#include	"inc\stc.h"
#include	"EEPROM.h"
#include	"stc.h"
#include	"usb.h"
#include	"uart.h"


	#define		ADC_CHN		7	//  ѡ��P1.7����, 2.5V REF ͨ��3.9K+3.9K��ѹ��P1.7, P1.7����18K������ⲿ�����ѹ(���+-12.5V), P1.7�Եؽ�һ��470pF����. �����Ƽ��ķ�ʽ.
//	#define		ADC_CHN		4	//  ѡ��P1.4����, P1.0ͨ��SPWM���һ��400Hz�����Ҳ�, ��������RC�˲�����P1.4. RC����Ϊ3.3K 10nF, �������Ļ������Ϊ10K.


/*************	���س�������	**************/
#define		K_BaseTimeUp	0x01
#define		K_BaseTimeDn	0x02
#define		K_VoltageUp		0x03
#define		K_VoltageDn		0x04
#define		K_RUN_STOP 		0x05
#define		K_TrigPhase		0x06
#define		K_TrigMode		0x07
#define		K_ShiftLeft		0x08
#define		K_ShiftRight	0x09
#define		K_WaveUp		0x0A
#define		K_WaveDown		0x0B
#define		K_RtnMssage		0x0C

#define		EE_ADDR		0x000000


/*************	IO������	**************/

//P0.7--T4CLKO: 1000Hz�������
//P1.7--ADC7:   ��������
//P1.0-_PWM1P:  ���SPWM, 100Hz���Ҳ�

	

/*************	���ر�������	**************/
u8	KeyState, KeyCode, KeyHoldCnt;
bit	B_KeyRepeat;

u16	xdata adc_sample[4008];
u8	edata	adc_tmp[410];
u8	edata	tmp[32];
bit	B_ADC_Busy, B_Sample_OK;
u16	adc_wr, adc_rd;
u16	adc;
u8	SampleIndex;
u16	sample_cnt;
bit	B_sample_ms;
u16	SampleTime;
bit	B_Run;
u16	LastDot;
u8	TimeScale;		//ʱ��ϵ��, 10: 10ȡ1, 5: 5ȡ1, 2: 2ȡ1, 1: 1ȡ1
u8	TimeScaleIndex;	//ʱ��ϵ������, 0-->10, 1-->5, 2-->2, 3-->1
bit	B_RUN_REQ;		//����ֹͣ
u16	CenterV;	//��ֱ��ѹ���

bit	B_TrigPhase;	//Ĭ�������ش���
u8	TrigMode;

u8	InputIndex;		//���ȵ�λ
u8	AutoTimeCnt;
u8	AutoTime;

bit	B_Shift;	//��λ��־, ֹͣʱ�л�ʱ������λ
u16	Shift;		//��λ����
u8	Current_TimeBase;
u8	WriteDelay;	//д����ʱ

u8	cnt_1ms, cnt_8ms, cnt_32ms;
bit	B_8ms, B_32ms;
bit	B_32ms_rtn;

u16 	TxCnt;


/*************	���غ�������	**************/
void	ShowOscope(void);
void	RealShowOscope(void);
void	Timer0_config(void);
void	Timer1_config(void);
void	Timer4_Config(void);
void	SetSampleTime(void);
void	ShowVoltage(void);

void	ShowTimeBase(void);	//��ʾʱ��
void	ShowPhase(void);
void	ShowRunStop(void);	//��ʾ����״̬
void	ShowStartTime(void);

void	ADC_config(void);
void	Compare_Config(void);	//�Ƚ�����ʼ��
void	PWMA_config(void);
void	PWMB_config(void);
void	AutoCheck(void);
void 	ShowTrigMode(void);
void	ShowRunStop(void);
void	OscStop(void);

void	TxData255(u8 dat);	//ת���ַ�����
void	TX_write2buff(u8 dat);	//д�뷢�ͻ��壬ָ��+1
void	TrigTx(void);	//��������
void	GPIO_INT_Config(void);


/****************  �ⲿ�����������ⲿ�������� *****************/

/**************************************************************/

//�������� SampleIndex   0     1      2      3      4     5      6     7      8     9     10     11     12     13     14   15   16    17    18    19    20
//����ʱ��/DIV         10us  20us   50us  100us  200us  500us   1ms   2ms    5ms  10ms   20ms   50ms  100ms  200ms  500ms  1s   2s    5s   10s   20s   50s
//��һά3, ��(SampleIndex % 3)������, �ڶ�ά������ϵ��, ����ΪÿN����ȡһ��, ����������ָ2.5��ȡһ��, ���з�����ÿ5��ȡ0 2����.
u8	const 	T_TimeScale[3][4]={	{10,5,2,1},
								{10,5,3,1},
								{10,4,2,1}};
  //����InputIndex            0   1    2    3   4    5
			//ADC                     /16 /6.4 /3.2 /1.6,  ʵ�� *5/80  *5/32 *5/16 *5/8
u8	const T_VoltageScale[6]={ 32,32,  80,  32,  16,  8};	//��ѹ����, adc/n, ���ȵ�λ, 2->2.5V/DIV, 3->1V/DIV, 4->0.5V/DIV, 5->0.25V/DIV
			//����                   2.5V  1V  500m 250m


//========================================================================
void  delay_ms(u16 ms)
{
     u16 i;
	 do
	 {
	 	i = MAIN_Fosc / 6000;
		while(--i)	;
     }while(--ms);
}

/****************  ������ *****************/
void	main(void)
{
	u16	j;

	WTST  = 0;
	CKCON = 0;
    EAXFR = 1;	//���������չ�Ĵ���

	P0M1 = 0;	P0M0 = 0;	//����Ϊ׼˫���
	P1M1 = 0;	P1M0 = 0;	//����Ϊ׼˫���
	P2M1 = 0;	P2M0 = 0;	//����Ϊ׼˫���
	P3M1 = 0;	P3M0 = 0;	//����Ϊ׼˫���
	P4M1 = 0;	P4M0 = 0;	//����Ϊ׼˫���
	P5M1 = 0;	P5M0 = 0;	//����Ϊ׼˫���
	P6M1 = 0;	P6M0 = 0;	//����Ϊ׼˫���
	P7M1 = 0;	P7M0 = 0;	//����Ϊ׼˫���
	
	P0n_push_pull(0x80);	//P0.7 T4CLKO 1000Hz

    P3M0 &= ~0x03;	//����Ϊ����
    P3M1 |= 0x03;
    
	IRC48MCR = 0x80;
	while (!(IRC48MCR & 0x01));

	uart_init();
	usb_init();
	EA = 1;

	delay_ms(500);
	while(DeviceState != DEVSTATE_CONFIGURED)	{	NOP(3);	}
	
	ADC_config();		//ADC��ʼ��
	Compare_Config();	//ģ��Ƚ�����ʼ��
	Timer0_config();	//Timer0��ʼ��, ����
	Timer4_Config();	//Timer4��ʼ��, ���1000Hz����.
	PWMA_config();		//PWM���ƴ�ֱλ�ơ�������ƽ

	CenterV = 2048;
	WriteDelay = 0;
	EEPROM_read_n(EE_ADDR, tmp, 6);
	if(tmp[4] == (tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]) ^ 0x55)	//У��
	{
		CenterV = (u16)tmp[0] * 256 + tmp[1];	SampleIndex = tmp[2];	InputIndex = tmp[3];	TrigMode = tmp[4];
		if(TrigMode & 0x80)	B_TrigPhase = 1;
		else				B_TrigPhase = 0;
		TrigMode &= 0x03;
		if(CenterV > 4000)		CenterV = 2048;		//Ĭ���е�
		if(SampleIndex > 20)	SampleIndex  = 6;	//Ĭ��1ms/div
		if(SampleIndex < 2)		SampleIndex  = 6;	//Ĭ��1ms/div
		if(InputIndex >= 6)		InputIndex   = 3;	//Ĭ��1V/div
		if(InputIndex <= 1)		InputIndex   = 3;	//Ĭ��1V/div
		if(TrigMode >= 3)		TrigMode     = 0;	//Ĭ���Զ�����
		WriteDelay = 0;		//д����ʱ
	}
	else	//У��ûͨ��
	{
		B_TrigPhase = 0;	//Ĭ�������ش���
		CenterV = 2048;	//Ĭ���е�
		SampleIndex  = 6;	//Ĭ��1ms/div
		InputIndex   = 3;	//Ĭ��1V/div
		TrigMode     = 0;	//Ĭ���Զ�����
		WriteDelay   = 250;	//д����ʱ
	}
	

	B_Shift = 0;
	Shift  = 0;
	B_ADC_Busy = 0;
	B_Run = 1;

	ShowTimeBase();		//��ʾˮƽʱ��
	ShowVoltage();		//��ʾ��ֱ��ѹ
	ShowRunStop();		//��ʾֹͣ/����
	ShowPhase();		//��ʾ����ģʽ(��������)
	ShowTrigMode();		//��ʾ������ʽ
	
	KeyCode = 0;
	P_SW2 |= 0x80;		//SFR enable   

	while (1)
	{
		if(B_8ms)	//8ms
		{
			B_8ms = 0;
			AutoCheck();	// ������ʱ����
		}

		if(B_32ms)
		{
			B_32ms = 0;
			if(WriteDelay != 0)		//��ʱд��
			{
				if(--WriteDelay == 0)
				{
					tmp[0] = (u8)(CenterV/256);
					tmp[1] = (u8)(CenterV%256);
					tmp[2] = SampleIndex;
					tmp[3] = InputIndex;
					tmp[4] = TrigMode;
					if(B_TrigPhase)	tmp[4] |= 0x80;
					tmp[5] = (tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]) ^ 0x55;	//У��
					EEPROM_SectorErase(EE_ADDR);
					EEPROM_write_n(EE_ADDR,tmp,6);
				}
			}
		}
				
		if(B_Run)
		{
			if(!B_ADC_Busy)		//����һ�β�������
			{
				B_Sample_OK = 0;
				adc_wr = 0;
				adc_rd = 0;
					 if(SampleIndex <= 2)	TimeScale = 1,	TimeScaleIndex = 3;	// 50us/DIV		���ȡ	ֹͣʱ2����ڲ�9���10������
				else if(SampleIndex == 3)	TimeScale = 2,	TimeScaleIndex = 2;	// 100us/DIV	2ȡ1	ֹͣʱ2����ڲ�4���5������
				else if(SampleIndex == 4)	TimeScale = 4,	TimeScaleIndex = 1;	// 200us/DIV	4ȡ1	ֹͣʱ2����ڲ�2.5���2.5������
				else 						TimeScale = 10,	TimeScaleIndex = 0;	// >=500us/DIV	10ȡ1
				Current_TimeBase = SampleIndex;
				B_ADC_Busy = 1;
				SetSampleTime();	//���ò���ʱ��
				if(B_TrigPhase)	CMPCR1 = 0x80 + 0x20;	// �Ƚ����������жϴ���
				else			CMPCR1 = 0x80 + 0x10;	// �Ƚ����½����жϴ���
					 if(SampleIndex <= 6)	CMPCR2 = 10;	// <=1ms/DIV (<=4us/sample)			�Ƚ���û�л��ͷ���ʱ������3��, ����������ٶ�
				else if(SampleIndex <= 8)	CMPCR2 = 30;	// 2ms 5ms/DIV (8us 20us/sample)	�Ƚ����л��ͷ���ʱע�͵���3��.
				else 						CMPCR2 = 63;	// 2ms 5ms/DIV (8us 20us/sample)
			}
			else if(SampleIndex <= 10)	// <=20ms/DIV
			{
				if(B_Sample_OK)		//�������
				{
					B_Sample_OK = 0;
					B_ADC_Busy  = 0;
					while(!B_32ms_rtn)	{	NOP(5);	}	//USB�ϴ��ٶ�̫�죬���������ϴ��ٶ�
					B_32ms_rtn = 0;
					ShowOscope();	//��ʾ����
					if(B_RUN_REQ || (TrigMode == 2))	OscStop();	//����ֹͣ �� ����ģʽ ��ֹͣ����
				}
			}

			else if(B_Sample_OK)	//50ms/DIV���ϲ���, ʵʱˢ��
			{
				B_Sample_OK = 0;
				RealShowOscope();	//ʵʱ��ʾ����
				if(adc_rd >= 4000)	//����
				{
					B_ADC_Busy = 0;
					if(B_RUN_REQ || (TrigMode == 2))	OscStop();	//����ֹͣ �� ����ģʽ ֹͣ����
				}
			}
		}
		else	B_ADC_Busy = 0;

		if (RxFlag)                         //��RxFlagΪ1ʱ,��ʾ�ѽ��յ�CDC��������
											//���յ����ݴ�С������RxCount����,ÿ����������64�ֽ�
											//���ݱ�����RxBuffer������
		{
			if((RxCount == 4) && (RxBuffer[0]==0xfe) &&(RxBuffer[1] == 0x01) && (RxBuffer[3] == 0xff))
			KeyCode = RxBuffer[2];			//PC�´������룺FE 01 DAT0 FF
			uart_recv_done();               //�Խ��յ����ݴ�����ɺ�,һ��Ҫ����һ���������,�Ա�CDC������һ�ʴ�������
		}

		if(KeyCode)	//�м�����
		{
			if(KeyCode == K_RUN_STOP)		//����/ֹͣ
			{
				if(!B_Run)			//��ֹͣ������
				{
					if(InputIndex >= 8)	InputIndex = 7;
					ShowVoltage();
					B_Run = 1;		//��������
					B_RUN_REQ = 0;
					ShowRunStop();
					B_Shift = 0;
					Shift  = 0;
				}
				else	//������
				{
					if(TrigMode != 0)			OscStop();	//��׼ģʽ ����ģʽ, ��ֱ��ֹͣ
					else if(SampleIndex >= 11)	OscStop();	//ɨ��ģʽ, ��ֱ��ֹͣ
					else	B_RUN_REQ = 1;	//������, ��ֹͣ����, ������ֹͣ
				}
			}

			else if(KeyCode == K_TrigPhase)	//�л������� �½��ش���
			{
				B_TrigPhase = ~B_TrigPhase;
				ShowPhase();
				B_ADC_Busy = 0;		//�л�����ģʽ������������(���û��ֹͣ�Ļ�)
				WriteDelay = 250;	//д����ʱ8��
			}

			else if(KeyCode == K_TrigMode)	//�л�������ʽ
			{
				if(++TrigMode >= 3)	TrigMode = 0;	//0: �Զ�, 1:��ͨ(��׼), 2:����
				ShowTrigMode();
				B_ADC_Busy = 0;		//�л�����ģʽ������������(���û��ֹͣ�Ļ�)
				WriteDelay = 250;	//д����ʱ8��
			}

			else if(KeyCode == K_BaseTimeUp)	// ʱ��+
			{
				if(SampleIndex < 20)
				{
					if(B_Run)	//�����иı�ʱ��, ������ı������
					{
						SampleIndex++;
						B_ADC_Busy = 0;
						ShowTimeBase();	//��ʾʱ��
						WriteDelay = 250;	//д����ʱ8��
					}
					else if(TimeScaleIndex != 0)	//ֹͣʱ����ʱ����ˢ�²���
					{
						SampleIndex++;
						TimeScaleIndex--;	//ֹͣʱ����
						Shift = Shift * TimeScale;
						TimeScale = T_TimeScale[Current_TimeBase%3][TimeScaleIndex];	//ʱ��ϵ������, 0-->10, 1-->5, 2-->2, 3-->1
						Shift = Shift / TimeScale;
						if(TimeScaleIndex == 0)	Shift = 0;
						ShowTimeBase();	//��ʾʱ��
						ShowStartTime();	//��ʾ������ʼʱ��
						ShowOscope();		//ֹͣ��ˢ�²���
						WriteDelay = 250;	//д����ʱ8��
					}
				}
			}

			else if(KeyCode == K_BaseTimeDn)		// ʱ��-
			{
				if(SampleIndex > 2)
				{
					if(B_Run)	//�����иı�ʱ��, ������ı������
					{
						SampleIndex--;
						B_ADC_Busy = 0;
						ShowTimeBase();	//��ʾʱ��
						WriteDelay = 250;	//д����ʱ8��
					}
					else if(TimeScaleIndex < 3)	//ֹͣʱ����ʱ����ˢ�²���
					{
						SampleIndex--;
						TimeScaleIndex++;	//ֹͣʱ����
						Shift = Shift * TimeScale;
						TimeScale = T_TimeScale[Current_TimeBase%3][TimeScaleIndex];	//ʱ��ϵ������, 0-->10, 1-->5, 2-->2, 3-->1
						Shift = Shift / TimeScale;
						ShowTimeBase();	//��ʾʱ��
						ShowStartTime();	//��ʾ������ʼʱ��
						ShowOscope();		//ֹͣ��ˢ�²���
						WriteDelay = 250;	//д����ʱ8��
					}
				}
			}

			else if(KeyCode == K_VoltageUp)	//��ֱ���� +
			{
				if(B_Run)	//������
				{
					if(InputIndex > 2)	//������ֱ����
					{
						InputIndex--;
						ShowVoltage();
						WriteDelay = 250;	//д����ʱ8��
					}
				}
			}
			else if(KeyCode == K_VoltageDn)	// ��ֱ���� -
			{
				if(B_Run)	//������
				{
					if(InputIndex < 5)	//������ֱ����
					{
						InputIndex++;
						ShowVoltage();
						WriteDelay = 250;	//д����ʱ8��
					}
				}
			}

			else if(KeyCode == K_WaveUp)	//��������
			{
				CenterV -= (T_VoltageScale[InputIndex]+3)/5;	//���ݷ��ȵ�λȡ����ϵ��
				if(CenterV >= 4000)	CenterV = 0;
				if(B_Run)	WriteDelay = 250;	//������, д����ʱ8��
				else		ShowOscope();		//ֹͣʱ��ˢ�²���
			}
			else if(KeyCode == K_WaveDown)	//��������
			{
				CenterV += (T_VoltageScale[InputIndex]+3)/5;	//���ݷ��ȵ�λȡ����ϵ��
				if(CenterV >= 4000)	CenterV = 4000;
				if(B_Run)	WriteDelay = 250;	//������, д����ʱ8��
				else		ShowOscope();		//ֹͣʱ��ˢ�²���
			}

			else if(KeyCode == K_ShiftLeft)	//��������
			{
				if(!B_Run)	//ֹͣʱ, �����ƶ�����
				{
					if(!B_KeyRepeat)	Shift++;
					else				Shift += 10;
					TimeScale = T_TimeScale[Current_TimeBase%3][TimeScaleIndex];	//ʱ��ϵ������, 0-->10, 1-->5, 2-->2, 3-->1
					if(TimeScale == 3)	j = 8000/5 - 400;
					else				j = 4000/TimeScale - 400;
					if(Shift >= j)	Shift = j;
					ShowStartTime();	//��ʾ������ʼʱ��
					ShowOscope();		//ֹͣ��ˢ�²���
				}
			}
			else if(KeyCode == K_ShiftRight)	//��������
			{
				if(!B_Run)	//ֹͣʱ, �����ƶ�����
				{
					if(!B_KeyRepeat)	{	if(Shift != 0)	Shift--;	}
					else if(Shift >= 10)	Shift -= 10;
					else					Shift = 0;
					TimeScale = T_TimeScale[Current_TimeBase%3][TimeScaleIndex];	//ʱ��ϵ������, 0-->10, 1-->5, 2-->2, 3-->1
					if(TimeScale == 3)	j = 8000/5 - 400;
					else				j = 4000/TimeScale - 400;
					if(Shift >= j)	Shift = j;
					ShowStartTime();	//��ʾ������ʼʱ��
					ShowOscope();		//ֹͣ��ˢ�²���
				}
			}
			
			else if(KeyCode == K_RtnMssage)	//���ز���
			{
				ShowTimeBase();		//��ʾˮƽʱ��
				ShowVoltage();		//��ʾ��ֱ��ѹ
				ShowRunStop();		//��ʾֹͣ/����
				ShowPhase();		//��ʾ����ģʽ(��������)
				ShowTrigMode();		//��ʾ������ʽ
			}	

			KeyCode = 0;
		}
	}
}
/**********************************************/



/*************** ת���ַ����� *******************************/
void	TxData255(u8 dat)
{
	if(dat >= 0xfb)
	{
		TX_write2buff(0xfd);	//ת���ַ�
		TX_write2buff(dat & 0x7f);
	}
	else	TX_write2buff(dat);
}


/*************** װ�ش��ڷ��ͻ��� *******************************/
void TX_write2buff(u8 dat)	//д�뷢�ͻ��壬ָ��+1
{
	TxBuffer[TxCnt++] = dat;
}

/*************** �������� *******************************/
void	TrigTx(void)	//��������
{
	uart_send(TxCnt);	//��Ҫ���͵����ݱ�����TxBuffer��������, Ȼ�����uart_send(n)�����������ݷ���,����Ϊ���͵��ֽ���. һ�����ɷ���64K,�����ڲ����Զ�����USB�ְ�.
}

/***************** ��ʾ������ʽ���� *****************************/
void ShowTrigMode(void)
{
	TxCnt = 0;
	TX_write2buff(0xfe);		//���ʼ
	TX_write2buff(0x06);		//�ϴ�������ʽ��FE 06 DATA0 FF
	TX_write2buff(TrigMode);	//������ʽ��0--�Զ���1--��ͨ����׼����2--���Ρ�
	TX_write2buff(0xff);		//�������
	TrigTx();	//��������
}

/***************** ��ʾ����״̬���� *****************************/
void	ShowRunStop(void)
{
	TxCnt = 0;
	TX_write2buff(0xfe);		//���ʼ
	TX_write2buff(0x04);		//�ϴ�����ģʽ��FE 04 DATA0 FF
	TX_write2buff((u8)B_Run);	//����ģʽ��0--ֹͣ��1--���С�
	TX_write2buff(0xff);		//�������
	TrigTx();	//��������
}


/***************** ֹͣ���к��� *****************************/
void	OscStop(void)
{
	CMPCR1 = 0x80;		// �رȽ����ж�
	TR0    = 0;			//�رղ�����ʱ��1
	B_ADC_Busy = 0;
	B_Run = 0;
	B_RUN_REQ = 0;
	AutoTimeCnt = 0;
	ShowRunStop();
	Shift = 0;			//ֹͣ�������λ����
	ShowStartTime();	//��ʾ������ʼʱ��

	if(Current_TimeBase == 4)	//200us/DIV
	{
		Current_TimeBase = 5;
		TimeScaleIndex   = 1;
	}
	else if(Current_TimeBase == 3)	//100us/DIV
	{
		Current_TimeBase = 5;
		TimeScaleIndex   = 2;
	}
	else if(Current_TimeBase == 2)	//50us/DIV
	{
		Current_TimeBase = 5;
		TimeScaleIndex   = 3;
	}
}


          //�������� SampleIndex   0     1      2      3      4     5     6     7      8     9     10     11     12     13   14    15    16    17    18    19    20
          //����ʱ�䵥λ       0.1us 0.1us     us     us     us   10us  10us  10us   0.1ms 0.1ms  0.1ms   1ms   1ms    1ms  10ms  10ms  10ms  0.1s  0.1s  0.1s   1s 
u16 const   StartTimeTable[]= {   4,    8,     2,     4,     8,    2,     4,    8,    2,    4,     8,     2,     4,     8,    2,    4,    8,    2,    4,    8,   2};
          //����ʱ��/DIV        10us  20us   50us  100us  200us  500us   1ms   2ms    5ms  10ms   20ms   50ms  100ms  200ms  500ms   1s   2s    5s   10s   20s   50s
u8	const	StartTimeDanWei[]={   2,    2,     3,     3,     3,    4,     4,    4,    5,    5,     5,     6,     6,     6,    7,    7,    7,    8,    8,    8,   9};
//    DAT2��ʱ�䵥λ��0--1ns��1--10ns, 2--100ns, 3--1us��4--10us, 5--100us, 6--1ms��7--10ms, 8--100ms, 9--1s��

/***************** ��ʾ��ʼʱ�亯�� *****************************/
void ShowStartTime(void)
{
	u16	j;

	TxCnt = 0;
	j = StartTimeTable[SampleIndex] * Shift;
	TX_write2buff(0xfe);	//���ʼ
	TX_write2buff(0x08);	//�ϴ���������ʱ�䣺FE 08 DATA0 DATA1 DATA2 FF
	TxData255((u8)(j >> 8));
	TxData255((u8)j);
	TX_write2buff(StartTimeDanWei[SampleIndex]);	//ʱ�䵥λ: ʱ�䵥λ��0--1ns��1--10ns, 2--100ns, 3--1us��4--10us, 5--100us, 6--1ms��7--10ms, 8--100ms, 9--1s��
	TX_write2buff(0xff);	//�������
	TrigTx();	//��������
}


//����InputIndex               0   1    2    3   4    5
          //ADC                        /16 /6.4 /3.2 /1.6	ʵ�� *5/80  *5/32 *5/16 *5/8
//u8 const T_VoltageScale[6]={ 32,32,  80,  32, 16,   8};	//��ѹ����, adc/n, ���ȵ�λ, 2->2.5V/DIV, 3->1V/DIV, 4->0.5V/DIV, 5->0.25V/DIV
      //����                          2.5V  1V  500m 250m

u8	CalculateV(u16	y)	//�����е��ѹ�ʹ�ֱ���ȼ��㲨�ε�ѹ
{
	u8	i;
	i = T_VoltageScale[InputIndex];	//���ݷ��ȵ�λȡ����ϵ��

	if(y >= CenterV)	//������
	{
		y = (y - CenterV);
		y = (y*5)/i;
		if(y >= 125)	y = 125;	//���
		i = 125 + (u8)y;
	}
	else		//������
	{
		y = (CenterV - y);
		y = (y*5)/i;
		if(y >= 125)	y = 125;	//���
		i = 125 - (u8)y;
	}
	return i;
}


	#define	X_START		40
	#define	Y_START		25
	#define	GridColor	GREEN	//������ɫ
	#define	WaveColor	YELLOW	//������ɫ

/************** ��ʾ���κ��� ( 50us< ���� <=20ms/DIV ��ֹͣ������ʱ��) ********************/
void	ShowOscope(void)
{
	u16	x,j;

	if(TimeScale == 3)
	{
		j = 8000/5 - 400;	//������λ����
		if(Shift >= j)	Shift = j;
		for(j=(Shift*5)/2,x=0; x<400; x++)
		{
			adc_tmp[x] = CalculateV(adc_sample[j]);	//��һ������
			j = j + 2;
			x++;
			adc_tmp[x] = CalculateV(adc_sample[j]);	//��һ������
			j = j + 3;
		}
	}
	else
	{
		j = 4000/TimeScale - 400;	//������λ����
		if(Shift >= j)	Shift = j;
		for(j=Shift*TimeScale,x=0; x<400; x++)
		{
			adc_tmp[x] = CalculateV(adc_sample[j]);	//��һ������
			j = j + TimeScale;
		}
	}
	
	TxCnt = 0;
	TX_write2buff(0xfb);	//�ϴ�һ֡���ݣ�FB DAT0 DAT1 ..... DAT399 FF
	for(x=0; x<400; x++)	TX_write2buff(adc_tmp[x]);
	TX_write2buff(0xff);	//�������
	TrigTx();	//��������
}


/************** ʵʱ��ʾ���κ���(>=50ms/DIV) ********************/
void	RealShowOscope(void)
{
	u16	x;
	u8	i;

	x = adc_rd/10;
	if(x >= 400)	return;	

	i = CalculateV(adc_sample[adc_rd]);	//��һ������
	TxCnt = 0;
	TX_write2buff(0xfc);	//�ϴ�һ�������ݣ�FC DAT0 DAT1 DAT2 FF
	TxData255((u8)(x>>8));	//X��λ�ø��ֽ�
	TxData255((u8)x);		//X��λ�õ��ֽ�
	TX_write2buff(i);			//Y����ֵ(0~250)
	TX_write2buff(0xff);	//�������
	TrigTx();	//��������
	adc_rd += 10;
}


//========================================================================
// ����: void Timer4_Config(void)
// ����: timer3��ʼ������.
// ����: none.
// ����: none.
// �汾: V1.0, 2018-12-20
//========================================================================
void Timer4_Config(void)
{
	P_SW2 |= 0x80;		//SFR enable   
	T4T3M &= 0x0f;		//ֹͣ����, ��ʱģʽ, 12Tģʽ, �����ʱ��

	T4T3M |=  (1<<5);	//1T mode
	T4H = (u8)((65536UL - MAIN_Fosc/2000) /256);	//��P0.7���1KHz����
	T4L = (u8)((65536UL - MAIN_Fosc/2000) %256);

	T3T4PIN = 0x00;		//ѡ��IO, 0x00: T3--P0.4, T3CLKO--P0.5, T4--P0.6, T4CLKO--P0.7;    0x01: T3--P0.0, T3CLKO--P0.1, T4--P0.2, T4CLKO--P0.3;
	T4T3M |=  (1<<4);	//�������ʱ��
	T4T3M |=  (1<<7);	//��ʼ����
}

//========================================================================
// ����: void PWMA_config(void)
// ����: PWMA��ʼ������.
// ����: none.
// ����: none.
// �汾: V1.0, 2022-6-25
//========================================================================
void PWMA_config(void)
{
	u8	ccer1;
	u8	ccer2;
	u8	ps;
	u8	eno;
	u8	PWMA_ISR_En;

	P_SW2 |= 0x80;		//SFR enable   
	PWMA_ENO    = 0;	// IO�����ֹ
	PWMA_IER    = 0;	// ��ֹ�ж�
	PWMA_SR1    = 0;	// ���״̬
	PWMA_SR2    = 0;	// ���״̬
	ccer1 = 0;
	ccer2 = 0;
	ps    = 0;
	eno   = 0;
	PWMA_ISR_En = 0;

	PWMA_PSCRH = 0x00;		// Ԥ��Ƶ�Ĵ���, ��Ƶ Fck_cnt = Fck_psc/(PSCR[15:0}+1), ���ض���PWMƵ�� = SYSclk/((PSCR+1)*(AAR+1)), �������PWMƵ�� = SYSclk/((PSCR+1)*(AAR+1)*2).
	PWMA_PSCRL = 4;
	PWMA_DTR  = 24;			// ����ʱ������, n=0~127: DTR= n T,   0x80 ~(0x80+n), n=0~63: DTR=(64+n)*2T,  
							//				0xc0 ~(0xc0+n), n=0~31: DTR=(32+n)*8T,   0xE0 ~(0xE0+n), n=0~31: DTR=(32+n)*16T,
	PWMA_ARRH   = (u8)(349/256);	// �Զ���װ�ؼĴ���,  ����PWM����
	PWMA_ARRL   = (u8)(349%256);

	PWMA_CCMR1  = 0x68;		// ͨ��ģʽ����, PWMģʽ1, Ԥװ������
	PWMA_CCR1H  = 87/256;	// �Ƚ�ֵ, ����ռ�ձ�(�ߵ�ƽʱ����)
	PWMA_CCR1L  = 87%256;
	ccer1 |= 0x05;			// �����Ƚ����, �ߵ�ƽ��Ч
	ps    |= 0;				// ѡ��IO, 0:ѡ��P1.0 P1.1, 1:ѡ��P2.0 P2.1, 2:ѡ��P6.0 P6.1, 
	eno   |= 0x01;			// IO�������,  bit7: ENO4N, bit6: ENO4P, bit5: ENO3N, bit4: ENO3P,  bit3: ENO2N,  bit2: ENO2P,  bit1: ENO1N,  bit0: ENO1P
//	PWMA_ISR_En|= 0x02;		// ʹ���ж�

	PWMA_ISR_En = 0x01;		// ʹ�ܸ����ж�

	PWMA_CCER1  = ccer1;	// ����/�Ƚ�ʹ�ܼĴ���1
	PWMA_CCER2  = ccer2;	// ����/�Ƚ�ʹ�ܼĴ���2
	PWMA_PS     = ps;		// ѡ��IO
	PWMA_IER    = PWMA_ISR_En;	//��������ͨ��1~4�жϴ���

	PWMA_BKR    = 0x80;		// �����ʹ�� �൱���ܿ���
	PWMA_CR1    = 0x81;		// ʹ�ܼ�����, �����Զ���װ�ؼĴ�������, ���ض���ģʽ, ���ϼ���,  bit7=1:д�Զ���װ�ؼĴ�������(�����ڲ��ᱻ����), =0:ֱ��д�Զ���װ�ؼĴ�����(���ڿ��ܻ��ҵ�)
	PWMA_EGR    = 0x01;		//����һ�θ����¼�, �����������Ԥ��Ƶ������, װ��Ԥ��Ƶ�Ĵ�����ֵ
	PWMA_ENO    = eno;		// ����IO���

	P1n_push_pull(0x01);	//P1.0 ����Ϊ�������
}
//	PWMA_PS   = (0<<6)+(0<<4)+(0<<2)+0;	//ѡ��IO, 4��Ӹߵ���(������)��ӦPWM1 PWM2 PWM3 PWM4, 0:ѡ��P1.x, 1:ѡ��P2.x, 2:ѡ��P6.x, 
//  PWMA_PS    PWM4N PWM4P    PWM3N PWM3P    PWM2N PWM2P    PWM1N PWM1P
//    00       P1.7  P1.6     P1.5  P1.4     P1.3  P5.4     P1.1  P1.0
//    01       P2.7  P2.6     P2.5  P2.4     P2.3  P2.2     P2.1  P2.0
//    02       P6.7  P6.6     P6.5  P6.4     P6.3  P6.2     P6.1  P6.0
//    03       P3.3  P3.4      --    --       --    --       --    --


//========================================================================
// ����: void PWMA_ISR(void) interrupt PWMA_VECTOR
// ����: PWMA�жϴ������. ��������ͨ�� TIM1-> CCRnH / TIM1-> CCRnL ��ȡ
// ����: None
// ����: none.
// �汾: V1.0, 2021-6-1
//========================================================================
#include	"SineTable.h"
u8	SineIndex;
void PWMA_ISR(void) interrupt PWMA_VECTOR
{
	u8	sr1;

	sr1 = PWMA_SR1;	//Ϊ�˿���, �жϱ�־��һ���ֲ���������
	PWMA_SR1 = 0;	//����жϱ�־
	PWMA_SR2 = 0;	//����жϱ�־
//	sr1 &= PWMA_ISR_En;	//ÿ��ͨ�����Ե��������жϴ���
	if(sr1 & 0x01)	//�����жϱ�־
	{
		PWMA_CCR1L = T_SIN[SineIndex];		//P1.0���100Hz���Ҳ�
		if(++SineIndex == 50)	SineIndex = 0;

		if(++cnt_1ms == 20)
		{
			cnt_1ms = 0;
			if(++cnt_8ms == 8)
			{
				cnt_8ms = 0;
				B_8ms = 1;
			}
			if(++cnt_32ms == 32)
			{
				cnt_32ms = 0;
				B_32ms = 1;
				B_32ms_rtn = 1;
			}
		}
	}

}

/***************** ADC���ú��� *****************************/
#define ADC_START	(1<<6)	/* �Զ���0 */
#define ADC_FLAG	(1<<5)	/* �����0 */

#define	ADC_SPEED	0		/* 0~15, ADCʱ�� = SYSclk/2/(n+1) */
#define	RES_FMT		(1<<5)	/* ADC�����ʽ 0: �����, ADC_RES: D9 D8 D7 D6 D5 D4 D3 D2, ADC_RESL: D1 D0 0  0  0  0  0  0 */
							/*             1: �Ҷ���, ADC_RES: 0  0  0  0  0  0  D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */

#define CSSETUP		(0<<7)	/* 0~1,  ADCͨ��ѡ��ʱ��      0: 1��ADCʱ��, 1: 2��ADCʱ��,  Ĭ��0(Ĭ��1��ADCʱ��)	*/
#define CSHOLD		(0<<5)	/* 0~3,  ADCͨ��ѡ�񱣳�ʱ��  (n+1)��ADCʱ��, Ĭ��1(Ĭ��2��ADCʱ��)					*/
#define SMPDUTY		10		/* 10~31, ADCģ���źŲ���ʱ��  (n+1)��ADCʱ��, Ĭ��10(Ĭ��11��ADCʱ��)				*/
							/* ADCת��ʱ��: 10λADC�̶�Ϊ10��ADCʱ��, 12λADC�̶�Ϊ12��ADCʱ��. 				*/

void	ADC_config(void)
{
	P1n_pure_input(0x80+0x10);	//����Ҫ��ADC��IO����������
	ADC_CONTR = 0x80 + ADC_CHN;	//ADC on + channel
	ADCCFG = RES_FMT + ADC_SPEED;
	P_SW2 |=  0x80;	//����XSFR
	ADCTIM = CSSETUP + CSHOLD + SMPDUTY;	//28 ADC CLOCK
}
//***********************************************************************************************

//========================================================================
// ����:void	Timer0_config(void)
// ����: timer0��ʼ������.
// ����: noe.
// ����: none.
// �汾: V1.0, 2018-12-20
//========================================================================
void	Timer0_config(void)
{
	TR0 = 0;	//ֹͣ����
	ET0 = 1;	//�����ж�
	PT0 = 1;	//�����ȼ��ж�
	TMOD &= ~0x03;
//	TMOD |= 0;	//����ģʽ, 0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
	Timer0_1T();	//1T mode
//	TR0 = 1;	//��ʼ����
}

//========================================================================
void timer0_ISR (void) interrupt TMR0_VECTOR
{
	adc = (u16)ADC_RES * 256 + (u16)ADC_RESL;
	ADC_RES = 0;
	ADC_RESL = 0;
	ADC_CONTR = 0x80 + ADC_START + ADC_CHN;	//ADC on + channel

	if(!B_sample_ms)	//  <=20ms/DIV, ������һ������ʾ
	{
		adc_sample[adc_wr] = adc;
		if(++adc_wr >= 4000)	B_Sample_OK = 1, TR0 = 0;
	}

	else if(--sample_cnt == 0)	// >=50ms/DIV ʵʱɨ����ʾ����
	{
		sample_cnt = SampleTime;
		adc_sample[adc_wr] = adc;
		if((adc_wr%10) == 0)	B_Sample_OK = 1;
		if(++adc_wr >= 4000)	B_Sample_OK = 1, TR0 = 0;
	}
}

/************************ �Ƚ������ú��� ****************************/
void	Compare_Config(void)	//�Ƚ�����ʼ��
{
	CMPCR1 = 0;
	CMPCR2 = 63;		//�ȽϽ���仯��ʱ������, 0~63
	CMPCR1 |= (1<<7);	//1: ����Ƚ���,     0:�رձȽ���
	CMPCR1 |= (0<<5);	//1: �����������ж�, 0: ��ֹ
	CMPCR1 |= (0<<4);	//1: �����½����ж�, 0: ��ֹ
	CMPCR1 |= (0<<1);	//1: ����ȽϽ�������IO(P3.4��P4.1),  0: �ȽϽ����ֹ�����IO
	CMPCR2 |= (0<<7);	//1: �Ƚ���������IOȡ��, 0: ��ȡ��
	CMPCR2 |= (0<<6);	//0: �����ڲ�0.1uF�˲�,    1: �ر�

	P_SW2 |=  0x80;	//����XSFR
	CMPEXCFG = (3<<6) | (1<<2) | 3;	// bit7 bi6(�Ƚ���DC��������ѡ��): 0--0mV, 1--10mV, 2--20mV, 3--30mV,  
									// bit2(�������ѡ��): 1: ѡ���ڲ�BandGap��ѹBGv��������, 0: ѡ���ⲿP3.6������
									// bit1 bit0(�������ѡ��): 0--P3.7, 1--P5.0, 2--P5.1, 3--ADCIN(ADC_CONTR��bit3:0ѡ��, ADC_CONTR�����λ��1)

	IP2  |= (1<<5);	//�Ƚ����ж����ȼ����
	IP2H |= (1<<5);

	CMPCR1 = 0x80;			// �رȽ����ж�
//	CMPCR1 = 0x80 + 0x20;	// �Ƚ����������ж�
//	CMPCR1 = 0x80 + 0x10;	// �Ƚ����½����ж�
}

/***************** �Ƚ����жϺ��� *****************************/
void CMP_ISR(void) interrupt CMP_VECTOR		//�Ƚ����жϺ���, ��⵽���綯�ƹ�0�¼�
{
	ADC_RES = 0;
//	ADC_RESL = 0;
	ADC_CONTR = 0x80 + ADC_START + ADC_CHN;	//ADC on + channel
	TR0 = 1;		//��ʼ����Timer1.

	AutoTimeCnt = 0;	//��ͬ���ź�, ��ȡ����ʱ����
	adc_wr  = 0;
	adc_rd  = 0;

	CMPCR1 = 0x80;		// �رȽ����ж� ��������жϱ�־λ
}

//================= ������ʱ���� ========================
void	AutoCheck(void)
{
	if(AutoTimeCnt != 0)		//8ms call
	{
		if(--AutoTimeCnt == 0)	//ͬ��������ʱ, ��ǿ������
		{
			ADC_RES = 0;
			ADC_RESL = 0;
			ADC_CONTR = 0x80 + ADC_START + ADC_CHN;	//ADC on + channel
			TR0 = 1;		//��ʼ����Timer1.

			CMPCR1 = 0x80;		// �رȽ����ж�
			adc_wr  = 0;
			adc_rd  = 0;
		}
	}
}




#define	D_MAIN_CLOCK	35

          //�������� SampleIndex   0     1      2      3      4     5      6     7      8     9     10     11     12     13     14   15   16    17    18    19    20
          //����ʱ�䵥λ          us    us     us     us     us    us     us    us     us    us     us     ms     ms     ms     ms   ms   ms    ms    ms    ms    ms 
u16	const   SampleTimeTable[]={   2,    2,     2,     2,     2,     2,    4,    8,    20,   40,    80,   200,   400,   800,     2,   4,    8,   20,   40,   80,  200};
          //����ʱ��/DIV        10us  20us   50us  100us  200us  500us   1ms   2ms    5ms  10ms   20ms   50ms  100ms  200ms  500ms   1s   2s    5s   10s   20s   50s
u16	const   T_Time_div[]=     {  10,   20,    50,   100,   200,   500,    1,    2,     5,   10,    20,    50,   100,   200,   500,   1,    2,    5,   10,   20,   50};
u8	const   T_Time_DanWei[]=  {   1,    1,     1,     1,     1,     1,    2,    2,     2,    2,     2,     2,     2,     2,     2,   3,    3,    3,    3,    3,    3};

/***************** ���ò���ʱ�亯�� *****************************/
void	SetSampleTime(void)
{
	u16	j;

		 if(SampleIndex <= 5)	ADCCFG = RES_FMT + 0;	// ADC CLOCK = 20MHz @2us/sample,  ADC CLOCK = SYSclk/2/(n+1), ת����Ҫ 28 ADC CLOCK
	else if(SampleIndex == 6)	ADCCFG = RES_FMT + 1;	// ADC CLOCK = 10MHz @4us/sample,  ADC CLOCK = SYSclk/2/(n+1)
	else if(SampleIndex == 7)	ADCCFG = RES_FMT + 3;	// ADC CLOCK =  5MHz @8us/sample,  ADC CLOCK = SYSclk/2/(n+1)
	else 						ADCCFG = RES_FMT + 9;	// ADC CLOCK =  2MHz >=20us/sample, ADC CLOCK = SYSclk/2/(n+1)

		 if(SampleIndex <= 8)	AutoTime = 100/8;	//ͬ����ʱ100ms	   <=5ms/div		<=60ms/frme
	else if(SampleIndex == 9)	AutoTime = 200/8;	//ͬ����ʱ200ms	   10ms/div			==120ms/frme
	else if(SampleIndex == 10)	AutoTime = 400/8;	//ͬ����ʱ500ms	   20ms/div			==240ms/frme
	else if(SampleIndex == 11)	AutoTime = 800/8;	//ͬ����ʱ800ms	   50ms/div			==600ms/frme
	else if(SampleIndex == 12)	AutoTime = 1600/8;	//ͬ����ʱ800ms	   100ms/div		==600ms/frme
	else 						AutoTime = 2000/8;	//ͬ����ʱ1200ms   >=200ms/div		>=1200ms/frme

	if(TrigMode == 0)	AutoTimeCnt = AutoTime;	//�Զ�ģʽ

	if(SampleIndex <= 10)	//50us 100us 200us/DIV����2us����, 500us~20ms/DIVʹ��10������
	{
		j = SampleTimeTable[SampleIndex];
		TR0 = 0;	//ֹͣ����
		B_sample_ms = 0;
		TH0 = (u8)((65536UL - j*D_MAIN_CLOCK) / 256);
		TL0 = (u8)((65536UL - j*D_MAIN_CLOCK) % 256);
	}
	
	else if(SampleIndex <= 13)	// 50ms 100ms 200ms/DIVʹ��10������
	{
		j = SampleTimeTable[SampleIndex];
		TR0 = 0;	//ֹͣ����
		SampleTime  = 1;	//������һ���жϲ���һ��
		sample_cnt  = 1;
		B_sample_ms = 1;	//ʵʱɨ����ʾ
		TH0 = (u8)((65536UL - j*D_MAIN_CLOCK) / 256);
		TL0 = (u8)((65536UL - j*D_MAIN_CLOCK) % 256);
	}

	else 		// >=500ms/DIV, ʵʱɨ��, ��ʱ���жϹ̶�Ϊ1ms
	{
		j = SampleTimeTable[SampleIndex];
		TR0 = 0;	//ֹͣ����
		SampleTime  = j;	//һ���������Ӧ���жϴ���
		sample_cnt  = j;
		B_sample_ms = 1;	//ʵʱɨ����ʾ
		TH0 = (u8)((65536UL - 1000*D_MAIN_CLOCK) / 256);	//1ms�ж�
		TL0 = (u8)((65536UL - 1000*D_MAIN_CLOCK) % 256);
	}
}

/***************** ��ʾʱ������ *****************************/
void	ShowTimeBase(void)
{
	TxCnt = 0;
	TX_write2buff(0xfe);	//���ʼ
	TX_write2buff(0x02);	//�ϴ�ʱ������: FE 02 DATA0 DATA1 DATA2 FF
	TX_write2buff((u8)(T_Time_div[SampleIndex] >> 8));
	TxData255((u8)T_Time_div[SampleIndex]);
	TX_write2buff(T_Time_DanWei[SampleIndex]);	//ʱ�䵥λ: 0--ns, 1--us, 2--ms, 3--s
	TX_write2buff(0xff);	//�������
	TrigTx();	//��������
}


	//InputIndex     0   1    2    3     4    5
	// V/DIV                 2.5V  1V  500mV 250mV
u16 const T_VOLTAGE[]={ 10000, 5000, 2500, 1000, 500, 250};

/***************** ��ʾ��ֱ��ѹ��λ V/DIV ���� *****************************/
void	ShowVoltage(void)
{
	TxCnt = 0;
	TX_write2buff(0xfe);	//���ʼ
	TX_write2buff(0x03);	//�ϴ���ֱ���Ȳ���: FE 03 DATA0 DATA1 DATA2 FF
	TX_write2buff((u8)(T_VOLTAGE[InputIndex] >> 8));
	TxData255((u8)T_VOLTAGE[InputIndex]);
	TX_write2buff(0x01);	//��ѹ��λ: mV
	TX_write2buff(0xff);	//�������
	TrigTx();	//��������
}

/***************** ��ʾ����ģʽ���� *****************************/
void	ShowPhase(void)
{
	TxCnt = 0;
	TX_write2buff(0xfe);	//���ʼ
	TX_write2buff(0x05);	//�ϴ�������ʽ: FE 05 DATA0 FF
	if(B_TrigPhase)	TX_write2buff(0x00);	//  B_TrigPhase=1:������, 0:�½���.
	else			TX_write2buff(0x01);	//  DAT0������ģʽ��0--�����أ�1--�½��ء�
	TX_write2buff(0xff);	//�������
	TrigTx();	//��������
}
