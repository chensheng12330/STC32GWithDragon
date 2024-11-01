//	@布丁橘长 2023/02/18
//	内部RTC实时时钟-使用外部32.768KHz时钟-显示时间
//	初始时间设置为：23年02月19日，23：00:00
//	8位MAX7219数码管模块显示，引脚定义：DIN = P6^6; CS = P6^5; CLK = P6^4;
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"
#include "MAX7219.h"

void RTC_config(void);				//RTC配置函数声明
void DISP_Clock(void);				//数码管显示时钟函数声明
void DISP_Date(void);					//数码管显示日期函数声明

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
	
	MAX7219_Ini();							//MAX7219初始化
  RTC_config();								//RTC配置

  while(1)
  {
		DISP_Clock();							//显示时钟
  }
} 
void DISP_Clock(void)												//显示时钟函数
{							
	Write7219(8,(u8)(HOUR / 10)); 						//左起第1位显示时钟十位
	Write7219(7,(u8)(HOUR % 10)); 						//左起第2位显示时钟个位
	Write7219(6,10); 													//左起第3位显示时钟横杆
	Write7219(5,(u8)(MIN / 10)); 							//左起第4位显示分钟十位
	Write7219(4,(u8)(MIN % 10)); 							//左起第5位显示分钟个位
	Write7219(3,10); 													//左起第6位显示分钟横杆
	Write7219(2,(u8)(SEC / 10)); 							//左起第7位显示秒钟十位
	Write7219(1,(u8)(SEC % 10)); 							//左起第8位显示时钟个位
}
void RTC_config(void)												//RTC配置函数
{
    INIYEAR = 23;    												//Y:23
    INIMONTH = 02;   												//M:02
    INIDAY = 21;     												//D:19
    INIHOUR = 23;    												//H:23
    INIMIN = 00;     												//M:45
    INISEC = 00;     												//S:55
    INISSEC = 0;      											//S/128:0
																						//STC32G 芯片使用内部32K时钟，休眠无法唤醒
		//IRC32KCR = 0x80;   											//启动内部32K晶振.
    //while (!(IRC32KCR & 1));  							//等待时钟稳定
		//RTCCFG = 0x03;    											//选择内部32K时钟源，并触发RTC寄存器初始化
	
		X32KCR = 0x80 + 0x40;   								//启动外部32K晶振, 低增益+0x00, 高增益+0x40.
    while (!(X32KCR & 1));  								//等待时钟稳定
		RTCCFG = 0x01;    											//选择外部32K时钟源，触发RTC寄存器初始化

    RTCCR = 0x01;     											//RTC使能

    while(RTCCFG & 0x01);										//等待初始化完成,需要在 "RTC使能" 之后判断. 
																						//设置RTC时间需要32768Hz的1个周期时间,大约30.5us. 由于同步, 所以实际等待时间是0~30.5us.
}