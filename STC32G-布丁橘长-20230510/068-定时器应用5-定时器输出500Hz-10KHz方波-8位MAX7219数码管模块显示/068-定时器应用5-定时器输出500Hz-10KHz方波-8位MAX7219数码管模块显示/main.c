//	@布丁橘长 2023/02/17
//	定时器输出不同频率时钟，按键选择频率（500Hz 1K 2K 5K 10K 20KHz），P3.2按键-频率加，P3.3按键-频率减
//	8位Max7219数码管模块显示，CS = P6^5; DIN = P6^6; CLK = P6^4;
//	定时器0时钟分频输出引脚：P3.5
//	STC32G12K128-LQFP64 屠龙刀三.1开发板 @主频12MHz

#include <STC32G.H>
#include "MAX7219.h"
#include "config.h"
#include "delay.h"

sbit  KEY1 = P3^2;			//P3.2按键-频率加
sbit 	KEY2 = P3^3;			//P3.3按键-频率减

void  delayms(u16 ms);	
void 	keyscan();
void Timer0_Init(void);

u8 key1press,key2press;
u8 number;
u16 freq;
//重装载值：频率	500  1K		2K	 5K		10K		20K  Hz
u8 TH0_Reload[7]={0xD1,0xE8,0xF4,0xFB,0xFD,0xFE};				//TH0不同频率对应的装载值
u8 TL0_Reload[7]={0x20,0x90,0x48,0x50,0xA8,0xD4};				//TL0不同频率对应的装载值

void main()
{
	EAXFR = 1; 			// 使能访问 XFR
	CKCON = 0x00; 	// 设置外部数据总线速度为最快
	WTST = 0x00;		// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	Timer0_Init();		//定时器0初始化
	T0CLKO = 1;  			//使能T0时钟输出
	
  Initial();       	//MAX7219初始化
	freq = 0;					//MAX7219显示的输出频率值
  while(1)
  { 
		keyscan();			//扫描按键
		switch(number)	//判断按键加减对应的number值，并装载不同频率的定时器装载值
		{
			case 0:TR0 = 0;freq = 0;break;																							//number=0，关闭定时器，不输出时钟
			case 1:TH0 = TH0_Reload[0];TL0 = TL0_Reload[0];TR0 = 1;freq = 500;break;		//number=1,装载500HZ对应的装载值，并启动定时器
			case 2:TH0 = TH0_Reload[1];TL0 = TL0_Reload[1];TR0 = 1;freq = 1000;break;		//number=2,装载1KHZ对应的装载值，并启动定时器
			case 3:TH0 = TH0_Reload[2];TL0 = TL0_Reload[2];TR0 = 1;freq = 2000;break;		//number=3,装载2KHZ对应的装载值，并启动定时器
			case 4:TH0 = TH0_Reload[3];TL0 = TL0_Reload[3];TR0 = 1;freq = 5000;break;		//number=4,装载5KHZ对应的装载值，并启动定时器
			case 5:TH0 = TH0_Reload[4];TL0 = TL0_Reload[4];TR0 = 1;freq = 10000;break;	//number=5,装载10KHZ对应的装载值，并启动定时器
			case 6:TH0 = TH0_Reload[5];TL0 = TL0_Reload[5];TR0 = 1;freq = 20000;break;	//number=6,装载20KHZ对应的装载值，并启动定时器
			default:break;
		}
		Write7219(8,15);											//数码管左边第1位，熄灭
		Write7219(7,15);											//数码管左边第2位，熄灭
		Write7219(6,15);											//数码管左边第3位，熄灭
		Write7219(5,(u8)(freq/10000%10)); 		//数码管左边第4位，频率万位
		Write7219(4,(u8)(freq/1000%10)); 			//数码管左边第5位，频率千位
		Write7219(3,(u8)(freq/100%10)); 			//数码管左边第6位，频率百位
		Write7219(2,(u8)(freq/10%10)); 				//数码管左边第7位，频率十位
		Write7219(1,(u8)(freq%10)); 					//数码管左边第8位，频率个位
  }
}
void keyscan()
{
//------------KEY1----------------------	
	if(KEY1 == 0 && key1press == 0)					//按键1按下，并且按键状态标志是0
	{
		delayms(10);													//延时去抖
		if(KEY1 == 0 && key1press == 0)				//再次判断按键是否按下
		{
			key1press = 1;											//按下的话，按键1状态置1
			if(number < 6)  number++;						//number+1
		}
	}
	if(KEY1 == 1) key1press = 0;						//按键1松开后，清零按键1状态
//------------KEY2----------------------	
	if(KEY2 == 0 && key2press == 0)					//按键2按下，并且按键状态标志是0
	{
		delayms(10);													//延时去抖
		if(KEY2 == 0 && key2press == 0)				//再次判断按键是否按下
		{
			key2press = 1;											//按下的话，按键2状态置1
			if(number > 0) number--;						//number-1
		}
	}
	if(KEY2 == 1) key2press = 0;						//按键2松开后，清零按键2状态
}

void Timer0_Init(void)		
{
	T0x12 = 1;							//定时器时钟1T模式
	TMOD &= 0xF1;						//设置定时器模式-模式1-16位不自动重装载
	TH0	= 0;
	TL0 = 0;								//初始值0
}