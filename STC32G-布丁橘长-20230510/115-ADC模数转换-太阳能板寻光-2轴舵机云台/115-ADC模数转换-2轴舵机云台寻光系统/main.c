//	@布丁橘长 2023/03/31
//	ADC示例：太阳能板寻光系统，2轴舵机云台，五向开关上下左右可以手动控制云台转动，五向开关中键自动寻光
// 	单片机上电自动寻光一次，寻光动作：1：云台复位到最右侧，太阳能板斜向下位置，
//																		2: 云台左转到底，太阳能板向上调整一级，
//																	  3: 云台右转到底，太阳能板向上调整一级，
//																		4：重复2-3过程3次
//																		5: 结束寻光，云台转动到光照最佳位置
//  板载五向开关：右键：P7.0 左键：P7.2 右键：P7.1 左键：P7.3 右键：P7.4
//  舵机型号SG92R，舵机1：P1.0，舵机2：P5.4
//  ADC采样引脚：P0.0
//  太阳能板5V
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"

#define PWM_PSC 			(120-1)	//120分频
#define PWM_PERIOD 		2000		//周期20毫秒
#define PWM_DUTY_0 		50			//占空比2.5%
#define PWM_DUTY_45		100			//占空比5%	
#define PWM_DUTY_90		150			//占空比7.5%
#define PWM_DUTY_135	200			//占空比10%
#define PWM_DUTY_180	250			//占空比12.5%

sbit KEY_LEFT 	= P7^2;				//左键：P7.2
sbit KEY_RIGHT 	= P7^1;				//右键：P7.1
sbit KEY_UP	= P7^3;						//左键：P7.3
sbit KEY_DOWN = P7^4;					//右键：P7.4
sbit KEY_MIDDLE = P7^0;				//中键：P7.0
void ADC_Config();						// ADC初始化设置
void PWMA_Config(void);				//PWMA初始化设置函数
void Timer0_Init(void);				//定时器0初始化函数
void keyscan();								//按键扫描函数
void Update_duty();						//更新占空比函数
u16 ADC_Read();

u8 keydelay_left,keydelay_right,keydelay_up,keydelay_down;	// 按键延时
u16 duty1,duty2;							// 舵机占空比1、2
bit updateduty1,updateduty2;	// 更新占空比标志1、2
u16 adc_result;								// ADC转换结果
bit B_10ms;										// 10ms标志 
u16 searchdelay;							// 寻光延时		
u16 postion1,postion2;				// 最佳位置1、2
u16 adc_result_new;						// ADC采样结果
u8 searchnums;								// 转动次数
bit start;										// 开始标志
bit dir;											// 转动方向标志
bit searchflag;								// 寻光标志

