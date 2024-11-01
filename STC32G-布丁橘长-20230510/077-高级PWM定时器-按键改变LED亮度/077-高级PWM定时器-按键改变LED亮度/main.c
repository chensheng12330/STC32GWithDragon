//	@布丁橘长 2023/03/01
//	PWM示例程序，PWM1P切换到P2.0引脚PWM1P_2，五向开关左右键，增加/减小P2.0LED的亮度
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//120分频
#define PWM_PERIOD 		1000	//周期10毫秒

sbit KEY_UP 		= P7^4;				//上键：P7.4
sbit KEY_DOWN 	= P7^3;				//下键：P7.3
sbit KEY_LEFT 	= P7^2;				//左键：P7.2
sbit KEY_RIGHT 	= P7^1;				//右键：P7.1
sbit KEY_MIDDLE = P7^0;				//中键：P7.0

void PWMA_Config(void);				//PWMA初始化设置函数
void Update_duty();						//更新占空比函数
void keyscan();								//按键扫描函数
void Timer0_Init(void);				//定时器0初始化函数

u16 duty;											//占空比变量
bit dutyupdate;								//占空比更新标志
bit B_10ms;										//10毫秒计时标志
u8 keydelay_left;							//左键延时计数
u8 keydelay_right;						//右键延时计数

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
	Timer0_Init();							//定时器0初始化
	EA = 1;											//使能EA总中断
	duty = PWM_PERIOD;					//初始占空比100%，LED熄灭
	while (1)
	{
		Update_duty();						//循环执行占空比更新函数
	}
}
void Timer0_Isr(void) interrupt 1
{
	B_10ms = 1;									//10毫秒计时标志
	keyscan();									//每10毫秒，执行一次按键扫描
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
void PWMA_Config(void)
{
	PWMA_PS = 0x01;											//PWM1P引脚切换到P2.0
	
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 				//119+1=120预分频
	
	PWMA_CCER1 = 0x00; 									// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCMR1 = 0x60; 									// 设置 CC1 为 PWMA 输出模式，PWM模式1
	PWMA_CCER1 = 0x01; 									// 使能 CC1 CC2通道
	
	PWMA_CCR1H = (u16)(PWM_PERIOD >> 8);// 设置初始占空比100
	PWMA_CCR1L = (u16)(PWM_PERIOD); 
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // 设置PWM周期
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x01; 										// 使能 PWM1P 端口输出
	PWMA_BKR = 0x80;										// 使能主输出
	
	PWMA_CR1 = 0x01; 										// 启动PWM定时器
}
void Update_duty()										//更新占空比函数
{
	PWMA_CCR1H = (u8)(duty >> 8); 
	PWMA_CCR1L = duty; 									//更新CC1通道占空比
	dutyupdate = 0;											//更新占空比标志清零
}
void keyscan()
{
//--------------------五向开关-左键-------------------------
	if(KEY_LEFT == 0)
	{
		if(B_10ms == 1)										//10毫秒到
		{
			B_10ms = 0;											//清零10毫秒标志
			keydelay_left++;								//五向开关左键，延时+1（每次10毫秒）
		}
		if(KEY_LEFT == 0 && keydelay_left == 2)		//左键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(duty < 1000) duty += 100;
			dutyupdate = 1;									//占空比更新标志置1
		}
	}
	if(KEY_LEFT == 1) keydelay_left = 0;//松开按键后，清零左键延时计数
//--------------------五向开关-右键-------------------------
	if(KEY_RIGHT == 0 )			
	{
		if(B_10ms == 1)										//10毫秒到
		{
			B_10ms = 0;											//清零10毫秒标志
			keydelay_right++;								//五向开关右键，延时+1（每次10毫秒）
		}
		if(KEY_RIGHT == 0 && keydelay_right == 2)		//左键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(duty > 0) duty -= 100;
			dutyupdate = 1;														//占空比更新标志置1
		}
	}
	if(KEY_RIGHT == 1) keydelay_right = 0;				//松开按键后，清零右键延时计数
}