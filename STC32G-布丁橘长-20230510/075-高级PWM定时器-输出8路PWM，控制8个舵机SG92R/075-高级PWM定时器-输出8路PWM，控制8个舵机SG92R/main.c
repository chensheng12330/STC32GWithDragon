//	@布丁橘长 2023/02/24
//	PWM示例程序，输出8路PWM，按键控制8个SG92R舵机转动，系统时钟12MHz，PWM：120预分频
//	舵机控制：PWM周期20毫秒，0度占空比2.5%,45度占空比5%,90度占空比7.5%,135度占空比10%,180度占空比12.5%,
//	舵机1-4由PWMA通道PWM1P/2P/3P/4P控制，PWMA：向上计数模式，PWM模式1
//	舵机5-6由PWMB通道PWM5、6、7、8 控制，PWMB：向下计数模式，PWM模式2
//	五向开关-左右键，控制舵机1左右转，五向开关-上下键，控制舵机2左右转，
//	板载按键：P3.2、P3.3键，控制舵机3，P3.4、P3.5键，控制舵机4，
//	独立按键：KEY1、KEY2控制舵机5，KEY3、KEY4控制舵机6，KEY5、KEY6控制舵机7，KEY7、KEY8控制舵机8，
//	五向开关-中键，奇数次/偶数次，控制8个舵机转到0度/90度
//	舵机型号SG92R，引脚定义：P1.0 P5.4 P1.4 P1.6 P0.0 P0.1 P0.2 P0.3 P0.4
//	五向开关引脚定义：上键：P7.4 下键：P7.3 左键：P7.2 右键：P7.1 中键：P7.0
//	板载按键引脚定义：P3.2 P3.3 P3.4 P3.5
//	8独立按键引脚定义：按键1：P2.0 按键2：P2.1 按键3：P2.2 按键4：P2.3 按键5：P2.4 按键6：P2.5 按键7：P2.6 按键8：P2.7
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

sbit KEY_UP 		= P7^4;				//上键：P7.4
sbit KEY_DOWN 	= P7^3;				//下键：P7.3
sbit KEY_LEFT 	= P7^2;				//左键：P7.2
sbit KEY_RIGHT 	= P7^1;				//右键：P7.1
sbit KEY_MIDDLE = P7^0;				//中键：P7.0

sbit KEY1 = P2^0;							//按键1：P2.0
sbit KEY2 = P2^1;							//按键2：P2.1
sbit KEY3 = P2^2;							//按键3：P2.2
sbit KEY4 = P2^3;							//按键4：P2.3
sbit KEY5 = P2^4;							//按键5：P2.4
sbit KEY6 = P2^5;							//按键6：P2.5
sbit KEY7 = P2^6;							//按键7：P2.6
sbit KEY8 = P2^7;							//按键8：P2.7

void PWM_Config(void);				//PWM初始化设置函数
void Timer0_Init(void);				//定时器0初始化函数
void keyscan();								//按键扫描函数
void Update_Duty();						//更新占空比函数

bit B_10ms;										//10ms计时标志
u8 keydelay_left,keydelay_right,keydelay_up,keydelay_down,keydelay_middle;					//五向开关延时变量
u8 key1delay,key2delay,key3delay,key4delay,key5delay,key6delay,key7delay,key8delay;	//独立按键延时变量
u8 P32delay,P33delay,P34delay,P35delay;												//P3.2-P3.5按键延时变量
u16 duty1,duty2,duty3,duty4,duty5,duty6,duty7,duty8;					//8个舵机对应的占空比变量
u8 angle1,angle2,angle3,angle4,angle5,angle6,angle7,angle8;		//8个舵机对应的角度位置变量
bit dutyupdate;								//更新占空比标志
u8 middlekeynums;							//五向开关中键，按键次数

