//	@�����ٳ� 2023/02/20
//	����RTC+�ⲿ32.768KHzʱ��ʾ������
//  �ϵ����ʾʱ�ӣ����򿪹�-�ϼ����л���ʾʱ��/����
//	ͨ���������򿪹أ������޸�ʱ�䡢���ڡ����ȣ��м�������/ȷ�������¼������ӡ���С�����Ҽ����л�λ��
//  ����״̬����ʾʱ�ӣ����м���ʾS1�����Ҽ���ѡ��S1��S2��S3��S1-ʱ�����ã�S2-�������ã�S3-�������ã��ٴΰ��м������Ӧ����
//	������ɣ����м�ȷ��������ʱ����ʾ״̬
//	��ʼʱ������Ϊ��23��02��20�գ�23��45:00
//	��ʼ���ȣ�10��16�����ȵ��ڣ�0-15��
//	8λMAX7219�����ģ����ʾ�����Ŷ��壺DIN = P6^6; CS = P6^5; CLK = P6^4;
//	���򿪹����Ŷ��壺�ϼ���P7.4���¼���P7.3�������P7.2���Ҽ���P7.1���м���P7.0
//	ʵ�鿪���壺STC32G12K128��������.1 ��Ƶ@12MHz

#include <STC32G.H>
#include "config.h"
#include "MAX7219.h"

#define NORMAL 0					//������ʾ����
#define SETSEL1	1					//SETSEL1:����ѡ�����-S1-ʱ������
#define SETSEL2 2					//SETSEL2:����ѡ�����-S2-��������
#define SETSEL3 3					//SETSEL3:����ѡ�����-S3-��������
#define SET1 4						//SET1:	ʱ�����ý���
#define SET2 5						//SET2���������ý���
#define SET3 6						//SET3���������ý���
#define SHOW_CLOCK 0			//��ʾ��־-��ʾʱ��
#define SHOW_DATE 1				//��ʾ��־-��ʾ����
#define DISP_BLACK 0x0F		//�����BCODE��-Ϩ��
#define SET_HOUR 0				//Сʱ���ñ�־
#define SET_MIN 1					//�������ñ�־
#define SET_SEC 2					//�������ñ�־
#define SET_YEAR 10				//�����ñ�־
#define SET_MONTH 11			//�����ñ�־
#define SET_DAY 12				//�����ñ�־
#define SET_LIGHT 20			//�������ñ�־

sbit KEY_Up 		= P7^4;				//�ϼ���P7.4
sbit KEY_Down 	= P7^3;				//�¼���P7.3
sbit KEY_Left 	= P7^2;				//�����P7.2
sbit KEY_Right 	= P7^1;				//�Ҽ���P7.1
sbit KEY_Middle = P7^0;				//�м���P7.0

void RTC_config(void);				//RTC����
void DISP_Clock(void);				//��ʾʱ�ӽ���
void DISP_Date(void);					//��ʾ���ڽ���
void DISP_Clock_SET(void);		//��ʾʱ�����ý���
void DISP_Date_SET(void);			//��ʾ�������ý���
void DISP_LIGHT_SET(void);		//��ʾ�������ý���
void DISP_SETSEL();						//��ʾ����ѡ�����
void Timer0_Init(void);				//��ʱ��0��ʼ��
void keyscan();								//����ɨ��
void RTC_read();							//��ȡRTC����

u8 clockdat;									//ʱ������λ��
bit B_1ms;										//1�����ʱ��־
bit B_300ms;									//300�����ʱ��־
u8 year,month,day,hour,min,sec,ssec;																		//������ʱ����ȱ���
u8 keyupflag,keydownflag,keyleftflag,keyrightflag,keymiddleflag;				//���������м����±�־
u8 keyupdelay,keydowndelay,keyleftdelay,keyrightdelay,keymiddledelay;		//���������м���ʱ��־
u8 setstate;									//����״̬
u16 timer0;										//��ʱ��conter
u8 blinkflag;									//��˸��־
u8 showstate;									//��ʾ״̬
u8 light;											//����

