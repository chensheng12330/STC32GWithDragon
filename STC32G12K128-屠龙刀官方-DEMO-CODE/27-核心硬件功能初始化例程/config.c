/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/


/*************  本程序功能说明  **************

本例程基于STC32G核心转接板（屠龙刀）进行编写测试。

按一次P3.3按键，修改一次IRC主频，串口打印IRC选择序号。

按一次P3.4按键，修改一次RST管脚功能，串口打印当前的RST管脚功能。

按一次P3.5按键，修改一次LVDS门槛电压级别，串口打印当前的LVDS门槛电压级别。

P2口流水灯在不同IRC主频时切换速度不同。

此外程序演示按键复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
   (主频修改后USB HID设备会丢失，通过USB HID触发自动下载功能也会失效，所以这里不演示通过USB发命令不停电下载)

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "../comm/stc32g.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "stdio.h"

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

/*************  本地常量声明    **************/

#define T22M_ADDR   CHIPID11   //22.1184MHz
#define T24M_ADDR   CHIPID12   //24MHz
#define T27M_ADDR   CHIPID13   //27MHz
#define T30M_ADDR   CHIPID14   //30MHz
#define T33M_ADDR   CHIPID15   //33.1776MHz
#define T35M_ADDR   CHIPID16   //35MHz
#define T36M_ADDR   CHIPID17   //36.864MHz
#define T40M_ADDR   CHIPID18   //40MHz
#define T44M_ADDR   CHIPID19   //44.2368MHz
#define T48M_ADDR   CHIPID20   //48MHz

#define VRT6M_ADDR  CHIPID21   //VRTRIM_6M
#define VRT10M_ADDR CHIPID22   //VRTRIM_10M
#define VRT27M_ADDR CHIPID23   //VRTRIM_27M
#define VRT44M_ADDR CHIPID24   //VRTRIM_44M

#define Baudrate      9600L   //波特率 115200 需要5M以上IRC频率

/*************  频率定义    **************/

typedef enum
{
	IRC_500K = 0,		/*0*/
	IRC_1M,
	IRC_2M,
	IRC_3M,
	IRC_5M,
	IRC_6M,
	IRC_8M,
	IRC_10M,
	IRC_11M,
	IRC_12M,
	IRC_15M,
	IRC_18M,
	IRC_20M,
	IRC_22M,
	IRC_24M,
	IRC_27M,
	IRC_30M,
	IRC_33M,
	IRC_35M,
	IRC_36M,
}IRC_IndexType;

typedef enum
{
	IRCBAND_6M = 0,		/*0*/
	IRCBAND_10M,
	IRCBAND_27M,
	IRCBAND_44M,
}IRCBAND_SelType;

/*************  本地变量声明    **************/
u8 index = IRC_24M;
u32 MAIN_Fosc =	24000000L;	//定义主时钟

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

bit Key1_Flag;
u16 Key1_cnt;

bit Key2_Flag;
u16 Key2_cnt;

bit Key3_Flag;
u16 Key3_cnt;

void KeyResetScan(void);
void delay_ms(u8 ms);
void HardwareMarquee(void);
void Timer0_Init(u32 clk);
void SetMCLK(u8 clk);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
	P_SW1 = (P_SW1 & 0x3f) | 0x00;    //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //定时器时钟1T模式
	S1BRT = 0;          //串口1选择定时器1为波特率发生器
	TL1 = (65536 -(MAIN_Fosc/Baudrate/4));
	TH1 = (65536 -(MAIN_Fosc/Baudrate/4))>>8;
	TR1 = 1;			//定时器1开始计时
}