void main(void)
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0xFF;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0xFF;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	duty1 = duty2 = duty3 = duty4 = PWM_DUTY_0;								//舵机1-4，初始位置0度
	duty5 = duty6 = duty7 = duty8 = PWM_PERIOD - PWM_DUTY_0;	//舵机5-8，初始位置0度（PWM模式2，向下计数模式）
	PWM_Config();							//PWM初始化设置函数
	Timer0_Init();						//定时器0初始化函数
	EA = 1;										//使能总中断

	while (1)
	{
		Update_Duty();						//循环执行更新占空比函数
	}
}
void PWM_Config(void)
{
	PWMB_PS = 0xAA;							//PWMB:P0.0-P0.3 
	
	PWMA_PSCRH = (u16)(PWM_PSC >> 8);
	PWMA_PSCRL = (u16)(PWM_PSC); 				//119+1=120预分频
	
	PWMB_PSCRH = (u16)(PWM_PSC >> 8);
	PWMB_PSCRL = (u16)(PWM_PSC); 				//119+1=120预分频
	
	PWMA_CCER1 = 0x00; // 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMA_CCER2 = 0x00; 
	PWMA_CCMR1 = 0x60; // 设置 CC1 为 PWM模式1
	PWMA_CCMR2 = 0x60; // 设置 CC2 为 PWM模式1
	PWMA_CCMR3 = 0x60; // 设置 CC3 为 PWM模式1
	PWMA_CCMR4 = 0x60; // 设置 CC4 为 PWM模式1
	PWMA_CCER1 = 0x11; // 使能 CC1 CC2通道
	PWMA_CCER2 = 0x11; // 使能 CC3 CC4通道
	
	PWMB_CCER1 = 0x00; // 写 CCMRx 前必须先清零 CCERx 关闭通道
	PWMB_CCER2 = 0x00; 
	PWMB_CCMR1 = 0x70; // 设置 CC5 为 PWM模式2
	PWMB_CCMR2 = 0x70; // 设置 CC6 为 PWM模式2
	PWMB_CCMR3 = 0x70; // 设置 CC7 为 PWM模式2
	PWMB_CCMR4 = 0x70; // 设置 CC8 为 PWM模式2
	PWMB_CCER1 = 0x11; // 使能 CC5 CC6通道
	PWMB_CCER2 = 0x11; // 使能 CC7 CC8通道
	
	PWMA_CCR1H = (u8)(PWM_DUTY_0 >> 8); // 设置CC1占空比时间 占空比2.5%
	PWMA_CCR1L = (u8)(PWM_DUTY_0); 
	
	PWMA_CCR2H = (u8)(PWM_DUTY_0 >> 8); // 设置CC2占空比时间 占空比2.5%
	PWMA_CCR2L = (u8)(PWM_DUTY_0); 
	
	PWMA_CCR3H = (u8)(PWM_DUTY_0 >> 8); // 设置CC3占空比时间 占空比2.5%
	PWMA_CCR3L = (u8)(PWM_DUTY_0); 
	
	PWMA_CCR4H = (u8)(PWM_DUTY_0 >> 8); // 设置CC4占空比时间 占空比2.5%
	PWMA_CCR4L = (u8)(PWM_DUTY_0); 
	
	PWMB_CCR5H = (u8)((PWM_PERIOD - PWM_DUTY_0) >> 8); // 设置CC5占空比时间 占空比2.5%
	PWMB_CCR5L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMB_CCR6H = (u8)((PWM_PERIOD) - PWM_DUTY_0 >> 8); // 设置CC6占空比时间 占空比2.5%
	PWMB_CCR6L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMB_CCR7H = (u8)((PWM_PERIOD - PWM_DUTY_0) >> 8); // 设置CC7占空比时间 占空比2.5%
	PWMB_CCR7L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMB_CCR8H = (u8)((PWM_PERIOD - PWM_DUTY_0) >> 8); // 设置CC8占空比时间 占空比2.5%
	PWMB_CCR8L = (u8)(PWM_PERIOD - PWM_DUTY_0); 
	
	PWMA_ARRH = (u8)(PWM_PERIOD >> 8); // 设置PWMA周期20毫秒
	PWMA_ARRL = (u8)(PWM_PERIOD); 
	
	PWMB_ARRH = (u8)(PWM_PERIOD >> 8); // 设置PWMB周期20毫秒
	PWMB_ARRL = (u8)(PWM_PERIOD); 
	
	PWMA_ENO = 0x55; // 使能 PWM1P 2P 3P 4P端口输出
	PWMA_BKR = 0x80; // PWMA使能主输出
	
	PWMB_ENO = 0x55; // 使能 PWM5 6 7 8 端口输出
	PWMB_BKR = 0x80; // PWMB使能主输出
	
	PWMA_CR1 = 0x01; // 启动PWMA定时器，向上计数模式
	PWMB_CR1 = 0x11; // 启动PWMB定时器，向下计数模式
}
void Timer0_Isr(void) interrupt 1
{
	B_10ms = 1;							//10毫秒计时标志
	keyscan();							//每10毫秒，执行一次按键扫描
}
void Timer0_Init(void)		//10毫秒@12.000MHz
{
	AUXR &= 0x7F;						//定时器时钟12T模式
	TMOD &= 0xF0;						//设置定时器模式
	TL0 = 0xF0;							//设置定时初始值
	TH0 = 0xD8;							//设置定时初始值
	TF0 = 0;								//清除TF0标志
	TR0 = 1;								//定时器0开始计时
	ET0 = 1;								//使能定时器0中断
}
void keyscan()						//按键扫描函数
{
//--------------------五向开关-左键-------------------------
	if(KEY_LEFT == 0)
	{
		if(B_10ms == 1)				//10毫秒到
		{
			B_10ms = 0;					//清零10毫秒标志
			keydelay_left++;		//五向开关左键，延时+1（每次10毫秒）
		}
		if(KEY_LEFT == 0 && keydelay_left == 2)		//左键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle1 < 4) angle1++;								//舵机1角度+1（右转）
			dutyupdate = 1;													//占空比更新标志置1
		}
	}
	if(KEY_LEFT == 1) keydelay_left = 0;				//松开按键后，清零左键延时计数
