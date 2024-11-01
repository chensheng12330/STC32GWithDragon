// 	@布丁橘长 2023/03/13
// 	PWM输入捕获示例：PWM1P(P1.0)捕获下降沿到下降沿一次，计算方波频率，MAX7219数码管显示
//	方波由PWM5(P2.0)输出，默认输出500Hz 占空比50%方波
//  MAX7219数码管模块引脚定义：CS = P6^5;DIN = P6^6;CLK = P6^4;（可在MAX7219.h中修改）
//  实验开发板：屠龙刀三.1 @主频12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"

#define PWMB_PSC (12 - 1)			// 12分频，时钟周期1us
#define PWMB_PERIOD_500 2000	// 周期2毫秒，500Hz

void SYS_Ini();								// STC32初始化设置
void SEG_Disp(void);					// 数码管显示
void PWM_Config();						// PWM初始化设置
void Timer0_Init(void);				// 定时器0初始化

int cap;											// 捕获差值
long int cap_new;							// 当前捕获计数值
int cap_old;									// 上次捕获计数值
u16 updateseg;							  // 更新数码管标志
long int freq;								// 方波频率
u8 segdelay;									// 数码管延时计数
bit B_1ms;										// 1ms计时标志
u8 capnums;									  // 捕获次数 

void main(void)
{
	SYS_Ini();									// STC32初始化设置
	PWM_Config();							  // PWM初始化设置
	Timer0_Init();							// 定时器0初始化
	EA = 1;											// 使能EA总中断
	MAX7219_Ini();							// MAX7219初始化
	SEG_Disp();									// 数码管显示

	while (1)
	{
		if(B_1ms == 1)						// 1毫秒到
		{
			segdelay++;							// 数码管延时+1
			B_1ms = 0;							// 清零1毫秒标志
			if(segdelay == 200)			// 每200毫秒刷新一次数码管
			{
				freq = (u16)(MAIN_Fosc / cap)+1;	// 计算方波频率，此处+1是为了加上被舍弃的小数部分
				SEG_Disp();						// 刷新数码管
				segdelay = 0;					// 清零数码管延时计数
			}
		}
	}
}
void SYS_Ini()								// STC32初始化设置
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00; 								// 设置程序代码等待参数，赋值为 0 可将 CPU 执行程序的速度设置为最快
	
	P0M1 = 0x00;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0xFF;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void PWM_ISR() interrupt 26
{
	if (PWMA_SR1 & 0X02)				// 当捕获中断标志置1
	{	
		PWMA_SR1 &= ~0X02;				// 清零捕获中断标志
		cap_old = cap_new;				// 更新当前捕获值
		cap_new =(PWMA_CCR1H << 8) + PWMA_CCR1L;	// 读取计数值低8位,并合并存入cap_new
		cap = cap_new - cap_old;		// 计算两次捕获差值
		capnums++;
		if(capnums == 2) PWMA_CCER1 = 0x00;	//关闭捕获/比较通道1
	}
}
void PWM_Config()							// PWM初始化设置
{
//--------------------------PWMA设置-捕获上升沿----------------------------------	
	PWMA_ENO = 0x00;						// 关闭捕获/输出通道
	PWMA_CCER1 = 0x00;					// 关闭通道
	PWMA_CCMR1 = 0x01; 					// 通道模式配置为输入
	PWMA_SMCR = 0x56;					  // 输入捕获，触发模式
	PWMA_CCER1 = 0x01;					// 使能捕获/比较通道1
//  PWMA_CCER1 |= 0x00;					// 设置捕获/比较通道1上升沿捕获
	PWMA_CCER1 |= 0x20;					// 设置捕获/比较通道1下降沿捕获

	PWMA_IER = 0x03; 						// 使能捕获中断、更新中断
	PWMA_CR1 |= 0x01; 					// 使能PWMA计数器
	
//------------PWMB设置-PWM5(P2.0)输出一路PWM：500Hz，占空比：50%-------------------
	PWMB_PSCRH = (u16)(PWMB_PSC >> 8);
	PWMB_PSCRL = (u16)(PWMB_PSC);	// 预分频
	
	PWMB_CCER1 = 0x00;					// 关闭通道
	PWMB_CCMR1 = 0x68; 					// 通道模式配置为输出，PWM模式1
	PWMB_CCER1 = 0x05;					// 使能捕获/比较通道1，使能CCR预装载

	PWMB_CCR5H = (u16)(PWMB_PERIOD_500 >> 2 >> 8);
	PWMB_CCR5L = (u16)(PWMB_PERIOD_500 >> 2);	// 设置占空比50%
	
	PWMB_ARRH = (u16)(PWMB_PERIOD_500 >> 8);
	PWMB_ARRL = (u16)(PWMB_PERIOD_500);	// 设置周期
	
	PWMB_ENO = 0x01;						// 使能PWM5输出
	PWMB_BKR = 0x80;						// 使能PWMB主输出
	PWMB_CR1 |= 0x01; 					// 使能PWMB计数器
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;
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
void SEG_Disp(void)													// MAX7219数码管显示函数								
{							
	Write7219(8,15); 													// 左起第1位熄灭
	Write7219(7,15); 													// 左起第2位熄灭
	Write7219(6,15); 													// 左起第3位熄灭
	Write7219(5,15); 													// 左起第4位熄灭
	Write7219(4,(u16)((freq / 1000)%10)); 		// 左起第5位显示频率千位
	Write7219(3,(u16)((freq / 100)%10)); 			// 左起第6位显示频率百位
	Write7219(2,(u16)((freq / 10)%10)); 			// 左起第7位显示频率十位
	Write7219(1,(u16)(freq % 10)); 						// 左起第8位显示频率个位
}