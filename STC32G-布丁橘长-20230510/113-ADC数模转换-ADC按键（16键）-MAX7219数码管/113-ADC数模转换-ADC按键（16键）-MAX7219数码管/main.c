// 	@�����ٳ� 2023/03/27
// 	ADCʾ����16��ADC������MAX7219�������ʾ���ţ�����ͨ��9��P0.0
//  ����������ΪADCͨ�������ţ���������Ϊ��������ģʽ������
//  ����Vref�ο���ѹΪ2.5V��������R114�����Ѿ���Vref����2.5V�ο���ѹ�������ٽ�Vref�������߽ӵ�VCC�����ǶϿ�R114��
//  AMX7219�����ģ�����Ŷ��壺 CS = P6^5; DIN = P6^6; CLK = P6^4; ������MAX7219.h���޸ģ�
//  ʵ�鿪���壺��������.1 @��Ƶ12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"

void SYS_Ini();								// STC32��ʼ������
void SEG_Disp(void);					// �������ʾ
void Timer0_Init(void);				// ��ʱ��0��ʼ��
void ADC_Config();						// ADC��ʼ������
u16 Get_ADC12bitResult();			// ��ȡADC	
void CalculateAdcKey(u16 adc);// �����ֵ

u8  ADC_KeyState,ADC_KeyState1,ADC_KeyState2,ADC_KeyState3; //��״̬
u8  ADC_KeyHoldCnt; //�����¼�ʱ
u8  KeyCode;    //���û�ʹ�õļ���, 1~16��Ч

bit B_1ms;										// 1ms��ʱ��־
u16 adc_result;								// ADCת�����
u8 segdelay;									// �������ʱ����				
u8 adcdelay;									// ADC������ʱ����

