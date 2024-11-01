// 	@布丁橘长 2023/03/21
// 	ADC示例：Steinhart-Hart方程法、NTC热敏电阻测温，10KB3950，P0.0引脚测量，MAX7219数码管模块显示
//  使用ADC算术滤波算法、Steinhart-Hart方程法
//  ！！！设置为ADC通道的引脚，必须设置为高阻输入模式！！！
//  本例Vref参考电压为2.5V（屠龙刀R114电阻已经将Vref连接2.5V参考电压，不能再将Vref引脚用线接到VCC，除非断开R114）
//  AMX7219数码管模块引脚定义： CS = P6^5; DIN = P6^6; CLK = P6^4; （可在MAX7219.h中修改）
//  实验开发板：屠龙刀三.1 @主频12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"
#include "ADfliter.h"
#include "NTC.h"

void SYS_Ini();								// STC32初始化设置
void SEG_Disp(void);					// 数码管显示
void Timer0_Init(void);				// 定时器0初始化
void ADC_Config();						// ADC初始化设置

bit B_1ms;										// 1ms计时标志
u16 adc_result;					// ADC转换结果
u16 adc_volt;						// ADC换算后的电压
u8 volt_int1;									// 温度整数部分 百位
u8 volt_int2;									// 温度整数部分	十位
u8 volt_int3;									// 温度整数部分 个位
u8 volt_float1;								// 温度小数点后1位
u8 segdelay;									// 数码管延时计数				
float temperature1;
u32 temperature;
void main(void)
{
	SYS_Ini();									// STC32初始化设置
	ADC_Config();								// ADC初始化设置
	Timer0_Init();							// 定时器0初始化
	EA = 1;											// 使能EA总中断
	MAX7219_Ini();							// MAX7219初始化
	SEG_Disp();									// 数码管显示
	ADC_START = 1;							// 先启动一次ADC
	while (1)
	{
		//adc_result = Fliter_range();							// 限幅滤波算法
		//adc_result = Fliter_middle();								// 中位值滤波算法
		adc_result = Fliter_average();						// 算术平均滤波算法
		
		temperature1 = CalculationTemperature(adc_result);
		temperature = temperature1*10;
		
		volt_int1 = (temperature /1000) % 10;				// 电池电压，整数个位部分
		volt_int2 = (temperature / 100) % 10;				// 电池电压，小数点后第1位
		volt_int3 = (temperature / 10)% 10;					// 电池电压，小数点后第2位
		volt_float1 = (temperature)% 10;					// 电池电压，小数点后第2位
		if(segdelay == 100)				// 每50毫秒刷新一次数码管
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
	P1M1 = 0xFF;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void ADC_Config()
{
	P0M1 = 0x01;P0M0 = 0x00;		// P0.0设置为高阻输入模式
	ADC_CONTR &= 0xF0;					
	ADC_CONTR |= 0x08;					// 设置ADC通道为P0.0
	ADCTIM = 0x3F;							// ADC时序设置：通道选择时间：1个时钟，通道保持时间：4个时钟，采样时间：32个时钟
	ADCCFG = 0x2F;							// ADC频率设置：系统时钟/2/16，数据右对齐
	ADC_POWER = 1;							// 使能ADC
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 1毫秒到
	segdelay++;
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
	Write7219(6,15); 						// 左起第3位显示 熄灭
	Write7219(5,15); 						// 左起第4位显示 熄灭
	Write7219(4,(u16)(volt_int1)); 			// 左起第5位显示 温度整数部分百位
	Write7219(3,(u16)(volt_int2)); 			// 左起第6位显示 温度整数部分十位
	Write7219(2,(u16)(volt_int3 | DP)); // 左起第7位显示 温度整数部分个位，并加上小数点
	Write7219(1,(u16)(volt_float1)); 		// 左起第8位显示 温度小数点第1位
}