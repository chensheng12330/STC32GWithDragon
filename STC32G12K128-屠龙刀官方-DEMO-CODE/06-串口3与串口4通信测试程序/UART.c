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

P5.0(RXD3)口串100欧电阻到P5.3(TXD4)口进行测试.

串口4定时发送一个字节累加数据给串口3.

串口3将接收到的数据从P2口的LED灯显示出来.

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 22.1184MHZ (用户可自行修改频率).

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件

/****************************** 用户定义宏 ***********************************/

#define MAIN_Fosc       22118400L   //定义主时钟
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define Baudrate3   115200UL
#define Baudrate4   115200UL

#define UART3_BUF_LENGTH    64
#define UART4_BUF_LENGTH    64

/*****************************************************************************/

/*************  本地变量声明    **************/
bit B_1ms;      //1ms标志
u16 Sec_Cnt;    //1秒计数
u8  TX_Data;    //发送数据

u8  TX3_Cnt;    //发送计数
u8  RX3_Cnt;    //接收计数
u8  TX4_Cnt;    //发送计数
u8  RX4_Cnt;    //接收计数
bit B_TX3_Busy; //发送忙标志
bit B_TX4_Busy; //发送忙标志
u8 RX3_TimeOut;
u8 RX4_TimeOut;

u8  RX3_Buffer[UART3_BUF_LENGTH]; //接收缓冲
u8  RX4_Buffer[UART4_BUF_LENGTH]; //接收缓冲

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

void UART3_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
void UART4_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
void UART3_TxByte(u8 dat);
void UART4_TxByte(u8 dat);
void PrintString3(u8 *puts);
void PrintString4(u8 *puts);
void KeyResetScan(void);
/******************** 主函数 **************************/
void main(void)
{
//    u8 i;
	
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    RSTFLAG |= 0x04;   //设置硬件复位后需要检测P3.2的状态选择运行区域，否则硬件复位后进入USB下载模式

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x0a;   //设置P5.1,P5.3为推挽输出
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    usb_init();

    //Timer0初始化
    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    UART3_config(3);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
    UART4_config(4);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
    EUSB = 1;   //IE2相关的中断使能后，需要重新设置EUSB
    EA = 1;     //打开总中断

//    PrintString3("STC32G UART3-UART4 Test Programme!\r\n");  //UART3发送一个字符串
//    PrintString4("STC32G UART4-UART3 Test Programme!\r\n");  //UART4发送一个字符串

    while (1)
    {
        if (bUsbOutReady) //USB调试及复位所需代码
        {
            //USB_SendData(UsbOutBuffer,64);    //发送数据缓冲区，长度
            
            usb_OUT_done();
        }

        if(B_1ms)
        {
            B_1ms = 0;
            KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码

            Sec_Cnt++;
            if(Sec_Cnt>=200)
            {
                Sec_Cnt = 0;
                UART4_TxByte(TX_Data++);  //发送计数值给串口3
            }

            if(RX3_TimeOut > 0)     //超时计数
            {
                if(--RX3_TimeOut == 0)
                {
                    P2 = ~RX3_Buffer[0];  //串口3把收到的数据发到P0口，通过P0口的LED显示
                    RX3_Cnt  = 0;   //清除字节数
                }
            }

//            if(RX4_TimeOut > 0)     //超时计数
//            {
//                if(--RX4_TimeOut == 0)
//                {
//                    for(i=0; i<RX4_Cnt; i++)    UART4_TxByte(RX4_Buffer[i]);    //把收到的数据通过串口4输出
//                    RX4_Cnt  = 0;   //清除字节数
//                }
//            }
        }
    }
}


/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
	B_1ms = 1;
}

//========================================================================
// 函数: void UART3_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
//void UART3_TxByte(u8 dat)
//{
//    B_TX3_Busy = 1;
//    S3BUF = dat;
//    while(B_TX3_Busy);
//}

//========================================================================
// 函数: void UART4_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
void UART4_TxByte(u8 dat)
{
    B_TX4_Busy = 1;
    S4BUF = dat;
    while(B_TX4_Busy);
}

