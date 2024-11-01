//		@布丁橘长 2022/11/16
//		1位共阴数码管-IO口直接驱动
//		显示0-9计时，9-0的倒计时，采用delay延时
//		实验采用 段码数组法
//		数码管型号：3611AS，1位共阴数码管，数码管公共端接GND，每段串一个限流电阻
//		数码管引脚定义：P20-A P21-B P22-C P23-D P24-E P25-F P26-G P27-DP
//		本实验采用推挽输出模式，高电平点亮、低电平熄灭
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz
// 
#include <STC32G.H>

#define MAIN_Fosc 35000000UL	//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

void delayms(u16 ms);			//延时函数声明
//共阴极段码数组
u8 SEG_Code[13] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,		//共阴数码管段码：0~9,	
										0x00,0x40,0xFF};	//数码管全灭（10），横杆-（11），数码管全亮（12）
//共阳极段码数组
u8 SEG_Code2[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//共阳数码管段码：0~9,	
										0xff,0xbf,0x00};	//数码管全灭（10），横杆-（11），数码管全亮（12）

void main()
{
	u8 i;
	
	EAXFR = 1;		//使能XFR访问
	CKCON = 0;		//设置外部数据总线速度最快
	WTST = 0;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P2M1 = 0x00;P2M0 = 0xFF;		//设置P2口为推挽输出模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出

	while(1)
	{
		for(i = 0;i < 10;i++)			//0-9计时
		{
			P2 = SEG_Code[i];				
			delayms(1000);
		}
		P2 = 0x00;								//全灭
		delayms(1000);						//延时1秒
		for(i = 0;i < 10;i++)			//0-9计时
		{
			P2 = SEG_Code[9-i];			//9-0倒计时	
			delayms(1000);
		}
		P2 = 0x00;								//全灭
		delayms(1000);						//延时1秒
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