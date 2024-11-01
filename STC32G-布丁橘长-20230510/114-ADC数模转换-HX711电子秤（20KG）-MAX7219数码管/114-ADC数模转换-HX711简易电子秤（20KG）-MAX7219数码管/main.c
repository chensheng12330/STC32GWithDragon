// 	@布丁橘长 2023/03/29
// 	HX711电子秤（20KG）-MAX7219数码管显示，测量范围0-20KG，分辨率1g
//  称重传感器（20KG,4线）：红色：接HX711模块E+，黑色：接HX711模块E-，白色：接HX711模块A+，绿色：接HX711模块A-
//  HX711模块：VCC接开发板VCC，GND接开发板GND，DT：接P0.0，SCK：接P0.1
//  AMX7219数码管模块引脚定义： CS = P6^5; DIN = P6^6; CLK = P6^4; （可在MAX7219.h中修改）
//  实验开发板：屠龙刀三.1 @主频12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"
#include "HX711.h"
#include "ADfliter.h"

void SYS_Ini();								// STC32初始化设置
void SEG_Disp(void);					// 数码管显示
void Timer0_Init(void);				// 定时器0初始化


bit B_1ms;										// 1ms计时标志
u8 segdelay;									// 数码管延时计数				
u8 weightdelay;								// 称重计时
float weight;									// 重量
u8 fuhao;											// 正负号
u32 ini_weight;								// 初始重量
float Gapvalue;									// 重量计算系数

void main(void)
{
	SYS_Ini();									// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	EA = 1;											// 使能EA总中断
	MAX7219_Ini();							// MAX7219初始化
	SEG_Disp();									// 数码管显示		
	Gapvalue = 102.86;						// 20KG传感器，重量计算系数，理论值107.37，实际系数等于102.86时，称重比较准确

	ini_weight = Fliter_middle(); // 上电获取毛重
	while (1)
	{
		if(B_1ms == 1)						// 1毫秒到									
		{
			B_1ms = 0;							// 清零1毫秒标志
			segdelay++;							// 数码管计时+1
			weightdelay++;					// 称重计时+1
		}
		if(weightdelay == 50)													// 每50毫秒称重一次
		{
			//weight = HX711_Read();										// 直接采样
			weight = Fliter_middle();										// 中位值滤波，5次采样
			if(weight >= ini_weight) 										// 大于0
			{
				weight = (weight- ini_weight)/Gapvalue+0.5;		// 单位：g，实际重量=（测量的重量-毛重）/计算系数，加0.5用于四舍五入
				fuhao = 15;																// 数码管第左起第2位熄灭
			}
			else																				// 小于0
			{
				weight = (ini_weight - weight)/Gapvalue+0.5; 	// 单位：g，实际重量=（测量的重量-毛重）/计算系数，加0.5用于四舍五入
				fuhao = 10;																// 数码管第左起第2位 显示负号
			}
			weightdelay = 0;														// 清零称重计时
		}
		if(segdelay == 50)				// 每50毫秒刷新一次数码管
		{	
			SEG_Disp();							// 刷新数码管
			segdelay = 0;						// 清零数码管计时
		}
	}
}
void SYS_Ini()								// STC32初始化设置
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00; 								// 设置程序代码等待参数，赋值为 0 可将 CPU 执行程序的速度设置为最快
	
	P0M1 = 0x00;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 1毫秒到
}
void Timer0_Init(void)				//1毫秒@12.000MHz
{
	AUXR |= 0x80;								//定时器时钟1T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0x20;									//设置定时初始值
	TH0 = 0xD1;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计数
	ET0 = 1;										//使能定时器0中断
}
void SEG_Disp(void)						// MAX7219数码管显示函数								
{							
	Write7219(8,15); 						// 左起第1位显示 熄灭
	Write7219(7,15); 						// 左起第2位显示 熄灭
	Write7219(6,fuhao); 				// 左起第3位显示 符号，大于0熄灭，小于零显示负号
	Write7219(5,(u16)((u32)(weight/10000)%10)); 		// 左起第4位显示 重量万位（千克十位）
	Write7219(4,(u16)((u32)(weight/1000)%10)); 			// 左起第5位显示 重量千位（千克个位）
	Write7219(3,(u16)((u32)(weight/100)%10)); 			// 左起第6位显示 重量百位
	Write7219(2,(u16)((u32)(weight/10)%10)); 				// 左起第7位显示 重量十位
	Write7219(1,(u16)((u32)(weight)%10)); 					// 左起第8位显示 重量个位
}