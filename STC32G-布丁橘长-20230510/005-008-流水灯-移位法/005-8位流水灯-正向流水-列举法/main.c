//		@布丁橘长 2022/11/15
//		8位流水灯实验-正向流水
//		LED1-LED8 逐个点亮，点亮时间500ms，循环，本实验采用 移位法 实现流水灯效果
//		LED电路由8个LED共阳极连接，每个LED配一个限流电阻。公共端接VCC，低电平点亮。（屠龙刀三.1板载LED电路）
//		LED引脚定义：P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		本实验采用准双向口模式，低电平点亮、高电平熄灭
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz
// 
#include <STC32G.H>

#define MAIN_Fosc  35000000UL		//定义主频 35MHz，请按实际频率修改（用于delay函数自适应主频）

#define ON  0				//共阳极LED，低电平点亮
#define OFF 1				//共阳极LED，高电平熄灭

typedef unsigned char u8;					//unsigned char 类型用 		u8 表示
typedef unsigned int u16;					//unsigned int	类型用 		u16表示
typedef unsigned long int u32;		//unsigned long int类型用 u32表示

sbit LED1 = P2^0;		//引脚定义：LED1：P2.0
sbit LED2 = P2^1;		//引脚定义：LED1：P2.1
sbit LED3 = P2^2;		//引脚定义：LED1：P2.2
sbit LED4 = P2^3;		//引脚定义：LED1：P2.3
sbit LED5 = P2^4;		//引脚定义：LED1：P2.4
sbit LED6 = P2^5;		//引脚定义：LED1：P2.5
sbit LED7 = P2^6;		//引脚定义：LED1：P2.6
sbit LED8 = P2^7;		//引脚定义：LED1：P2.7

void  delayms(u16 ms);		//简单延时函数

void main()
{
	WTST = 0;		//设置等待时间为0个时钟，CPU运行速度最快
	
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		LED1 = ON;				//点亮LED1
		delayms(500);			//延时500毫秒
		LED1 = OFF;				//熄灭LED1
		
		LED2 = ON;				//点亮LED2
		delayms(500);			//延时500毫秒
		LED2 = OFF;				//熄灭LED2
		
		LED3 = ON;				//点亮LED3
		delayms(500);			//延时500毫秒
		LED3 = OFF;				//熄灭LED3
		
		LED4 = ON;				//点亮LED4
		delayms(500);			//延时500毫秒
		LED4 = OFF;				//熄灭LED4
		
		LED5 = ON;				//点亮LED5
		delayms(500);			//延时500毫秒
		LED5 = OFF;				//熄灭LED5
		
		LED6 = ON;				//点亮LED6
		delayms(500);			//延时500毫秒
		LED6 = OFF;				//熄灭LED6
		
		LED7 = ON;				//点亮LED7
		delayms(500);			//延时500毫秒
		LED7 = OFF;				//熄灭LED7
		
		LED8 = ON;				//点亮LED8
		delayms(500);			//延时500毫秒
		LED8 = OFF;				//熄灭LED8
	}
}

void  delayms(u16 ms)		//简单延时函数，自适应主频，1毫秒*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}