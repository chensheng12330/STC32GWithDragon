//	@布丁橘长 2023/04/05
// 	EEPROM基本操作示例，并使用USB-HID不停电下载（开发板直连电脑，不需要下载器）
//  板载P3.2、P3.3按键分别控制数字加、减（对应数码管亮度），并存入EEPROM第0扇区第0字节，Max7219数码管模块显示，
//  按键更改亮度后，断电再重新上电，亮度值不会丢失，保持断电前的亮度
//  使用STC-ISP软件，点击发送用户自定义命令并开始下载，
//	或点击下载/编程按钮，即可自动下载，无需按住P3.2和OFF按键（第一次下载需要按P3.2和OFF）
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"
#include "usb.h"
#include "EEPROM.h"
#include "MAX7219.h"

sbit KEY1 = P3^2;							// 板载P3.2按键
sbit KEY2 = P3^3;							// 板载P3.3按键

void sysini(void);						// STC32初始化设置
void Timer0_Init(void);				// 定时器0初始化	
void SEG_Disp(void);					// MAX7219数码管显示函数	
void keyscan();								// 按键扫描函数

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#"; 	//设置自动复位到ISP区的用户接口命令

bit B_1ms;										// 1ms标志
u8 number;										// 数码管亮度值
u8 key1delay,key2delay;				// 按键1、按键2延时计数	
bit key1flag,key2flag;				// 按键1、按键2按下标志
bit segupdate;								// 数码管更新标志

void main(void)
{
	u8 temp;										// 临时变量（用于读取EEPROM）
	sysini();										// STC32初始化设置
	Timer0_Init();							// 定时器0初始化
	MAX7219_Ini();  						// 初始化Max7219
//------USB调试及复位所需代码-----
  P3M0 &= ~0x03;
  P3M1 |= 0x03;
  IRC48MCR = 0x80;
  while (!(IRC48MCR & 0x01));
	usb_init();
//--------------------------------
	EA = 1;														// 使能EA总中断
	
	temp = IapRead(SECTOR0);					// 上电后，读取第0扇区第0字节，存入temp		
	if(temp != 0xFF) number = temp;		// 如果不是0xFF，将EEPROM读取值赋值给number
	Write7219(INTENSITY,number);      // 设置数码管亮度
	SEG_Disp();												// 数码管刷新显示		
	
	while(1)
	{
		keyscan();											// 按键扫描
		if(segupdate == 1)							// 当更新数码管标志置1时
		{									
			segupdate = 0;								// 清零数码管更新标志
			Write7219(INTENSITY,number);  // 设置数码管亮度
			SEG_Disp();										// 数码管更新显示
		}
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

void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 提供1毫秒节拍，可用于延时计数
//--------USB调试及复位所需代码------		
		if (bUsbOutReady) 
    {
        usb_OUT_done();
    }
//-----------------------------------
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
void keyscan()
{
//--------------------板载P3.2按键----------------------------
	if(KEY1 == 0)														// 按键1按下
	{	
		if(B_1ms == 1)												// 按键计时
		{
			B_1ms = 0;
			key1delay++;												// 每1毫秒按键1延时+1
		}
		if(key1flag == 0 && key1delay == 10) 	// 10毫秒去抖，并且防重复进入
		{
			key1flag = 1;												// 按键1按下标志置1
			if(number < 15) number++;						// number+1，number取值0-15
			IapErase(SECTOR0);									// 擦除第0扇区，IAP方式
			IapProgram(SECTOR0,number);					// 第0扇区第0字节，写入number值，IAP方式
			segupdate = 1;											// 数码管更新标志置1
		}
	}
	if(KEY1 == 1) 													// 松开按键1后
	{
		key1flag = 0;													// 清零按键1按下标志
		key1delay = 0;												// 清零按键1计时
	}
//--------------------板载P3.3按键----------------------------
	if(KEY2 == 0)														// 按键2按下
	{
		if(B_1ms == 1)												// 按键计时
		{
			B_1ms = 0;
			key2delay++;												// 每1毫秒按键2延时+1
		}
		if(key2flag == 0 && key2delay == 10)	// 10毫秒去抖，并且防重复进入
		{
			key2flag = 1;												// 按键2按下标志置1
			if(number > 0) number--;						// number-1，number取值0-15
			IapErase(SECTOR0);									// 擦除第0扇区，IAP方式
			IapProgram(SECTOR0,number);					// 第0扇区第0字节，写入number值，IAP方式
			segupdate = 1;											// 数码管更新标志置1
		}
	}
	if(KEY2 == 1) 													// 松开按键2后
	{
		key2flag = 0;													// 清零按键2按下标志
		key2delay = 0;												// 清零按键2计时
	}
}
void SEG_Disp(void)									// MAX7219数码管显示函数								
{							
	Write7219(8,15); 									// 左起第1位显示 熄灭
	Write7219(7,15); 									// 左起第2位显示 熄灭
	Write7219(6,15); 									// 左起第3位显示 熄灭
	Write7219(5,15); 									// 左起第4位显示 熄灭
	Write7219(4,15); 									// 左起第5位显示 熄灭
	Write7219(3,15); 									// 左起第6位显示 熄灭
	Write7219(2,(u8)(number/10)); 		// 左起第7位显示 数字十位
	Write7219(1,(u8)(number%10)); 		// 左起第8位显示 数字个位
}