void UartPutc(unsigned char dat)
{
	SBUF = dat; 
	while(TI==0);
	TI = 0;
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/********************** 主函数 ************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    RSTFLAG |= 0x04;   //设置硬件复位后需要检测P3.2的状态选择运行区域，否则硬件复位后进入USB下载模式

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x40;   //设置为准双向口, P1.6设置位推挽输出
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    Timer0_Init(MAIN_Fosc/1000);	//Timer 0 中断频率, 1000次/秒
    UartInit();
    
    MCLKOCR = 0x81; //主时钟频率2分频输出到P1.6口

    EA = 1;     //允许总中断

    //芯片复位指示
    P20 = 0;		//LED On
    delay_ms(250);
    P20 = 1;		//LED Off
    delay_ms(250);
    P20 = 0;		//LED On
    delay_ms(250);
    P20 = 1;		//LED Off
    delay_ms(250);
    printf("核心硬件功能初始化程序.\r\n");

    while(1)
    {
        HardwareMarquee();
    }
} 
/**********************************************/

void HardwareMarquee(void)
{
    P20 = 0;		//LED On
    delay_ms(250);
    P20 = 1;		//LED Off
    P21 = 0;		//LED On
    delay_ms(250);
    P21 = 1;		//LED Off
    P22 = 0;		//LED On
    delay_ms(250);
    P22 = 1;		//LED Off
    P23 = 0;		//LED On
    delay_ms(250);
    P23 = 1;		//LED Off
    P24 = 0;		//LED On
    delay_ms(250);
    P24 = 1;		//LED Off
    P25 = 0;		//LED On
    delay_ms(250);
    P25 = 1;		//LED Off
    P26 = 0;		//LED On
    delay_ms(250);
    P26 = 1;		//LED Off
    P27 = 0;		//LED On
    delay_ms(250);
    P27 = 1;		//LED Off
    P26 = 0;		//LED On
    delay_ms(250);
    P26 = 1;		//LED Off
    P25 = 0;		//LED On
    delay_ms(250);
    P25 = 1;		//LED Off
    P24 = 0;		//LED On
    delay_ms(250);
    P24 = 1;		//LED Off
    P23 = 0;		//LED On
    delay_ms(250);
    P23 = 1;		//LED Off
    P22 = 0;		//LED On
    delay_ms(250);
    P22 = 1;		//LED Off
    P21 = 0;		//LED On
    delay_ms(250);
    P21 = 1;		//LED Off
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-3
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = 2000;   //固定用12M主频计算，不同IRC延时就不同
        while(--i);   //10T per loop
    }while(--ms);
}

//========================================================================
// 函数: void Timer0_Init(u32 clk)
// 描述: 定时器0初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-2
// 备注: 
//========================================================================
void Timer0_Init(u32 clk)
{
    // Timer0初始化
    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)((65536UL - clk) / 256);
    TL0 = (u8)((65536UL - clk) % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// 函数: void KeyResetScan(void)
// 描述: P3.2口按键长按1秒触发软件复位，进入USB下载模式。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyResetScan(void)
{
    u8 temp;
    
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//连续1000ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

                USBCON = 0x00;      //清除USB设置
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
                while (1);
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }

	if(!P33)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt >= 50)		//50ms防抖
			{
				Key1_Flag = 1;			//设置按键状态，防止重复触发

                index++;
                if(index > IRC_36M) index = 0;
                SetMCLK(index);
                printf("IRC index=%bd.\r\n",index);
			}
		}
	}
	else
	{
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	if(!P34)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;
			if(Key2_cnt >= 50)		//50ms防抖
			{
				Key2_Flag = 1;			//设置按键状态，防止重复触发

                if(RSTCFG & 0x10)
                {
                    RSTCFG &= ~0x10;
                    printf("RST管脚用做IO口.\r\n");
                }
                else
                {
                    RSTCFG |= 0x10;
                    printf("RST管脚用做复位脚.\r\n");
                }
			}
		}
	}
	else
	{
		Key2_cnt = 0;
		Key2_Flag = 0;
	}

	if(!P35)
	{
		if(!Key3_Flag)
		{
			Key3_cnt++;
			if(Key3_cnt >= 50)		//50ms防抖
			{
				Key3_Flag = 1;			//设置按键状态，防止重复触发

                temp = (RSTCFG & 0x03);
                temp++;
                RSTCFG &= ~0x03;
                RSTCFG |= (temp & 0x03);
                printf("设置LVDS门槛电压级别：%bd.\r\n",(temp & 0x03));
			}
		}
	}
	else
	{
		Key3_cnt = 0;
		Key3_Flag = 0;
	}
}

