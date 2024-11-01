#include <STC32G.H>

#define ON 	0		//������LED���͵�ƽ����
#define OFF 1		//������LED���ߵ�ƽϨ��

#define MAIN_Fosc 12000000UL		//������Ƶ12MHz

typedef unsigned char u8;				//unsigned char			������u8 ��ʾ
typedef unsigned int u16;				//unsigned int			������u16��ʾ
typedef unsigned long int u32;	//unsigned long int	������u32��ʾ

sbit LED1 = P2^0;				//LED1ʹ��P2.0

void delay1(u16 ms);		//delay1 2��forѭ��ʵ��
void delay2(u16 ms);		//delay2 do...whileѭ��ʵ��

void main()
{
	WTST = 0;										//���ö�ȡָ��ȴ�ʱ��Ϊ0��CPU���г����ٶ����
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ
	while(1)
	{
		LED1 = ON;								//����LED1
		delay1(1000);							//��ʱ1000���루�͵�ƽ��ʱ����delay1��ʱ��
		LED1 = OFF;								//Ϩ��LED1
		delay2(1000);							//��ʱ1000���루�ߵ�ƽ��ʱ����delay2��ʱ��
	}
}

void delay1(u16 ms)						//����ʱ����������Ӧ��Ƶ��1����*ms
{
	volatile u16 i;
	volatile u32 j;							//volatile�ؼ��ַ�ֹ����i,j���������Ż�
	for(i = 0; i < ms;i++)
		for(j = 0;j < MAIN_Fosc / 9000;j++);
}

void  delay2(u16 ms)					//����ʱ����������Ӧ��Ƶ��1����*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}