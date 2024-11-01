//		@布丁橘长 2022/11/16
//		2位共阴数码管-IO口直接驱动
//		显示数字12，采用delay延时
//		实验采用 段码数组法
//		数码管型号：3621AS，2位共阴数码管，A-DP接P20-P27，P00-COM2,P01->COM1
//		数码管引脚定义：P20-A P21-B P22-C P23-D P24-E P25-F P26-G P27-DP，P00-COM2,P01->COM1
//		本实验采用推挽输出模式，高电平点亮、低电平熄灭
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz
// 		注意！！！数码管需要使用限流电阻，否则容易损坏IO口或数码管

#include <STC32G.H>

#define MAIN_Fosc 35000000UL	//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit COM1 = P0^1;		//P0.1控制数码管第1位（左起）COM1
sbit COM2 = P0^0;		//P0.0控制数码管第2位（左起）COM2

void delayms(u16 ms);			//延时函数声明

u8 SEG_Code[13] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,		//共阴数码管段码：0~9,	
										0x00,0x40,0xFF};	//数码管全灭（10），横杆-（11），数码管全亮（12）
			
void main()
{
	EAXFR = 1;		//使能XFR访问
	CKCON = 0;		//设置外部数据总线速度最快
	WTST = 0;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0xFF;		//设置P2口为推挽输出模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		P2 = SEG_Code[1];		//显示数字1
		COM1 = 0;COM2 = 1;	//点亮第1位，熄灭第2位
		delayms(5);	
		P2 = SEG_Code[2];		//显示数字2
		COM1 = 1;COM2 = 0;	//点亮第1位，熄灭第2位
		delayms(5);	
	}
}
void  delayms(u16 ms)							//简单延时函数，自适应主频，1毫秒*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}