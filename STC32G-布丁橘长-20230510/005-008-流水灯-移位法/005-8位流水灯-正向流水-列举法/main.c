//		@�����ٳ� 2022/11/15
//		8λ��ˮ��ʵ��-������ˮ
//		LED1-LED8 �������������ʱ��500ms��ѭ������ʵ����� ��λ�� ʵ����ˮ��Ч��
//		LED��·��8��LED���������ӣ�ÿ��LED��һ���������衣�����˽�VCC���͵�ƽ����������������.1����LED��·��
//		LED���Ŷ��壺P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		��ʵ�����׼˫���ģʽ���͵�ƽ�������ߵ�ƽϨ��
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  35MHz
// 
#include <STC32G.H>

#define MAIN_Fosc  35000000UL		//������Ƶ 35MHz���밴ʵ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

#define ON  0				//������LED���͵�ƽ����
#define OFF 1				//������LED���ߵ�ƽϨ��

typedef unsigned char u8;					//unsigned char ������ 		u8 ��ʾ
typedef unsigned int u16;					//unsigned int	������ 		u16��ʾ
typedef unsigned long int u32;		//unsigned long int������ u32��ʾ

sbit LED1 = P2^0;		//���Ŷ��壺LED1��P2.0
sbit LED2 = P2^1;		//���Ŷ��壺LED1��P2.1
sbit LED3 = P2^2;		//���Ŷ��壺LED1��P2.2
sbit LED4 = P2^3;		//���Ŷ��壺LED1��P2.3
sbit LED5 = P2^4;		//���Ŷ��壺LED1��P2.4
sbit LED6 = P2^5;		//���Ŷ��壺LED1��P2.5
sbit LED7 = P2^6;		//���Ŷ��壺LED1��P2.6
sbit LED8 = P2^7;		//���Ŷ��壺LED1��P2.7

void  delayms(u16 ms);		//����ʱ����

void main()
{
	WTST = 0;		//���õȴ�ʱ��Ϊ0��ʱ�ӣ�CPU�����ٶ����
	
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	
	while(1)
	{
		LED1 = ON;				//����LED1
		delayms(500);			//��ʱ500����
		LED1 = OFF;				//Ϩ��LED1
		
		LED2 = ON;				//����LED2
		delayms(500);			//��ʱ500����
		LED2 = OFF;				//Ϩ��LED2
		
		LED3 = ON;				//����LED3
		delayms(500);			//��ʱ500����
		LED3 = OFF;				//Ϩ��LED3
		
		LED4 = ON;				//����LED4
		delayms(500);			//��ʱ500����
		LED4 = OFF;				//Ϩ��LED4
		
		LED5 = ON;				//����LED5
		delayms(500);			//��ʱ500����
		LED5 = OFF;				//Ϩ��LED5
		
		LED6 = ON;				//����LED6
		delayms(500);			//��ʱ500����
		LED6 = OFF;				//Ϩ��LED6
		
		LED7 = ON;				//����LED7
		delayms(500);			//��ʱ500����
		LED7 = OFF;				//Ϩ��LED7
		
		LED8 = ON;				//����LED8
		delayms(500);			//��ʱ500����
		LED8 = OFF;				//Ϩ��LED8
	}
}

void  delayms(u16 ms)		//����ʱ����������Ӧ��Ƶ��1����*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}