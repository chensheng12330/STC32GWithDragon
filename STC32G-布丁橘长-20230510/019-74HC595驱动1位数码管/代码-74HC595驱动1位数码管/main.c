//		@布丁橘长 2023/01/16
//		74HC595驱动1位共阴极数码管
//		显示数字0-9计时
//		实验采用 段码数组法
//		数码管型号：3641AS，2个4位共阴数码管，2个数码管A-DP相连后，每段加限流电阻，A-DP接P20-P27，com1接P00
//		数码管引脚定义：P20-A P21-B P22-C P23-D P24-E P25-F P26-G P27-DP，P00-COM1
//		本实验P2口采用推挽输出模式，P0口采用准双向口模式
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit DS = P2^0;			//串行输入接P2.0
sbit ST = P2^1;			//锁存时钟接P2.1
sbit SH = P2^2;			//串行输入时钟接P2.2
sbit COM1 = P0^0;			//数码管公共端COM1接P0.0

void delayms(u16 ms);						//延时函数声明
void Write595(u8 dat);

u8 SEG_Code[13] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,		//共阴数码管段码：0~9,	
										0x00,0x40,0xFF};	//数码管全灭（10），横杆-（11），数码管全亮（12）

void main()
{	
	u8 i,number;
	
	EAXFR = 1;		//使能XFR访问
	CKCON = 0;		//设置外部数据总线速度最快
	WTST = 0;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0xFF;		//设置P2口为推挽输出模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	number = 0;
	while(1)
	{
		for(i = 0;i < 200;i++)	//每个数字显示200*5毫秒（粗略计算1秒）
		{
			Write595(number);					//显示0-9计数
			COM1 = 0;							//共阴极数码管，公共端给低电平点亮
			delayms(5);						//延时5毫秒
		}
		number++;
		if(number > 9) number = 0;
	}
}
void Write595(u8 dat)				//向595发送写数据函数
{
	u16 i;
	u8 datcode;
	datcode = SEG_Code[dat];	//十进制数转段码
	for(i = 0;i < 8;i++)		//写数据
	{
		SH = 0;							//拉低串行输入时钟SCLK
		DS = datcode & 0x80;	//取最高位
		SH = 1;							//SCKL时钟上升沿，数据存移位寄存器
	 datcode <<= 1;					//每次一位
	}
	ST = 0;							//拉低ST时钟
	ST = 1;							//ST时钟上升沿，更新输出存储器数据
}
void  delayms(u16 ms)		//简单延时函数，自适应主频，1毫秒*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}