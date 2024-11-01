//		@�����ٳ� 20230110
//		IO���ڲ���������������Ӧ�ã������ⲿ�������裬IO��ֱ������LED
//		ʹ��P7.0��ֱ������LED�������ⲿ�������裬��LED��˸
//		��ʵ����ø�������ģʽ
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  @35MHz
 
#include <STC32G.H>


#define MAIN_Fosc  35000000UL		//������Ƶ 35MHz���밴ʵ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

typedef unsigned char u8;				//unsigned char 		������ u8 ��ʾ
typedef unsigned int u16;				//unsigned int  		������ u16��ʾ
typedef unsigned long int u32;	//unsigned long int ������ u32��ʾ

void delayms(u16 ms);			//��ʱ��������

void main()
{
	WTST = 0;			//���õȴ�ʱ��Ϊ0��ʱ�ӣ�CPU�����ٶ����
	
	EAXFR = 1; 		//ʹ��XFR; �����޷�����XFR��PxPD��������	 	//EAXFR:��չSFRʹ�ܼĴ���

	P7M1 = 0xFF;P7M0 = 0x00;		//����P7��Ϊ��������ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	
	while(1)
	{
		P7PU = 0x01;			//��P7.0��������
		delayms(500);
		P7PU = 0x00;			//�ر�P7.0��������
		delayms(500);
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