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

上电通过测量内部15通道ADC，反推外部电源电压，电源电压在 4.7V~5.3V 之间正常，反之报错。

所有LED灯间隔一个交替闪烁，如果相邻的LED灯同时亮/灭，说明两者之间有短路。

串口4向串口3发送累加数据，串口3收到后与发送值比较，不同的话报错。

报错时所有LED灯同时快速亮/灭。

注意：
1. 串口3，串口4通信测试需要连接 R79,R80 电阻，并取消P5.0,P5.1,P5.2,P5.3口切换输出高低电平。

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 24MHZ (用户可自行修改频率)。

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件
#include "intrins.h"

/****************************** 用户定义宏 ***********************************/

#define MAIN_Fosc       24000000L   //定义主时钟
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define Baudrate3   115200UL
#define Baudrate4   115200UL

#define UART3_BUF_LENGTH    64
#define UART4_BUF_LENGTH    64

/*****************************************************************************/

u16 Bandgap;
u16 vcc;

u8  TX_Data;    //发送数据

u8  TX3_Cnt;    //发送计数
u8  RX3_Cnt;    //接收计数
u8  TX4_Cnt;    //发送计数
u8  RX4_Cnt;    //接收计数
bit B_Err_Flag; //错误标志
bit B_TX3_Busy; //发送忙标志
bit B_TX4_Busy; //发送忙标志
u8 RX3_TimeOut;
u8 RX4_TimeOut;

u8  xdata RX3_Buffer[UART3_BUF_LENGTH]; //接收缓冲
u8  xdata RX4_Buffer[UART4_BUF_LENGTH]; //接收缓冲

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

void Timer0_Init(void);
void ADC_Init(void);
u16  Get_ADC12bitResult(u8 channel); //channel = 0~15
void delay_ms(u8 ms);
void TestError(void);
void TestIO(void);
void UART3_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
void UART4_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
void UART3_TxByte(u8 dat);
void UART4_TxByte(u8 dat);
void KeyResetScan(void);

/******************** 主函数 **************************/
void main(void)
{
    u8  i;
    u16 j;
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
    Timer0_Init();
    ADC_Init();
    UART3_config(3);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
    UART4_config(4);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
    EUSB = 1;   //IE2相关的中断使能后，需要重新设置EUSB
    EA = 1;     //打开总中断

    delay_ms(2);
    Bandgap = (CHIPID7 << 8) + CHIPID8;
    B_Err_Flag = 0;
    
    Get_ADC12bitResult(15);  //先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
    Get_ADC12bitResult(15);  //先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
    for(j=0, i=0; i<16; i++)
    {
        j += Get_ADC12bitResult(15); //读内部基准ADC, 读15通道
    }
    j >>= 4;   //16次平均

    vcc = (int)(4096L * Bandgap / j);            //(12位ADC算法)计算VREF管脚电压,即电池电压
    
    if(((vcc > 2400) && (vcc < 2600)) || ((vcc > 4700) && (vcc < 5300)))  //使用2.5V基准电压 或者 使用VCC做基准电压
    {
        while(1)
        {
            TestIO();
//            UART4_TxByte(++TX_Data);  //发送计数值给串口3（需要取消P5.0,P5.3口切换显示）
//            if(B_Err_Flag)
//            {
//                TestError();
//            }
        }
    }
    else
    {
//        P0 = vcc >> 8;
//        P2 = vcc;
//        while(1);
        TestError();
    }
}

/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
    if(RX3_TimeOut > 0)     //超时计数
    {
        if(--RX3_TimeOut == 0)
        {
            if(RX3_Buffer[0] != TX_Data) B_Err_Flag = 1;  //判断串口3收到的数据是否正确
            RX3_Cnt  = 0;   //清除字节数
        }
    }

    if (bUsbOutReady) //USB调试及复位所需代码
    {
//        memcpy(UsbInBuffer, UsbOutBuffer, 64);      //原路返回, 用于测试HID
//        usb_IN();
        
        usb_OUT_done();
    }

    KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
}

//========================================================================
// 函数: void TestError(void)
// 描述: 延时函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-2
// 备注: 
//========================================================================
void TestError(void)
{
    while(1)
    {
        P0 = 0;		//LED On
        P1 = 0;		//LED On
        P2 = 0;		//LED On
        P3 = 0;		//LED On
        P4 = 0;		//LED On
        P5 = 0;		//LED On
        P6 = 0;		//LED On
        P7 = 0;		//LED On
        delay_ms(250);
        P0 = 0xff;	//LED Off
        P1 = 0xff;	//LED Off
        P2 = 0xff;	//LED Off
        P3 = 0xff;	//LED Off
        P4 = 0xff;	//LED Off
        P5 = 0xff;	//LED Off
        P6 = 0xff;	//LED Off
        P7 = 0xff;	//LED Off
        delay_ms(250);
    }
}

//========================================================================
// 函数: void TestIO(void)
// 描述: 延时函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-2
// 备注: 
//========================================================================
void TestIO(void)
{
    P7 = 0xb5;	//LED On
    P4 = 0x4a;	//LED On
    P0 = 0xaa;	//LED On
    P5 = 0x16;	//LED On
    P6 = 0x5a;	//LED On
    P1 = 0x5a;	//LED On
    P2 = 0x55;	//LED On
    P3 = 0x55;	//LED On
    
    delay_ms(250);
    delay_ms(250);

    P7 = ~0xb5;	//LED Off
    P4 = ~0x4a;	//LED Off
    P0 = ~0xaa;	//LED Off
    P5 = ~0x16;	//LED Off
    P6 = ~0x5a;	//LED Off
    P1 = ~0x5a;	//LED Off
    P2 = ~0x55;	//LED Off
    P3 = ~0x55;	//LED Off
    
    delay_ms(250);
    delay_ms(250);
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
    //  Timer0初始化
    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// 函数: void ADC_Init(void)
// 描述: ADC初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-2
// 备注: 
//========================================================================
void ADC_Init(void)
{
    ADCTIM = 0x3f;		//设置 ADC 内部时序，ADC采样时间建议设最大值
    ADCCFG = 0x2f;		//设置 ADC 时钟为系统时钟/2/16
    ADC_CONTR = 0x8f;   //使能 ADC 模块
}

//========================================================================
// 函数: u16 Get_ADC12bitResult(u8 channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 12位ADC结果.
// 版本: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult(u8 channel)  //channel = 0~15
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //设置ADC转换通道
    ADC_START = 1;//启动ADC转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //清除ADC结束标志
    return  (((u16)ADC_RES << 8) | ADC_RESL);
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
          while(--i);   //6T per loop
     }while(--ms);
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
    T2R = 0;		//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //禁止中断
    T2R = 1;		//Timer run enable
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
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate3));
        S3CON = 0x10;       //8位数据, 使用Timer2做波特率发生器, 允许接收
    }
    else
    {
        T3R = 0;		//Timer stop
        S3CON = 0x50;       //8位数据, 使用Timer3做波特率发生器, 允许接收
        T3H = (65536UL - (MAIN_Fosc / 4) / Baudrate3) / 256;
        T3L = (65536UL - (MAIN_Fosc / 4) / Baudrate3) % 256;
        T3_CT = 0;	//Timer3 set As Timer
        T3x12 = 1;	//Timer3 set as 1T mode
        T3R = 1;		//Timer run enable
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
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate4));
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
    S4_S = 0;       //UART4 switch bit2 to: 0: P0.2 P0.3, 1: P5.2 P5.3

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
        RX4_Buffer[RX4_Cnt] = S2BUF;
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
