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

/*************  功能说明    **************

本例程基于STC32G核心转接板（屠龙刀）进行编写测试。

本程序演示5个定时器的使用, 本例程均使用16位自动重装.

定时器0做16位自动重装, 中断频率为1000HZ，中断函数从P2.0取反输出500HZ方波信号.

定时器1做16位自动重装, 中断频率为2000HZ，中断函数从P2.1取反输出1000HZ方波信号.

定时器2做16位自动重装, 中断频率为3000HZ，中断函数从P2.2取反输出1500HZ方波信号.

定时器3做16位自动重装, 中断频率为4000HZ，中断函数从P2.3取反输出2000HZ方波信号.

定时器4做16位自动重装, 中断频率为5000HZ，中断函数从P2.4取反输出2500HZ方波信号.

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件

#define MAIN_Fosc       24000000UL  //定义主时钟

#define Timer0_Reload   (MAIN_Fosc / 1000)      //Timer 0 中断频率, 1000次/秒
#define Timer1_Reload   (MAIN_Fosc / 2000)      //Timer 1 中断频率, 2000次/秒
#define Timer2_Reload   (MAIN_Fosc / 3000)      //Timer 2 中断频率, 3000次/秒
#define Timer3_Reload   (MAIN_Fosc / 4000)      //Timer 3 中断频率, 4000次/秒
#define Timer4_Reload   (MAIN_Fosc / 5000)      //Timer 4 中断频率, 5000次/秒

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";              //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

void Timer0_init(void);
void Timer1_init(void);
void Timer2_init(void);
void Timer3_init(void);
void Timer4_init(void);

void delay_ms(u8 ms);
void KeyResetScan(void);

//========================================================================
// 函数: void main(void)
// 描述: 主函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    RSTFLAG |= 0x04;   //设置硬件复位后需要检测P3.2的状态选择运行区域，否则硬件复位后进入USB下载模式

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    usb_init();
    Timer0_init();
    Timer1_init();
    Timer2_init();
    Timer3_init();
    Timer4_init();
    EUSB = 1;   //IE2相关的中断位操作使能后，需要重新设置EUSB
    EA = 1;     //打开总中断

    while (1)
    {
        if (bUsbOutReady) //USB调试及复位所需代码
        {
            //USB_SendData(UsbOutBuffer,64);    //发送数据缓冲区，长度
            
            usb_OUT_done();
        }

        KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
        delay_ms(1);
    }
}

//========================================================================
// 函数: void Timer0_init(void)
// 描述: timer0初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer0_init(void)
{
        TR0 = 0;    //停止计数

    #if (Timer0_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer0设置的中断过快!"

    #elif ((Timer0_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET0 = 1;    //允许中断
    //  PT0 = 1;    //高优先级中断
        TMOD &= ~0x03;
        TMOD |= 0;  //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
    //  T0_CT = 1;  //计数
        T0_CT = 0;  //定时
    //  T0CLKO = 1; //输出时钟
        T0CLKO = 0; //不输出时钟

        #if (Timer0_Reload < 65536UL)
            T0x12 = 1;  //1T mode
            TH0 = (u8)((65536UL - Timer0_Reload) / 256);
            TL0 = (u8)((65536UL - Timer0_Reload) % 256);
        #else
            T0x12 = 0;  //12T mode
            TH0 = (u8)((65536UL - Timer0_Reload/12) / 256);
            TL0 = (u8)((65536UL - Timer0_Reload/12) % 256);
        #endif

        TR0 = 1;    //开始运行

    #else
        #error "Timer0设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer1_init(void)
// 描述: timer1初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer1_init(void)
{
        TR1 = 0;    //停止计数

    #if (Timer1_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer1设置的中断过快!"

    #elif ((Timer1_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET1 = 1;    //允许中断
    //  PT1 = 1;    //高优先级中断
        TMOD &= ~0x30;
        TMOD |= (0 << 4);   //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
    //  T1_CT = 1;  //计数
        T1_CT = 0;  //定时
    //  T1CLKO = 1; //输出时钟
        T1CLKO = 0; //不输出时钟

        #if (Timer1_Reload < 65536UL)
            T1x12 = 1;  //1T mode
            TH1 = (u8)((65536UL - Timer1_Reload) / 256);
            TL1 = (u8)((65536UL - Timer1_Reload) % 256);
        #else
            T1x12 = 0;  //12T mode
            TH1 = (u8)((65536UL - Timer1_Reload/12) / 256);
            TL1 = (u8)((65536UL - Timer1_Reload/12) % 256);
        #endif

        TR1 = 1;    //开始运行

    #else
        #error "Timer1设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer2_init(void)
// 描述: timer2初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer2_init(void)
{
        T2R = 0;    //停止计数

    #if (Timer2_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer2设置的中断过快!"

    #elif ((Timer2_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET2 = 1;    //允许中断
    //  T2_CT = 1;  //计数
        T2_CT = 0;  //定时
    //  T2CLKO = 1; //输出时钟
        T2CLKO = 0; //不输出时钟

        #if (Timer2_Reload < 65536UL)
            T2x12 = 1;    //1T mode
            T2H = (u8)((65536UL - Timer2_Reload) / 256);
            T2L = (u8)((65536UL - Timer2_Reload) % 256);
        #else
            T2x12 = 0;    //12T mode
            T2H = (u8)((65536UL - Timer2_Reload/12) / 256);
            T2L = (u8)((65536UL - Timer2_Reload/12) % 256);
        #endif

        T2R = 1;    //开始运行

    #else
        #error "Timer2设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer3_init(void)
// 描述: timer3初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer3_init(void)
{
        T3R = 0;    //停止计数

    #if (Timer3_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer3设置的中断过快!"

    #elif ((Timer3_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET3 = 1;    //允许中断
    //  T3_CT = 1;  //计数
        T3_CT = 0;  //定时
    //  T3CLKO = 1; //输出时钟
        T3CLKO = 0; //不输出时钟

        #if (Timer3_Reload < 65536UL)
            T3x12 = 1;    //1T mode
            T3H = (u8)((65536UL - Timer3_Reload) / 256);
            T3L = (u8)((65536UL - Timer3_Reload) % 256);
        #else
            T3x12 = 0;    //12T mode
            T3H = (u8)((65536UL - Timer3_Reload/12) / 256);
            T3L = (u8)((65536UL - Timer3_Reload/12) % 256);
        #endif

        T3R = 1;    //开始运行

    #else
        #error "Timer3设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer4_init(void)
// 描述: timer4初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer4_init(void)
{
        T4R = 0;    //停止计数

    #if (Timer4_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer4设置的中断过快!"

    #elif ((Timer4_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET4 = 1;    //允许中断
    //  T4_CT = 1;  //计数
        T4_CT = 0;  //定时
    //  T4CLKO = 1; //输出时钟
        T4CLKO = 0; //不输出时钟

        #if (Timer4_Reload < 65536UL)
            T4x12 = 1;    //1T mode
            T4H = (u8)((65536UL - Timer4_Reload) / 256);
            T4L = (u8)((65536UL - Timer4_Reload) % 256);
        #else
            T4x12 = 0;    //12T mode
            T4H = (u8)((65536UL - Timer4_Reload/12) / 256);
            T4L = (u8)((65536UL - Timer4_Reload/12) % 256);
        #endif

        T4R = 1;    //开始运行

    #else
        #error "Timer4设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void timer0_int (void) interrupt TIMER0_VECTOR
// 描述:  timer0中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer0_int (void) interrupt 1
{
    P20 = ~P20;
}

//========================================================================
// 函数: void timer1_int (void) interrupt TIMER1_VECTOR
// 描述:  timer1中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer1_int (void) interrupt 3
{
    P21 = ~P21;
}

//========================================================================
// 函数: void timer2_int (void) interrupt TIMER2_VECTOR
// 描述:  timer2中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer2_int (void) interrupt 12
{
    P22 = ~P22;
}

//========================================================================
// 函数: void timer3_int (void) interrupt TIMER3_VECTOR
// 描述:  timer3中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer3_int(void) interrupt 19
{
    P23 = ~P23;
}

//========================================================================
// 函数: void timer4_int (void) interrupt TIMER4_VECTOR
// 描述:  timer4中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer4_int(void) interrupt 20
{
    P24 = ~P24;
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
        i = MAIN_Fosc / 6000;
        while(--i);   //6T per loop
    }while(--ms);
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
}
