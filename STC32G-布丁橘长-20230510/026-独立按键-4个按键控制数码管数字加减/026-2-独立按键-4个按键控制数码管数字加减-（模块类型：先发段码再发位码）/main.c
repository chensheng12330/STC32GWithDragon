//		@布丁橘长 2023/02/03
//		独立按键-按键KEY1数码管数字加，按键KEY2数码管数字减，按键KEY3数码管显示88888888，按键KEY4返回显示数字
//		开发板板载按键：KEY1-P3.2，KEY2-P3.3，KEY3-P3.4，KEY4-P3.5
//		8位数码管模块（2片595级联驱动）-共阳极数码管
//		数码管引脚定义：DS(DIO)-P2.2，SH(SCK)-P2.1，ST(RCK)-P2.0
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit DS = P2^2;			//串行输入DS(DIO)接P2.2
sbit SH = P2^1;			//串行输入时钟SH(SCK)接P2.1
sbit ST = P2^0;			//输出锁存时钟ST(RCK)接P2.0
sbit KEY1 = P3^2;		//板载按键，KEY1-P3.2
sbit KEY2 = P3^3;		//板载按键，KEY2-P3.3
sbit KEY3 = P3^4;		//板载按键，KEY3-P3.4
sbit KEY4 = P3^5;		//板载按键，KEY4-P3.5

void delayms(u16 ms);						//延时函数声明
void keyscan();									//按键扫描函数			
void Write595(u8 dat);					//595写数据函数声明
void Output595();								//595输出显示函数声明
void SEG_Disp(u8 dat,u8 wei);		//数码管显示函数


//共阳极段码数组
u8 SEG_Code[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//共阳数码管段码：0~9,	
										0xff,0xbf,0x00};	//数码管全灭（10），横杆-（11），数码管全亮（12）
//共阳极位码数组-4位数码管
//u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//共阳极位码数组，COM1接595Q0，COM2接Q1...即数码管左边第一位接Q0...第八位接Q7
u8 WEI_Code[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};		//共阳极位码数组，COM1接595Q7，COM2接Q6...即数码管左边第一位接Q7...第八位接Q0
		
u8 k1,k2,k3,k4,seg;	//全局变量-KEY1，KEY2,KEY3,KEY4状态，数码管状态
u8 number;					//全局变量-number:数码管显示的数字				

void main()
{
	u8 i;							//局部变量-i：用于for循环
	
	EAXFR = 1;				//使能XFR访问
	CKCON = 0x00;			//设置外部数据总线速度最快
	WTST = 0x00;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		keyscan();
		if(seg == 1) 										//数码管显示标志为1时，显示88888888
		{
			for(i = 1;i < 9;i++)
			{
				SEG_Disp(8,i);							//数码管8位都显示8
				delayms(1);
			}
		}
		else 														//数码管显示标志为0时，显示数字
		{
			for(i = 1;i < 7;i++)
			{
				SEG_Disp(10,i);							//数码管左边6位熄灭
				delayms(1);
			}
			SEG_Disp((u8)(number/10),7);	//第7位显示数字十位
			delayms(1);
			SEG_Disp((u8)(number%10),8);	//第8位显示数字个位
			delayms(1);
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
void keyscan()
{
	if(KEY1 == 0 && k1 == 0)					//判断按键KEY1是否按下，只有在按键是松开状态才会进入if
	{	
		delayms(10);										//延时消抖
		if(KEY1 == 0 && k1 == 0)				//再次判断KEY1是否按下，只有在按键是松开状态才会进入if
		{
			if(number < 99)	number++;			//数码管显示的数字+1,number最大值99
			k1 = 1;												//按键松开前防止重复进入，防止number一直增加，松开按键前不再判断进入if
		}
	}
	if(KEY1 == 1) k1 = 0; 						//松开KEY1按键后，清零k1
	
	if(KEY2 == 0 && k2 == 0)					//判断按键KEY2是否按下，只有在按键是松开状态才会进入if
	{
		delayms(10);										//延时消抖
		if(KEY2 == 0 && k2 == 0)				//按键松开前防止重复进入，再次判断KEY1是否按下，只有在按键是松开状态才会进入if
		{
			if(number >= 1)	number--;			//number最小值0
			k2 = 1;				
		}
	}
	if(KEY2 == 1) k2 = 0;							//松开KEY2按键后，清零k2
	
	if(KEY3 == 0 && k3 == 0)					//判断按键KEY3是否按下
	{
		delayms(10);										//延时消抖
		if(KEY3 == 0 && k3 == 0)				//再次判断KEY1是否按下
		{
			seg = 1;											//数码管状态置1，显示88888888
			k3 = 1;												//按键松开前防止重复进入
		}
	}
	if(KEY3 == 1) k3 = 0;							//松开KEY3按键后，清零k3
	
	if(KEY4 == 0 && k4 == 0)					//判断按键KEY4是否按下
	{
		delayms(10);										//延时消抖
		if(KEY4 == 0 && k4 == 0)				//再次判断KEY1是否按下
		{
			seg = 0;											//清零数码管状态，显示数字
			k4 = 1;												//按键松开前防止重复进入
		}
	}
	if(KEY4 == 1) k4 = 0;							//松开KEY4按键后，清零k4
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