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

USB-CDC1转CAN1，USB-CDC2转CAN2总线收发测试用例.

CAN1总线收到的数据上传到USB-CDC1接口；
CAN2总线收到的数据上传到USB-CDC2接口；

USB-CDC1接口读取的数据包转发到CAN1总线；
USB-CDC2接口读取的数据包转发到CAN2总线；

默认CAN总线波特率500KHz, 用户可自行修改.

下载时, 选择时钟 24MHz.

******************************************/

#include "stc.h"
#include "usb.h"
#include "can.h"

void sys_init();

void main()
{
    sys_init();
    CanInit();
    usb_init();
    EA = 1;
    
    while (1)
    {
        CanPolling();
    }
}

void sys_init()
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
}