void main(void)
{
	EAXFR = 1; 									// ʹ�ܷ��� XFR
	CKCON = 0x00; 							// �����ⲿ���������ٶ�Ϊ���
	WTST = 0x00;								// ���ó������ȴ��������ȴ�ʱ��Ϊ0��ʱ�ӣ�CPUִ�г����ٶ����

	P0M1 = 0x00;P0M0 = 0x00;		//����P0��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P1M1 = 0x00;P1M0 = 0x00;		//����P1��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P2M1 = 0x00;P2M0 = 0x00;		//����P2��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P3M1 = 0x00;P3M0 = 0x00;		//����P3��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P4M1 = 0x00;P4M0 = 0x00;		//����P4��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P5M1 = 0x00;P5M0 = 0x00;		//����P5��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P6M1 = 0x00;P6M0 = 0x00;		//����P6��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	P7M1 = 0x00;P7M0 = 0x00;		//����P7��Ϊ׼˫���ģʽ //00��׼˫��� 01��������� 10���������� 11����©���
	
	year = 23;									//��ʼ�꣺2023
	month = 2;									//��ʼ�£�02
	day = 21;										//��ʼ�գ�20
	hour = 21;									//��ʼʱ��21
	min = 45;										//��ʼ�֣�45
	sec = 0;										//��ʼ�룺0
	light = 10;									//��ʼ���ȣ�10��0-15��16�����ȵ��ڣ�
  RTC_config();								//RTC����
	MAX7219_Ini();							//MAX7219��ʼ��
	Timer0_Init();							//��ʱ��0��ʼ��
	EA = 1;											//ʹ�����ж�

  while(1)
  {
		if(setstate == NORMAL && showstate == SHOW_CLOCK) 		DISP_Clock();		//����״̬Ϊ��ͨģʽ��������ʾ״̬Ϊ��ʾʱ�ӣ�����ʾʱ��
		else if(setstate == NORMAL && showstate == SHOW_DATE)	DISP_Date();		//����״̬Ϊ��ͨģʽ��������ʾ״̬Ϊ��ʾ���ڣ�����ʾ����
		else if(setstate == SET1) DISP_Clock_SET();														//����״̬Ϊ����1ģʽ������ʾ����ʱ��
		else if(setstate == SET2) DISP_Date_SET();														//����״̬Ϊ����2ģʽ������ʾ��������
		else if(setstate == SET3) DISP_LIGHT_SET();														//����״̬Ϊ����3ģʽ������ʾ��������
		else if(setstate == SETSEL1 || setstate == SETSEL2 || setstate == SETSEL3) DISP_SETSEL();//����״̬Ϊ����ѡ��ģʽ������ʾ����ѡ�����
		keyscan();								//����ɨ��
  }
} 
void Timer0_Isr(void) interrupt 1						//��ʱ��0�ṩ1������ģ����ڰ���ɨ���
{
	B_1ms = 1;									//1�����־
	timer0++;										//�����ʱ++
	if(timer0 >= 300) 					//��ʱ300����
	{
		B_300ms = 1;							//300�����־
		timer0 = 0;								//����300�������
	}
}
void Timer0_Init(void)				//1����@12.000MHz
{
	AUXR |= 0x80;								//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;								//���ö�ʱ��ģʽ
	TL0 = 0x20;									//���ö�ʱ��ʼֵ
	TH0 = 0xD1;									//���ö�ʱ��ʼֵ
	TF0 = 0;										//���TF0��־
	TR0 = 1;										//��ʱ��0��ʼ��ʱ
	ET0 = 1;										//ʹ�ܶ�ʱ��0�ж�
}
void DISP_SETSEL()						//����ѡ�����
{
	if(setstate == SETSEL1)			//ѡ�����-S1
	{
		Write7219(8,5); 					//��ߵ�1λ����ʾS
		Write7219(7,1); 					//��ߵ�2λ����ʾ1
	}
	if(setstate == SETSEL2)			//ѡ�����-S2
	{
		Write7219(8,5); 					//��ߵ�1λ����ʾS
		Write7219(7,2); 					//��ߵ�2λ����ʾ2
	}		
	if(setstate == SETSEL3)			//ѡ�����-S3
	{
		Write7219(8,5); 					//��ߵ�1λ����ʾS
		Write7219(7,3); 					//��ߵ�2λ����ʾ3
	}	
		Write7219(6,DISP_BLACK);	//���������6λϨ��
		Write7219(5,DISP_BLACK);
		Write7219(4,DISP_BLACK);
		Write7219(3,DISP_BLACK);
		Write7219(2,DISP_BLACK);
		Write7219(1,DISP_BLACK);
}
void DISP_Clock(void)												//��ʾʱ�Ӻ���
{			
	Write7219(8,(u8)(HOUR / 10)); 						//�����1λ��ʾʱ��ʮλ
	Write7219(7,(u8)(HOUR % 10)); 						//�����2λ��ʾʱ�Ӹ�λ
	Write7219(6,10); 													//�����3λ��ʾʱ�Ӻ��
	Write7219(5,(u8)(MIN / 10)); 							//�����4λ��ʾ����ʮλ
	Write7219(4,(u8)(MIN % 10));							//�����5λ��ʾ���Ӹ�λ							
	Write7219(3,10); 													//�����6λ��ʾ���Ӻ��
	Write7219(2,(u8)(SEC / 10)); 							//�����7λ��ʾ����ʮλ
	Write7219(1,(u8)(SEC % 10)); 							//�����8λ��ʾʱ�Ӹ�λ
}
void DISP_Date(void)												//��ʾ���ں���
{							
	Write7219(8,2); 													//�����1λ��ʾ��ǧλ
	Write7219(7,0); 													//�����2λ��ʾ���λ
	Write7219(6,(u8)(YEAR / 10)); 						//�����3λ��ʾ��ʮλ
	Write7219(5,(u8)(YEAR % 10)); 						//�����4λ��ʾ���λ
	Write7219(4,(u8)(MONTH / 10)); 						//�����5λ��ʾ��ʮλ
	Write7219(3,(u8)(MONTH % 10)); 						//�����6λ��ʾ�¸�λ
	Write7219(2,(u8)(DAY / 10)); 							//�����7λ��ʾ��ʮλ
	Write7219(1,(u8)(DAY % 10)); 							//�����8λ��ʾ�¸�λ
}
void DISP_Clock_SET(void)										//��ʾ����ʱ�Ӻ���
{	
	if(clockdat != SET_HOUR || blinkflag % 2 == 0)
	{
		Write7219(8,(u8)(HOUR / 10)); 					//�����1λ��ʾʱ��ʮλ
		Write7219(7,(u8)(HOUR % 10)); 					//�����2λ��ʾʱ�Ӹ�λ
	}
	Write7219(6,10); 													//�����3λ��ʾʱ�Ӻ��
	if(clockdat != SET_MIN || blinkflag % 2 == 0)
	{
		Write7219(5,(u8)(MIN / 10)); 						//�����4λ��ʾ����ʮλ
		Write7219(4,(u8)(MIN % 10)); 						//�����5λ��ʾ���Ӹ�λ	
	}
	Write7219(3,10); 													//�����6λ��ʾ���Ӻ��
	if(clockdat != SET_SEC || blinkflag % 2 == 0)
	{
		Write7219(2,(u8)(SEC / 10)); 						//�����7λ��ʾ����ʮλ
		Write7219(1,(u8)(SEC % 10)); 						//�����8λ��ʾʱ�Ӹ�λ	
	}
	if(B_300ms == 1)
	{
		blinkflag++;
		B_300ms = 0;
		if(blinkflag > 99) blinkflag = 0;				//��������ܸ�λ��˸ʱ��Ϩ��״̬
		if(blinkflag % 2 == 1)
		{
			if(clockdat == SET_MIN)
			{
				Write7219(5,DISP_BLACK);
				Write7219(4,DISP_BLACK);
			}
			if(clockdat == SET_SEC)
			{
				Write7219(2,DISP_BLACK);
				Write7219(1,DISP_BLACK);
			}
			if(clockdat == SET_HOUR)
			{
				Write7219(8,DISP_BLACK);
				Write7219(7,DISP_BLACK);
			}
		}
	}
}
void DISP_Date_SET()
{
	if(clockdat != SET_YEAR || blinkflag % 2 == 0)
	{
		Write7219(8,2); 													//�����1λ��ʾ��ǧλ
		Write7219(7,0); 													//�����2λ��ʾ���λ
		Write7219(6,(u8)(YEAR / 10)); 						//�����3λ��ʾ��ʮλ
		Write7219(5,(u8)(YEAR % 10)); 						//�����4λ��ʾ���λ
	}
	if(clockdat != SET_MONTH || blinkflag % 2 == 0)
	{
		Write7219(4,(u8)(MONTH / 10)); 						//�����5λ��ʾ��ʮλ
		Write7219(3,(u8)(MONTH % 10)); 						//�����6λ��ʾ�¸�λ
	}
	if(clockdat != SET_DAY || blinkflag % 2 == 0)
	{
		Write7219(2,(u8)(DAY / 10)); 							//�����7λ��ʾ��ʮλ
		Write7219(1,(u8)(DAY % 10)); 							//�����8λ��ʾ�¸�λ
	}
	if(B_300ms == 1)
	{
		blinkflag++;
		B_300ms = 0;
		if(blinkflag > 99) blinkflag = 0;					
		if(blinkflag % 2 == 1)
		{
			if(clockdat == SET_YEAR)								//��������ܸ�λ��˸ʱ��Ϩ��״̬
			{
				Write7219(8,DISP_BLACK); 													
				Write7219(7,DISP_BLACK); 													
				Write7219(6,DISP_BLACK); 						
				Write7219(5,DISP_BLACK); 					
			}
			if(clockdat == SET_MONTH)
			{
				Write7219(4,DISP_BLACK); 													
				Write7219(3,DISP_BLACK); 	
			}			
			if(clockdat == SET_DAY)
			{
				Write7219(2,DISP_BLACK); 													
				Write7219(1,DISP_BLACK); 
			}
		}
	}
}
void DISP_LIGHT_SET()
{
	if(B_300ms == 1)												//��300�����־Ϊ1
	{
		blinkflag++;													//ÿ300���룬��˸��־+1
		B_300ms = 0;													//����300�����־
		if(blinkflag > 99) blinkflag = 0;			//�����������
		if(blinkflag % 2 == 1)								//ÿ300������һ��
		{
			Write7219(8,(u8)(light / 10)); 			//��ߵ�1λ����ʾ����ʮλ									
			Write7219(7,(u8)(light % 10)); 			//��ߵ�2λ����ʾ���ȸ�λ														
		}
		else if(blinkflag % 2 == 0)						//ÿ300������һ��
		{
			Write7219(8,DISP_BLACK); 						//��ߵ�1��2λϨ��						
			Write7219(7,DISP_BLACK); 												
		}
	}	
	Write7219(6,DISP_BLACK); 								//���������6λ��Ϩ��
	Write7219(5,DISP_BLACK); 						
	Write7219(4,DISP_BLACK); 													
	Write7219(3,DISP_BLACK); 			
	Write7219(2,DISP_BLACK); 													
	Write7219(1,DISP_BLACK); 
}
void RTC_config(void)							//RTC���ú���
{
    INIYEAR = year;    						//RTC��ʼ���Ĵ�����װ��������ʱ�������ֵ						
    INIMONTH = month;   											
    INIDAY = day;     												
    INIHOUR = hour;    												
    INIMIN = min;     												
    INISEC = sec;     												
    INISSEC = 0;      											
																	//STC32G оƬʹ���ڲ�32Kʱ�ӣ������޷�����
	//	IRC32KCR = 0x80;   					//�����ڲ�32K����.
  //  while (!(IRC32KCR & 1));  	//�ȴ�ʱ���ȶ�
	//	RTCCFG = 0x03;    					//ѡ���ڲ�32Kʱ��Դ��������RTC�Ĵ�����ʼ��

		X32KCR = 0x80 + 0x40;   			//�����ⲿ32K����, ������+0x00, ������+0x40.
    while (!(X32KCR & 1));  			//�ȴ�ʱ���ȶ�
		RTCCFG = 0x01;   							//ѡ���ⲿ32Kʱ��Դ������RTC�Ĵ�����ʼ��
	
    RTCCR = 0x01;     						//RTCʹ��
    while(RTCCFG & 0x01);					//�ȴ���ʼ�����,��Ҫ�� "RTCʹ��" ֮���ж�. 
																	//����RTCʱ����Ҫ32768Hz��1������ʱ��,��Լ30.5us. ����ͬ��, ����ʵ�ʵȴ�ʱ����0~30.5us.
}
void RTC_read()										//��ȡRTC����
{
	year = YEAR;										//��ȡֵ�ֱ�����Ӧ�ı���
	month = MONTH;
	day = DAY;
	hour = HOUR;
	min = MIN;
	sec = SEC;
}
void keyscan()
{
//------------------------�м�----------------------------------------------
	if(KEY_Middle == 0)
	{
		if(keymiddleflag == 0)				//�м����¼�ʱ
		{
			if(B_1ms == 1)							//1���뵽
			{
				keymiddledelay++;					//�м�������ʱ+1
				B_1ms = 0;								//����1�����־
			}
			if(keymiddledelay == 10)		//10���뵽��10����������
			{
				keymiddleflag = 1;				//�м����±�־��1
				if(setstate == NORMAL) 						setstate = SETSEL1;														//�����������ʾ״̬�����м���״̬��Ϊ������ѡ��1-S1
				else if(setstate == SETSEL1) 	{setstate = SET1;clockdat = SET_HOUR;}						//���״̬������ѡ��1��״̬��Ϊ������1-ʱ�����ã����������HOUR
				else if(setstate == SETSEL2)	{setstate = SET2;clockdat = SET_YEAR;}						//���״̬������ѡ��2��״̬��Ϊ������2-�������ã����������YEAR
				else if(setstate == SETSEL3)  {setstate = SET3;clockdat = SET_LIGHT;}						//���״̬������ѡ��3��״̬��Ϊ������3-�������ã����������LIGHT
				else if(setstate == SET1)			{setstate = NORMAL;clockdat = SET_HOUR;}					//���״̬������1-ʱ�����ã�״̬��Ϊ����ͨ�����������HOUR
				else if(setstate == SET2)			{setstate = NORMAL;clockdat = SET_HOUR;}					//���״̬������2-�������ã�״̬��Ϊ����ͨ�����������HOUR
				else if(setstate == SET3)			{setstate = NORMAL;Write7219(INTENSITY,light);}		//���״̬������3-�������ã�״̬��Ϊ����ͨ
			}
		}
	}
	if(KEY_Middle == 1) 						//�ɿ��м���
	{
		keymiddledelay = 0;						//�����м���ʱ
		keymiddleflag = 0;						//�����м����±�־
	}
//------------------------�ϼ�----------------------------------------------
	if(KEY_Up == 0)
	{
		if(keyupflag == 0)						//�м����¼�ʱ
		{
			if(B_1ms == 1)							//1���뵽
			{
				keyupdelay++;							//�ϼ���ʱ+1
				B_1ms = 0;								//����1�����־
			}
			if(keyupdelay == 10)				//10���뵽
			{
				keyupflag = 1;						//�ϼ����±�־��1
				if(setstate == NORMAL && showstate == SHOW_CLOCK) showstate = SHOW_DATE;				//���״̬����ͨ��������ʾ״̬��ʱ�䣬״̬��Ϊ����ʾ����
				else if(setstate == NORMAL && showstate == SHOW_DATE) showstate = SHOW_CLOCK;		//���״̬����ͨ��������ʾ״̬�����ڣ�״̬��Ϊ����ʾʱ��
				if(setstate == SET1 || setstate == SET2 || setstate == SET3)										//���״̬������1������2������3
				{
					RTC_read();							//��ȡRTC����
					switch(clockdat)				//�жϹ��λ��
					{
						case 0:	if(HOUR < 23) hour++;RTC_config();break;										//�����HOUR��Сʱ+1������RTC�Ĵ�������ʼ��
						case 1:	if(MIN < 59) 	min++;RTC_config();break;											//�����MIN������+1������RTC�Ĵ�������ʼ��
						case 2:	if(SEC < 59) 	sec++;RTC_config();break;											//�����SEC������+1������RTC�Ĵ�������ʼ��
						case 10:if(YEAR < 99)	year++;RTC_config();break;										//�����YEAR����+1������RTC�Ĵ�������ʼ��
						case 11:if(MONTH < 12) month++;RTC_config();break;									//�����MONTH����+1������RTC�Ĵ�������ʼ��
						case 12:if(DAY < 31)	day++;RTC_config();break;											//�����DAY����+1������RTC�Ĵ�������ʼ��
						case 20:if(light < 15) light++; Write7219(INTENSITY,light); break;	//�����light������+1������RTC�Ĵ�������ʼ��
						default:break;
					}
				}
			}
		}
	}
	if(KEY_Up == 1) 								//�ϼ��ɿ�
	{
		keyupdelay = 0;								//�����ϼ���ʱ
		keyupflag = 0;								//�����ϼ����±�־
	}
	//------------------------�¼�----------------------------------------------
	if(KEY_Down == 0)								//�¼�����
	{	
		if(keydownflag == 0)					//�¼����¼�ʱ
		{
			if(B_1ms == 1)							//1���뵽
			{
				keydowndelay++;						//�¼���ʱ+1
				B_1ms = 0;								//����1�����־
			}
			if(keydowndelay == 10)			//10���뵽
			{
				keydownflag = 1;					//�¼����±�־��1
				if(setstate == SET1 || setstate == SET2 || setstate == SET3)					//���������1������2������3״̬
				{
					RTC_read();							//��ȡRTC����
					switch(clockdat)				//�жϹ��λ��
					{
						case 0:	if(HOUR > 0) hour--;RTC_config();break;										//�����HOUR��Сʱ-1�����²���ʼ��RTC
						case 1:	if(MIN > 0) 	min--;RTC_config();break;										//�����MIN������-1�����²���ʼ��RTC
						case 2:	if(SEC > 0) 	sec--;RTC_config();break;										//�����SEC������-1�����²���ʼ��RTC
						case 10:if(YEAR > 0)	year--;RTC_config();break;									//�����YEAR����-1�����²���ʼ��RTC
						case 11:if(MONTH > 1) month--;RTC_config();break;									//�����MONTH����-1�����²���ʼ��RTC
						case 12:if(DAY > 1)	day--;RTC_config();break;											//�����DAY����-1�����²���ʼ��RTC
						case 20:if(light > 0) light--;Write7219(INTENSITY,light);break;		//�����light������-1�����²���ʼ��RTC
						default:break;
					}
				}
			}
		}
	}
	if(KEY_Down == 1) 							//�ɿ��¼�
	{
		keydowndelay = 0;							//�����¼���ʱ
		keydownflag = 0;							//�����¼����±�־
	}
//------------------------���----------------------------------------------
	if(KEY_Left == 0)								//�������
	{
		if(keyleftflag == 0)					//�����ʱ
		{
			if(B_1ms == 1)							//1���뵽
			{
				keyleftdelay++;						//������¼�ʱ+1
				B_1ms = 0;								//����1�����־
			}
			if(keyleftdelay == 10)			//10���뵽
			{
				keyleftflag = 1;					//������±�־�� 
				if(setstate == SET1)			//���״̬������1-ʱ������
				{
					if(clockdat > SET_HOUR)clockdat--;								//���������HOURλ�ã����λ��-1�����ƣ�
				}
				else if(setstate == SET2)														//���״̬��SET2-��������
				{
					if(clockdat > SET_YEAR) clockdat--;								//���������YEARλ�ã����λ��-1�����ƣ�
				}
				else if(setstate == SETSEL1) setstate = SETSEL3;		//���������ѡ�����-S1���л���S3
				else if(setstate == SETSEL2) setstate = SETSEL1;		//���������ѡ�����-S2���л���S1
				else if(setstate == SETSEL3) setstate = SETSEL2;		//���������ѡ�����-S3���л���S2
			}
		}
	}
	if(KEY_Left == 1) 							//�ɿ����
	{
		keyleftdelay = 0;							//���������ʱ
		keyleftflag = 0;							//����������±�־
	}
	//------------------------�Ҽ�----------------------------------------------
	if(KEY_Right == 0)							//�Ҽ�����
	{
		if(keyrightflag == 0)					//�Ҽ����¼�ʱ
		{
			if(B_1ms == 1)							//1���뵽
			{
				keyrightdelay++;					//�Ҽ����¼�ʱ+1
				B_1ms = 0;								//1���뵽
			}
			if(keyrightdelay == 10)			//10���뵽
			{
				keyrightflag = 1;					//�Ҽ����±�־��1
				if(setstate == SET1 )			//���������1-����ʱ��״̬
				{
					if(clockdat < SET_SEC) clockdat++;		//������λ��С��SEC���������
				}
				else if(setstate == SET2)								//���������2-��������״̬
				{
					if(clockdat < SET_DAY) clockdat++;		//������λ��С��DAY���������		
				}
				else if(setstate == SETSEL1) setstate = SETSEL2;		//���������ѡ��-S1���л���S2
				else if(setstate == SETSEL2) setstate = SETSEL3;		//���������ѡ��-S2���л���S3
				else if(setstate == SETSEL3) setstate = SETSEL1;		//���������ѡ��-S3���л���S1
			}
		}
	}
	if(KEY_Right == 1) 							//�Ҽ��ɿ�
	{
		keyrightdelay = 0;						//�����Ҽ���ʱ
		keyrightflag = 0;							//�����Ҽ����±�־
	}
}