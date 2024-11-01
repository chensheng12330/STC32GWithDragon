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

P3.3口按键演示"ShowLong"函数输出长整型数据;

P3.4口按键演示"ShowFloat"函数输出浮点数数据;

P3.5(RST)口按键演示"ShowCode"函数输出8字节数组数据;

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

/*****************************************************************************/

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

u8 xdata cod[8];
float fdata;
u16 Counter;

void delay_ms(u8 ms);
void KeyResetScan(void);

/******************** 主函数 **************************/
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
    EUSB = 1;   //IE2相关的中断使能后，需要重新设置EUSB
    EA = 1;     //打开总中断

    fdata = 3.1415;

    while (1)
    {
        delay_ms(1);
        if (bUsbOutReady)
        {
            //USB_SendData(UsbOutBuffer,64);    //发送数据缓冲区，长度
            
            usb_OUT_done(); //接收应答（固定格式）
        }

        if (DeviceState == DEVSTATE_CONFIGURED)  //判断USB设备是否配置完成，配置完成后才可以进行数据收发
        {
            Counter++;
            if(Counter >= 1000)
            {
                Counter = 0;
                SEG7_ShowFloat(fdata);  //输出数码管浮点数数据
                fdata += 0.0001;   //计算结果会有一点误差，所以最终显示的不一定是4位小数
            }
        }
        
        KeyResetScan();   //长按P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
        
        if (!P33)
        {
            while (!P33);
            SEG7_ShowLong(0x98765432, 16);  //输出数码管长整型数据
        }
//        else if (!P34)
//        {
//            while (!P34);
//            SEG7_ShowFloat(3.1415);  //输出数码管浮点数数据
//        }
        else if (!P35)
        {
            cod[0] = 0x3f;  //数码管段码
            cod[1] = 0x06;
            cod[2] = 0x5b;
            cod[3] = 0x4f;
            cod[4] = 0x66;
            cod[5] = 0x6d;
            cod[6] = 0x7d;
            cod[7] = 0x27;
            while (!P35);
            SEG7_ShowCode(cod);  //输出数码管码值
        }
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