//========================================================================
// 函数: void timer0_int(void)
// 描述: Timer0 1ms中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void timer0_int(void) interrupt 1  //1ms 中断函数
{
    KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
}

//========================================================================
// 函数: void SetMCLK(u8 clk)
// 描述: 设置主频
// 参数: clk: 主频序号
// 返回: none.
// 版本: VER1.0
// 日期: 2022-10-14
// 备注: 
//========================================================================
void SetMCLK(u8 clk) //设置主频
{
	if(clk > IRC_36M) return;	//Error
	switch(clk)
	{
	case IRC_500K:
		//选择500KHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 48;
		MAIN_Fosc =	500000L;	//定义主时钟
		break;

	case IRC_1M:
		//选择1MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 24;
		MAIN_Fosc =	1000000L;	//定义主时钟
		break;

	case IRC_2M:
		//选择2MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 12;
		MAIN_Fosc =	2000000L;	//定义主时钟
		break;

	case IRC_3M:
		//选择3MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 8;
		MAIN_Fosc =	3000000L;	//定义主时钟
		break;

	case IRC_5M:
		//选择5.5296MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 4;
		MAIN_Fosc =	5529600L;	//定义主时钟
		break;

	case IRC_6M:
		//选择6MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 4;
		MAIN_Fosc =	6000000L;	//定义主时钟
		break;

	case IRC_8M:
		//选择8MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 3;
		MAIN_Fosc =	8000000L;	//定义主时钟
		break;

	case IRC_10M:
		//选择10MHz
		CLKDIV = 0x04;
		IRTRIM = T40M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 4;
		MAIN_Fosc =	10000000L;	//定义主时钟
		break;

	case IRC_11M:
		//选择11.0592MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 2;
		MAIN_Fosc =	11059200L;	//定义主时钟
		break;

	case IRC_12M:
		//选择12MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 2;
		MAIN_Fosc =	12000000L;	//定义主时钟
		break;

	case IRC_15M:
		//选择15MHz
		CLKDIV = 0x04;
		IRTRIM = T30M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 2;
		MAIN_Fosc =	15000000L;	//定义主时钟
		break;

	case IRC_18M:
		//选择18.432MHz
		CLKDIV = 0x04;
		IRTRIM = T36M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 2;
		MAIN_Fosc =	18432000L;	//定义主时钟
		break;

	case IRC_20M:
		//选择20MHz
		CLKDIV = 0x04;
		IRTRIM = T40M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 2;
		MAIN_Fosc =	20000000L;	//定义主时钟
		break;

	case IRC_24M:
		//选择24MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	24000000L;	//定义主时钟
		break;

	case IRC_27M:
		//选择27MHz
		CLKDIV = 0x04;
		IRTRIM = T27M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	27000000L;	//定义主时钟
		break;

	case IRC_30M:
		//选择30MHz
		CLKDIV = 0x04;
		IRTRIM = T30M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	30000000L;	//定义主时钟
		break;

	case IRC_33M:
		//选择33.1176MHz
		CLKDIV = 0x04;
		IRTRIM = T33M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	33117600L;	//定义主时钟
		break;

	case IRC_35M:
		//选择35MHz
		CLKDIV = 0x04;
		IRTRIM = T35M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 1;
		MAIN_Fosc =	35000000L;	//定义主时钟
		break;

	case IRC_36M:
		//选择36.864MHz
		CLKDIV = 0x04;
		IRTRIM = T36M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = IRCBAND_44M;
		CLKDIV = 1;
		MAIN_Fosc =	36864000L;	//定义主时钟
		break;

	default:
		//选择22.1184MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	22118400L;	//定义主时钟
		break;
	}
	Timer0_Init(MAIN_Fosc/1000);
	UartInit();
}
