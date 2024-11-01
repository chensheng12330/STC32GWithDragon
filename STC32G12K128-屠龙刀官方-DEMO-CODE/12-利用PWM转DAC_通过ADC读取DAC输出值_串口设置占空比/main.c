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

从P1.0(PWM1P)、P1.1(PWM1N)输出16位的互补对称PWM, 输出的PWM经过RC滤波成直流电压送P1.4、P1.5做ADC采样并用串口打印结果.

串口1配置为115200bps, 8,n, 1, 切换到P1.6 P1.7, 下载后就可以直接测试. 通过串口1设置占空比.

串口命令使用ASCII码的数字，比如： 10，就是设置占空比为10/256， 100： 就是设置占空比为100/256。

可以设置的值为0~256, 0为连续低电平.

核心转接板ADC默认基准电压是2.5V，ADC正常采样电压范围是0~2.5V，AD口电压大于等于2.5V时采样的ADC值为4095。

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件
#include "stdio.h"
#include "intrins.h"

/****************************** 用户定义宏 ***********************************/

#define MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

/*****************************************************************************/

bit B_1ms;          //1ms标志
u8  cnt200ms;

#define     Baudrate1           (65536 - MAIN_Fosc / 115200 / 4)
#define     UART1_BUF_LENGTH    128     //串口缓冲长度

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

u8  RX1_TimeOut;
u8  TX1_Cnt;    //发送计数
u8  RX1_Cnt;    //接收计数
bit B_TX1_Busy; //发送忙标志
u16 adc;

u8  RX1_Buffer[UART1_BUF_LENGTH]; //接收缓冲

void UART1_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void UART1_TxByte(u8 dat);
void UpdatePwm(u16 pwm_value);
u16  Get_ADC12bitResult(u8 channel); //channel = 0~15
void KeyResetScan(void);

/********************* 主函数 *************************/
void main(void)
{
    u8  i;
    u16 j;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    RSTFLAG |= 0x04;   //设置硬件复位后需要检测P3.2的状态选择运行区域，否则硬件复位后进入USB下载模式

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x30;   P1M0 = 0x00;   //设置 P1.4,P1.5 为 ADC 口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    usb_init();

    //  Timer0初始化
    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    //  ADC初始化
    ADCTIM = 0x3f;      //设置 ADC 内部时序，ADC采样时间建议设最大值
    ADCCFG = 0x2f;      //设置 ADC 转换结果右对齐,时钟为系统时钟/2/16
    ADC_CONTR = 0x84;   //使能 ADC 模块

    PWMA_CCER1 = 0x00;  //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCMR1 = 0x60;  //设置 PWM1 模式1 输出
    PWMA_CCER1 = 0x05;  //使能 CC1NE,CC1E 通道

    PWMA_ARRH = 0;      //设置周期时间
    PWMA_ARRL = 255;
    PWMA_CCR1H = 0;
    PWMA_CCR1L = 220; //设置占空比时间

    PWMA_ENO = 0x03;  //使能 PWM1P/PWM1N 输出
    PWMA_PS = 0x00;   //高级 PWM 通道输出脚选择位, 0x00:P1.0/P1.1, 0x01:P2.0/P2.1, 0x02:P6.0/P6.1
    PWMA_BKR = 0x80;  //使能主输出
    PWMA_CR1 |= 0x01; //开始计时

//    UpdatePwm(220);

    UART1_config(2);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
    EUSB = 1;   //IE2相关的中断使能后，需要重新设置EUSB
    EA = 1;     //打开总中断

    printf("PWM和ADC测试程序, 输入占空比为 0~256!\r\n");  //UART1发送一个字符串
    
    while (1)
    {
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码

            if(++cnt200ms >= 200)   //200ms读一次ADC
            {
                cnt200ms = 0;
                Get_ADC12bitResult(4);
                Get_ADC12bitResult(4);
                j = Get_ADC12bitResult(4);  //参数0~15,查询方式做一次ADC, 返回值就是结果, == 4096 为错误
                printf("ADC4=0x%04x ",j);   //显示ADC值
                Get_ADC12bitResult(5);
                Get_ADC12bitResult(5);
                j = Get_ADC12bitResult(5);  //参数0~15,查询方式做一次ADC, 返回值就是结果, == 4096 为错误
                printf("ADC5=0x%04x ",j);
                printf("\r\n");
            }

            if(RX1_TimeOut > 0)     //超时计数
            {
                if(--RX1_TimeOut == 0)
                {
                    if((RX1_Cnt > 0) && (RX1_Cnt <= 3)) //限制为3位数字
                    {
                        F0 = 0; //错误标志
                        j = 0;
                        for(i=0; i<RX1_Cnt; i++)
                        {
                            if((RX1_Buffer[i] >= '0') && (RX1_Buffer[i] <= '9'))    //限定为数字
                            {
                                j = j * 10 + RX1_Buffer[i] - '0';
                            }
                            else
                            {
                                F0 = 1; //接收到非数字字符, 错误
                                printf("错误! 接收到非数字字符! 占空比为0~256!\r\n");
                                break;
                            }
                        }
                        if(!F0)
                        {
                            if(j > 256) printf("错误! 输入占空比过大, 请不要大于256!\r\n");
                            else
                            {
                                UpdatePwm(j);
                                printf("已更新占空比!\r\n");
                            }
                        }
                    }
                    else  printf("错误! 输入字符过多! 输入占空比为0~256!\r\n");
                    RX1_Cnt = 0;
                }
            }
        }

        if (bUsbOutReady) //USB调试及复位所需代码
        {
            usb_OUT_done();
        }
    }
}
//========================================================================


//========================================================================
// 函数: void UART1_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    SBUF = dat;
    B_TX1_Busy = 1;
    while(B_TX1_Busy);
}

char putchar(char c)
{
	UART1_TxByte(c);
	return c;
}

//========================================================================
// 函数: void PrintString1(u8 *puts)
// 描述: 串口1发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
//void PrintString1(u8 *puts) //发送一个字符串
//{
//    for (; *puts != 0;  puts++)   UART1_TxByte(*puts);  //遇到停止符0结束
//}

//========================================================================
// 函数: void SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // 选择Timer2做波特率发生器
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
// 函数: void UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_config(u8 brt)
{
    /*********** 波特率使用定时器2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)Baudrate1);
    }

    /*********** 波特率使用定时器1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;//Timer1_16bitAutoReload;
        TH1 = (u8)(Baudrate1 / 256);
        TL1 = (u8)(Baudrate1 % 256);
        ET1 = 0;    //禁止中断
        TR1 = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//  PS  = 1;    //高优先级中断
    ES  = 1;    //允许中断
    REN = 1;    //允许接收
    P_SW1 &= 0x3f;
    P_SW1 |= 0x80;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;
}

//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;    //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;    //Clear Tx flag
        B_TX1_Busy = 0;
    }
}

/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
    B_1ms = 1;      //1ms标志
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
// 函数: UpdatePwm(u16 pwm_value)
// 描述: 更新PWM值. 
// 参数: pwm_value: pwm值, 这个值是输出高电平的时间.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(u16 pwm_value)
{
    PWMA_CCR1H = (u8)(pwm_value >> 8); //设置占空比时间
    PWMA_CCR1L = (u8)(pwm_value);
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
	u16 i;
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i);
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
