//	@布丁橘长 2023/04/10
//  低功耗模式：IDLE-空闲模式，以及唤醒示例:
//	P2口流水灯完成后，LED熄灭，进入空闲模式，IIC的SDA引脚下降沿中断唤醒，唤醒后，P2口循环流水灯
//  使用外部32.768KHz晶振，屠龙刀开发板上，需要断开R133
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"
#include "delay.h"

void sysini(void);						// STC32初始化设置
void Blink();									// P2口流水灯
void RTC_config(void);				//RTC初始化设置
void main(void)
{
	sysini();										// STC32初始化设置
	
	I2C_S1 = 0; I2C_S0 = 0; 		//SDA/P1.4 下降沿唤醒
// I2C_S1 = 0; I2C_S0 = 1; 		//SDA_2/P2.4 下降沿唤醒
// I2C_S1 = 1; I2C_S0 = 1; 		//SDA_4/P3.3 下降沿唤醒
	I2CCFG = 0x80; 							// 使能 I2C 模块的从机模式
	I2CSLCR = 0x40; 						// 使能起始信号中断
	
	EA = 1;											// 使能EA总中断
	
	Blink();										// 完成一次P2口流水灯
	P2 = 0xFF;									// 熄灭所有LED

	IDL = 1;										// 进入低功耗模式-IDLE空闲模式
	while (1)
	{
		Blink();									// P2口循环流水灯
	}
}
void IIC_isr() interrupt 24
{
	I2CSLST &= ~0x40;						// 清零中断标志
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