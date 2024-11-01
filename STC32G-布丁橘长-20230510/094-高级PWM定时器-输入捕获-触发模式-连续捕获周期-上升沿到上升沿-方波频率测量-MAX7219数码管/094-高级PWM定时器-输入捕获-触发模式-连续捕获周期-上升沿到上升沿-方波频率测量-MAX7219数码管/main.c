// 	@布丁橘长 2023/03/13
// 	PWM输入捕获示例：PWM1P(P1.0)捕获上升沿到上升沿，脉冲计数，计算方波频率，MAX7219数码管显示
//	方波由PWM5(P2.0)输出，上电为500Hz，板载按键，KEY1(P3.2) 频率减，KEY2(P3.3)频率加，4个频率：500Hz,1K,2K,5KHz
//  MAX7219数码管模块引脚定义：CS = P6^5;DIN = P6^6;CLK = P6^4;（可在MAX7219.h中修改）
//  实验开发板：屠龙刀三.1 @主频12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"

#define PWMB_PSC (12 - 1)			// 12分频，时钟周期1us
#define PWMB_PERIOD_500 2000	// 周期2毫秒，500Hz
#define PWMB_PERIOD_1K 1000		// 周期1毫秒，1KHz		
#define PWMB_PERIOD_2K 500		// 周期500us，2KHz
#define PWMB_PERIOD_5K 200		// 周期200us，5KHz

sbit KEY1 = P3^2;							// 板载按键，P3.2
sbit KEY2 = P3^3;							// 板载按键，P3.3

void SYS_Ini();								// STC32初始化设置
void SEG_Disp(void);					// 数码管显示
void PWM_Config();						// PWM初始化设置
void Timer0_Init(void);				// 定时器0初始化
void keyscan();								// 按键扫描

int cap;											// 捕获差值
long int cap_new;							// 当前捕获计数值
int cap_old;									// 上次捕获计数值
u16 updateseg;							  // 更新数码管标志
long int cap_overflow;				// 捕获溢出值
long int freq;								// 方波频率
u8 segdelay;									// 数码管延时计数
bit B_1ms;										// 1ms计时标志
u8 key1delay,key2delay;				// 按键1、按键2延时计数
u16 period;										// PWM周期
u8 number;										// PWM周期对应的数值：0-3	
bit updateperiod;							// 更新周期标志

void main(void)
{
	SYS_Ini();									// STC32初始化设置
	PWM_Config();							  // PWM初始化设置
	Timer0_Init();							// 定时器0初始化
	EA = 1;											// 使能EA总中断
	MAX7219_Ini();							// MAX7219初始化
	SEG_Disp();									// 数码管显示
	period = PWMB_PERIOD_500;		// 初始PWM频率500Hz
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
	if(PWMA_SR1 & 0x01)					// 更新中断(PWM计数器溢出)
	{
		PWMA_SR1 &= ~0x01;				// 清零更新中断标志
		cap_overflow += 65536;		// 捕获值要加上计数器一次溢出65536
	}
	if (PWMA_SR1 & 0X02)				// 当捕获中断标志置1
	{	
		PWMA_SR1 &= ~0X02;				// 清零捕获中断标志
		cap_old = cap_new;				// 更新当前捕获值
		cap_new =(PWMA_CCR1H << 8) + PWMA_CCR1L;	// 读取计数值低8位,并合并存入cap_new
		cap = cap_new + cap_overflow - cap_old;		// 计算两次捕获差值
		cap_overflow = 0;					// 清零捕获溢出补偿值
	}
}
void PWM_Config()							// PWM初始化设置
{
//--------------------------PWMA设置-捕获上升沿----------------------------------	
	PWMA_ENO = 0x00;						// 关闭输出使能
	PWMA_CCER1 = 0x00;					// 关闭通道
	PWMA_CCMR1 = 0x01; 					// 通道模式配置为输入
	PWMA_SMCR = 0x56;					  // 输入1，触发模式
	PWMA_CCER1 = 0x01;					// 使能捕获/比较通道1
  PWMA_CCER1 |= 0x00;					// 设置捕获/比较通道1上升沿捕获
//	PWMA_CCER1 |= 0x20;					// 设置捕获/比较通道1下降沿捕获

	PWMA_IER = 0x03; 						// 使能捕获中断、更新中断
	PWMA_CR1 |= 0x01; 					// 使能PWMA计数器
	
//------------PWMB设置-PWM5(P2.0)输出一路PWM：500Hz-5KHz，占空比：50%-------------------
	PWMB_PSCRH = (u16)(PWMB_PSC >> 8);
	PWMB_PSCRL = (u16)(PWMB_PSC);	// 预分频
	
	PWMB_CCER1 = 0x00;					// 关闭通道
	PWMB_CCMR1 = 0x68; 					// 通道模式配置为输出，PWM模式1
	PWMB_CCER1 = 0x05;					// 使能捕获/比较通道1，使能CCR预装载

	PWMB_CCR5H = (u16)(PWMB_PERIOD_500 >> 1 >> 8);
	PWMB_CCR5L = (u16)(PWMB_PERIOD_500 >> 1);	// 设置占空比50%
	
	PWMB_ARRH = (u16)(PWMB_PERIOD_500 >> 8);
	PWMB_ARRL = (u16)(PWMB_PERIOD_500);	// 设置周期
	
	PWMB_ENO = 0x01;						// 使能PWM5输出
	PWMB_BKR = 0x80;						// 使能PWMB主输出
	PWMB_CR1 |= 0x01; 					// 使能PWMB计数器
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;
	keyscan();
	if(updateperiod == 1)				// 更新周期标志置1时
	{
		PWMB_ARRH = (u16)(period >> 8);
		PWMB_ARRL = (u16)(period);				// 更新周期
		PWMB_CCR5H = (u16)(period >> 1 >> 8);
		PWMB_CCR5L = (u16)(period >> 1);	// 更新占空比
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
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}
void keyscan()
{
//--------------------KEY1--------------------------------------------------------------
	if(KEY1 == 0)								// 按键1按下
	{
		if(B_1ms == 1)						// 1毫秒到
		{
			key1delay++;						// 按键1计时+1
			B_1ms = 0;							// 清零按键1延时计数
		}
		if(KEY1 == 0 && key1delay == 10)	// 按键1按下，并且延时10毫秒到
		{
			if(number > 0) number -= 1;			// number-1，number对应4个频率：500Hz，1K，2K，5K
			updateperiod = 1;								// 更新周期标志置1
		}
	}
	if(KEY1 == 1) key1delay = 0; 				// 松开按键后，清零按键1计时
//--------------------KEY2---------------------------------------------------------------
	if(KEY2 == 0)								// 按键2按下
	{
		if(B_1ms == 1)						// 1毫秒到
		{
			key2delay++;						// 按键2计时+1
			B_1ms = 0;							// 清零按键2延时计数
		}
		if(KEY2 == 0 && key2delay == 10)	// 按键1按下，并且延时10毫秒到
		{
			if(number < 3) number += 1;			// number+1，number对应4个频率：500Hz，1K，2K，5K
			updateperiod = 1;								// 更新周期标志置1
		}
	}
	if(KEY2 == 1) key2delay = 0;				// 松开按键2后，清零按键2计时
	switch(number)											
	{
		case 0:period = PWMB_PERIOD_500;break;	// number = 0: 500Hz
		case 1:period = PWMB_PERIOD_1K;break;		// number = 1: 1KHz
		case 2:period = PWMB_PERIOD_2K;break;		// number = 2: 2KHz
		case 3:period = PWMB_PERIOD_5K;break;		// number = 3: 5KHz
		default:break;
	}
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