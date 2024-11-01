//		@布丁橘长 2023/02/05
//		五向开关-上键：数码管数字加，下键：数码管数字减，左键：数字左移一位，右键：数字右移一位,中键：清零数字
//		五向开关引脚定义：上键：P7.4 下键：P7.3 左：P7.2 右：P7.1 中键：P7.0 
//		8位共阳极数码管模块-74HC595级联驱动：DS(DIO):P6.6 SH(SCK):P6.5 ST(RCK):P6.4
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit DS = P6^6;						//595串行输入DS(DIO)接P6.6
sbit SH = P6^5;						//595串行输入时钟SH(SCK)接P6.5
sbit ST = P6^4;						//595锁存时钟ST(RCK)接P6.4

sbit KEY_Up 		= P7^4;		//上键：P7.4
sbit KEY_Down 	= P7^3;		//下键：P7.3
sbit KEY_Left 	= P7^2;		//左键：P7.2
sbit KEY_Right 	= P7^1;		//右键：P7.1
sbit KEY_Middle = P7^0;		//中键：P7.0

void delayms(u16 ms);						//延时函数声明
void Write595(u8 dat);					//595写数据函数声明
void Output595();								//595输出显示函数声明
void SEG_Disp(u8 dat,u8 wei);		//数码管显示函数声明
void keyscan();									//按键扫描函数声明

//共阳极段码数组
u8 SEG_Code[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//共阳数码管段码：0~9,	
										0xff,0xbf,0x00};	//数码管全灭（10），横杆-（11），数码管全亮（12）
//共阳极位码数组-4位数码管
//u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//共阳极位码数组，COM1接595Q0，COM2接Q1...即数码管左边第一位接Q0...第八位接Q7
u8 WEI_Code[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};		//共阳极位码数组，COM1接595Q7，COM2接Q6...即数码管左边第一位接Q7...第八位接Q0

u8 keynum;					//按键号
u8 number;					//数码管显示的数字
u8 seg_wei;					//数字显示的位置

void main()
{
	EAXFR = 1;				//使能XFR访问
	CKCON = 0x00;			//设置外部数据总线速度最快
	WTST = 0x00;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	seg_wei = 4;			//初始在数码管左边第4位显示							
	while(1)
	{
		keyscan();										//按键扫描
		SEG_Disp(number,seg_wei);			//在第seg_wei位显示数字number
	}
}
void keyscan()
{
	static u8 keystate_up,keystate_down,keystate_left,keystate_right,keystate_middle;	//5个按键的开合状态（1：按下，0：松开）
	if(KEY_Up == 0 && keystate_up == 0)						//上键按下
	{
		delayms(10);																//延时去抖
		if(KEY_Up == 0 && keystate_up == 0)					//再次判断上键是否按下
		{
			keystate_up = 1;													//上键状态置1（1：按下，0：松开）
			if(number <= 8) number++;									//数码管显示的数字+1，最大值9
		}
	}
	if(KEY_Up == 1) keystate_up = 0;							//按键松开后，清零上键状态（1：按下，0：松开）
	
	if(KEY_Down == 0 && keystate_down == 0)				//下键按下
	{
		delayms(10);																//延时去抖
		if(KEY_Down == 0 && keystate_down == 0)			//再次判断下键是否按下
		{
			keystate_down = 1;												//下键状态置1（1：按下，0：松开）
			if(number >= 1) number--;									//数码管显示的数字-1，最小值0
		}
	}
	if(KEY_Down == 1) keystate_down = 0;					//按键松开后，清零下键状态（1：按下，0：松开）
	
	if(KEY_Left == 0 && keystate_left == 0)				//左键按下
	{
		delayms(10);																//延时去抖
		if(KEY_Left == 0 && keystate_left == 0)			//再次判断左键是否按下
		{
			keystate_left = 1;												//左键状态置1（1：按下，0：松开）
			if(seg_wei >= 1) seg_wei--;								//显示的位置减1，左移一位	
		}
	}
	if(KEY_Left == 1) keystate_left = 0;					//按键松开后，清零左键状态（1：按下，0：松开）
	
	if(KEY_Right == 0 && keystate_right == 0)			//右键按下
	{
		delayms(10);																//延时去抖
		if(KEY_Right == 0 && keystate_right == 0)		//再次判断右键是否按下
		{
			keystate_right = 1;												//右键状态置1（1：按下，0：松开）
			if(seg_wei <= 7) seg_wei++;								//显示的位置+1，右移一位
		}
	}
	if(KEY_Right == 1) keystate_right = 0;				//松开按键后，清零右键状态（1：按下，0：松开）
		
	if(KEY_Middle == 0 && keystate_middle == 0)		//中键按下
	{
		delayms(10);																//延时去抖
		if(KEY_Middle == 0 && keystate_middle == 0)	//再次判断中键是否按下
		{
			keystate_middle = 1;											//中键状态置1（1：按下，0：松开）
			number = 0;																//清零数码管显示的数字
		}
	}
	if(KEY_Middle == 1) keystate_middle = 0;			//松开按键后，清零中键状态（1：按下，0：松开）
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