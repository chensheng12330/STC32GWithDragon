//		@�����ٳ� 2022/11/15
//		8λ��ˮ��ʵ��-��ʽ��ˮ��,10�ֱ仯
//		1��	LED���������м����������ֱ��ȫ��
//		2:	�ٴ��м����������Ϩ��ֱ��ȫ��
//		3:	1��3��5��7�������
//		4:	2��4��6��8���������ֱ��8��ȫ��
//		5:	�����ƺ�ż���ƽ�����˸3��,8��ȫ��
//		6:	LED1����ʼ������ˮ��LED8����ʼ������ˮ������һ�Σ�1-8 8-1����ȫ��
//		7:	���ױ����ƣ�LED1-LED4Ϊһ�飬LED5-LED8Ϊһ�飬���汬��
//		8:	���׵���ɳ©Ч����LED1��ʼ������ˮ����LED8�󱣳ֳ�����������LED1��ʼ����LED7��LED7��LED8����...ֱ��ȫ��
//		9:	LED1��ʼ������ˮ����һ��һ��LED��ˮ���ڶ���2��LED��ˮ...ֱ��6��LED��ˮ
//		10:	���״�ש��Ч����LED3-LED8��ʼ������LED1��ʼ��ˮ����LED3�󣬴�LED3��ʼ������ˮ���ٴ�LED1��ʼ������ˮ����LED4�󣬿�ʼ����...ֱ��LED8����LED1
//		ʵ����� ���鷨
//		LED��·��8��LED���������ӣ�ÿ��LED��һ���������衣�����˽�VCC���͵�ƽ����������������.1����LED��·��
//		LED���Ŷ��壺P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		��ʵ�����׼˫���ģʽ���͵�ƽ�������ߵ�ƽϨ��
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  35MHz
// 
#include <STC32G.H>

#define u8 		unsigned char
#define u16 	unsigned int
#define u32		unsigned long

#define MAIN_Fosc 35000000UL	//������Ƶ35MHz�������ʵ��ʹ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

void delayms(u16 ms);	//��ʱ��������

void LED_Show1();			//LED��ʽ1��������
void LED_Show2();			//LED��ʽ2��������
void LED_Show3();			//LED��ʽ3��������
void LED_Show4();			//LED��ʽ4��������
void LED_Show5();			//LED��ʽ5��������
void LED_Show6();			//LED��ʽ6��������
void LED_Show7();			//LED��ʽ7��������
void LED_Show8();			//LED��ʽ8��������
void LED_Show9();			//LED��ʽ9��������
void LED_Show10();		//LED��ʽ10��������

u8 LED_Code[8] ={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};	//LED1-LED8����ʱ��Ӧ��P2ֵ
u8 sum;

