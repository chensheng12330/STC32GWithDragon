//	@布丁橘长 2023/03/04
//	PWM示例程序，五向开关左右键，控制舵机SG92R任意角度转动，长按连续转动，短按单步转动
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//120分频
#define PWM_PERIOD 		2000		//周期20毫秒
#define PWM_DUTY_0 		50			//占空比2.5%
#define PWM_DUTY_45		100			//占空比5%	
#define PWM_DUTY_90		150			//占空比7.5%
#define PWM_DUTY_135	200			//占空比10%
#define PWM_DUTY_180	250			//占空比12.5%

sbit KEY_LEFT 	= P7^2;				//左键：P7.2
sbit KEY_RIGHT 	= P7^1;				//右键：P7.1

void PWMA_Config(void);				//PWMA初始化设置函数
void Timer0_Init(void);				//定时器0初始化函数
void keyscan();								//按键扫描函数
void Update_duty();						//更新占空比函数

u8 keydelay_left,keydelay_right;
u16 duty;
bit updateduty;
bit servoflag;
void main(void)
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0xFF;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	duty = PWM_DUTY_0;					//舵机初始角度0
	PWMA_Config();							//PWMA初始化设置
	Update_duty();							//更新占空比
	Timer0_Init();							//定时器0初始化
	EA = 1;											//使能EA总中断

	while (1)
	{
		Update_duty();						//循环执行占空比更新函数
	}
}
void PWMA_Config(void)
{
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); //预分频
	
	PWMA_CCER1 = 0x00;					// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCMR1 = 0x60; 					// 设置 CC1 为 PWMA 输出模式,PWM模式1
	PWMA_CCER1 = 0x01;					// 使能 CC1 通道
	
	PWMA_CCR1H = (u16)(PWM_DUTY_0 >> 8); 
	PWMA_CCR1L = (u16)(PWM_DUTY_0); 		//设置初始占空比
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // 设置PWM周期
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x01; 						// 使能 PWM1P 端口输出
	PWMA_BKR = 0x80; 						// 使能主输出
	
	PWMA_CR1 = 0x01; 						// 启动PWM定时器
}
void Timer0_Isr(void) interrupt 1
{
	keyscan();
}
void Timer0_Init(void)				//10毫秒@12.000MHz
{
	TM0PS = 0x00;								//设置定时器时钟预分频 ( 注意:并非所有系列都有此寄存器,详情请查看数据手册 )
	AUXR &= 0x7F;								//定时器时钟12T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0xF0;									//设置定时初始值
	TH0 = 0xD8;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}
void keyscan()								//定时器10毫秒，扫描1次按键，这里不去抖处理
{
//--------------------五向开关-左键-------------------------
	if(KEY_LEFT == 0)						//左键按下
	{
		if(duty < 250) duty += 1;	//角度+1（左转）
		updateduty = 1;						//更新占空比标志置1
	}
//--------------------五向开关-右键-------------------------
	if(KEY_RIGHT == 0 )					//右键按下
	{
		if(duty >  50) duty -= 1 ;	//角度-（右转）
		updateduty = 1;						//更新占空比标志置1
	}
}
void Update_duty()						//更新占空比函数
{
	if(updateduty == 1)					//更新占空比标志为1时
	{
		PWMA_CCR1H = (u8)(duty >> 8); 
		PWMA_CCR1L = duty; 				//重新设置CC1通道占空比
		updateduty = 0;						//占空比更新标志置1
	}
}