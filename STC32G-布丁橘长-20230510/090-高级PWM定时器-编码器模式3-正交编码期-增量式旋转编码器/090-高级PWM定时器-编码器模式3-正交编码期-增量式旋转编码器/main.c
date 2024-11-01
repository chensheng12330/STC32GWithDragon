// @布丁橘长 2023/03/08
// PWM编码器模式-编码器模式3-正交编码器（增量式光电旋转编码器），正转数字加，反转数字减，MAX7219数码管显示
// MAX7219数码管模块引脚定义：CLK = P6^4; CS = P6^5; DIN = P6^6; 
// 编码器A相接PWM1P（P1.0），B相接PWM2P（P5.4）
// 实验开发板：屠龙刀三.1-STC32G12K128-LQFP64 @主频12MHz

#include "STC32G.h"
#include "config.h"
#include "MAX7219.h"

void PWM_Config();						//PWM初始化设置函数
void Encoder_Check();					//编码器检测函数(读取CNTR值)
void SEG_Disp(void);					//MAX7219数码管显示函数

u8 cnt_H, cnt_L;							//计数值高8位、低8位
u16 count,newcount;						//计数值、新计数值
u8 leftmove,rightmove;				//左转标志、右转标志
u16 number;										//数码管计数值
bit numberchange;							//数值改变标志

void main(void)
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00;							  // 设置外部数据总线速度为最快
	WTST = 0x00; 								// 设置程序代码等待参数，赋值为 0 可将 CPU 执行程序的速度设置为最快
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	EA = 1;											//使能EA总中断
	PWM_Config();								//PWM初始化设置
	MAX7219_Ini();							//MAX7219初始化
	SEG_Disp();									//数码管显示函数
	
	while (1)
	{
		if(numberchange == 1)			//数值改变标志置1时
		{
			SEG_Disp();							//数码管更新显示
			numberchange = 0;				//清零数值改变标志
		}
	}
}
void PWM_Config()
{
	PWMA_ENO = 0x00;					  // 配置成 TRGI 的 pin 需关掉 ENO 对应 bit 并配成 input
	
	PWMA_CCER1 = 0x00;					// 清零CCER，关闭通道
	PWMA_CCMR1 = 0x21; 					// 通道模式配置为输入，接编码器 , 滤波器 4 时钟
	PWMA_CCMR2 = 0x21; 					// 通道模式配置为输入，接编码器 , 滤波器 4 时钟
	PWMA_CCER1 = 0x55; 					// 配置通道使能和极性
	PWMA_CCER2 = 0x55; 					// 配置通道使能和极性
	
//	PWMA_SMCR = 0x01; 					// 编码器模式 1
//	PWMA_SMCR = 0x02; 					// 编码器模式 2	
	PWMA_SMCR = 0x03; 					// 编码器模式 3
	
	PWMA_IER = 0x02; 						// 使能中断
	PWMA_CR1 |= 0x01; 					// 使能PWM计数器
}
/******************** PWM 中断读编码器计数值 **************************/
void PWM_ISR() interrupt 26
{
	P20 = ~P20;
	if (PWMA_SR1 & 0X02)				//捕获1中断标志置1时
	{
		cnt_H = PWMA_CCR1H;				//读取PWM计数器高8位		
		cnt_L = PWMA_CCR1L;				//读取PWM计数器低8位
		PWMA_SR1 &= ~0X02;				//清零捕获1中断标志
		Encoder_Check();					//检测编码器计数值
	}
}
void Encoder_Check()					//检测编码器计数值
{
	newcount = cnt_H * 256 + cnt_L;	//获取新的计数器值
	if(newcount < count) 				//如果新计数值 小于 计数值
	{
		leftmove = 1;							//左转标志置1
		count = newcount;					//更新计数值
		numberchange = 1;
		if(number > 0) number--;
	}
	else if(newcount > count)		//如果新计数值 大于 计数值
	{
		rightmove = 1;						//右转标志置1
		count = newcount;					//更新计数值
		numberchange = 1;
		if(number < 999) number++;
	}
}
void SEG_Disp(void)											
{		
	
	Write7219(8,15); 						//左起第1位熄灭
	Write7219(7,15); 						//左起第2位熄灭
	Write7219(6,15); 						//左起第3位熄灭
	Write7219(5,15); 						//左起第4位熄灭
	Write7219(4,15); 						//左起第5位熄灭
	Write7219(3,(u8)((number / 100)%10)); //左起第6位显示数字百位
	Write7219(2,(u8)((number / 10)%10)); 	//左起第7位显示数字十位
	Write7219(1,(u8)(number % 10)); 			//左起第8位显示数字个位
}