//	@�����ٳ� 2023/04/22
//  printf���ڴ�ӡʾ����USART1-ģʽ1����Ƶ22.1184MHz,������115200,8λ���ݷ�ʽ��1λ��ʼλ����У��λ��1λֹͣλ��
//  ����1ʹ��Ĭ������P3.0(RxD) P3.1(TxD)
//	����Ƭ��P3.2���������͸����������ݸ����Զˣ����Զ˷����ַ�'1'-'8'����Ƭ��P2�ڶ�ӦLED��˸3��
//	ʵ�鿪���壺STC32G12K128��������.1 ��Ƶ@22.1184MHz

#include <STC32G.H>
#include <stdio.h>
#include "config.h"
#include "delay.h"

#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// �� 2 ������Ϊ���� Keil ���������Զ�ʵ��������������
																										// ������115200																							
sbit KEY1 = P3^2;							// ����P3.2����
																									
bit busy;											// ����æ��־
char wptr;										// дָ��
char rptr;										// ��ָ��
char buffer[16];							// ���ջ��棬����16
bit B_1ms;										// 1�����־
bit key1flag;									// ����1���±�־
bit blinkflag;								// ��˸��־
char key1delay;								// ����1��ʱ
u8 LEDnums;										// LED��ţ�����P2�ڵڼ���LED��˸

u8 number1;										// ���Ա��������ʹ��� unsigned char ����
char number2;									// ���Ա�������ӡ����	char ����
u16 humidi;										// ���Ա�����ʪ��     unsigned int ����
int temper1;									// ���Ա������¶�1    int  ����
int temper2;									// �����������¶�2    int  ����
float temper3;								// ���Ա������¶�3    float����
float temper4;								// ���Ա������¶�4    float����

u8 str[] = "STC32G12K128-��������.1";	 // �ַ�������

void sysini(void);						// STC32��ʼ������
void Blink(u8 nums);					// P2�ڵ�nums��LED��˸��numsȡֵ1-8
void Timer0_Init(void);				// 1����@11.0592MHz
void UartSend(char dat);			// ���ڷ����ַ�
void UartSendStr(char *p);		// ���ڷ����ַ���
void Uart1Init();							// UART1��ʼ��
void UartPutc(u8 dat);				// ���ڷ����ַ�����
char putchar(char c);					// putchar���������ڴ��ڴ�ӡ

void main(void)
{
	sysini();										// STC32��ʼ������
	Timer0_Init();							// ��ʱ��0��ʼ��
	Uart1Init();								// ����1��ʼ��
	ES = 1;											// ʹ�ܴ���1�ж�
	EA = 1;											// ʹ��EA���ж�
	
	temper1 = 25;								// ���Ա������¶�1 = 25��    	int  ���ͣ�����
	temper2 = -2;								// ���Ա������¶�2 = -2��			int  ���ͣ�����
	temper3 = 25.6;							// ���Ա������¶�3 = 25.6��		float���ͣ�����
	temper4 = -2.5;							// ���Ա������¶�4 = -2.5��		float���ͣ�����
	humidi = 50;								// ���Ա�����ʪ��  = 50 %			unsigned int����
	
	while (1)
	{
		if(KEY1 == 0)							// P3.2��������
		{
			if(key1flag == 0)				// �����±�־Ϊ0ʱ
			{																			
				if(B_1ms == 1)				// 1���뵽
				{	
					B_1ms = 0;					// ����1�����־
					key1delay++;				// ������ʱ,ÿ1����+1
				}					
				if(key1delay >=10)		// ����ʱ�����10����
				{
					key1delay = 0;			// ���㰴����ʱ
					key1flag = 1;				// ���±�־��1
					number1++;													// ÿ�η��ͺ�number1+1
					number2++;													// ÿ�η��ͺ�number2+1
// Keil ��չ�� b h l����Ӧ�ֽڿ�ȣ�b-8λ��h-16λ(Ĭ��)��l-32λ
// printf���з��Ŷ�Ӧ %d���޷��Ŷ�Ӧ%u �� %x�� �����ͣ�%f���ַ��� %s
// u8  ��Ӧ %bu �� %bx��	char ��Ӧ %bd
// u16 ��Ӧ %hu	�� %hx��	int  ��Ӧ %d �� %hd
// u32 ��Ӧ %lu �� %lx��  long ��Ӧ %ld
					printf("��%bu�η���\r\n",number1);	// ����unsigned char���ͣ�number1��	u8	���ͣ���Ҫʹ�� %bu �� %bx 
					printf("��%bd�δ�ӡ\r\n",number2);	// ����					char���ͣ�number2��	char���ͣ���Ҫʹ�� %bd
					printf("ʪ�ȣ�%hu%%\r\n",humidi);		// ����unsigned int	���ͣ�humidi��	u16	���ͣ���Ҫʹ�� %hu �� %hx
					printf("�¶�1��%hd�� ",temper1);			// ����					int	���ͣ�temper��	int	���ͣ���Ҫʹ�� %hd
					printf("�¶�2��%hd�� ",temper2);			// ����					int	���ͣ�temper��	int	���ͣ���Ҫʹ�� %hd
					printf("�¶�3��%4.1f�� ",temper3);				// ����					float	���ͣ�temper��	int	���ͣ���Ҫʹ�� %f��2λ������1λС�� %4.1f
					printf("�¶�4��%3.1f��\r\n",temper4);		// ����					float	���ͣ�temper��	int	���ͣ���Ҫʹ�� %f��1λ������1λС�� %3.1f
					printf("%s\r\n",str);								// �����ַ���"STC32G12K128-��������.1"
					printf("HelloWorld!\r\n");					// �����ַ���"HelloWorld!"
				}
			}
		}
		if(KEY1 == 1)							// �����ɿ�
		{														
			key1flag = 0;						// ���㰴�����±�־
			key1delay = 0;					// ���㰴����ʱ
		}
		if(blinkflag == 1) 				// ��LED��˸��־��1ʱ
		{
			blinkflag = 0; 					// LED��˸��־��1
			if(buffer[0] > 48 && buffer[0] < 57)	LEDnums = buffer[0] - 48;	
			// LED��ŵ��ڽ��յ����ַ���48��������յ��ַ���1����LED���=��1��-48=1;
			Blink(LEDnums);
		}	
	}
}
void sysini()
{
	EAXFR = 1; 									// ʹ�ܷ��� XFR
	CKCON = 0x00; 							// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00;								// ���ó������ȴ��������ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����

	P0M1 = 0x00;P0M0 = 0x00;		// ����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0x00;		// ����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		// ����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		// ����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		// ����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		// ����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		// ����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		// ����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 1�����־
}

