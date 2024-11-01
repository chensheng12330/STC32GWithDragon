// 	@布丁橘长 2023/03/16
// 	PWM输入捕获示例：PWM输入信号测量模式，复位模式，同时测量周期和占空比（高电平脉宽），MAX7219数码管模块显示
//  CC1捕获上升沿，CC2捕获下降沿，使能CC1、CC2捕获中断
//  PWM1P（P1.0）为信号测量引脚，PWM5（P2.0）输出占空比、周期可调的PWM，板载P3.2 P3.3按键调节占空比，P3.4 P3.5按键调节周期
//	方波由PWM5(P2.0)输出，初始周期：1毫秒，初始占空比50%（即高电平500us）
//  MAX7219数码管模块引脚定义：CS = P6^5;DIN = P6^6;CLK = P6^4;（可在MAX7219.h中修改）
//  实验开发板：屠龙刀三.1 @主频12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"

#define PWMB_PSC (12 - 1)			// 12分频，时钟周期1us
#define PWMB_PERIOD 1000			// 周期1000us，1KHz		
#define PWMB_DUTY 500					// 高电平500us，占空比50%

sbit KEY1 = P3^2;							// 板载按键，P3.2
sbit KEY2 = P3^3;							// 板载按键，P3.3
sbit KEY3 = P3^4;							// 板载按键，P3.4
sbit KEY4 = P3^5;							// 板载按键，P3.5

void SYS_Ini();								// STC32初始化设置
void SEG_Disp(void);					// 数码管显示
void PWM_Config();						// PWM初始化设置
void Timer0_Init(void);				// 定时器0初始化
void keyscan();								// 按键扫描

int cap;											// 捕获差值
int cap1,cap2;								// CC1、CC2捕获值
u8 segdelay;									// 数码管延时计数
bit B_1ms;										// 1ms计时标志
u8 key1delay,key2delay;				// 按键1、按键2延时计数
u8 key3delay,key4delay;				// 按键3、按键4延时计数
u16 duty;											// PWM占空比
u16 cap_duty;									// 测量的PWM高电平脉宽
u16 period;										// PWM周期
u16 cap_period;								// 测量的PWM周期
bit updateduty;								// 更新占空比标志
bit updateperiod;							// 更新周期标志

