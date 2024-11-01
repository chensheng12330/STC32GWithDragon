//		@�����ٳ� 2023/02/04
//		4*4������̣��������ʾ�������01-16
//		����������Ŷ��壺�У�C4-P2.7 C3-P2.6 C2-P2.5 C1-P2.4 �У�R1-P2.3 R2-P2.2 R3-P2.1 R4-P2.0(������Ϊ��һ�У������λ��һ��)
//		8λ�����������ģ��-595����������DS(DIO):P6.6 SH(SCK):P6.5 ST(RCK):P6.4
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//������Ƶ35MHz�������ʵ��ʹ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

typedef unsigned char u8;				//unsigned char 		������ u8 ��ʾ
typedef unsigned int u16;				//unsigned int  		������ u16��ʾ
typedef unsigned long int u32;	//unsigned long int ������ u32��ʾ

sbit DS = P6^6;				//��������DS(DIO)��P6.6
sbit SH = P6^5;				//��������ʱ��SH(SCK)��P6.5
sbit ST = P6^4;				//����ʱ��ST(RCK)��P6.4

void delayms(u16 ms);						//��ʱ��������
void Write595(u8 dat);					//595д���ݺ�������
void Output595();								//595�����ʾ��������
void SEG_Disp(u8 dat,u8 wei);		//�������ʾ��������
u8 Matrix_key();		 						//���󰴼���������

//��������������
u8 SEG_Code[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//��������ܶ��룺0~9,	
										0xff,0xbf,0x00};	//�����ȫ��10�������-��11���������ȫ����12��
//������λ������-4λ�����
//u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//������λ�����飬COM1��595Q0��COM2��Q1...���������ߵ�һλ��Q0...�ڰ�λ��Q7
u8 WEI_Code[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};		//������λ�����飬COM1��595Q7��COM2��Q6...���������ߵ�һλ��Q7...�ڰ�λ��Q0

void main()
{
	u8 keynum;
	
	EAXFR = 1;				//ʹ��XFR����
	CKCON = 0x00;			//�����ⲿ���������ٶ����
	WTST = 0x00;			//���ó����ȡ�ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����
	
	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0x00;		//����P1��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		//����P3��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		//����P4��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		//����P5��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		//����P6��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	
	while(1)
	{
		keynum = Matrix_key();							//��ȡ������
		if(keynum >= 10)										//����������7λֻ�а����Ŵ��ڵ���10����ʾ
		{
			SEG_Disp((u8)(keynum / 10),7);		//����������7λ��ʾ������ʮλ
			delayms(1);
		}
		SEG_Disp((u8)(keynum % 10),8);			//����������7λ��ʾ�����Ÿ�λ
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
u8 Matrix_key()
{
	u8 temp,keyvalue;		//temp��P2ֵ���棬keyvalue:������
	u8 keystate;				//����״̬
	P2 = 0xF0;					//��ɨ��
	if(P2 != 0xF0 && keystate == 0)			//�ж��Ƿ��а������£���������Ѿ��ǰ���״̬�򲻽���if
	{
		delayms(5);				//����ȥ��
		if(P2 != 0xF0 && keystate == 0)		//�ٴ��ж��Ƿ��а������£���������Ѿ��ǰ���״̬�򲻽���if
		{
			keystate = 1;		//����״̬Ϊ1����������
			temp = P2;			//��ȡP2ֵ
			switch(temp)
			{
				case 0xE0:keyvalue = 1;break;			//��1���а������£���ֵ����1
				case 0xD0:keyvalue = 2;break;			//��2���а������£���ֵ����2
				case 0xB0:keyvalue = 3;break;			//��3���а������£���ֵ����3
				case 0x70:keyvalue = 4;break;			//��4���а������£���ֵ����4
			}
			P2 = 0x0F;				//��ת��ƽ����ɨ��
			if(P2 != 0x0F)		//�ж��Ƿ��а�������
			{
				temp = P2;			//��ȡP2ֵ
				switch(temp)
				{
					case 0x07:keyvalue += 0;break;		//��1�а��£���ֵkeyvalue+0
					case 0x0B:keyvalue += 4;break;		//��2�а��£���ֵkeyvalue+4
					case 0x0D:keyvalue += 8;break;		//��3�а��£���ֵkeyvalue+8
					case 0x0E:keyvalue += 12;break;		//��4�а��£���ֵkeyvalue+12
				}
			}
			
		}
	}
	if(P2 == 0xF0) keystate = 0;		//�ɿ����������㰴��״̬
	return keyvalue;								//���ذ�����
}
void Write595(u8 dat)			//595д���ݺ���
{
	u16 i;
	for(i = 0;i < 8;i++)		
	{
		SH = 0;								//���ʹ�������ʱ��SCLK
		DS = dat & 0x80;			//ȡ���λ
		SH = 1;								//SCKLʱ�������أ����ݴ���λ�Ĵ���
	  dat <<= 1;						//ÿ��һλ
	}
}
void Output595()					//595�������
{
	ST = 0;									//����STʱ��
	ST = 1;									//STʱ�������أ���������洢������
}

void SEG_Disp(u8 dat,u8 wei)			//�������ʾ����
{
	u8 datcode,weicode;
	datcode = SEG_Code[dat];				//ȡʮ����dat��Ӧ�Ķ���
	weicode = WEI_Code[wei-1];			//ȡʮ����wei��Ӧ��λ��
	
	Write595(datcode);							//�ȷ�����
	Write595(weicode);							//�ٷ�λ��
	Output595();										//595�����ʾ
}