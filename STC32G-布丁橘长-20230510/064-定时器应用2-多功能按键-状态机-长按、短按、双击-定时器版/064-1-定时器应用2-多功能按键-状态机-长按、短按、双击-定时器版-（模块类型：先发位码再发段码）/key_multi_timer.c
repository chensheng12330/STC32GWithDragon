//多功能按键函数，实现按键：单击、双击、长按
//占用定时器4，需要在主函数对定时器4初始化：Timer4_Init();并使能中断EA=1

#include <STC32G.H>
#include "key_multi_timer.h"
#include "config.h"

static u8 keynumber;
static u8 key_state; 									//按键状态变量
static u16 key_time; 									//按键计时变量
static u8 key_press_nums;							//按键次数变量		
static u16 key_time_double;						//按键双击时间
static u8 keynum;											//键值
static u8 new_keynum;									//新的键值
u8 key_press, key_return;							//键值和返回值
	
void Timer4_Isr(void) interrupt 20
{
	key_time++; 
	if(key_press_nums > 0)
	{
		key_time_double++;
		if((key_time_double >DOUBLE_KEY_TIME )&&( key_time_double< LONG_KEY_TIME))  key_state = KEY_STATE_2;	
	}
}
void Timer4_Init(void)		//10毫秒@12.000MHz
{
	TM4PS = 0x01;			//设置定时器时钟预分频 ( 注意:并非所有系列都有此寄存器,详情请查看数据手册 )
	T4T3M |= 0x20;			//定时器时钟1T模式
	T4L = 0xA0;				//设置定时初始值
	T4H = 0x15;				//设置定时初始值
	T4T3M |= 0x80;			//定时器4开始计时
	IE2 |= 0x40;			//使能定时器4中断
}

u8 key_multi(void)
{	
	key_return = None_click; 									//清除返回按键值
	if(!KEY1) {key_press = KEY1;new_keynum = 1;}									
	else if(!KEY2) {key_press = KEY2;new_keynum = 2;}							
	else if(!KEY3) {key_press = KEY3;new_keynum = 3;}							
	else if(!KEY4) {key_press = KEY4;new_keynum = 4;}							
	else key_press = 1;
		
	switch (key_state)
	{
		case KEY_STATE_0: 											//按键状态0：判断有无按键按下
		{
			if (!key_press) 											//有按键按下
			{
				key_time = 0; 											//清零时间间隔计数
				
				key_state = KEY_STATE_1; 						//然后进入 按键状态1
			}
		}break;
		case KEY_STATE_1: 											//按键状态1：软件消抖(确定按键是否有效，而不是误触)。按键有效的定义：按键持续按下超过设定的消抖时间。
		{
			if (!key_press)
			{
				TR0 = 1;
				if(key_time>=SINGLE_KEY_TIME )			//消抖时间为：SINGLE_KEY_TIME*10ms = 10ms;
				{
					key_state = KEY_STATE_2; 					//如果按键时间超过 消抖时间，即判定为按下的按键有效。进入按键状态2，继续判定到底是单击、双击还是长按
					key_press_nums++;									//按键次数+1
				}
			}
			else key_state = KEY_STATE_0; 				//如果按键时间没有超过，判定为误触，按键无效，返回 按键状态0，继续等待按键
		}break;
		case KEY_STATE_2: 											//按键状态2：判定按键有效的种类：是单击，双击，还是长按
		{
			if(key_press) 												//如果按键在 设定的双击时间内释放，则判定为单击/双击
			{
				if(key_time_double <= DOUBLE_KEY_TIME ) key_state = KEY_STATE_0;						//双击时间达到前，再次扫描按键
				if((key_time_double >DOUBLE_KEY_TIME )&&( key_time_double< LONG_KEY_TIME)) 	//达到双击设定时间后
				{
					if(key_press_nums == 1)
					{ TR0 = 0;key_return = Single_click + new_keynum*10;
						key_state = KEY_STATE_0; 					//返回 按键状态0，继续等待按键
					key_press_nums = 0;								//清空按键次数
					key_time_double = 0;							//清空双击计时
					}				//返回有效按键值：单击
					else if(key_press_nums >=2 && new_keynum == keynum) 
					{
					TR0 = 0;key_return = Double_click + new_keynum*10;
					key_state = KEY_STATE_0; 					//返回 按键状态0，继续等待按键
					key_press_nums = 0;								//清空按键次数
					key_time_double = 0;							//清空双击计时
					}//返回有效按键值：双击
				}
			}
			else
			{	
				if(key_time >= LONG_KEY_TIME) 			//如果按键时间超过 设定的长按时间(LONG_KEY_TIME*10ms=80*10ms=800ms), 则判定为 长按
				{
					TR0 = 0;
					key_return = Long_click + new_keynum*10; 	//返回有效键值值：长按
					key_state = KEY_STATE_3; 					//去状态3，等待按键释放
				}
			}
		}break;
		case KEY_STATE_3: 											//等待按键释放
		{
			if (key_press)
			{
				key_state = KEY_STATE_0; 						//按键释放后，进入 按键状态0 ，进行下一次按键的判定
				key_press_nums = 0;									//清空按键次数
				key_time_double=0;
				TR0 = 0;
			}
		}break;
		default:{key_state = KEY_STATE_0;}break;	//特殊情况：key_state是其他值得情况，清零key_state。这种情况一般出现在 没有初始化key_state，第一次执行这个函数的时候
	}
	keynum = new_keynum;											//将新获取的键值赋值给键值
	return key_return; 												//返回 按键值
}