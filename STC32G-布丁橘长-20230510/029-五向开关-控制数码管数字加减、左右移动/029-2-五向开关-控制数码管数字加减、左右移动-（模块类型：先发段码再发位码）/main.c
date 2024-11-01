//		@�����ٳ� 2023/02/05
//		���򿪹�-�ϼ�����������ּӣ��¼�����������ּ����������������һλ���Ҽ�����������һλ,�м�����������
//		���򿪹����Ŷ��壺�ϼ���P7.4 �¼���P7.3 ��P7.2 �ң�P7.1 �м���P7.0 
//		8λ�����������ģ��-74HC595����������DS(DIO):P6.6 SH(SCK):P6.5 ST(RCK):P6.4
//		ʵ�鿪���壺STC32G12K128-LQFP64 ��������.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//������Ƶ35MHz�������ʵ��ʹ��Ƶ���޸ģ�����delay��������Ӧ��Ƶ��

typedef unsigned char u8;				//unsigned char 		������ u8 ��ʾ
typedef unsigned int u16;				//unsigned int  		������ u16��ʾ
typedef unsigned long int u32;	//unsigned long int ������ u32��ʾ

sbit DS = P6^6;						//595��������DS(DIO)��P6.6
sbit SH = P6^5;						//595��������ʱ��SH(SCK)��P6.5
sbit ST = P6^4;						//595����ʱ��ST(RCK)��P6.4

sbit KEY_Up 		= P7^4;		//�ϼ���P7.4
sbit KEY_Down 	= P7^3;		//�¼���P7.3
sbit KEY_Left 	= P7^2;		//�����P7.2
sbit KEY_Right 	= P7^1;		//�Ҽ���P7.1
sbit KEY_Middle = P7^0;		//�м���P7.0

void delayms(u16 ms);						//��ʱ��������
void Write595(u8 dat);					//595д���ݺ�������
void Output595();								//595�����ʾ��������
void SEG_Disp(u8 dat,u8 wei);		//�������ʾ��������
void keyscan();									//����ɨ�躯������

//��������������
u8 SEG_Code[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//��������ܶ��룺0~9,	
										0xff,0xbf,0x00};	//�����ȫ��10�������-��11���������ȫ����12��
//������λ������-4λ�����
//u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//������λ�����飬COM1��595Q0��COM2��Q1...���������ߵ�һλ��Q0...�ڰ�λ��Q7
u8 WEI_Code[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};		//������λ�����飬COM1��595Q7��COM2��Q6...���������ߵ�һλ��Q7...�ڰ�λ��Q0

u8 keynum;					//������
u8 number;					//�������ʾ������
u8 seg_wei;					//������ʾ��λ��

void main()
{
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
	P7M1 = 0x00;P7M0 = 0x00;		//����P7��Ϊ׼˫���ģʽ 00��׼˫��� 01��������� 10���������� 11����©���
	
	seg_wei = 4;			//��ʼ���������ߵ�4λ��ʾ							
	while(1)
	{
		keyscan();										//����ɨ��
		SEG_Disp(number,seg_wei);			//�ڵ�seg_weiλ��ʾ����number
	}
}
void keyscan()
{
	static u8 keystate_up,keystate_down,keystate_left,keystate_right,keystate_middle;	//5�������Ŀ���״̬��1�����£�0���ɿ���
	if(KEY_Up == 0 && keystate_up == 0)						//�ϼ�����
	{
		delayms(10);																//��ʱȥ��
		if(KEY_Up == 0 && keystate_up == 0)					//�ٴ��ж��ϼ��Ƿ���
		{
			keystate_up = 1;													//�ϼ�״̬��1��1�����£�0���ɿ���
			if(number <= 8) number++;									//�������ʾ������+1�����ֵ9
		}
	}
	if(KEY_Up == 1) keystate_up = 0;							//�����ɿ��������ϼ�״̬��1�����£�0���ɿ���
	
	if(KEY_Down == 0 && keystate_down == 0)				//�¼�����
	{
		delayms(10);																//��ʱȥ��
		if(KEY_Down == 0 && keystate_down == 0)			//�ٴ��ж��¼��Ƿ���
		{
			keystate_down = 1;												//�¼�״̬��1��1�����£�0���ɿ���
			if(number >= 1) number--;									//�������ʾ������-1����Сֵ0
		}
	}
	if(KEY_Down == 1) keystate_down = 0;					//�����ɿ��������¼�״̬��1�����£�0���ɿ���
	
	if(KEY_Left == 0 && keystate_left == 0)				//�������
	{
		delayms(10);																//��ʱȥ��
		if(KEY_Left == 0 && keystate_left == 0)			//�ٴ��ж�����Ƿ���
		{
			keystate_left = 1;												//���״̬��1��1�����£�0���ɿ���
			if(seg_wei >= 1) seg_wei--;								//��ʾ��λ�ü�1������һλ	
		}
	}
	if(KEY_Left == 1) keystate_left = 0;					//�����ɿ����������״̬��1�����£�0���ɿ���
	
	if(KEY_Right == 0 && keystate_right == 0)			//�Ҽ�����
	{
		delayms(10);																//��ʱȥ��
		if(KEY_Right == 0 && keystate_right == 0)		//�ٴ��ж��Ҽ��Ƿ���
		{
			keystate_right = 1;												//�Ҽ�״̬��1��1�����£�0���ɿ���
			if(seg_wei <= 7) seg_wei++;								//��ʾ��λ��+1������һλ
		}
	}
	if(KEY_Right == 1) keystate_right = 0;				//�ɿ������������Ҽ�״̬��1�����£�0���ɿ���
		
	if(KEY_Middle == 0 && keystate_middle == 0)		//�м�����
	{
		delayms(10);																//��ʱȥ��
		if(KEY_Middle == 0 && keystate_middle == 0)	//�ٴ��ж��м��Ƿ���
		{
			keystate_middle = 1;											//�м�״̬��1��1�����£�0���ɿ���
			number = 0;																//�����������ʾ������
		}
	}
	if(KEY_Middle == 1) keystate_middle = 0;			//�ɿ������������м�״̬��1�����£�0���ɿ���
}
void  delayms(u16 ms)		//����ʱ����������Ӧ��Ƶ��1����*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
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