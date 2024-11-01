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

P2口显示流水灯后进入休眠状态.

按板上的P3.2(INT0)、P3.3(INT1)按键唤醒MCU, 再次显示流水灯.

由于开启了定时器0中断, 所以P3.4(T0)按键也可唤醒MCU, 再次显示流水灯.

INT2, INT3, INT4 实验板上没有引出测试按键，供需要时参考使用.

此外程序演示P3.2口复位进入USB下载模式的方法：
   通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
   由于开启USB中断会导致P3.0口通信信号唤醒MCU，本例程不使用USB指令触发MCU复位功能。

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "intrins.h"

#define MAIN_Fosc       24000000L   //定义主时钟

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** 用户定义宏 ***********************************/

#define     Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

/*****************************************************************************/

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

void Timer0_Init(void);
void delay_ms(u8 ms);

/********************** 主函数 ************************/
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

    Timer0_Init();
    EA = 1;     //允许总中断

    while(1)
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
        P74 = 0;		//LED On
        delay_ms(250);
        P74 = 1;		//LED Off

        IE0  = 0;   //外中断0标志位
        IE1  = 0;   //外中断1标志位
        EX0 = 1;    //INT0 Enable
        EX1 = 1;    //INT1 Enable

        IT0 = 1;        //INT0 下降沿中断       
    //  IT0 = 0;        //INT0 上升,下降沿中断  
        IT1 = 1;        //INT1 下降沿中断       
    //  IT1 = 0;        //INT1 上升,下降沿中断  

//        EX2 = 1;    //使能 INT2 下降沿中断
//        EX3 = 1;    //使能 INT3 下降沿中断
//        EX4 = 1;    //使能 INT4 下降沿中断

        while(!P32);  //P3.2口按键按下时不进休眠
        PD = 1;     //Sleep
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
} 

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-3-9
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

/********************* INT0中断函数 *************************/
void INT0_int (void) interrupt 0      //进中断时已经清除标志
{
    EX0 = 0;    //INT0 Disable
}

/********************* INT1中断函数 *************************/
void INT1_int (void) interrupt 2      //进中断时已经清除标志
{
    EX1 = 0;    //INT1 Disable
}

/********************* INT2中断函数 *************************/
void INT2_int (void) interrupt 10     //进中断时已经清除标志
{
    EX2 = 0;    //INT2 Disable
}

/********************* INT3中断函数 *************************/
void INT3_int (void) interrupt 11     //进中断时已经清除标志
{
    EX3 = 0;    //INT3 Disable
}

/********************* INT4中断函数 *************************/
void INT4_int (void) interrupt 16     //进中断时已经清除标志
{
    EX4 = 0;    //INT4 Disable
}

//========================================================================
// 函数: void Timer0_Init(void)
// 描述: 定时器0初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-2
// 备注: 
//========================================================================
void Timer0_Init(void)
{
    // Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
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
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//连续1000ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

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

//========================================================================
// 函数: void timer0(void)
// 描述: Timer0 1ms中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void timer0(void) interrupt 1
{
    KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
}