void main()
{
	WTST = 0;	//���õȴ�ʱ��Ϊ0��ʱ�ӣ�CPU�����ٶ����
	
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	
	while(1)
	{
		sum = 0;
		LED_Show1();			//LED��ʽ1- �������м����
		LED_Show2();			//LED��ʽ2- �м�������Ϩ��
		LED_Show3();			//LED��ʽ3- �м�������Ϩ��
		LED_Show4();			//LED��ʽ4- 1��3��5��7������������ֵ���״̬
		LED_Show5();			//LED��ʽ5- 2��4��6��8������������ֵ���״̬,ֱ��8��ȫ��
		LED_Show6();			//LED��ʽ6- �����ƺ�ż���ƽ�����˸3�Σ�Ȼ��ȫ��
		LED_Show7();			//LED��ʽ7- LED1��LED8˫��������ˮһ��
		LED_Show8();			//LED��ʽ8- LED1-LED4Ϊһ�飬LED5-LED8Ϊһ�飬���汬��
		LED_Show9();			//LED��ʽ9- ��һ��1��LED��ˮ���ڶ���2��LED��ˮ....��6��6��LED��ˮ
		LED_Show10();			//LED��ʽ10-���״�ש��Ч��
	}
}
void LED_Show1()
{
	u8 i = 0;
	while(i < 4)		//1-�������м����
	{
		sum += ~LED_Code[i] + ~LED_Code[7-i];//ȡ������ӣ��Ϳ��Ե���2���ƣ��ۼӾͿ��Ա����ѵ����ĵ�
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show2()
{
	u8 i = 4;
	sum = 0xFF;
	while(i < 8)		//2-�м�������Ϩ��
	{
		sum -= ~LED_Code[i]+~LED_Code[7-i];//ȡ����������Ϳ���Ϩ��2���ƣ��ۼ��Ϳ��Ա�����Ϩ��ĵ�
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show3()
{
	u8 i = 0;
	while(i < 4)		//3-1��3��5��7������������ֵ���״̬
	{
		sum += ~LED_Code[2*i];
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show4()
{
	u8 i = 0;
	while(i < 4)		//4-2��4��6��8������������ֵ���״̬,ֱ��8��ȫ��
	{
		sum += ~LED_Code[2*i+1];
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show5()		//5-�����ƺ�ż���ƽ�����˸3�Σ�Ȼ��ȫ��
{
	u8 i = 0;
	u8 sum1 = 0;
	u8 sum2 = 0;
	for(i = 0;i < 4;i++)			//����������ȫ������ֵ����ż����ȫ������ֵ
	{
		sum1 += ~LED_Code[2*i];
		sum2 += ~LED_Code[2*i+1];
	}
	i = 0;
	while(i < 3)			//������˸3��
	{
		P2 = ~sum1;
		delayms(500);
		P2 = ~sum2;
		delayms(500);
		i++;
	}
	P2 = 0xFF;			//��˸��ɺ�Ϩ��
}
void LED_Show6()		//LED1��LED8˫��������ˮһ��
{
	u8 i;
	for(i = 0;i < 8;i++)
	{
		sum = ~LED_Code[i] + ~LED_Code[7-i];
		P2 = ~sum;
		if(i != 4)delayms(500);
	}
	P2 = 0xFF;
}
void LED_Show7()		//LED1-LED4Ϊһ�飬LED5-LED8Ϊһ�飬���汬��
{
	u8 i = 0;
	u8 j;
	u8 sum1 = 0;
	u8 sum2 = 0;
	for(i = 0;i < 4;i++)			//�����1��ȫ����ֵ���͵�2��ȫ������ֵ
	{
		sum1 += ~LED_Code[i];
		sum2 += ~LED_Code[4+i];
	}
	i = 0;
	while(i < 3)							//�ظ�3��
	{
		for(j = 0;j < 12;j++)		//��һ��LED����12��
		{
			P2 = ~sum1;
			delayms(15);
			P2 = 0xFF;
			delayms(45);
		}
		for(j = 0;j < 12;j++)		//�ڶ���LED����12��
		{
			P2 = ~sum2;
			delayms(15);
			P2 = 0xFF;
			delayms(45);
		}
		i++;
	}
	P2 = 0xFF;
}
void LED_Show8()//8-���׵���ɳ©Ч��
{
	u8 i,j;
	sum = 0;
	for(i = 0;i < 8;i++)
	{
		for(j = 0;j < 8-i;j++)
		{
			P2 = ~(sum + ~LED_Code[j]);		//�ƶ���LED�͵ײ�LED�ۼ�ֵ��Ӻ���һ�����
			delayms(500);
		}
		sum += ~LED_Code[7-i];		//���ײ��Ѿ�������LEDֵ�ۼ�
	}
}
void LED_Show9()		//��һ��1��LED��ˮ���ڶ���2��LED��ˮ....��6��6��LED��ˮ
{
	u8 i,j;
	for(i = 0;i < 6;i++)		//��ˮ����
	{
		for(j = i;j < 8;j++)	//��ˮ����
		{
			if(i == 0) sum = 0;		//��һ��
			if(i == 1) sum = ~LED_Code[j-1];		//�ڶ���
			if(i == 2) sum = ~LED_Code[j-2] + ~LED_Code[j-1];		//������
			if(i == 3) sum = ~LED_Code[j-3] + ~LED_Code[j-2]+~LED_Code[j-1];		//���Ĵ�
			if(i == 4) sum = ~LED_Code[j-4] + ~LED_Code[j-3] + ~LED_Code[j-2]+~LED_Code[j-1];		//�����
			if(i == 5) sum = ~LED_Code[j-5] + ~LED_Code[j-4] + ~LED_Code[j-3] + ~LED_Code[j-2]+~LED_Code[j-1];		//������	
			P2 = ~(sum + ~LED_Code[j]);		//��Ӻ�һ�����
			delayms(500);
		}
	}
}
void LED_Show10()//10-���״�ש��Ч��
{
	u8 i,j;
	sum = 0;
	for(i = 0;i < 6;i++)//����Ĵ���
	{
		for(j = 0;j < 2+i;j++)	//������ˮ������->��ש�飩
		{
			if(i == 5) sum = ~LED_Code[7];		//��һ��
			else if(i == 4) sum = ~LED_Code[7] + ~LED_Code[6];		//�ڶ���
			else if(i == 3) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5];		//������
			else if(i == 2) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4];		//���Ĵ�
			else if(i == 1) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4] + ~LED_Code[3];		//�����
			else if(i == 0) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4] + ~LED_Code[3] + ~LED_Code[2];		//������	
			P2 = ~(sum + ~LED_Code[j]);		//��Ӻ�һ�����
			if(j != i+1) delayms(500);
		}
		for(j = 2+i;j > 0;j--)	//������ˮ����ש���->���أ�
		{
			if(i == 5) sum = 0;		//��һ��
			else if(i == 4) sum = ~LED_Code[7];		//�ڶ���
			else if(i == 3) sum = ~LED_Code[7] + ~LED_Code[6];		//������
			else if(i == 2) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5];		//���Ĵ�
			else if(i == 1) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4];		//�����
			else if(i == 0) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4] + ~LED_Code[3];		//������	
			P2 = ~(sum + ~LED_Code[j]);		//��Ӻ�һ�����
			if(j != i+2) delayms(500);
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