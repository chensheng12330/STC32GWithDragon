//		@布丁橘长 2023/02/02
//		独立按键-按键KEY1点亮LED1，按键KEY2熄灭LED1，按键KEY3点亮8个LED，按键KEY4熄灭8个LED
//		开发板板载按键：KEY1-P3.2，KEY2-P3.3，KEY3-P3.4，KEY4-P3.5
//		开发板板载LED：LED1-P2.0，LED2-P2.1，LEDF3-P2.2，LED4-P2.3，LED5-P2.4，LED6-P2.5，LED7-P2.6，LED8-P2.7
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit LED1 = P2^0;		//板载LED，LED1-P2.0
sbit KEY1 = P3^2;		//板载按键，KEY1-P3.2
sbit KEY2 = P3^3;		//板载按键，KEY2-P3.3
sbit KEY3 = P3^4;		//板载按键，KEY3-P3.4
sbit KEY4 = P3^5;		//板载按键，KEY4-P3.5

void delayms(u16 ms);		//延时函数声明
void keyscan();					//按键扫描函数			
	
void main()
{
	EAXFR = 1;				//使能XFR访问
	CKCON = 0x00;			//设置外部数据总线速度最快
	WTST = 0x00;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		keyscan();
	}
}

void  delayms(u16 ms)		//简单延时函数，自适应主频，1毫秒*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}
void keyscan()
{
	if(KEY1 == 0)					//判断按键KEY1是否按下
	{	
		delayms(10);				//延时消抖
		if(KEY1 == 0)				//再次判断KEY1是否按下
		{
			LED1 = 0;					//共阳极LED电路，低电平点亮，点亮LED1
		}
	}
	if(KEY2 == 0)					//判断按键KEY2是否按下
	{
		delayms(10);				//延时消抖
		if(KEY2 == 0)				//再次判断KEY1是否按下
		{
			LED1 = 1;					//共阳极LED电路，高电平熄灭，熄灭LED1
		}
	}
	if(KEY3 == 0)					//判断按键KEY3是否按下
	{
		delayms(10);				//延时消抖
		if(KEY3 == 0)				//再次判断KEY1是否按下
		{
			P2 = 0x00;				//共阳极LED电路，点亮P2口所有LED
		}
	}
	if(KEY4 == 0)					//判断按键KEY4是否按下
	{
		delayms(10);				//延时消抖
		if(KEY4 == 0)				//再次判断KEY1是否按下
		{
			P2 = 0xFF;				//共阳极LED电路，熄灭P2口所有LED
		}
	}
}