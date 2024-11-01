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

高级PWM定时器 PWM5,PWM6,PWM7,PWM8 每个通道都可独立实现PWM输出.

4个通道PWM设置对应P20~P23输出口.

通过P2口上连接的LED灯，利用PWM实现呼吸灯效果.

PWM周期和占空比可以自定义设置，最高可达65535.

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件

/****************************** 用户定义宏 ***********************************/

#define MAIN_Fosc       24000000L   //定义主时钟
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

/*****************************************************************************/

#define PWM5_1      0x00	//P2.0
#define PWM5_2      0x01	//P1.7
#define PWM5_3      0x02	//P0.0
#define PWM5_4      0x03	//P7.4

#define PWM6_1      0x00	//P2.1
#define PWM6_2      0x04	//P5.4
#define PWM6_3      0x08	//P0.1
#define PWM6_4      0x0C	//P7.5

#define PWM7_1      0x00	//P2.2
#define PWM7_2      0x10	//P3.3
#define PWM7_3      0x20	//P0.2
#define PWM7_4      0x30	//P7.6

#define PWM8_1      0x00	//P2.3
#define PWM8_2      0x40	//P3.4
#define PWM8_3      0x80	//P0.3
#define PWM8_4      0xC0	//P7.7

#define ENO5P       0x01
#define ENO6P       0x04
#define ENO7P       0x10
#define ENO8P       0x40

#define PWM_PERIOD  1023    //设置周期值

/***************************** 本地变量声明 **********************************/
bit B_1ms;          //1ms标志

u16 PWM5_Duty;
u16 PWM6_Duty;
u16 PWM7_Duty;
u16 PWM8_Duty;

bit PWM5_Flag;
bit PWM6_Flag;
bit PWM7_Flag;
bit PWM8_Flag;

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

void UpdatePwm(void);
void KeyResetScan(void);

/******************************** 主函数 ************************************/
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

    PWM5_Flag = 0;
    PWM6_Flag = 0;
    PWM7_Flag = 0;
    PWM8_Flag = 0;
	
    PWM5_Duty = 0;
    PWM6_Duty = 256;
    PWM7_Duty = 512;
    PWM8_Duty = 1024;

    usb_init();

    //  Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    PWMB_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMB_CCER2 = 0x00;
    PWMB_CCMR1 = 0x60; //通道模式配置
    PWMB_CCMR2 = 0x60;
    PWMB_CCMR3 = 0x60;
    PWMB_CCMR4 = 0x60;
    PWMB_CCER1 = 0x33; //配置通道输出使能和极性
    PWMB_CCER2 = 0x33;

    PWMB_ARRH = (u8)(PWM_PERIOD >> 8); //设置周期时间
    PWMB_ARRL = (u8)PWM_PERIOD;

    PWMB_ENO = 0x00;
    PWMB_ENO |= ENO5P; //使能输出
    PWMB_ENO |= ENO6P; //使能输出
    PWMB_ENO |= ENO7P; //使能输出
    PWMB_ENO |= ENO8P; //使能输出

    PWMB_PS = 0x00;    //高级 PWM 通道输出脚选择位
    PWMB_PS |= PWM5_1; //选择 PWM5_1 通道
    PWMB_PS |= PWM6_1; //选择 PWM6_1 通道
    PWMB_PS |= PWM7_1; //选择 PWM7_1 通道
    PWMB_PS |= PWM8_1; //选择 PWM8_1 通道

    PWMB_BKR = 0x80;   //使能主输出
    PWMB_CR1 |= 0x01;  //开始计时

    EUSB = 1;   //IE2相关的中断位操作使能后，需要重新设置EUSB
    EA = 1;     //打开总中断

    while (1)
    {
        if (bUsbOutReady) //USB调试及复位所需代码
        {
            //USB_SendData(UsbOutBuffer,64);    //发送数据缓冲区，长度
            
            usb_OUT_done();
        }
    }
}


/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
    if(!PWM5_Flag)
    {
        PWM5_Duty++;
        if(PWM5_Duty > PWM_PERIOD) PWM5_Flag = 1;
    }
    else
    {
        PWM5_Duty--;
        if(PWM5_Duty <= 0) PWM5_Flag = 0;
    }

    if(!PWM6_Flag)
    {
        PWM6_Duty++;
        if(PWM6_Duty > PWM_PERIOD) PWM6_Flag = 1;
    }
    else
    {
        PWM6_Duty--;
        if(PWM6_Duty <= 0) PWM6_Flag = 0;
    }

    if(!PWM7_Flag)
    {
        PWM7_Duty++;
        if(PWM7_Duty > PWM_PERIOD) PWM7_Flag = 1;
    }
    else
    {
        PWM7_Duty--;
        if(PWM7_Duty <= 0) PWM7_Flag = 0;
    }

    if(!PWM8_Flag)
    {
        PWM8_Duty++;
        if(PWM8_Duty > PWM_PERIOD) PWM8_Flag = 1;
    }
    else
    {
        PWM8_Duty--;
        if(PWM8_Duty <= 0) PWM8_Flag = 0;
    }
    
    UpdatePwm();
    KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
}


//========================================================================
// 函数: UpdatePwm(void)
// 描述: 更新PWM占空比. 
// 参数: none.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(void)
{
    PWMB_CCR5H = (u8)(PWM5_Duty >> 8); //设置占空比时间
    PWMB_CCR5L = (u8)(PWM5_Duty);
    PWMB_CCR6H = (u8)(PWM6_Duty >> 8); //设置占空比时间
    PWMB_CCR6L = (u8)(PWM6_Duty);
    PWMB_CCR7H = (u8)(PWM7_Duty >> 8); //设置占空比时间
    PWMB_CCR7L = (u8)(PWM7_Duty);
    PWMB_CCR8H = (u8)(PWM8_Duty >> 8); //设置占空比时间
    PWMB_CCR8L = (u8)(PWM8_Duty);
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
