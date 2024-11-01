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

使用例程测试需要修改基准电压为VCC，将R114电阻移到R113。

从P1.0(PWM1P)输出16位的PWM, 输出的PWM经过RC滤波成直流电压送P1.4进行ADC采样并对输出结果进行反馈。

串口1配置为115200,8,n,1, 使用P3.0 P3.1接口通信, 下载后就可以直接测试。通过串口1设置占空比。

串口命令使用ASCII码的数字，比如： 10，就是设置占空比为10/256， 100： 就是设置占空比为100/256。

可以设置的值为0~256, 0为连续低电平, 256为连续高电平。

下载时, 选择时钟 22.1184MHz (用户可自行修改频率)。

******************************************/

#include "../comm/stc32g.h"   //包含此头文件后，不需要再包含"reg51.h"头文件
#include "stdio.h"
#include "intrins.h"

#define MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）

typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;

/****************************** 用户定义宏 ***********************************/

#define Baudrate1       115200L
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define PERIOD          255L   //PWM周期
#define THRESHOLD       2      //PWM调节差值门槛

#define UART1_BUF_LENGTH    128     //串口缓冲长度

/*****************************************************************************/


/*************  本地变量声明    **************/

bit B_1ms;          //1ms标志
u8  cnt200ms;

u8  RX1_TimeOut;
u8  TX1_Cnt;    //发送计数
u8  RX1_Cnt;    //接收计数
bit B_TX1_Busy; //发送忙标志
u16 adc;
u16 duty;
u16 feedback;

u8  RX1_Buffer[UART1_BUF_LENGTH];   //接收缓冲

void UART1_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void UART1_TxByte(u8 dat);
void UpdatePwm(u16 pwm_value);
u16 Get_ADC12bitResult(u8 channel); //channel = 0~15

/******************** 主函数 **************************/
void main(void)
{
    u8  i;
    u16 j;
    
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    P1M1 = 0x30;   P1M0 = 0x00;   //设置 P1.4,P1.5 为 ADC 口
    
    duty = 128;
    feedback = duty;

    //  Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    //  ADC初始化
    ADCTIM = 0x3f;      //设置 ADC 内部时序，ADC采样时间建议设最大值
    ADCCFG = 0x2f;      //设置 ADC 转换结果右对齐,时钟为系统时钟/2/16
    ADC_CONTR = 0x84;   //使能 ADC 模块

    PWMA_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCMR1 = 0x60; //设置 PWM1 模式1 输出
    PWMA_CCER1 = 0x05; //使能 CC1E 通道
    PWMA_CCMR1 |= 0x08; //输出比较预装载使能(CCxE为1才可写)

    PWMA_ARRH = (u8)(PERIOD>>8); //设置周期时间
    PWMA_ARRL = (u8)PERIOD;
    PWMA_CCR1H = (u8)(duty>>8);
    PWMA_CCR1L = (u8)duty;       //设置占空比时间

    PWMA_ENO = 0x01; //使能 PWM1P 输出
//  PWMA_ENO = 0x02; //使能 PWM1N 输出
    PWMA_PS = 0x00;  //高级 PWM1(P/N) 输出脚通道选择位, 0x00:P1.0/P1.1, 0x01:P2.0/P2.1, 0x02:P6.0/P6.1
    PWMA_BKR = 0x80; //使能主输出
    PWMA_CR1 |= 0x01; //开始计时

    UART1_config(2);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
    EA = 1;     //打开总中断

    printf("PWM转DAC测试程序, 输入占空比为 0~256!\r\n");  //UART1发送一个字符串
    
    while (1)
    {
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            if(++cnt200ms >= 200)   //200ms读一次ADC
            {
                cnt200ms = 0;
                adc = Get_ADC12bitResult(4);  //参数0~15,查询方式做一次ADC, 返回值就是结果
                j = (u16)((duty*4096UL)/(PERIOD+1));    //计算ADC采样理论值
                if(j>adc)
                {
                    if(((j-adc)>THRESHOLD) && (feedback<=PERIOD)) feedback++;
                }
                else
                {
                    if(((adc-j)>THRESHOLD) && (feedback>0)) feedback--;
                }
                printf("ADC=%u,duty=%u,feedback=%u\r\n",adc,duty,feedback);
                UpdatePwm(feedback);    //更新占空比
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
                                printf("错误! 接收到非数\xfd字字符! 占空比为0~%u!\r\n",(u16)PERIOD+1);
                                break;
                            }
                        }
                        if(!F0)
                        {
                            if(j > (PERIOD+1)) printf("错误! 输入占空比过\xfd大, 请不要大于%u!\r\n",(u16)PERIOD+1);
                            else
                            {
                                duty = j;
                                feedback = j;
                                UpdatePwm(duty);    //更新占空比
                                printf("更新占空比=%u.\r\n",duty);
                            }
                        }
                    }
                    else  printf("错误! 输入字符过\xfd多! 例\xfd程限制3位数\xfd字!\r\n");  //带FD编码汉字需在后面添加"\xfd"才能正常显示
                    RX1_Cnt = 0;
                }
            }
        }
    }
}

//========================================================================
// 函数: void UART1_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    B_TX1_Busy = 1;
    SBUF = dat;
    while(B_TX1_Busy);
}

char putchar(char c)
{
	UART1_TxByte(c);
	return c;
}

//========================================================================
// 函数: void SetTimer2Baudraye(u32 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u32 dat)  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //禁止中断
    T2R = 1;    //Timer run enable
}

//========================================================================
// 函数: void UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    /*********** 波特率使用定时器2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate1);
    }

    /*********** 波特率使用定时器1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;      //S1 BRT Use Timer1;
        T1_CT = 0;      //Timer1 set As Timer
        T1x12 = 1;      //Timer1 set as 1T mode
        TMOD &= ~0x30;  //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
        ET1 = 0;    //禁止中断
        TR1  = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//  PS  = 1;    //高优先级中断
    ES  = 1;    //允许中断
    REN = 1;    //允许接收
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
//  PCON2 |=  (1<<4);   //内部短路RXD与TXD, 做中继, ENABLE,DISABLE

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
        RI = 0;
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;    //防溢出
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;
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
    ADC_CONTR = (ADC_CONTR & 0xF0) | 0x40 | channel;    //启动 AD 转换
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

