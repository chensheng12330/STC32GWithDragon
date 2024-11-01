//	@布丁橘长 2023/03/02
//	PWM-比较输出模式-翻转电平演示程序，使用中间对齐模式，
//	PWMA：PWM1P/2P/3P/4P占空比10%、20%、30%、40%，
//	PWMB：PWM5/6/7/8占空比50%、60%、70%、80%，
//	8个通道接8位共阳极LED模块，LED效果：LED1-LED8亮度从低到高排列
//	LED模块引脚定义：LED1-LED8：P1.0 P5.4 P1.4 P1.6 P2.0 P2.1 P2.2 P2.3
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 		(240 - 1)	//240分频
#define PWM_PERIOD 	100				//半周期0.5毫秒

void PWM_Config(void);				//PWMA初始化设置函数

void main(void)
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	PWM_Config();								//PWMA初始化设置
	while (1);
}

void PWM_Config(void)
{
//----------------------------PWMA设置---------------------------------------------------
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 				//240预分频
	
	PWMA_CCER1 = 0x00; 									// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCER2 = 0x00;									
	PWMA_CCMR1 = 0x30; 									// 设置 CC1 为 PWMA 输出模式，翻转电平
	PWMA_CCMR2 = 0x30; 									// 设置 CC2 为 PWMA 输出模式，翻转电平
	PWMA_CCMR3 = 0x30; 									// 设置 CC3 为 PWMA 输出模式，翻转电平
	PWMA_CCMR4 = 0x30; 									// 设置 CC4 为 PWMA 输出模式，翻转电平
	
	PWMA_CCER1 = 0x11; 									// 使能 CC1 CC2通道
	PWMA_CCER2 = 0x11; 									// 使能 CC3 CC4通道
	
	PWMA_CCR1H = (u16)(10 >> 8);				// 设置 CC1 初始占空比 10 %
	PWMA_CCR1L = (u16)(10); 
	PWMA_CCR2H = (u16)(20 >> 8);				// 设置 CC2 初始占空比 20 %
	PWMA_CCR2L = (u16)(20); 
	PWMA_CCR3H = (u16)(30 >> 8);				// 设置 CC3 初始占空比 30 %
	PWMA_CCR3L = (u16)(30); 
	PWMA_CCR4H = (u16)(40 >> 8);				// 设置 CC4 初始占空比 40 %
	PWMA_CCR4L = (u16)(40); 
	
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // 设置PWM周期
	PWMA_ARRL = PWM_PERIOD; 
	
	PWMA_ENO = 0x55; 										// 使能 PWM1P/2P/3P/4P 端口输出
	PWMA_BKR = 0x80;										// 使能主输出
	
	PWMA_CR1 = 0x21; 										// 启动PWMA定时器，中央对齐模式
//----------------------------PWMB设置---------------------------------------------------
	PWMB_PSCRH = (u16)(PWM_PSC >> 8);
	PWMB_PSCRL = (u16)(PWM_PSC); 				//240预分频
	
	PWMB_CCER1 = 0x00; 									// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMB_CCER2 = 0x00;									
	PWMB_CCMR1 = 0x30; 									// 设置 CC5 为 PWMB 输出模式，翻转电平
	PWMB_CCMR2 = 0x30; 									// 设置 CC6 为 PWMB 输出模式，翻转电平
	PWMB_CCMR3 = 0x30; 									// 设置 CC7 为 PWMB 输出模式，翻转电平
	PWMB_CCMR4 = 0x30; 									// 设置 CC8 为 PWMB 输出模式，翻转电平
	
	PWMB_CCER1 = 0x11; 									// 使能 CC5 CC6通道
	PWMB_CCER2 = 0x11; 									// 使能 CC7 CC8通道
	
	PWMB_CCR5H = (u16)(50 >> 8);				// 设置 CC5 初始占空比 50 %
	PWMB_CCR5L = (u16)(50); 
	PWMB_CCR6H = (u16)(60 >> 8);				// 设置 CC6 初始占空比 60 %
	PWMB_CCR6L = (u16)(60); 
	PWMB_CCR7H = (u16)(70 >> 8);				// 设置 CC7 初始占空比 70 %
	PWMB_CCR7L = (u16)(70); 
	PWMB_CCR8H = (u16)(80 >> 8);				// 设置 CC8 初始占空比 80 %
	PWMB_CCR8L = (u16)(80); 
	
	PWMB_ARRH = (u16)(PWM_PERIOD >> 8); // 设置PWM周期
	PWMB_ARRL = (u16)(PWM_PERIOD); 
	
	PWMB_ENO = 0x55; 										// 使能 PWM5/6/7/8 端口输出
	PWMB_BKR = 0x80;										// 使能主输出
	
	PWMB_CR1 = 0x21; 										// 启动PWMB定时器，中央对齐模式
}