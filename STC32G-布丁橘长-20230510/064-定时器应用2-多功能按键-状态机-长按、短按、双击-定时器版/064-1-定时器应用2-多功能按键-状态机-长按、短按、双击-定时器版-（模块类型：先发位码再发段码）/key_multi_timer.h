#ifndef __KEY_MULTI_TIMER_H__
#define __KEY_MULTI_TIMER_H__

#include <STC32G.h>

sbit KEY1 = P3^2;		//���Ŷ��壺����1 KEY1->P32		
sbit KEY2 = P3^3;		//���Ŷ��壺����2 KEY2->P33		
sbit KEY3 = P3^4;		//���Ŷ��壺����3 KEY3->P34		
sbit KEY4 = P3^5;		//���Ŷ��壺����4 KEY4->P35		

#define KEY_STATE_0 0 										//����״̬0���ж����ް�������
#define KEY_STATE_1 1											//����״̬1����ʱ����
#define KEY_STATE_2 2											//����״̬2���жϵ�����˫��������
#define KEY_STATE_3 3											//����״̬3���ȴ������ɿ�

#define LONG_KEY_TIME 50 									//�ж�����ʱ�䣺LONG_KEY_TIME*10S = 500MS
#define SINGLE_KEY_TIME 1 								//�ж�����ʱ�䣺SINGLE_KEY_TIME*10MS = 10MS
#define DOUBLE_KEY_TIME 30 								//�ж�˫��ʱ�䣺DOUBLE_KEY_TIME*10MS = 300MS

#define None_click 0 											//�ް���  	����ֵ��0
#define Single_click 1 										//����		����ֵ��1	
#define Double_click 2										//˫��		����ֵ��2
#define Long_click 3 											//����		����ֵ��3

#define Key1_Single_Click 11							
#define Key1_Double_Click 12							
#define Key1_Long_Click 13								

#define Key2_Single_Click 21						
#define Key2_Double_Click 22							
#define Key2_Long_Click 23								

#define Key3_Single_Click 31							
#define Key3_Double_Click 32							
#define Key3_Long_Click 33								

#define Key4_Single_Click 41							
#define Key4_Double_Click 42							
#define Key4_Long_Click 43								

unsigned char key_multi(void);						//�๦�ܰ���ɨ�躯�������� ��ֵ+������ ����ֵ
unsigned char keyscan();									//��ͨ����ɨ�躯�������� ��ֵ
void Timer4_Init(void);										//��ʱ��4��ʼ������
#endif