void main(void)
{
	SYS_Ini();									// STC32��ʼ������
	ADC_Config();								// ADC��ʼ������
	Timer0_Init();							// ��ʱ��0��ʼ��
	EA = 1;											// ʹ��EA���ж�
	MAX7219_Ini();							// MAX7219��ʼ��
	SEG_Disp();									// �������ʾ
	KeyCode = 0;				
	while (1)
	{
		if(B_1ms == 1)
		{
			B_1ms = 0;
			segdelay++;
			adcdelay++;
		}
		if(adcdelay == 10)
		{
			adc_result = Get_ADC12bitResult(); 
      if(adc_result < 4096)    CalculateAdcKey(adc_result); //���㰴��
			adcdelay = 0;
		}
		if(segdelay == 100)				// ÿ50����ˢ��һ�������
		{	
			SEG_Disp();							// ˢ�������
			segdelay = 0;						// ��������ܼ�ʱ
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
void ADC_Config()
{
	P0M1 = 0x01;P0M0 = 0x00;		// P0.0����Ϊ��������ģʽ
	ADC_CONTR &= 0xF0;					
	ADC_CONTR |= 0x08;					// ����ADCͨ��ΪP0.0
	ADCTIM = 0x3F;							// ADCʱ�����ã�ͨ��ѡ��ʱ�䣺1��ʱ�ӣ�ͨ������ʱ�䣺4��ʱ�ӣ�����ʱ�䣺32��ʱ��
	ADCCFG = 0x2F;							// ADCƵ�����ã�ϵͳʱ��/2/16�������Ҷ���
	ADC_POWER = 1;							// ʹ��ADC
}
//========================================================================
// ����: u16 Get_ADC12bitResult(u8 channel)
// ����: ��ѯ����һ��ADC���.
// ����: channel: ѡ��Ҫת����ADC.
// ����: 12λADC���.
// �汾: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult()  
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_START = 1;							//����ADCת��
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   		//wait for ADC finish
    ADC_FLAG = 0;    						//���ADC������־
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}
/***************** ADC���̼������ *****************************
��·������㷨���: Coody
��ADC���̷����ںܶ�ʵ�ʲ�Ʒ�����, ��֤�����ȶ��ɿ�, ��ʹ����ʹ�õ���Ĥ,���ܿɿ�.
16����,�����ϸ�������Ӧ��ADCֵΪ (4096 / 16) * k = 256 * k, k = 1 ~ 16, �ر��, k=16ʱ,��Ӧ��ADCֵ��4095.
����ʵ�ʻ���ƫ��,���ж�ʱ�������ƫ��, ADC_OFFSETΪ+-ƫ��, ��ADCֵ�� (256*k-ADC_OFFSET) �� (256*k+ADC_OFFSET)֮��Ϊ����Ч.
���һ����ʱ��,�Ͳ���һ��ADC,����10ms.
Ϊ�˱���żȻ��ADCֵ����, ���߱���ADC���������½�ʱ����, ʹ������3��ADCֵ����ƫ�Χ��ʱ, ADCֵ����Ϊ��Ч.
�����㷨, �ܱ�֤�����ǳ��ɿ�.
**********************************************/
#define ADC_OFFSET  120												//ƫ��ֵ����Ϊ��120,
void CalculateAdcKey(u16 adc)
{
    u8  i;
    u16 j;
    
    if(adc < (256-ADC_OFFSET))							// �ų�ADC��0-256��Χʱ�����
    {
      ADC_KeyState = 0;  										// ��״̬��0
      ADC_KeyHoldCnt = 0;										// ������ʱ����
			KeyCode = 0;													// ������0
    }
    j = 256;
    for(i=1; i<=16; i++)										// �Ӱ���1-����16���䣬���ƥ��
    {
        if((adc >= (j - ADC_OFFSET)) && (adc <= (j + ADC_OFFSET)))  break;  //�ж��Ƿ���ƫ�Χ��
        j += 256;
    }
    ADC_KeyState3 = ADC_KeyState2;					// state1��2��3�������ݣ�����������¼����3�εİ������Ƿ���ͬ
    ADC_KeyState2 = ADC_KeyState1;
    if(i > 16)  ADC_KeyState1 = 0;  				//����Ч
    else                        						//����Ч
    {
        ADC_KeyState1 = i;
        if((ADC_KeyState3 == ADC_KeyState2) && (ADC_KeyState2 == ADC_KeyState1) &&
           (ADC_KeyState3 > 0) && (ADC_KeyState2 > 0) && (ADC_KeyState1 > 0))
        {
            if(ADC_KeyState == 0)   				//��һ�μ�⵽
            {
                KeyCode  = i;  							//�������
                ADC_KeyState = i;  					//�����״̬
                ADC_KeyHoldCnt = 0;
            }
						
            if(ADC_KeyState == i)   				//������⵽ͬһ������
            {
                if(++ADC_KeyHoldCnt >= 100) //����1���,��10��ÿ����ٶ�Repeat Key
                {
                    ADC_KeyHoldCnt = 90;
                    KeyCode  = i;   				//�������
                }
            }
            else ADC_KeyHoldCnt = 0; 				//����ʱ�������0
        }
    }
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 1���뵽
}
void Timer0_Init(void)				//1����@12.000MHz
{
	AUXR |= 0x80;								//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;								//���ö�ʱ��ģʽ
	TL0 = 0x20;									//���ö�ʱ��ʼֵ
	TH0 = 0xD1;									//���ö�ʱ��ʼֵ
	TF0 = 0;										//���TF0��־
	TR0 = 1;										//��ʱ��0��ʼ����
	ET0 = 1;										//ʹ�ܶ�ʱ��0�ж�
}
void SEG_Disp(void)						// MAX7219�������ʾ����								
{							
	Write7219(8,15); 						// �����1λ��ʾ Ϩ��
	Write7219(7,15); 						// �����2λ��ʾ Ϩ��
	Write7219(6,15); 						// �����3λ��ʾ Ϩ��
	Write7219(5,15); 						// �����4λ��ʾ Ϩ��
	Write7219(4,15); 						// �����5λ��ʾ Ϩ��
	Write7219(3,15); 						// �����6λ��ʾ Ϩ��
	Write7219(2,(u16)(KeyCode/10)); 	//�����7λ��ʾ ��ֵʮλ
	Write7219(1,(u16)(KeyCode%10)); 	//�����8λ��ʾ ��ֵ��λ
}