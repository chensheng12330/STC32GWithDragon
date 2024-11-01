//		@布丁橘长 2023/02/13
//		定时器4-1T-时钟分频输出
//		P0.7为T4CLKO-时钟输出引脚，当定时器 4 计数发生溢出时，P0.7 口的电平自动发生翻转。
//		本实验定时器4定时10毫秒，时钟输出引脚T4CKLO（P0.7）：输出时钟周期-20毫秒，频率-50Hz
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1 12MHz

#include <STC32G.H>

void Timer4_Init(void);		//定时器4初始化函数声明

void main()
{	
	EAXFR = 1; 			// 使能访问 XFR
	CKCON = 0x00; 	// 设置外部数据总线速度为最快
	WTST = 0x00;		// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	Timer4_Init();
	T4CLKO = 1;  //使能T4时钟输出
	
	while(1);
}
void Timer4_Init(void)		//10毫秒@12.000MHz
{
	T4x12 = 0;							//定时器时钟12T模式
	T4L = 0xF0;							//设置定时初始值
	T4H = 0xD8;							//设置定时初始值
	T4R = 1;								//定时器4开始计时
}