void main(void)
{
	SYS_Ini();									// STC32初始化设置
	PWM_Config();							  // PWM初始化设置
	Timer0_Init();							// 定时器0初始化
	EA = 1;											// 使能EA总中断
	MAX7219_Ini();							// MAX7219初始化
	duty = PWMB_DUTY;						// 初始占空比50%
	period = PWMB_PERIOD;				// 初始周期1000us
	SEG_Disp();									// 数码管显示
	
	while (1)
	{
		if(segdelay == 200)				// 每200毫秒刷新一次数码管
		{
			cap_period = cap1 / 12;	// 计算周期时间(12M时，计数一次为1/12us)
			cap_duty = cap2 / 12;		// 计算高电平时间(12M时，计数一次为1/12us)
			SEG_Disp();							// 刷新数码管
			segdelay = 0;						// 清零数码管计时
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
	if (PWMA_SR1 & 0X02)				// 当CC1捕获中断标志置1
	{	
		PWMA_SR1 &= ~0X02;				// 清零CC1捕获中断标志
		cap1 =(PWMA_CCR1H << 8) + PWMA_CCR1L;	// 读取捕获1计数值CCR1
	}
	if(PWMA_SR1 & 0x04)					// 当CC2捕获中断标志置1
	{
		PWMA_SR1 &= ~0x02;				// 清零CC2捕获中断标志
		cap2 =(PWMA_CCR2H << 8) + PWMA_CCR2L;	// 读取捕获2计数值CCR2
	}
}
void PWM_Config()							// PWM初始化设置
{
//--------------------------PWMA设置-CC1捕获上升沿、CC2捕获下降沿-----------------------	
	PWMA_CCER1 = 0x00;					// 关闭通道
	PWMA_CCMR1 = 0x01; 					// CC1通道配置为输入，映射在TI1FP1上
	PWMA_CCMR2 = 0x02; 					// CC1通道配置为输入，映射在TI1FP2上
	PWMA_CCER1 = 0x11;					// 使能CC1、CC2通道
	PWMA_CCER1 |= 0x00;					// 设置CC1捕获上升沿
	PWMA_CCER1 |= 0x20;					// 设置CC2捕获下降沿
	
	PWMA_SMCR = 0x54;						// 设置为复位模式
	
	PWMA_IER = 0x06; 						// 使能CC1、CC2捕获中断
	PWMA_CR1 |= 0x01; 					// 使能PWMA计数器
	
//------------PWMB设置-PWM5(P2.0)输出一路PWM：初始周期1000us，占空比：50%------------------------
	PWMB_PSCRH = (u16)(PWMB_PSC >> 8);
	PWMB_PSCRL = (u16)(PWMB_PSC);	// 预分频
	
	PWMB_CCER1 = 0x00;					// 关闭通道
	PWMB_CCMR1 = 0x68; 					// 通道模式配置为输出，PWM模式1
	PWMB_CCER1 = 0x05;					// 使能捕获/比较通道5，使能CCR预装载

	PWMB_CCR5H = (u16)(PWMB_PERIOD >> 1 >> 8);
	PWMB_CCR5L = (u16)(PWMB_PERIOD >> 1);	// 设置占空比50%
	
	PWMB_ARRH = (u16)(PWMB_PERIOD >> 8);
	PWMB_ARRL = (u16)(PWMB_PERIOD);	// 设置周期
	
	PWMB_ENO = 0x01;						// 使能PWM5输出
	PWMB_BKR = 0x80;						// 使能PWMB主输出
	PWMB_CR1 |= 0x01; 					// 使能PWMB计数器
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 1毫秒到
	segdelay++;
	keyscan();
	if(updateduty == 1)					// 更新占空比标志置1时
	{
		PWMB_CCR5H = (u16)(duty >> 8);
		PWMB_CCR5L = (u16)(duty);	// 更新占空比
		updateduty = 0;						// 清零更新占空比标志
	}
	if(updateperiod == 1)				// 更新周期标志置1时
	{
		PWMB_ARRH = (u16)(period >> 8);
		PWMB_ARRL = (u16)(period);	// 更新周期
		updateperiod = 0;					// 清零更新周期标志
	}
}
void Timer0_Init(void)				//1毫秒@12.000MHz
{
	AUXR |= 0x80;								//定时器时钟1T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0x20;									//设置定时初始值
	TH0 = 0xD1;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计数
	ET0 = 1;										//使能定时器0中断
}
void keyscan()
{
//--------------------KEY1---------------------------------------------------------------------
	if(KEY1 == 0)								// 按键1按下
	{
		if(B_1ms == 1)						// 1毫秒到
		{
			key1delay++;						// 按键1计时+1
			B_1ms = 0;							// 清零按键1延时计数
		}
		if(KEY1 == 0 && key1delay == 10)	// 按键1按下，并且延时10毫秒到
		{
			if(duty > 50) duty -= 50;			// 高电平时间每次减少50us
			updateduty = 1;								// 更新周期标志置1
		}
	}
	if(KEY1 == 1) key1delay = 0; 				// 松开按键1后，清零按键1计时
//--------------------KEY2----------------------------------------------------------------------
	if(KEY2 == 0)								// 按键2按下
	{
		if(B_1ms == 1)						// 1毫秒到
		{
			key2delay++;						// 按键2计时+1
			B_1ms = 0;							// 清零按键2延时计数
		}
		if(KEY2 == 0 && key2delay == 10)	// 按键2按下，并且延时10毫秒到
		{
			if(duty < 950) duty += 50;			// 高电平时间每次增加50us,最大950（数码管只用了3位显示）
			updateduty = 1;									// 更新周期标志置1
		}
	}
	if(KEY2 == 1) key2delay = 0;				// 松开按键2后，清零按键2计时
	//--------------------KEY3---------------------------------------------------------------------
	if(KEY3 == 0)								// 按键3按下
	{
		if(B_1ms == 1)						// 1毫秒到
		{
			key3delay++;						// 按键3计时+1
			B_1ms = 0;							// 清零按键3延时计数
		}
		if(KEY3 == 0 && key3delay == 10)		// 按键3按下，并且延时10毫秒到
		{
			if(period > 500) period -= 100;		// 周期每次减少100us，最小500us
			updateperiod = 1;									// 更新周期标志置1
		}
	}
	if(KEY3 == 1) key3delay = 0; 					// 松开按键3后，清零按键3计时
//--------------------KEY4----------------------------------------------------------------------
	if(KEY4 == 0)								// 按键4按下
	{
		if(B_1ms == 1)						// 1毫秒到
		{
			key4delay++;						// 按键4计时+1
			B_1ms = 0;							// 清零按键4延时计数
		}
		if(KEY4 == 0 && key4delay == 10)		// 按键4按下，并且延时10毫秒到
		{
			if(period < 2000) period += 100;	// 周期每次增加100us，最大2000us
			updateperiod = 1;									// 更新周期标志置1
		}
	}
	if(KEY4 == 1) key4delay = 0;					// 松开按键4后，清零按键4计时
}
void SEG_Disp(void)						// MAX7219数码管显示函数								
{							
	Write7219(8,(u16)((cap_period / 1000)%10)); // 左起第1位显示 周期千位
	Write7219(7,(u16)((cap_period / 100)%10)); 	// 左起第2位显示 周期百位
	Write7219(6,(u16)((cap_period / 10)%10)); 	// 左起第3位显示 周期十位
	Write7219(5,(u16)(cap_period % 10)); 				// 左起第4位显示 周期个位
	Write7219(4,10); 														// 左起第5位显示 横杆
	Write7219(3,(u16)((cap_duty / 100)%10)); 		// 左起第6位显示 高电平时间百位
	Write7219(2,(u16)((cap_duty / 10)%10)); 		// 左起第7位显示 高电平时间十位
	Write7219(1,(u16)(cap_duty % 10)); 					// 左起第8位显示 高电平时间个位
}