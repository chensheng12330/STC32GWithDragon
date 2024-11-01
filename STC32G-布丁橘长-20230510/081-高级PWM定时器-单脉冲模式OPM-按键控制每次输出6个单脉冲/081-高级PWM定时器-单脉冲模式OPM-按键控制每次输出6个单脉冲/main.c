//	@布丁橘长 2023/03/06
//	单脉冲模式示例-按键（P3.2）控制输出6个单脉冲，PWM1P（P1.0）输出，低电平脉宽30毫秒，高电平延时70毫秒
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 	(12000-1)		//12000分频，时钟周期1毫秒
#define PWM_PERIOD 	100				//周期100毫秒
#define PWM_DUTY 		70				//高电平70毫秒

sbit KEY1 	= P3^2;						//板载按键：P3.2

void PWMA_Config(void);				//PWMA初始化设置函数
void Timer0_Init(void);				//定时器0初始化函数
void keyscan();								//按键扫描函数

bit B_10ms;										//10ms计时标志
u8 key1delay;									//按键1按下计时
u8 rep;												//脉冲重复次数
u8 pulseflag;									//脉冲发送标志

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
	Timer0_Init();							//定时器0初始化
	EA = 1;											//使能EA总中断

	while (1);
}
void PWMA_Config(void)
{
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 		//预分频
	
	PWMA_CCER1 = 0x00; 					// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCMR1 = 0x78; 					// 设置 CC1 为 PWMA 输出模式，PWM模式2，使能CCR预装载
	PWMA_CCER1 = 0x03; 					// 使能 CC1通道,低电平有效
	
	PWMA_CCR1H = (u16)(PWM_DUTY >> 8); // 设置占空比时间
	PWMA_CCR1L = (u16)(PWM_DUTY); 
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // 设置PWM周期
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x01; 						// 使能 PWM1P 端口输出
	PWMA_BKR = 0x80; 						// 使能主输出
	
	PWMA_IER = 0x01;						// 使能更新中断
	PWMA_CR1 = 0x89; 						// 单脉冲模式，使能ARR预装载，启动定时器
}
void PWM_ISR() interrupt 26
{
	if(PWMA_SR1 & 0x01)					//判断更新中断标志位，是否置1
	{
		PWMA_SR1 &= ~0x01;				//清零更新中断标志位
		if(rep >= 6) 							//重复次数大于等于6
		{
			pulseflag = 0;					//脉冲发送标志清零
			rep = 0;								//重复次数清零
		}
		if(rep < 6 && pulseflag == 1) PWMA_CR1 |= 0x01;	//脉冲次数小于6，启动一次PWM定时器
		rep++;										//重复次数+1
	}
}
void Timer0_Isr(void) interrupt 1
{
	B_10ms = 1;									//10毫秒标志
	keyscan();									//每10毫秒扫描一次按键
}
void Timer0_Init(void)				//10毫秒@12.000MHz
{
	AUXR &= 0x7F;								//定时器时钟12T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0xF0;									//设置定时初始值
	TH0 = 0xD8;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}
void keyscan()
{
	if(KEY1 == 0)								//P3.2按键按下
	{
		if(B_10ms == 1)						//10毫秒到
		{
			B_10ms = 0;							//清零10毫秒标志
			key1delay++;						//左键延时+1（每次10毫秒）
		}
		if(KEY1 == 0 && key1delay == 2)	//左键按下，并且计时次数2次（延时去抖时间在10-20毫秒之间）
		{
			pulseflag = 1;					//脉冲发送标志置1
			PWMA_CR1 |= 0x01;				//启动PWM定时器
		}
	}
	if(KEY1 == 1) key1delay = 0;	//松开按键后，清零按键计时次数
}