void main(void)
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0xFF;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0xFF;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	duty1 = PWM_DUTY_0;					// 舵机1初始角度0
	duty2 = PWM_DUTY_135;				// 舵机2初始角度135
	PWMA_Config();							// PWMA初始化设置
	ADC_Config();								// ADC初始化设置
	Update_duty();							// 更新占空比
	Timer0_Init();							// 定时器0初始化
	EA = 1;											// 使能EA总中断
	start = 1;
	while (1)
	{
		if(searchflag == 1)				// 寻光标志置1时
		{
			duty1 = PWM_DUTY_0;			// 舵机1初始角度0
			duty2 = PWM_DUTY_135;		// 舵机2初始角度135
			searchflag = 0;					// 清零寻光标志
			updateduty1 = 1;				// 更新占空比标志1置1
			updateduty2 = 1;				// 更新占空比标志2置1
			Update_duty();					// 更新占空比
			adc_result = 0;					// 清零采样结果
			start = 1;							// 开始寻光标志置1
		}
		if(start == 1)						
		{
			if(B_10ms == 1)			// 1毫秒到
			{
				B_10ms = 0;				// 清零1毫秒标志
				searchdelay++;		// 寻光延时+1
			}
			if(searchdelay == 1)									// 1毫秒到
			{
				if(searchnums%2 == 1) dir = 1;			// 转动次数奇数，方向1
				else if(searchnums%2 == 0)dir = 0;	// 转动次数偶数，方向0
				if(dir == 0)												// 方向0时
				{
					if(duty1<=250)duty1++;						// 舵机1左转
				}
				else if(dir == 1)										// 方向0时
				{
					if(duty1>= 50)duty1--;						// 舵机1右转
				}
				searchdelay = 0;										// 清零寻光延时
				updateduty1 = 1;										// 更新占空比标志1置1
				Update_duty();											// 执行占空比更新函数
				adc_result_new = ADC_Read();				// 读取ADC转换结果
				if(adc_result_new > adc_result) 		// 当前采样值大于上一次采样值
				{
					adc_result = adc_result_new;			// 当前采样值，赋值给上一次采样值
					postion1 = duty1;									// 保存舵机1位置
					postion2 = duty2;									// 保存舵机2位置
				}
			}
		if(duty1 > 250 || duty1 < 50)						// 舵机1转动到0度或180度位置
		{
			if(duty1 > 250) duty1 = 250;					// 防止重复进入if中
			if(duty1 < 50) duty1 = 50;						// 防止重复进入if中
			searchnums++;													// 转动次数+1
			duty2 -= 30;													// 舵机2（太阳能板）向上调整一级
			updateduty1 = 1;											// 更新占空比标志1置1
			updateduty2 = 1;											// 更新占空比标志2置1
			Update_duty();												// 执行占空比更新函数
			if(searchnums == 6)										// 转动6次后
			{
				start = 0;													// 清零开始标志
				searchnums = 0;											// 清零转动次数
				duty1 = postion1;										// 把最佳位置1赋值给舵机1占空比
				duty2 = postion2;										// 把最佳位置2赋值给舵机2占空比
				updateduty1 = 1;										// 更新占空比标志1置1
				updateduty2 = 1;										// 更新占空比标志2置1
				Update_duty();											//执行占空比更新函数
			}
		}
		}
	if(start == 0)	Update_duty();						// 执行占空比更新函数
	}
}
void PWMA_Config(void)
{
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); //预分频
	
	PWMA_CCER1 = 0x00;					// 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCMR1 = 0x60; 					// 设置 CC1为 PWMA 输出模式,PWM模式1
	PWMA_CCMR2 = 0x60; 					// 设置 CC1为 PWMA 输出模式,PWM模式1
	PWMA_CCER1 = 0x11;					// 使能 CC1 CC2通道
	
	PWMA_CCR1H = (u16)(PWM_DUTY_0 >> 8); 
	PWMA_CCR1L = (u16)(PWM_DUTY_0); 		//设置初始占空比
	
	PWMA_CCR2H = (u16)(PWM_DUTY_135 >> 8); 
	PWMA_CCR2L = (u16)(PWM_DUTY_135); 		//设置初始占空比
	
	PWMA_ARRH = (u16)(PWM_PERIOD >> 8); // 设置PWM周期
	PWMA_ARRL = (u16)(PWM_PERIOD); 
	
	PWMA_ENO = 0x05; 						// 使能 PWM1P PWM2P端口输出
	PWMA_BKR = 0x80; 						// 使能主输出
	
	PWMA_CR1 = 0x01; 						// 启动PWM定时器
}
void ADC_Config()
{
	P0M1 = 0x01;P0M0 = 0x00;		// P0.0设置为高阻输入模式
	ADC_CONTR &= 0xF0;							
	ADC_CONTR |= 0x08;					// 设置ADC通道为P0.0
	ADCTIM = 0x3F;							// ADC时序设置：通道选择时间：1个时钟，通道保持时间：4个时钟，采样时间：32个时钟
//	ADCCFG = 0x0F;						// ADC频率设置：系统时钟/2/16，数据左对齐
	ADCCFG = 0x2F;							// ADC频率设置：系统时钟/2/16，数据右对齐
	ADC_POWER = 1;							// 使能ADC
}
u16 ADC_Read()
{
	u16 adc_res;
	ADC_START = 1;											// 启动ADC转换
	while(!ADC_FLAG);										// 等待转换完成
	adc_res = ADC_RES*256 + ADC_RESL;		// 读取采样结果（右对齐方式）
	ADC_FLAG = 0;												// 清零ADC转换完成标志
	return adc_res;											// 返回ADC转换结果
}
void Timer0_Isr(void) interrupt 1
{
	keyscan();									// 按键扫描
	B_10ms = 1;									// 10毫秒标志
}
void Timer0_Init(void)				//10毫秒@12.000MHz
{
	TM0PS = 0x00;								//设置定时器时钟预分频 ( 注意:并非所有系列都有此寄存器,详情请查看数据手册 )
	AUXR &= 0x7F;								//定时器时钟12T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0xF0;									//设置定时初始值
	TH0 = 0xD8;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}
void keyscan()								//定时器10毫秒，扫描1次按键，这里不去抖处理
{
//--------------------五向开关-左键-------------------------
	if(KEY_LEFT == 0)						//左键按下
	{
		if(duty1 < 250) duty1 += 1;	//角度+1（舵机1左转）
		updateduty1= 1;						//更新占空比标志置1
	}
//--------------------五向开关-右键-------------------------
	if(KEY_RIGHT == 0 )					//右键按下
	{
		if(duty1 >  50) duty1 -= 1 ;	//角度-1（舵机1右转）
		updateduty1 = 1;						//更新占空比标志置1
	}
//--------------------五向开关-上键-------------------------
	if(KEY_UP == 0)						//上键按下
	{
		if(duty2 < 250) duty2 += 1;	//角度+1（舵机2上转）
		updateduty2 = 1;						//更新占空比标志置1
	}
//--------------------五向开关-下键-------------------------
	if(KEY_DOWN == 0 )					//下键按下
	{
		if(duty2 >  50) duty2 -= 1 ;	//角度-1（舵机2下转）
		updateduty2 = 1;						//更新占空比标志置1
	}
//--------------------五向开关-中键-------------------------
	if(KEY_MIDDLE == 0 )					//中键按下
	{
		searchflag = 1;
	}
}
void Update_duty()						//更新占空比函数
{
	if(updateduty1 == 1)				//更新占空比标志为1时
	{
		PWMA_CCR1H = (u8)(duty1 >> 8); 
		PWMA_CCR1L = duty1; 			//重新设置CC1通道占空比
		updateduty1 = 0;					//清零占空比更新标志
	}
		if(updateduty2 == 1)			//更新占空比标志为1时
	{
		PWMA_CCR2H = (u8)(duty2 >> 8); 
		PWMA_CCR2L = duty2; 			//重新设置CC2通道占空比
		updateduty2 = 0;					//清零占空比更新标志
	}
}