//--------------------五向开关-右键-------------------------
	if(KEY_RIGHT == 0 )			
	{
		if(B_10ms == 1)				//10毫秒到
		{
			B_10ms = 0;					//清零10毫秒标志
			keydelay_right++;		//五向开关右键，延时+1（每次10毫秒）
		}
		if(KEY_RIGHT == 0 && keydelay_right == 2)	//左键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle1 > 0) angle1--;								//舵机1角度-1（左转）
			dutyupdate = 1;													//占空比更新标志置1
		}
	}
	if(KEY_RIGHT == 1) keydelay_right = 0;			//松开按键后，清零右键延时计数
	//--------------------五向开关-中键-------------------------
	if(KEY_MIDDLE == 0 )
	{
		if(B_10ms == 1)				//10毫秒到
		{
			B_10ms = 0;					//清零10毫秒标志
			keydelay_middle++;	//五向开关中键，延时+1（每次10毫秒）
		}
		if(KEY_MIDDLE == 0 && keydelay_middle == 2)//中键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			middlekeynums++;		//五向开关中键，延时+1（每次10毫秒）
			if(middlekeynums % 2 == 1)								//中键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
			{
				angle1 = angle2 = angle3 = angle4 = 2;	//奇数次按下，舵机转到90度位置
				angle5 = angle6 = angle7 = angle8 = 2;	
			}
			else if(middlekeynums % 2 == 0)						//偶数次按下，舵机转到0度位置
			{
				angle1 = angle2 = angle3 = angle4 = 0;
				angle5 = angle6 = angle7 = angle8 = 0;
			}
			dutyupdate = 1;														//更新占空比标志置1
			if(middlekeynums > 99) middlekeynums = 0;	//防溢出，按键次数大于99，清零
		}
	}
	if(KEY_MIDDLE == 1) keydelay_middle = 0;			//松开中键后，清零中键延时计数
	//--------------------五向开关-上键-------------------------
	if(KEY_UP == 0)				
	{
		if(B_10ms == 1)				//10毫秒到
		{
			B_10ms = 0;					//清零10毫秒标志
			keydelay_up++;			//五向开关上键，延时+1（每次10毫秒）
		}
		if(KEY_UP == 0 && keydelay_up == 2)					//上键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle2 < 4) angle2++;			//舵机2角度+1（右转）		
			dutyupdate = 1;								//更新占空比标志置1
		}
	}
	if(KEY_UP == 1) keydelay_up = 0;	//松开上键后，清零上键延时计数
