//		@布丁橘长 2022/11/15
//		8位流水灯实验-反向流水
//		LED8->LED1 逐个点亮，点亮时间500ms，循环，本实验采用 移位法 实现流水灯效果
//		LED电路由8个LED共阳极连接，每个LED配一个限流电阻。公共端接VCC，低电平点亮。（屠龙刀三.1板载LED电路）
//		LED引脚定义：P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		本实验采用准双向口模式，低电平点亮、高电平熄灭
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz
// 
#include <STC32G.H>

#define u8 		unsigned char
#define u16 	unsigned int
#define u32		unsigned long

#define MAIN_Fosc  35000000UL		//定义主频 35MHz，请按实际频率修改（用于delay函数自适应主频）

void  delayms(u16 ms);		//简单延时函数

void main()
{
	u8 i;
	
	WTST = 0;		//设置等待时间为0个时钟，CPU运行速度最快
	
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		for(i = 0;i < 8;i++)
		{
			P2 = ~(0x80 >> i);
			delayms(500);
		}
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