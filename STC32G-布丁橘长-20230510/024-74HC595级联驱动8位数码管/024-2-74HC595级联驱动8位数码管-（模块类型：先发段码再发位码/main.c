//		@布丁橘长 2023/01/18
//		2片74HC595级联驱动8位共阳极数码管,显示20230120
//		8位数码管模块-595级联驱动-共阳极数码管
//		实验采用 段码数组+位码数组
//		A-DP接初级595输出Q0-Q7，公共端接次级595，COM1-Q0,COM2-Q1...COM8-Q7(数码管左边第一位接Q0，第二位Q1...)
//		先发位码，再发段码
//		74HC595引脚定义：DS(DIO)：P2.2 SH(SCK)-P2.1 ST(RCK)：P2.0
//		本实验采用准双向口模式
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit DS = P2^2;				//串行输入DS(DIO)接P2.2
sbit SH = P2^1;				//串行输入时钟SH(SCK)接P2.1
sbit ST = P2^0;				//锁存时钟ST(RCK)接P2.0

void delayms(u16 ms);						//延时函数声明
void Write595(u8 dat);					//595写数据函数声明
void Output595();								//595输出显示函数声明
void SEG_Disp(u8 dat,u8 wei);		//数码管显示函数


//共阳极段码数组
u8 SEG_Code[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//共阳数码管段码：0~9,	
										0xff,0xbf,0x00};	//数码管全灭（10），横杆-（11），数码管全亮（12）
//共阳极位码数组-4位数码管
//u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//共阳极位码数组，COM1接595Q0，COM2接Q1...即数码管左边第一位接Q0...第八位接Q7
u8 WEI_Code[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};		//共阳极位码数组，COM1接595Q7，COM2接Q6...即数码管左边第一位接Q7...第八位接Q0

/*======================================================================================================
//共阴极段码数组
u8 SEG_Code[13] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,		//共阴数码管段码：0~9,	
										0x00,0x40,0xFF};	//数码管全灭（10），横杆-（11），数码管全亮（12）
//共阴极位码数组
u8 SEG_WeiCode[8] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};					//COM1-COM8位码，共阴极数码管
========================================================================================================*/

void main()
{
	EAXFR = 1;				//使能XFR访问
	CKCON = 0x00;			//设置外部数据总线速度最快
	WTST = 0x00;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		SEG_Disp(2,1);					//数码管左边第1位显示数字2	
		delayms(1);							//延时1毫秒
			
		SEG_Disp(0,2);					//数码管左边第2位显示数字0	
		delayms(1);							//延时1毫秒
			
		SEG_Disp(2,3);					//数码管左边第3位显示数字2	
	delayms(1);							//延时1毫秒
			
		SEG_Disp(3,4);					//数码管左边第4位显示数字3	
		delayms(1);							//延时1毫秒
			
		SEG_Disp(0,5);					//数码管左边第5位显示数字0	
		delayms(1);							//延时1毫秒
			
		SEG_Disp(1,6);					//数码管左边第6位显示数字1	
		delayms(1);							//延时1毫秒
			
		SEG_Disp(2,7);					//数码管左边第7位显示数字2	
		delayms(1);							//延时1毫秒
			
		SEG_Disp(0,8);					//数码管左边第8位显示数字0	
		delayms(1);							//延时1毫秒
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

void Write595(u8 dat)			//595写数据函数
{
	u16 i;
	for(i = 0;i < 8;i++)		
	{
		SH = 0;								//拉低串行输入时钟SCLK
		DS = dat & 0x80;			//取最高位
		SH = 1;								//SCKL时钟上升沿，数据存移位寄存器
	  dat <<= 1;						//每次一位
	}
}
void Output595()					//595输出函数
{
	ST = 0;									//拉低ST时钟
	ST = 1;									//ST时钟上升沿，更新输出存储器数据
}

void SEG_Disp(u8 dat,u8 wei)			//数码管显示函数
{
	u8 datcode,weicode;
	datcode = SEG_Code[dat];				//取十进制dat对应的段码
	weicode = WEI_Code[wei-1];			//取十进制wei对应的位码
	
	Write595(datcode);							//先发段码
	Write595(weicode);							//再发位码
	Output595();										//595输出显示
}