//		@布丁橘长 2023/01/16
//		8位共阴数码管-IO口直接驱动
//		显示20230117
//		实验采用 段码数组+位码数组法
//		数码管型号：3641AS，2个4位共阴数码管，2个数码管A-DP相连后，每段加限流电阻，A-DP接P20-P27，com1-com8接P00-P07
//		数码管引脚定义：P20-A P21-B P22-C P23-D P24-E P25-F P26-G P27-DP，
//		数码管引脚定义：P00-COM8,P01->COM7 P02->COM6 P03->COM5 P04->COM4 P05->COM3 P06->COM2 P07->COM1
//		本实验P2口采用推挽输出模式，P0口采用准双向口模式
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz
// 
#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

void delayms(u16 ms);					//延时函数声明

u8 SEG_Code[13] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,		//共阴数码管段码：0~9,	
										0x00,0x40,0xFF};	//数码管全灭（10），横杆-（11），数码管全亮（12）
u8 SEG_WeiCode[8] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};					//COM1-COM8位码
									
void main()
{
	EAXFR = 1;		//使能XFR访问
	CKCON = 0;		//设置外部数据总线速度最快
	WTST = 0;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0xFF;		//设置P2口为推挽输出模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出

	while(1)		//显示20230117
	{
			P2 = SEG_Code[2];
			P0 = SEG_WeiCode[0];		//第1位显示2
			delayms(1);
		
			P2 = SEG_Code[0];
			P0 = SEG_WeiCode[1];		//第2位显示0
			delayms(1);
		
			P2 = SEG_Code[2];
			P0 = SEG_WeiCode[2];		//第3位显示2
			delayms(1);
		
			P2 = SEG_Code[3];
			P0 = SEG_WeiCode[3];		//第4位显示3
			delayms(1);
		
			P2 = SEG_Code[0];
			P0 = SEG_WeiCode[4];		//第5位显示0
			delayms(1);
		
			P2 = SEG_Code[1];
			P0 = SEG_WeiCode[5];		//第6位显示1
			delayms(1);
			
			P2 = SEG_Code[1];
			P0 = SEG_WeiCode[6];		//第7位显示1
			delayms(1);
			
			P2 = SEG_Code[7];
			P0 = SEG_WeiCode[7];		//第8位显示7
			delayms(1);
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