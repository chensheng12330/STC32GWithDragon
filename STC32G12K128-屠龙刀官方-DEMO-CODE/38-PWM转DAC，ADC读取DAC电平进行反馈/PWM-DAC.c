/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/


/*************  ����˵��    **************

�����̻���STC32G����ת�Ӱ壨�����������б�д���ԡ�

ʹ�����̲�����Ҫ�޸Ļ�׼��ѹΪVCC����R114�����Ƶ�R113��

��P1.0(PWM1P)���16λ��PWM, �����PWM����RC�˲���ֱ����ѹ��P1.4����ADC�����������������з�����

����1����Ϊ115200,8,n,1, ʹ��P3.0 P3.1�ӿ�ͨ��, ���غ�Ϳ���ֱ�Ӳ��ԡ�ͨ������1����ռ�ձȡ�

��������ʹ��ASCII������֣����磺 10����������ռ�ձ�Ϊ10/256�� 100�� ��������ռ�ձ�Ϊ100/256��

�������õ�ֵΪ0~256, 0Ϊ�����͵�ƽ, 256Ϊ�����ߵ�ƽ��

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��)��

******************************************/

#include "../comm/stc32g.h"   //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "stdio.h"
#include "intrins.h"

#define MAIN_Fosc       22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�

typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;

/****************************** �û������ ***********************************/

#define Baudrate1       115200L
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define PERIOD          255L   //PWM����
#define THRESHOLD       2      //PWM���ڲ�ֵ�ż�

#define UART1_BUF_LENGTH    128     //���ڻ��峤��

/*****************************************************************************/


/*************  ���ر�������    **************/

bit B_1ms;          //1ms��־
u8  cnt200ms;

u8  RX1_TimeOut;
u8  TX1_Cnt;    //���ͼ���
u8  RX1_Cnt;    //���ռ���
bit B_TX1_Busy; //����æ��־
u16 adc;
u16 duty;
u16 feedback;

u8  RX1_Buffer[UART1_BUF_LENGTH];   //���ջ���

void UART1_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void UART1_TxByte(u8 dat);
void UpdatePwm(u16 pwm_value);
u16 Get_ADC12bitResult(u8 channel); //channel = 0~15