//--------------------五向开关-下键-------------------------
	if(KEY_DOWN == 0 )
	{
		if(B_10ms == 1)			//10毫秒到
		{													
			B_10ms = 0;				//清零10毫秒标志
			keydelay_down++;	//下键延时+1
		}
		if(KEY_DOWN == 0 && keydelay_down == 2)		//下键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle2 > 0) angle2--;					//舵机2角度-1（左转）
			dutyupdate = 1;										//更新占空比标志置1
		}
	}
	if(KEY_DOWN == 1) keydelay_down= 0;		//松开上键后，清零上键延时计数
		//--------------------P3.2键-------------------------
	if(P32 == 0)
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			P32delay++;				//P3.2键，延时+1（每次10毫秒）
		}
		if(P32 == 0 && P32delay == 2)			//P3.2键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）	
		{
			if(angle3 < 4) angle3++;				//舵机3角度+1（右转）	
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(P32 == 1) P32delay = 0;					//松开P3.2键后，清零上键延时计数
//--------------------P3.3键-------------------------
	if(P33 == 0 )
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			P33delay++;				//P3.3键，延时+1（每次10毫秒）
		}
		if(P33 == 0 && P33delay == 2)			//P3.3键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle3 > 0) angle3--;				//舵机3角度-1（左转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(P33 == 1) P33delay = 0;					//松开P3.3键后，清零上键延时计数
		//--------------------P3.4键-------------------------
	if(P34 == 0)
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			P34delay++;				//P3.4键，延时+1（每次10毫秒）
		}
		if(P34 == 0 && P34delay == 2)			//P3.4键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）	
		{
			if(angle4 < 4) angle4++;				//舵机4角度+1（右转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(P34 == 1) P34delay = 0;					//松开P3.4键后，清零上键延时计数
//--------------------P3.5键-------------------------
	if(P35 == 0 )			
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			P35delay++;				//P3.5键，延时+1（每次10毫秒）
		}
		if(P35 == 0 && P35delay == 2)			//P3.5键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle4 > 0) angle4--;				//舵机4角度-1（左转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(P35 == 1) P35delay = 0;					//松开P3.5键后，清零上键延时计数
//--------------------KEY1键-------------------------
	if(KEY1 == 0)
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key1delay++;			//KEY1键，延时+1（每次10毫秒）
		}
		if(KEY1 == 0 && key1delay == 2)		//KEY1键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle5 < 4) angle5++;				//舵机5角度+1（右转）	
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(KEY1 == 1) key1delay = 0;				//松开KEY1键后，清零上键延时计数
//--------------------KEY2键-------------------------
	if(KEY2 == 0 )
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key2delay++;			//KEY2键，延时+1（每次10毫秒）
		}
		if(KEY2 == 0 && key2delay == 2)		//KEY2键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle5 > 0) angle5--;				//舵机5角度-1（左转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(KEY2 == 1) key2delay = 0;				//松开KEY2键后，清零上键延时计数
	//--------------------KEY3键-------------------------
	if(KEY3 == 0)
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key3delay++;			//KEY3键，延时+1（每次10毫秒）
		}
		if(KEY3 == 0 && key3delay == 2)		//KEY3键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）	
		{
			if(angle6 < 4) angle6++;				//舵机6角度+1（右转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(KEY3 == 1) key3delay = 0;				//松开KEY3键后，清零上键延时计数
//--------------------KEY4键-------------------------
	if(KEY4 == 0 )
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key4delay++;			//KEY4键，延时+1（每次10毫秒）
		}
		if(KEY4 == 0 && key4delay == 2)		//KEY4键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle6 > 0) angle6--;				//舵机6角度-1（左转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(KEY4 == 1) key4delay = 0;				//松开KEY4键后，清零上键延时计数
