//	@布丁橘长 2023/03/03
//	PWMA互补输出示例，PWM1P、PWM1N（P1.0、P1.1）输出一组互补对称波形，周期200us，占空比50%
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 		(12-1)		//12分频，时钟周期1us
#define PWM_PERIOD 	200				//周期200us
#define PWM_DUTY    100				//占空100us

void PWMA_Config(void);				//PWMA初始化设置函数

void main(void)
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	PWMA_Config();							//PWMA初始化设置
	
	while (1);
}
void PWMA_Config(void)
{
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 				//预分频
	
	PWMA_CCER1 = 0x00; 									// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCMR1 = 0x60; 									// 设置 CC1 CC1N为 PWMA 输出模式，PWM模式1
	PWMA_CCER1 = 0x05; 									// 使能 CC1 CC1N通道
	
	PWMA_CCR1H = (u16)(PWM_DUTY>> 8);		// 设置初始占空
	PWMA_CCR1L = (u16)(PWM_DUTY); 
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // 设置PWM周期
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x03; 										// 使能 PWM1P PWM1N端口输出
	PWMA_BKR = 0x80;										// 使能主输出
	
	PWMA_CR1 = 0x01; 										// 启动PWMA定时器
}