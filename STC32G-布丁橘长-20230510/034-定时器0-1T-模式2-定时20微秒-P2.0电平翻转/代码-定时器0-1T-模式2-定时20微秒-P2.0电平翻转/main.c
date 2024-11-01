//		@布丁橘长 2022/11/17
//		定时器0-1T模式-模式2-8位自动重装载模式：定时20微秒，每20微秒翻转一次P2.0电平
//		LED电路由8个LED共阳极连接，每个LED配一个限流电阻。公共端接VCC，低电平点亮。
//		LED引脚定义：P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1 主频12MHz

#include <STC32G.H>

//#define MAIN_Fosc 12000000UL		//定义主频12MHz，请根据实际使用频率修改

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit LED1 = P2^0;		//引脚定义：LED1->P20
sbit LED2 = P2^1;		//引脚定义：LED2->P21
sbit LED3 = P2^2;		//引脚定义：LED3->P22
sbit LED4 = P2^3;		//引脚定义：LED4->P23
sbit LED5 = P2^4;		//引脚定义：LED5->P24
sbit LED6 = P2^5;		//引脚定义：LED6->P25
sbit LED7 = P2^6;		//引脚定义：LED7->P26
sbit LED8 = P2^7;		//引脚定义：LED8->P27

void Timer0_Init(void);		//1毫秒@12.000MHz  定时器0初始化函数声明

void main()
{	
	EAXFR = 1; 			// 使能访问 XFR
	CKCON = 0x00; 	// 设置外部数据总线速度为最快
	WTST = 0x00;		// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0xFF;		//设置P2口为推挽输出模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	Timer0_Init();
	EA = 1;					//使能总中断	
	while(1);
}
void Timer0_Isr(void) interrupt 1
{
	LED1 = ~LED1;
}
void Timer0_Init(void)		//20微秒@12.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x02;			//设置定时器模式-模式2-8位自动重装载
	TL0 = 0x10;				//设置定时初始值
	TH0 = 0x10;				//设置定时重载值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}