//--------------------KEY5键-------------------------
	if(KEY5 == 0)
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key5delay++;			//KEY5键，延时+1（每次10毫秒）
		}
		if(KEY5 == 0 && key5delay == 2)		//KEY5键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle7 < 4) angle7++;				//舵机7角度+1（右转）	
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(KEY5 == 1) key5delay = 0;				//松开KEY5键后，清零上键延时计数
//--------------------KEY6键-------------------------
	if(KEY6 == 0 )
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key6delay++;			//KEY6键，延时+1（每次10毫秒）
		}
		if(KEY6 == 0 && key6delay == 2)	//KEY6键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle7 > 0) angle7--;			//舵机7角度-1（左转）
			dutyupdate = 1;								//更新占空比标志置1
		}
	}
	if(KEY6 == 1) key6delay = 0;			//松开KEY6键后，清零上键延时计数
	//--------------------KEY7键-------------------------
	if(KEY7 == 0)
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key7delay++;			//KEY7键，延时+1（每次10毫秒）
		}
		if(KEY7 == 0 && key7delay == 2)		//KEY7键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{	
			if(angle8 < 4) angle8++;				//舵机8角度+1（右转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(KEY7 == 1) key7delay = 0;				//松开KEY7键后，清零上键延时计数
//--------------------KEY8键-------------------------
	if(KEY8 == 0 )
	{
		if(B_10ms == 1)			//10毫秒到
		{
			B_10ms = 0;				//清零10毫秒标志
			key8delay++;			//KEY8键，延时+1（每次10毫秒）
		}
		if(KEY8 == 0 && key8delay == 2)		//KEY8键按下，且延时次数2次（按键去抖时间在10-20毫秒之间）
		{
			if(angle8 > 0) angle8--;				//舵机8角度-1（左转）
			dutyupdate = 1;									//更新占空比标志置1
		}
	}
	if(KEY8 == 1) key8delay = 0;				//松开KEY8键后，清零上键延时计数
}
void Update_Duty()
{
//--------------------更新占空比-------------------------
	if(dutyupdate == 1)
	{
		switch(angle1)
		{
			case 0:duty1 = PWM_DUTY_0;break;			//angle=0，装载0度占空比到duty变量
			case 1:duty1 = PWM_DUTY_45;break;			//angle=1，装载45度占空比到duty变量
			case 2:duty1 = PWM_DUTY_90;break;			//angle=2，装载90度占空比到duty变量
			case 3:duty1 = PWM_DUTY_135;break;		//angle=3，装载135度占空比到duty变量
			case 4:duty1 = PWM_DUTY_180;break;		//angle=4，装载180度占空比到duty变量
			default:break;
		}
		switch(angle2)
		{
			case 0:duty2 = PWM_DUTY_0;break;			//angle=0，装载0度占空比到duty变量
			case 1:duty2 = PWM_DUTY_45;break;			//angle=1，装载45度占空比到duty变量
			case 2:duty2 = PWM_DUTY_90;break;			//angle=2，装载90度占空比到duty变量
			case 3:duty2 = PWM_DUTY_135;break;		//angle=3，装载135度占空比到duty变量
			case 4:duty2 = PWM_DUTY_180;break;		//angle=4，装载180度占空比到duty变量
			default:break;
		}
		switch(angle3)
		{
			case 0:duty3 = PWM_DUTY_0;break;			//angle=0，装载0度占空比到duty变量
			case 1:duty3 = PWM_DUTY_45;break;			//angle=1，装载45度占空比到duty变量
			case 2:duty3 = PWM_DUTY_90;break;			//angle=2，装载90度占空比到duty变量
			case 3:duty3 = PWM_DUTY_135;break;		//angle=3，装载135度占空比到duty变量
			case 4:duty3 = PWM_DUTY_180;break;		//angle=4，装载180度占空比到duty变量
			default:break;
		}
		switch(angle4)
		{
			case 0:duty4 = PWM_DUTY_0;break;		//angle=0，装载0度占空比到duty变量
			case 1:duty4 = PWM_DUTY_45;break;		//angle=1，装载45度占空比到duty变量
			case 2:duty4 = PWM_DUTY_90;break;		//angle=2，装载90度占空比到duty变量
			case 3:duty4 = PWM_DUTY_135;break;	//angle=3，装载135度占空比到duty变量
			case 4:duty4 = PWM_DUTY_180;break;	//angle=4，装载180度占空比到duty变量
			default:break;
		}
		switch(angle5)
		{
			case 0:duty5 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0，装载0度占空比到duty变量
			case 1:duty5 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1，装载45度占空比到duty变量
			case 2:duty5 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2，装载90度占空比到duty变量
			case 3:duty5 = PWM_PERIOD -PWM_DUTY_135;break;		//angle=3，装载135度占空比到duty变量
			case 4:duty5 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4，装载180度占空比到duty变量
			default:break;
		}
		switch(angle6)
		{
			case 0:duty6 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0，装载0度占空比到duty变量
			case 1:duty6 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1，装载45度占空比到duty变量
			case 2:duty6 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2，装载90度占空比到duty变量
			case 3:duty6 = PWM_PERIOD - PWM_DUTY_135;break;		//angle=3，装载135度占空比到duty变量
			case 4:duty6 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4，装载180度占空比到duty变量
			default:break;
		}
		switch(angle7)
		{
			case 0:duty7 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0，装载0度占空比到duty变量
			case 1:duty7 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1，装载45度占空比到duty变量
			case 2:duty7 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2，装载90度占空比到duty变量
			case 3:duty7 = PWM_PERIOD - PWM_DUTY_135;break;		//angle=3，装载135度占空比到duty变量
			case 4:duty7 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4，装载180度占空比到duty变量
			default:break;
		}
		switch(angle8)
		{
			case 0:duty8 = PWM_PERIOD - PWM_DUTY_0;break;			//angle=0，装载0度占空比到duty变量
			case 1:duty8 = PWM_PERIOD - PWM_DUTY_45;break;		//angle=1，装载45度占空比到duty变量
			case 2:duty8 = PWM_PERIOD - PWM_DUTY_90;break;		//angle=2，装载90度占空比到duty变量
			case 3:duty8 = PWM_PERIOD - PWM_DUTY_135;break;		//angle=3，装载135度占空比到duty变量
			case 4:duty8 = PWM_PERIOD - PWM_DUTY_180;break;		//angle=4，装载180度占空比到duty变量
			default:break;
		}
		PWMA_CCR1H = (u8)(duty1 >> 8); 
		PWMA_CCR1L = duty1; 						//更新CC1通道占空比
		
		PWMA_CCR2H = (u8)(duty2 >> 8); 
		PWMA_CCR2L = duty2; 						//更新CC2通道占空比
		
		PWMA_CCR3H = (u8)(duty3 >> 8); 
		PWMA_CCR3L = duty3; 						//更新CC3通道占空比
		
		PWMA_CCR4H = (u8)(duty4 >> 8); 
		PWMA_CCR4L = duty4; 						//更新CC4通道占空比
		
		PWMB_CCR5H = (u8)(duty5 >> 8); 
		PWMB_CCR5L = duty5;							//更新CC5通道占空比
		
		PWMB_CCR6H = (u8)(duty6 >> 8); 
		PWMB_CCR6L = duty6; 						//更新CC6通道占空比
		
		PWMB_CCR7H = (u8)(duty7 >> 8); 
		PWMB_CCR7L = duty7; 						//更新CC7通道占空比
		
		PWMB_CCR8H = (u8)(duty8 >> 8); 
		PWMB_CCR8L = duty8; 						//更新CC8通道占空比
		dutyupdate = 0;									//清零占空比更新标志
	}
}