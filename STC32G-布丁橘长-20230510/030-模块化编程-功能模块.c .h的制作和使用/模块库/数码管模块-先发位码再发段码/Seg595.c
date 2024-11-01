#include <Seg595.h>

//共阳极段码数组
u8 SEG_Code[13] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,		//共阳数码管段码：0~9,	
										0xff,0xbf,0x00};	//数码管全灭（10），横杆-（11），数码管全亮（12）
//共阳极位码数组-4位数码管
u8 WEI_Code[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//共阳极位码数组，COM1接595Q0，COM2接Q1...即数码管左边第一位接Q0...第八位接Q7

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
	
	Write595(weicode);							//先发位码
	Write595(datcode);							//再发段码
	Output595();										//595输出显示
}