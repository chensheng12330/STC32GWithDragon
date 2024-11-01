//		@布丁橘长 2023/01/18
//		1片74HC595驱动8位共阴极数码管
//		显示20230119
//		实验采用 段码数组+位码数组
//		数码管型号：3641AS，2个4位共阴数码管，2个数码管A-DP相连后，每段加限流电阻
//		A-DP接初级595输出Y0-Y7，COM1-COM8接P0.7-P0.0，COM1对应P0.7，COM8对应P0.0
//		74HC595引脚定义：DS：P2.0 ST：P2.1 SH-P2.2
//		本实验采用准双向口模式
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz
 
#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit DS = P2^0;				//串行输入接P2.0
sbit ST = P2^1;				//锁存时钟接P2.1
sbit SH = P2^2;				//串行输入时钟接P2.2

sbit COM4 = P0^0;			//数码管公共端COM4接P0.0
sbit COM3 = P0^1;			//数码管公共端COM3接P0.1
sbit COM2 = P0^2;			//数码管公共端COM2接P0.2
sbit COM1 = P0^3;			//数码管公共端COM1接P0.3

void delayms(u16 ms);					//延时函数声明
void Write595(u8 dat);				//595写数据函数声明

u8 SEG_Code[13] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,		//共阴数码管段码：0~9,	
										0x00,0x40,0xFF};	//数码管全灭（10），横杆-（11），数码管全亮（12）
u8 SEG_WeiCode[8] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};					//COM1-COM8位码，共阴极数码管

void main()
{
	u8 i;
	
	EAXFR = 1;				//使能XFR访问
	CKCON = 0x00;			//设置外部数据总线速度最快
	WTST = 0x00;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		for(i = 0;i < 250;i++)										//每个数字显示250*4毫秒（粗略计算1秒）
		{
			Write595(2);							//显示2
			P0 = SEG_WeiCode[0];			//数码管左起第1位位码
			delayms(1);								//延时1毫秒
			
			Write595(0);							//显示0
			P0 = SEG_WeiCode[1];			//数码管左起第2位位码
			delayms(1);								//延时1毫秒
			
			Write595(2);							//显示2
			P0 = SEG_WeiCode[2];			//数码管左起第3位位码
			delayms(1);								//延时1毫秒
			
			Write595(3);							//显示3
			P0 = SEG_WeiCode[3];			//数码管左起第4位位码
			delayms(1);								//延时1毫秒
			
			Write595(0);							//显示0
			P0 = SEG_WeiCode[4];			//数码管左起第5位位码
			delayms(1);								//延时1毫秒
			
			Write595(1);							//显示1
			P0 = SEG_WeiCode[5];			//数码管左起第6位位码
			delayms(1);								//延时1毫秒
			
			Write595(1);							//显示1
			P0 = SEG_WeiCode[6];			//数码管左起第7位位码
			delayms(1);								//延时1毫秒
			
			Write595(9);							//显示9
			P0 = SEG_WeiCode[7];			//数码管左起第8位位码
			delayms(1);								//延时1毫秒
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

void Write595(u8 dat)				//向595写数据函数
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