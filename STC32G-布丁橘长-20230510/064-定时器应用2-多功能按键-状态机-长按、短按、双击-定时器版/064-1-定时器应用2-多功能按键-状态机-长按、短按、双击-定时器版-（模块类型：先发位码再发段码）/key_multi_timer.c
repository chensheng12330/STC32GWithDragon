//�๦�ܰ���������ʵ�ְ�����������˫��������
//ռ�ö�ʱ��4����Ҫ���������Զ�ʱ��4��ʼ����Timer4_Init();��ʹ���ж�EA=1

#include <STC32G.H>
#include "key_multi_timer.h"
#include "config.h"

static u8 keynumber;
static u8 key_state; 									//����״̬����
static u16 key_time; 									//������ʱ����
static u8 key_press_nums;							//������������		
static u16 key_time_double;						//����˫��ʱ��
static u8 keynum;											//��ֵ
static u8 new_keynum;									//�µļ�ֵ
u8 key_press, key_return;							//��ֵ�ͷ���ֵ
	
void Timer4_Isr(void) interrupt 20
{
	key_time++; 
	if(key_press_nums > 0)
	{
		key_time_double++;
		if((key_time_double >DOUBLE_KEY_TIME )&&( key_time_double< LONG_KEY_TIME))  key_state = KEY_STATE_2;	
	}
}
void Timer4_Init(void)		//10����@12.000MHz
{
	TM4PS = 0x01;			//���ö�ʱ��ʱ��Ԥ��Ƶ ( ע��:��������ϵ�ж��д˼Ĵ���,������鿴�����ֲ� )
	T4T3M |= 0x20;			//��ʱ��ʱ��1Tģʽ
	T4L = 0xA0;				//���ö�ʱ��ʼֵ
	T4H = 0x15;				//���ö�ʱ��ʼֵ
	T4T3M |= 0x80;			//��ʱ��4��ʼ��ʱ
	IE2 |= 0x40;			//ʹ�ܶ�ʱ��4�ж�
}

u8 key_multi(void)
{	
	key_return = None_click; 									//������ذ���ֵ
	if(!KEY1) {key_press = KEY1;new_keynum = 1;}									
	else if(!KEY2) {key_press = KEY2;new_keynum = 2;}							
	else if(!KEY3) {key_press = KEY3;new_keynum = 3;}							
	else if(!KEY4) {key_press = KEY4;new_keynum = 4;}							
	else key_press = 1;
		
	switch (key_state)
	{
		case KEY_STATE_0: 											//����״̬0���ж����ް�������
		{
			if (!key_press) 											//�а�������
			{
				key_time = 0; 											//����ʱ��������
				
				key_state = KEY_STATE_1; 						//Ȼ����� ����״̬1
			}
		}break;
		case KEY_STATE_1: 											//����״̬1���������(ȷ�������Ƿ���Ч����������)��������Ч�Ķ��壺�����������³����趨������ʱ�䡣
		{
			if (!key_press)
			{
				TR0 = 1;
				if(key_time>=SINGLE_KEY_TIME )			//����ʱ��Ϊ��SINGLE_KEY_TIME*10ms = 10ms;
				{
					key_state = KEY_STATE_2; 					//�������ʱ�䳬�� ����ʱ�䣬���ж�Ϊ���µİ�����Ч�����밴��״̬2�������ж������ǵ�����˫�����ǳ���
					key_press_nums++;									//��������+1
				}
			}
			else key_state = KEY_STATE_0; 				//�������ʱ��û�г������ж�Ϊ�󴥣�������Ч������ ����״̬0�������ȴ�����
		}break;
		case KEY_STATE_2: 											//����״̬2���ж�������Ч�����ࣺ�ǵ�����˫�������ǳ���
		{
			if(key_press) 												//��������� �趨��˫��ʱ�����ͷţ����ж�Ϊ����/˫��
			{
				if(key_time_double <= DOUBLE_KEY_TIME ) key_state = KEY_STATE_0;						//˫��ʱ��ﵽǰ���ٴ�ɨ�谴��
				if((key_time_double >DOUBLE_KEY_TIME )&&( key_time_double< LONG_KEY_TIME)) 	//�ﵽ˫���趨ʱ���
				{
					if(key_press_nums == 1)
					{ TR0 = 0;key_return = Single_click + new_keynum*10;
						key_state = KEY_STATE_0; 					//���� ����״̬0�������ȴ�����
					key_press_nums = 0;								//��հ�������
					key_time_double = 0;							//���˫����ʱ
					}				//������Ч����ֵ������
					else if(key_press_nums >=2 && new_keynum == keynum) 
					{
					TR0 = 0;key_return = Double_click + new_keynum*10;
					key_state = KEY_STATE_0; 					//���� ����״̬0�������ȴ�����
					key_press_nums = 0;								//��հ�������
					key_time_double = 0;							//���˫����ʱ
					}//������Ч����ֵ��˫��
				}
			}
			else
			{	
				if(key_time >= LONG_KEY_TIME) 			//�������ʱ�䳬�� �趨�ĳ���ʱ��(LONG_KEY_TIME*10ms=80*10ms=800ms), ���ж�Ϊ ����
				{
					TR0 = 0;
					key_return = Long_click + new_keynum*10; 	//������Ч��ֵֵ������
					key_state = KEY_STATE_3; 					//ȥ״̬3���ȴ������ͷ�
				}
			}
		}break;
		case KEY_STATE_3: 											//�ȴ������ͷ�
		{
			if (key_press)
			{
				key_state = KEY_STATE_0; 						//�����ͷź󣬽��� ����״̬0 ��������һ�ΰ������ж�
				key_press_nums = 0;									//��հ�������
				key_time_double=0;
				TR0 = 0;
			}
		}break;
		default:{key_state = KEY_STATE_0;}break;	//���������key_state������ֵ�����������key_state���������һ������� û�г�ʼ��key_state����һ��ִ�����������ʱ��
	}
	keynum = new_keynum;											//���»�ȡ�ļ�ֵ��ֵ����ֵ
	return key_return; 												//���� ����ֵ
}