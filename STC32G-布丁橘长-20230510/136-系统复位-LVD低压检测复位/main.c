//	@布丁橘长 2023/04/08
//  低压复位示例
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define ENLVR 0x40 						//RSTCFG.6
#define LVD2V0 0x00 					//LVD@2.0V
#define LVD2V4 0x01 					//LVD@2.4V
#define LVD2V7 0x02 					//LVD@2.7V
#define LVD3V0 0x03 					//LVD@3.0V

void sysini(void);						// STC32初始化设置

void main(void)
{
	sysini();										// STC32初始化设置
	
	LVDF = 0; 									// 清零LVD低压检测标志位							
// RSTCFG = ENLVR | LVD3V0; 	// 使能 3.0V 时低压复位 , 不产生 LVD 中断
	RSTCFG = LVD3V0; 						// 使能 3.0V 时低压中断
	ELVD = 1; 									// 使能 LVD 中断
	EA = 1;											//使能EA总中断
	while (1);

}
void Lvd_Isr() interrupt 6		// 低压中断
{
	LVDF = 0; 									// 清中断标志
}
void sysini()
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}