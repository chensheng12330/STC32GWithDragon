//		@布丁橘长 2022/11/15
//		8位流水灯实验-反向流水
//		LED8->LED1逐个点亮、熄灭，点亮时间500ms，本实验采用 数组法
//		LED电路由8个LED共阳极连接，每个LED配一个限流电阻。公共端接VCC，低电平点亮。（屠龙刀三.1板载LED电路）
//		LED引脚定义：P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		本实验采用准双向口模式，低电平点亮、高电平熄灭
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  @35MHz
 
#include <STC32G.H>

#define MAIN_Fosc  35000000UL		//定义主频 35MHz，请按实际频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

void delayms(u16 ms);			//延时函数声明

void main()
{
	u8 LED_Code[8] ={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};	//点亮P2口LED1-LED8对应的十六进制值
	u8 i;
	
	WTST = 0;	//设置等待时间为0个时钟，CPU运行速度最快
	
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		for(i = 0;i < 8 ;i++)			//反向流水
		{
			P2 = LED_Code[7-i];			//数组法，从LED_Code[7]开始取值，赋值给P2，LED8~LED1依次点亮
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