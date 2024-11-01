//		@布丁橘长 2022/11/15
//		点亮共阳极LED电路实验。
//		LED电路由8个LED共阳极连接，每个LED配一个限流电阻。公共端接VCC，低电平点亮。
//		LED引脚定义：P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		本实验采用准双向口模式，低电平点亮8个LED
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1

#include <STC32G.H>

#define ON 	0				//定义0 为 开
#define	OFF	1				//定义1 为 关

sbit LED1 = P2^0;		//引脚定义：LED1->P20
sbit LED2 = P2^1;		//引脚定义：LED2->P21
sbit LED3 = P2^2;		//引脚定义：LED3->P22
sbit LED4 = P2^3;		//引脚定义：LED4->P23
sbit LED5 = P2^4;		//引脚定义：LED5->P24
sbit LED6 = P2^5;		//引脚定义：LED6->P25
sbit LED7 = P2^6;		//引脚定义：LED7->P26
sbit LED8 = P2^7;		//引脚定义：LED8->P27

sbit LED9 	= P7^0;	//引脚定义：LED9-> P70
sbit LED10 	= P7^1;	//引脚定义：LED10->P71
sbit LED11  = P7^2;	//引脚定义：LED11->P72
sbit LED12  = P7^3;	//引脚定义：LED12->P73
sbit LED13  = P7^4;	//引脚定义：LED13->P74
sbit LED14  = P7^5;	//引脚定义：LED14->P75
sbit LED15  = P7^6;	//引脚定义：LED15->P76
sbit LED16  = P7^7;	//引脚定义：LED16->P77
void main()
{
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0xFF;		//设置P7口为推挽输出模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		LED1 = ON;		//点亮LED1
		LED2 = ON;		//点亮LED2
		LED3 = ON;		//点亮LED3
		LED4 = ON;		//点亮LED4	
		LED5 = ON;		//点亮LED5
		LED6 = ON;		//点亮LED6
		LED7 = ON;		//点亮LED7
		LED8 = ON;		//点亮LED8
//		P2 = 0x00;		//点亮P2口所有LED
//		LED1 = OFF;		//熄灭LED1
//		LED2 = OFF;		//熄灭LED2
//		LED3 = OFF;		//熄灭LED3
//		LED4 = OFF;		//熄灭LED4
//		LED5 = OFF;		//熄灭LED5
//		LED6 = OFF;		//熄灭LED6
//		LED7 = OFF;		//熄灭LED7
//		LED8 = OFF;		//熄灭LED8
//		P2 = 0xFF;		//熄灭P2口所有LED
		LED9 = ON;
		LED10 = ON;
		LED11 = ON;
		LED12 = ON;
		LED13 = ON;
		LED14 = ON;
		LED15 = ON;
		LED16 = ON;
//	P7 = 0x00;			//点亮P7口所有LED
	}
}