//		@�����ٳ� 2023/01/16
//		8λ���������-IO��ֱ������
//		��ʾ20230117
//		ʵ����� ��������+λ�����鷨
//		������ͺţ�3641AS��2��4λ��������ܣ�2�������A-DP������ÿ�μ��������裬A-DP��P20-P27��com1-com8��P00-P07
//		��������Ŷ��壺P20-A P21-B P22-C P23-D P24-E P25-F P26-G P27-DP��
//		��������Ŷ��壺P00-COM8,P01->COM7 P02->COM6 P03->COM5 P04->COM4 P05->COM3 P06->COM2 P07->COM1
//		��ʵ��P2�ڲ����������ģʽ��P0�ڲ���׼˫���ģʽ
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  35MHz
// 
#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//������Ƶ35MHz�������ʵ��ʹ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

typedef unsigned char u8;				//unsigned char 		������ u8 ��ʾ
typedef unsigned int u16;				//unsigned int  		������ u16��ʾ
typedef unsigned long int u32;	//unsigned long int ������ u32��ʾ

void delayms(u16 ms);					//��ʱ��������

u8 SEG_Code[13] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,		//��������ܶ��룺0~9,	
										0x00,0x40,0xFF};	//�����ȫ��10�������-��11���������ȫ����12��
u8 SEG_WeiCode[8] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};					//COM1-COM8λ��
									
void main()
{
	EAXFR = 1;		//ʹ��XFR����
	CKCON = 0;		//�����ⲿ���������ٶ����
	WTST = 0;			//���ó����ȡ�ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����
	
	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0x00;		//����P1��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0xFF;		//����P2��Ϊ�������ģʽ 00��׼˫��� 01��������� 10���������� 11����©���

	while(1)		//��ʾ20230117
	{
			P2 = SEG_Code[2];
			P0 = SEG_WeiCode[0];		//��1λ��ʾ2
			delayms(1);
		
			P2 = SEG_Code[0];
			P0 = SEG_WeiCode[1];		//��2λ��ʾ0
			delayms(1);
		
			P2 = SEG_Code[2];
			P0 = SEG_WeiCode[2];		//��3λ��ʾ2
			delayms(1);
		
			P2 = SEG_Code[3];
			P0 = SEG_WeiCode[3];		//��4λ��ʾ3
			delayms(1);
		
			P2 = SEG_Code[0];
			P0 = SEG_WeiCode[4];		//��5λ��ʾ0
			delayms(1);
		
			P2 = SEG_Code[1];
			P0 = SEG_WeiCode[5];		//��6λ��ʾ1
			delayms(1);
			
			P2 = SEG_Code[1];
			P0 = SEG_WeiCode[6];		//��7λ��ʾ1
			delayms(1);
			
			P2 = SEG_Code[7];
			P0 = SEG_WeiCode[7];		//��8λ��ʾ7
			delayms(1);
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