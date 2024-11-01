//		@�����ٳ� 2022/11/15
//		8λ��ˮ��ʵ��-������ˮ
//		LED8->LED1���������Ϩ�𣬵���ʱ��500ms����ʵ����� ���鷨
//		LED��·��8��LED���������ӣ�ÿ��LED��һ���������衣�����˽�VCC���͵�ƽ����������������.1����LED��·��
//		LED���Ŷ��壺P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		��ʵ�����׼˫���ģʽ���͵�ƽ�������ߵ�ƽϨ��
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  @35MHz
 
#include <STC32G.H>

#define MAIN_Fosc  35000000UL		//������Ƶ 35MHz���밴ʵ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

typedef unsigned char u8;				//unsigned char 		������ u8 ��ʾ
typedef unsigned int u16;				//unsigned int  		������ u16��ʾ
typedef unsigned long int u32;	//unsigned long int ������ u32��ʾ

void delayms(u16 ms);			//��ʱ��������

void main()
{
	u8 LED_Code[8] ={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};	//����P2��LED1-LED8��Ӧ��ʮ������ֵ
	u8 i;
	
	WTST = 0;	//���õȴ�ʱ��Ϊ0��ʱ�ӣ�CPU�����ٶ����
	
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	
	while(1)
	{
		for(i = 0;i < 8 ;i++)			//������ˮ
		{
			P2 = LED_Code[7-i];			//���鷨����LED_Code[7]��ʼȡֵ����ֵ��P2��LED8~LED1���ε���
			delayms(500);
		}
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