/******************** ������ **************************/
void main(void)
{
    u8  i;
    u16 j;
    
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x30;   P1M0 = 0x00;   //���� P1.4,P1.5 Ϊ ADC ��
    
    duty = 128;
    feedback = duty;

    //  Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    //  ADC��ʼ��
    ADCTIM = 0x3f;      //���� ADC �ڲ�ʱ��ADC����ʱ�佨�������ֵ
    ADCCFG = 0x2f;      //���� ADC ת������Ҷ���,ʱ��Ϊϵͳʱ��/2/16
    ADC_CONTR = 0x84;   //ʹ�� ADC ģ��

    PWMA_CCER1 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMA_CCMR1 = 0x60; //���� PWM1 ģʽ1 ���
    PWMA_CCER1 = 0x05; //ʹ�� CC1E ͨ��
    PWMA_CCMR1 |= 0x08; //����Ƚ�Ԥװ��ʹ��(CCxEΪ1�ſ�д)

    PWMA_ARRH = (u8)(PERIOD>>8); //��������ʱ��
    PWMA_ARRL = (u8)PERIOD;
    PWMA_CCR1H = (u8)(duty>>8);
    PWMA_CCR1L = (u8)duty;       //����ռ�ձ�ʱ��

    PWMA_ENO = 0x01; //ʹ�� PWM1P ���
//  PWMA_ENO = 0x02; //ʹ�� PWM1N ���
    PWMA_PS = 0x00;  //�߼� PWM1(P/N) �����ͨ��ѡ��λ, 0x00:P1.0/P1.1, 0x01:P2.0/P2.1, 0x02:P6.0/P6.1
    PWMA_BKR = 0x80; //ʹ�������
    PWMA_CR1 |= 0x01; //��ʼ��ʱ

    UART1_config(2);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
    EA = 1;     //�����ж�

    printf("PWMתDAC���Գ���, ����ռ�ձ�Ϊ 0~256!\r\n");  //UART1����һ���ַ���
    
    while (1)
    {
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            if(++cnt200ms >= 200)   //200ms��һ��ADC
            {
                cnt200ms = 0;
                adc = Get_ADC12bitResult(4);  //����0~15,��ѯ��ʽ��һ��ADC, ����ֵ���ǽ��
                j = (u16)((duty*4096UL)/(PERIOD+1));    //����ADC��������ֵ
                if(j>adc)
                {
                    if(((j-adc)>THRESHOLD) && (feedback<=PERIOD)) feedback++;
                }
                else
                {
                    if(((adc-j)>THRESHOLD) && (feedback>0)) feedback--;
                }
                printf("ADC=%u,duty=%u,feedback=%u\r\n",adc,duty,feedback);
                UpdatePwm(feedback);    //����ռ�ձ�
            }

            if(RX1_TimeOut > 0)     //��ʱ����
            {
                if(--RX1_TimeOut == 0)
                {
                    if((RX1_Cnt > 0) && (RX1_Cnt <= 3)) //����Ϊ3λ����
                    {
                        F0 = 0; //�����־
                        j = 0;
                        for(i=0; i<RX1_Cnt; i++)
                        {
                            if((RX1_Buffer[i] >= '0') && (RX1_Buffer[i] <= '9'))    //�޶�Ϊ����
                            {
                                j = j * 10 + RX1_Buffer[i] - '0';
                            }
                            else
                            {
                                F0 = 1; //���յ��������ַ�, ����
                                printf("����! ���յ�����\xfd���ַ�! ռ�ձ�Ϊ0~%u!\r\n",(u16)PERIOD+1);
                                break;
                            }
                        }
                        if(!F0)
                        {
                            if(j > (PERIOD+1)) printf("����! ����ռ�ձȹ�\xfd��, �벻Ҫ����%u!\r\n",(u16)PERIOD+1);
                            else
                            {
                                duty = j;
                                feedback = j;
                                UpdatePwm(duty);    //����ռ�ձ�
                                printf("����ռ�ձ�=%u.\r\n",duty);
                            }
                        }
                    }
                    else  printf("����! �����ַ���\xfd��! ��\xfd������3λ��\xfd��!\r\n");  //��FD���뺺�����ں������"\xfd"����������ʾ
                    RX1_Cnt = 0;
                }
            }
        }
    }
}

//========================================================================
// ����: void UART1_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2014-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    B_TX1_Busy = 1;
    SBUF = dat;
    while(B_TX1_Busy);
}

char putchar(char c)
{
	UART1_TxByte(c);
	return c;
}

//========================================================================
// ����: void SetTimer2Baudraye(u32 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u32 dat)  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;    //Timer run enable
}

//========================================================================
// ����: void UART1_config(u8 brt)
// ����: UART1��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART1_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    /*********** ������ʹ�ö�ʱ��2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate1);
    }

    /*********** ������ʹ�ö�ʱ��1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;      //S1 BRT Use Timer1;
        T1_CT = 0;      //Timer1 set As Timer
        T1x12 = 1;      //Timer1 set as 1T mode
        TMOD &= ~0x30;  //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
        ET1 = 0;    //��ֹ�ж�
        TR1  = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//  PS  = 1;    //�����ȼ��ж�
    ES  = 1;    //�����ж�
    REN = 1;    //�������
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
//  PCON2 |=  (1<<4);   //�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE

    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;
}

//========================================================================
// ����: void UART1_int (void) interrupt UART1_VECTOR
// ����: UART1�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;    //�����
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;
        B_TX1_Busy = 0;
    }
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0(void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}

//========================================================================
// ����: u16 Get_ADC12bitResult(u8 channel)
// ����: ��ѯ����һ��ADC���.
// ����: channel: ѡ��Ҫת����ADC.
// ����: 12λADC���.
// �汾: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult(u8 channel)  //channel = 0~15
{
    ADC_CONTR = (ADC_CONTR & 0xF0) | 0x40 | channel;    //���� AD ת��
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //���ADC������־
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}

//========================================================================
// ����: UpdatePwm(u16 pwm_value)
// ����: ����PWMֵ. 
// ����: pwm_value: pwmֵ, ���ֵ������ߵ�ƽ��ʱ��.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(u16 pwm_value)
{
    PWMA_CCR1H = (u8)(pwm_value >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR1L = (u8)(pwm_value);
}

