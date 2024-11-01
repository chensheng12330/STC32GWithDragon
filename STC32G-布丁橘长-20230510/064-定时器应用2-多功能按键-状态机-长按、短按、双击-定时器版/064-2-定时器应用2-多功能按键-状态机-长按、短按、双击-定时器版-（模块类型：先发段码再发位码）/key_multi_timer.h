#ifndef __KEY_MULTI_TIMER_H__
#define __KEY_MULTI_TIMER_H__

#include <STC32G.h>

sbit KEY1 = P3^2;		//引脚定义：按键1 KEY1->P32		
sbit KEY2 = P3^3;		//引脚定义：按键2 KEY2->P33		
sbit KEY3 = P3^4;		//引脚定义：按键3 KEY3->P34		
sbit KEY4 = P3^5;		//引脚定义：按键4 KEY4->P35		

#define KEY_STATE_0 0 										//按键状态0：判断有无按键按下
#define KEY_STATE_1 1											//按键状态1：延时消抖
#define KEY_STATE_2 2											//按键状态2：判断单击、双击、长按
#define KEY_STATE_3 3											//按键状态3：等待按键松开

#define LONG_KEY_TIME 50 									//判定长按时间：LONG_KEY_TIME*10S = 500MS
#define SINGLE_KEY_TIME 1 								//判定单击时间：SINGLE_KEY_TIME*10MS = 10MS
#define DOUBLE_KEY_TIME 30 								//判定双击时间：DOUBLE_KEY_TIME*10MS = 300MS

#define None_click 0 											//无按键  	返回值：0
#define Single_click 1 										//单击		返回值：1	
#define Double_click 2										//双击		返回值：2
#define Long_click 3 											//长按		返回值：3

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

unsigned char key_multi(void);						//多功能按键扫描函数，返回 键值+键功能 复合值
unsigned char keyscan();									//普通按键扫描函数，返回 键值
void Timer4_Init(void);										//定时器4初始化设置
#endif