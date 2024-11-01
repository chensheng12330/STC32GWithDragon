//	@布丁橘长 2023/03/01
//	PWM示例程序，使用PWM1P输出PWM实现呼吸灯效果，并将PWM1P切换到P2.0引脚PWM1P_2
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//120分频
#define PWM_PERIOD 		1000		//周期10毫秒

void PWMA_Config(void);				//PWMA初始化设置函数
void Update_duty();						//更新占空比函数

u16 duty;											//占空比变量
bit updateduty;								//占空比更新标志
bit blinkflag;								//呼吸灯状态标志

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
	
	PWMA_Config();							//PWMA初始化设置
	Update_duty();							//更新占空比
	EA = 1;											//使能EA总中断
	duty = PWM_PERIOD;					//初始占空比100%，LED熄灭
	while (1)
	{
		Update_duty();						//循环执行占空比更新函数
	}
}
void PWM_ISR() interrupt 26
{
	if(PWMA_SR1 & 0x01)											//PWM中断后，查询中断标志位，更新中断标志是否置1
	{
		PWMA_SR1 &= ~0x01;										//清零更新中断标志位
		if(blinkflag == 0) duty -= 10;				//由暗到亮过程：duty-10
		else if(blinkflag == 1) duty += 10;		//有亮到暗过程：duty+10
		if(duty >= 1000) blinkflag = 0;				//防溢出周期1000
		else if(duty <= 0) blinkflag = 1;			//防溢出
		updateduty = 1;												//占空比更新标志置1
	}
}
void PWMA_Config(void)
{
	PWMA_PS = 0x01;													//PWM1P引脚切换到P2.0	
	
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 						//119+1=120预分频
	
	PWMA_CCER1 = 0x00; 											// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCMR1 = 0x60; 											// 设置 CC1 为 PWMA 输出模式，PWM模式1
	PWMA_CCER1 = 0x01; 											// 使能 CC1 通道
	
	PWMA_CCR1H = (u16)(PWM_PERIOD >> 8); 		// 设置初始占空比100%，LED熄灭
	PWMA_CCR1L = (u16)(PWM_PERIOD); 
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8);			// 设置PWM周期
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x01; 												// 使能 PWM1P 端口输出
	PWMA_BKR = 0x80;												// 使能主输出
	
	PWMA_IER = 0x01;												// 使能更新中断
	PWMA_CR1 = 0x01; 												// 启动PWM定时器
}
void Update_duty()												//更新占空比函数
{
	PWMA_CCR1H = (u8)(duty >> 8); 
	PWMA_CCR1L = duty; 											//更新CC1通道占空比
	updateduty = 0;													//占空比更新标志清零
}