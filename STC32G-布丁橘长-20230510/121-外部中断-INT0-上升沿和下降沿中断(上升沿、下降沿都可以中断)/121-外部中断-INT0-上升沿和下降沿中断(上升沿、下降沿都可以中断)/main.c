//	@布丁橘长 2023/04/06
//  外部中断0示例：上升沿和下降沿中断模式（无论上升沿或下降沿都可以中断）
// 	实验效果：单片机运行P2口流水灯，P3.2按键按下，中断流水灯，LED全闪3次后，继续流水灯，P3.2按键松开，中断流水灯，LED闪烁3次，继续流水灯
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"
#include "delay.h"

void sysini(void);						// STC32初始化设置
void Timer0_Init(void);				// 定时器0初始化	
void Blink();									// 流水灯

bit B_1ms;										// 1ms标志
u8 j;													// 闪烁次数		
u16 blinkdelay;								// 流水灯延时

void main(void)
{
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	
	IT0 = 0;										// INT0触发方式：上升沿和下降沿（上升沿和下降沿都可以触发）
//	IT0 = 1;									// INT0触发方式：下降沿触发（只能下降沿触发）
	EX0 = 1;										// 使能INT0
	
	EA = 1;											// 使能EA总中断
	
	while(1)
	{
		Blink();									// 循环执行流水灯
	}
}
void sysini()
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0xFF;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void INT0_Isr(void) interrupt 0
{
	P2 = 0xFF;					// 熄灭所有LED
		
	P2 = ~P2;						
	delayms(200);				// 第1次闪烁 亮
	P2 = ~P2;
	delayms(200);				// 第1次闪烁 灭
	
	P2 = ~P2;
	delayms(200);				// 第2次闪烁 亮
	P2 = ~P2;
	delayms(200);				// 第2次闪烁 灭
	
	P2 = ~P2;					
	delayms(200);				// 第3次闪烁 亮	
	P2 = ~P2;						
	delayms(200);				// 第3次闪烁 灭
}
void Blink()
{
	static u8 i;
	if(B_1ms == 1)
	{
		B_1ms = 0;
		blinkdelay++;
	}
	if(blinkdelay >=500)
	{
		blinkdelay = 0;
		P2 = ~(0x01 << i);
		i++;
		if(i > 7) i = 0;
	}
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 提供1毫秒节拍，可用于延时计数
}
void Timer0_Init(void)				//1毫秒@12.000MHz
{
	AUXR |= 0x80;								//定时器时钟1T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0x20;									//设置定时初始值
	TH0 = 0xD1;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}