//	@布丁橘长 2023/04/10
//  低功耗模式：IDLE-空闲模式，以及唤醒示例:
//	P2口流水灯完成后，LED熄灭，进入空闲模式，P7.3 IO口中断唤醒，唤醒后，P2口循环流水灯
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"
#include "delay.h"

void sysini(void);						// STC32初始化设置
void Blink();									// P2口流水灯

void main(void)
{
	sysini();										// STC32初始化设置
	
	P7IM1 = 0x00;P7IM0 = 0x00;	// P7.3设置为下降沿中断
// 	P7IM1 = 0x00;P7IM0 = 0x08;	// P7.3设置为上升沿中断
//	P7IM1 = 0x08;P7IM0 = 0x00;	// P7.3设置为低电平中断
//	P7IM1 = 0x08;P7IM0 = 0x08;	// P7.3设置为高电平中断
	
	P7INTE = 0x08;							// 使能P7.3口中断
	
	EA = 1;											// 使能EA总中断
	
	Blink();										// 完成一次P2口流水灯
	P2 = 0xFF;									// 熄灭所有LED

	IDL = 1;										// 进入低功耗模式-IDLE空闲模式
	while (1)
	{
		Blink();									// P2口循环流水灯
	}
}
void IOINT_Isr(void) interrupt 13		// 13号中断为保留中断，因为所有IO口中断号均大于31，这里借用13号中断中转
{																		// 关于Keil中断号扩展详细信息，可以参照布丁橘长-STC32G系列视频第36期，或STC32G手册第5.9章节
	u8 intf;
	intf = P7INTF;
	if (intf)
	{
		P7INTF = 0x00;						// 清零中断标志
		if (intf & 0x01)					// Px.0 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}
		if (intf & 0x02)					// Px.1 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}

		if (intf & 0x04)					// Px.2 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}
		if (intf & 0x08)					// Px.3 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}
		if (intf & 0x10)					// Px.4 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}
		if (intf & 0x20)					// Px.5 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}
		if (intf & 0x40)					// Px.6 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}
		if (intf & 0x80)					// Px.7 口中断(x=0,1,2,3,4,5,6,7)
		{
			
		}
	}
}

void sysini()
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void Blink()									// P2口流水灯
{
	u8 i;
	for(i = 0;i < 8;i++)
	{
		P2 = ~(0x01 << i);				// 移位法
		delayms(200);							// 延时200ms
	}
}