//========================================================================
// 函数: void PrintString3(u8 *puts)
// 描述: 串口3发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
//void PrintString3(u8 *puts)
//{
//    for (; *puts != 0;  puts++)     //遇到停止符0结束
//    {
//        UART3_TxByte(*puts);
//    }
//}

//========================================================================
// 函数: void PrintString4(u8 *puts)
// 描述: 串口4发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
//void PrintString4(u8 *puts)
//{
//    for (; *puts != 0;  puts++)     //遇到停止符0结束
//    {
//        UART4_TxByte(*puts);
//    }
//}

//========================================================================
// 函数: SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u16 dat)
{
    T2R = 0;	//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //禁止中断
    T2R = 1;	//Timer run enable
}

//========================================================================
// 函数: void UART3_config(u8 brt)
// 描述: UART3初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART3_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
{
    if(brt == 2)
    {
        SetTimer2Baudraye((u8)(65536UL - (MAIN_Fosc / 4) / Baudrate3));
        S3CON = 0x10;       //8位数据, 使用Timer2做波特率发生器, 允许接收
    }
    else
    {
        T3R = 0;          //Timer stop
        S3CON = 0x50;     //8位数据, 使用Timer3做波特率发生器, 允许接收
        T3H = (65536UL - (MAIN_Fosc / 4) / Baudrate3) / 256;
        T3L = (65536UL - (MAIN_Fosc / 4) / Baudrate3) % 256;
        T3_CT = 0;	//Timer3 set As Timer
        T3x12 = 1;	//Timer3 set as 1T mode
        T3R = 1;	//Timer run enable
    }
    ES3  = 1;       //允许UART3中断
    S3_S = 1;       //UART3 switch bit1 to: 0: P0.0 P0.1,  1: P5.0 P5.1

    B_TX3_Busy = 0;
    TX3_Cnt = 0;
    RX3_Cnt = 0;
}

//========================================================================
// 函数: void UART4_config(u8 brt)
// 描述: UART4初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART4_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
{
    if(brt == 2)
    {
        SetTimer2Baudraye((u8)(65536UL - (MAIN_Fosc / 4) / Baudrate4));
        S4CON = 0x10;       //8位数据, 使用Timer2做波特率发生器, 允许接收
    }
    else
    {
        T4R = 0;	//Timer stop
        S4CON = 0x50;       //8位数据, 使用Timer4做波特率发生器, 允许接收
        T4H = (65536UL - (MAIN_Fosc / 4) / Baudrate4) / 256;
        T4L = (65536UL - (MAIN_Fosc / 4) / Baudrate4) % 256;
        T4_CT = 0;	//Timer3 set As Timer
        T4x12 = 1;	//Timer3 set as 1T mode
        T4R = 1;	//Timer run enable
    }
    ES4  = 1;       //允许UART4中断
    S4_S = 1;       //UART4 switch bit2 to: 0: P0.2 P0.3, 1: P5.2 P5.3

    B_TX4_Busy = 0;
    TX4_Cnt = 0;
    RX4_Cnt = 0;
}

//========================================================================
// 函数: void UART3_int (void) interrupt UART3_VECTOR
// 描述: UART3中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART3_int (void) interrupt 17
{
    if(S3RI)
    {
        S3RI = 0;    //Clear Rx flag
        RX3_Buffer[RX3_Cnt] = S3BUF;
        if(++RX3_Cnt >= UART3_BUF_LENGTH)   RX3_Cnt = 0;
        RX3_TimeOut = 5;
    }

    if(S3TI)
    {
        S3TI = 0;   //Clear Tx flag
        B_TX3_Busy = 0;
    }
}

//========================================================================
// 函数: void UART4_int (void) interrupt UART4_VECTOR
// 描述: UART4中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART4_int (void) interrupt 18
{
    if(S4RI)
    {
        S4RI = 0;    //Clear Rx flag
        if(RX4_Cnt >= UART4_BUF_LENGTH)   RX4_Cnt = 0;
        RX4_Buffer[RX4_Cnt] = S4BUF;
        RX4_Cnt++;
        RX4_TimeOut = 5;
    }

    if(S4TI)
    {
        S4TI = 0;    //Clear Tx flag
        B_TX4_Busy = 0;
    }
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
