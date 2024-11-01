//	@布丁橘长 2023/04/01
// 	不停电下载示例：USB-HID方式，PC端发送指令，触发MCU复位并自动下载
//  使用STC-ISP软件，点击发送用户自定义命令并开始下载，
//	或点击下载按钮，即可自动下载，无需按住P3.2和OFF按键（第一次下载需要按P3.2和OFF）
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "usb.h"

#define MAIN_Fosc 24000000UL		//定义主频24MHz，请根据实际使用频率修改

void sysini(void);						// STC32初始化设置
void Timer0_Init(void);				// 定时器0初始化	
void Blink(void);							// LED闪烁函数，此处为P2口流水灯效果

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#"; 	//设置自动复位到ISP区的用户接口命令

bit B_1ms;										// 1ms标志
u16 blinkdelay;								// LED闪烁延时计数

void main(void)
{
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	
//------USB调试及复位所需代码-----
  P3M0 &= ~0x03;
  P3M1 |= 0x03;
  IRC48MCR = 0x80;
  while (!(IRC48MCR & 0x01));
	usb_init();
//--------------------------------
	EA = 1;											// 使能EA总中断
	
	while(1)
	{
		Blink();									// LED闪烁函数，此处为P2口流水灯效果
	}
}
void sysini()
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0xFF;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void Blink(void)							// LED闪烁函数，此处为P2口流水灯效果
{
	static u8 i;								
	if(B_1ms == 1)							// 1毫秒到
	{
		B_1ms = 0;								// 清零1毫秒标志
		blinkdelay++;							// 闪烁延时标志置1
	}
	if(blinkdelay == 500)				// 500毫秒到
	{
		P2 = ~(0x01 << i);				// 0x01左移i位，即P2口8个LED轮流点亮
		i++;											
		if(i > 7) i = 0;					// i在0-7范围自增
		blinkdelay = 0;						// 清零闪烁延时标志
	}
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 提供1毫秒节拍，可用于延时计数
	//------USB调试及复位所需代码------		
		if (bUsbOutReady) 
    {
        usb_OUT_done();
    }
//--------------------------------
}
void Timer0_Init(void)				//1毫秒@12.000MHz
{
	AUXR |= 0x80;								//定时器时钟1T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0x20;									//设置定时初始值
	TH0 = 0xD1;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}