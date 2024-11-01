//		@布丁橘长 20230110
//		IO口内部上拉、下拉电阻应用，不加外部限流电阻，IO口直接驱动LED
//		使用P7.0口直接驱动LED，不加外部限流电阻，让LED闪烁
//		本实验采用高阻输入模式
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  @35MHz
 
#include <STC32G.H>


#define MAIN_Fosc  35000000UL		//定义主频 35MHz，请按实际频率修改（用于delay函数自适应主频）

typedef unsigned char u8;				//unsigned char 		类型用 u8 表示
typedef unsigned int u16;				//unsigned int  		类型用 u16表示
typedef unsigned long int u32;	//unsigned long int 类型用 u32表示

void delayms(u16 ms);			//延时函数声明

void main()
{
	WTST = 0;			//设置等待时间为0个时钟，CPU运行速度最快
	
	EAXFR = 1; 		//使能XFR; 否则无法访问XFR，PxPD功能受限	 	//EAXFR:扩展SFR使能寄存器

	P7M1 = 0xFF;P7M0 = 0x00;		//设置P7口为高阻输入模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		P7PU = 0x01;			//打开P7.0上拉电阻
		delayms(500);
		P7PU = 0x00;			//关闭P7.0上拉电阻
		delayms(500);
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