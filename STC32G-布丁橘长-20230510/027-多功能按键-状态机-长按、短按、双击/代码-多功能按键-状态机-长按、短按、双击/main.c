//		@布丁橘长 2022/11/19
//		独立按键-多功能按键-状态机-长按、短按、双击
//		KEY1短按：LED1点亮。 KEY1长按：LED1熄灭。KEY1双击：LED1、LED2点亮，再次双击，LED1、LED2熄灭
//		KEY2短按：LED3点亮。 KEY2长按：LED3熄灭。KEY2双击：LED3、LED4点亮，再次双击，LED3、LED4熄灭
//		KEY3短按：LED5点亮。 KEY3长按：LED5熄灭。KEY3双击：LED5、LED6点亮，再次双击，LED5、LED6熄灭
//		KEY4短按：LED7点亮。 KEY4长按：LED7熄灭。KEY4双击：LED7、LED8点亮，再次双击，LED7、LED8熄灭
//		8位共阳极LED模块，由8个LED共阳极连接，每个LED配一个限流电阻。公共端接VCC，低电平点亮。
//		板载4个独立按键，每个按键一端接GND，另一端串一个300欧电阻后，接P32-P35
//		LED引脚定义：	P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		按键引脚定义：P32->KEY1 P33->KEY2 P34->KEY3 P35->KEY4，在key_multi.h中定义
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1 35MHz 

#include <STC32G.H>
#include <key_multi.h>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

#define ON 	0				//定义0 为 开（共阳极LED电路）
#define	OFF	1				//定义1 为 关（共阳极LED电路）

sbit LED1 = P2^0;		//引脚定义：LED1->P20
sbit LED2 = P2^1;		//引脚定义：LED2->P21
sbit LED3 = P2^2;		//引脚定义：LED3->P22
sbit LED4 = P2^3;		//引脚定义：LED4->P23
sbit LED5 = P2^4;		//引脚定义：LED5->P24
sbit LED6 = P2^5;		//引脚定义：LED6->P25
sbit LED7 = P2^6;		//引脚定义：LED7->P26
sbit LED8 = P2^7;		//引脚定义：LED8->P27

u8 Key1_nums,Key2_nums,Key3_nums,Key4_nums;		//KEY1双击次数，KEY2双击次数，KEY3双击次数，KEY4双击次数
u8 keyvalue;		//键值

void main()
{
	EAXFR = 1; 			// 使能访问 XFR
	CKCON = 0x00; 	// 设置外部数据总线速度为最快
	WTST = 0x00;		// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		keyvalue = key_multi();		//读取键值（键值和长按短按双击的复合键值）
		
		if(keyvalue == Key1_Single_Click) LED1 = ON;			//KEY1短按，点亮LED1
		if(keyvalue == Key1_Long_Click) 	LED1 = OFF;			//KEY1长按，熄灭LED1
		if(keyvalue == Key1_Double_Click) 								//KEY1双击，点亮LED1、LED2，再次双击熄灭LED1、LED2
		{
			Key1_nums++;		//KEY1双击次数
			if(Key1_nums%2 == 1)		//奇数次双击，点亮LED1、LED2
			{
				LED1 = ON;
				LED2 = ON;
			}
			if(Key1_nums%2 == 0)		//偶数次双击，熄灭LED1、LED2
			{
				LED1 = OFF;
				LED2 = OFF;
			}
			if(Key1_nums >= 99) Key1_nums = 0;		//防止KEY1双击次数溢出，清零
		}
		
		if(keyvalue == Key2_Single_Click) LED3 = ON;			//KEY2短按，点亮LED3
		if(keyvalue == Key2_Long_Click) 	LED3 = OFF;			//KEY2长按，熄灭LED3
		if(keyvalue == Key2_Double_Click) 								//KEY3双击，点亮LED3、LED4，再次双击，熄灭LED3、LED4
		{
			Key2_nums++;		//KEY2双击次数
			if(Key2_nums%2 == 1)		//奇数次双击，点亮LED3、LED4
			{
				LED3 = ON;
				LED4 = ON;
			}
			if(Key2_nums%2 == 0)		//偶数次双击，熄灭LED3、LED4
			{
				LED3 = OFF;
				LED4 = OFF;
			}
			if(Key2_nums >= 99) Key2_nums = 0;	//防止KEY2双击次数溢出，清零
		}
		
		if(keyvalue == Key3_Single_Click) LED5 = ON;			//KEY3短按，点亮LED5
		if(keyvalue == Key3_Long_Click) 	LED5 = OFF;			//KEY3长按，熄灭LED5
		if(keyvalue == Key3_Double_Click) 								//KEY3双击，点亮LED5、LED6，再次双击，熄灭LED5、LED6
		{
			Key3_nums++;		//KEY3双击次数
			if(Key3_nums%2 == 1)		//奇数次双击，点亮LED5、LED6
			{
				LED5 = ON;
				LED6 = ON;
			}
			if(Key3_nums%2 == 0)		//偶数次双击，熄灭LED5、LED6
			{
				LED5 = OFF;
				LED6 = OFF;
			}
			if(Key3_nums >= 99) Key3_nums = 0;		//防止KEY3双击次数溢出，清零
		}
		
		if(keyvalue == Key4_Single_Click) LED7 = ON;			//KEY4短按，点亮LED7
		if(keyvalue == Key4_Long_Click) 	LED7 = OFF;			//KEY4长按，熄灭LED7
		if(keyvalue == Key4_Double_Click) 								//KEY4双击，点亮LED7、LED8，再次双击，熄灭LED7、LED8
		{
			Key4_nums++;		//KEY4双击次数
			if(Key4_nums%2 == 1)		//奇数次双击，点亮LED7、LED8	
			{
				LED7 = ON;
				LED8 = ON;
			}
			if(Key4_nums%2 == 0)		//偶数次双击，熄灭LED7、LED8
			{
				LED7 = OFF;
				LED8 = OFF;
			}
			if(Key4_nums >= 99) Key4_nums = 0;		//防止KEY4双击次数溢出，清零
		}
	}
}