void Timer0_Init(void)				// 1����@11.0592MHz
{
	AUXR |= 0x80;								// ��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;								// ���ö�ʱ��ģʽ
	TL0 = 0xCD;									// ���ö�ʱ��ʼֵ
	TH0 = 0xD4;									// ���ö�ʱ��ʼֵ
	TF0 = 0;										// ���TF0��־
	TR0 = 1;										// ��ʱ��0��ʼ��ʱ
	ET0 = 1;										// ʹ�ܶ�ʱ��0�ж�
}

void Uart1Isr() interrupt 4
{
	if (TI)											// �����жϱ�־��1ʱ
	{
		TI = 0;										// ���㷢���жϱ�־
		busy = 0;									// ����æ״̬
	}
	
	if (RI)											// �����жϱ�־��1ʱ
	{
		RI = 0;										// ��������жϱ�־
//		buffer[wptr++] = SBUF;	// ���ճ���16
//		wptr &= 0x0f;						// ���ճ���16
		
		buffer[0] = SBUF;					// ���ճ���1
		blinkflag = 1;						// ��˸��־��1
	}
}
void Uart1Init()							// UART1��ʼ��
{
	/*
	S1_S1 = 0;
	S1_S0 = 1;									// ����1�л���P3.6(RxD) P3.7(TxD)
	*/
	SCON = 0x50;								// ģʽ1��8λ���ݣ�������ʹ��
	T2L = BRT;						
	T2H = BRT >> 8;							// �����ʶ�Ӧ����װ��ֵ
	S1BRT = 1;									// ��ʱ��2�������ʷ�����
	T2x12 = 1;									// 1Tģʽ
	T2R = 1;										// ������ʱ��2
	wptr = 0x00;								// ����дָ��
	rptr = 0x00;								// �����ָ��
	busy = 0;										// ����æ��־
}
void UartPutc(u8 dat)					// ���ڷ����ַ�����
{
	SBUF = dat;									// �����ַ�dat
	while(!TI);									// �ȴ��������
	TI = 0;											// ���㷢����ɱ�־
}
char putchar(char c)					// printf��Ҫ����putchar���������ﹹ��һ��ʹ�ô��ڷ����ַ���putchar����
{															// Ҳ���Բ��ع�putchar��ֱ��ʹ��keil�Դ��ģ�
															// ��ʹ��printfǰ��Ҫ��ѯTI״̬��TIΪ1���ܷ��ͣ��������Ҳ��Ҫ����TI
	UartPutc(c);								// ���ڷ����ַ�
	return c;										// �����ַ�
}
void Blink(u8 nums)						// P2�ڵ�nums��LED��˸��numsȡֵ1-8
{
	u8 i;
	for(i = 0;i < 3;i++)				// ��˸3��
	{
		P2 = ~(0x01 << (nums-1));	// ������nums��LED					
		delayms(200);							// ��ʱ200ms
		P2 = 0xFF;								// Ϩ������			
		delayms(200);							// ��ʱ200ms
	}
}