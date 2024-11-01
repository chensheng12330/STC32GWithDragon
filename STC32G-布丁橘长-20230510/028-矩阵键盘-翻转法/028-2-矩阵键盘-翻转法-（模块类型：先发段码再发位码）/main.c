//		@布丁橘长 2023/02/04
//		4*4矩阵键盘：数码管显示按键序号01-16
//		矩阵键盘引脚定义：列：C4-P2.7 C3-P2.6 C2-P2.5 C1-P2.4 行：R1-P2.3 R2-P2.2 R3-P2.1 R4-P2.0(最上面为第一行，最左边位第一列)
//		8位共阳极数码管模块-595级联驱动：DS(DIO):P6.6 SH(SCK):P6.5 ST(RCK):P6.4
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz

#include <STC32G.H>

#define MAIN_Fosc 35000000UL		//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

sbit DS = P6^6;				//串行输入DS(DIO)接P6.6
sbit SH = P6^5;				//串行输入时钟SH(SCK)接P6.5
sbit ST = P6^4;				//锁存时钟ST(RCK)接P6.4

void delayms(u16 ms);						//延时函数声明
void Write595(u8 dat);					//595写数据函数声明
void Output595();								//595输出显示函数声明
void SEG_Disp(u8 dat,u8 wei);		//数码管显示函数声明
u8 Matrix_key();		 						//矩阵按键函数声明

//共阳极段码数组
u8 SEG_Code[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//共阳数码管段码：0~9,	
										0xff,0xbf,0x00};	//数码管全灭（10），横杆-（11），数码管全亮（12）
//共阳极位码数组-4位数码管
//u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//共阳极位码数组，COM1接595Q0，COM2接Q1...即数码管左边第一位接Q0...第八位接Q7
u8 WEI_Code[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};		//共阳极位码数组，COM1接595Q7，COM2接Q6...即数码管左边第一位接Q7...第八位接Q0

void main()
{
	u8 keynum;
	
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
	
	while(1)
	{
		keynum = Matrix_key();							//获取按键号
		if(keynum >= 10)										//数码管左起第7位只有按键号大于等于10才显示
		{
			SEG_Disp((u8)(keynum / 10),7);		//数码管左起第7位显示按键号十位
			delayms(1);
		}
		SEG_Disp((u8)(keynum % 10),8);			//数码管左起第7位显示按键号个位
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
u8 Matrix_key()
{
	u8 temp,keyvalue;		//temp：P2值缓存，keyvalue:按键号
	u8 keystate;				//按键状态
	P2 = 0xF0;					//列扫描
	if(P2 != 0xF0 && keystate == 0)			//判断是否有按键按下，如果按键已经是按下状态则不进入if
	{
		delayms(5);				//按键去抖
		if(P2 != 0xF0 && keystate == 0)		//再次判断是否有按键按下，如果按键已经是按下状态则不进入if
		{
			keystate = 1;		//按键状态为1，按键按下
			temp = P2;			//读取P2值
			switch(temp)
			{
				case 0xE0:keyvalue = 1;break;			//第1列有按键按下，键值等于1
				case 0xD0:keyvalue = 2;break;			//第2列有按键按下，键值等于2
				case 0xB0:keyvalue = 3;break;			//第3列有按键按下，键值等于3
				case 0x70:keyvalue = 4;break;			//第4列有按键按下，键值等予4
			}
			P2 = 0x0F;				//翻转电平，行扫描
			if(P2 != 0x0F)		//判断是否有按键按下
			{
				temp = P2;			//读取P2值
				switch(temp)
				{
					case 0x07:keyvalue += 0;break;		//第1行按下，键值keyvalue+0
					case 0x0B:keyvalue += 4;break;		//第2行按下，键值keyvalue+4
					case 0x0D:keyvalue += 8;break;		//第3行按下，键值keyvalue+8
					case 0x0E:keyvalue += 12;break;		//第4行按下，键值keyvalue+12
				}
			}
			
		}
	}
	if(P2 == 0xF0) keystate = 0;		//松开按键后，清零按键状态
	return keyvalue;								//返回按键号
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