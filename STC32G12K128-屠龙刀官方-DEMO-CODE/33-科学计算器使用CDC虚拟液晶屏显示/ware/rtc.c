#include "rtc.h"

bit B1S_Flag;

void RTC_config(void)
{
//    INIYEAR = 21;     //Y:2021
//    INIMONTH = 12;    //M:12
//    INIDAY = 31;      //D:31
//    INIHOUR = 23;     //H:23
//    INIMIN = 59;      //M:59
//    INISEC = 50;      //S:50
//    INISSEC = 0;      //S/128:0

    ALAHOUR = 0;	//闹钟小时
    ALAMIN  = 0;	//闹钟分钟
    ALASEC  = 0;	//闹钟秒
    ALASSEC = 0;	//闹钟1/128秒

    //STC32G 芯片使用内部32K时钟，休眠无法唤醒
    IRC32KCR = 0x80;   //启动内部32K晶振.
    while (!(IRC32KCR & 1));  //等待时钟稳定
    RTCCFG = 0x03;    //选择内部32K时钟源，触发RTC寄存器初始化

//    X32KCR = 0x80 + 0x40;   //启动外部32K晶振, 低增益+0x00, 高增益+0x40.
//    while (!(X32KCR & 1));  //等待时钟稳定
//    RTCCFG = 0x01;    //选择外部32K时钟源，触发RTC寄存器初始化

    RTCIF = 0x00;     //清中断标志
    RTCIEN = 0x04;    //中断使能, 0x80:闹钟中断, 0x40:日中断, 0x20:小时中断, 0x10:分钟中断, 0x08:秒中断, 0x04:1/2秒中断, 0x02:1/8秒中断, 0x01:1/32秒中断
    RTCCR = 0x01;     //RTC使能

    while(RTCCFG & 0x01);	//等待初始化完成,需要在 "RTC使能" 之后判断. 
    //设置RTC时间需要32768Hz的1个周期时间,大约30.5us. 由于同步, 所以实际等待时间是0~30.5us.
    //如果不等待设置完成就睡眠, 则RTC会由于设置没完成, 停止计数, 唤醒后才继续完成设置并继续计数.
}

void RTC_Isr() interrupt 36
{
    if(RTCIF & 0x04) //判断是否秒中断
    {
        RTCIF &= ~0x04; //清中断标志
        B1S_Flag = 1;
    } 
}

//    if(B1S_Flag) {
//    B1S_Flag = 0;
//    printf("Year=20%bd ", YEAR);
//    printf("Month=%bd ", MONTH);
//    printf("Day=%bd ", DAY);
//    printf("Hour=%bd ", HOUR);
//    printf("Minute=%bd ", MIN);
//    printf("Second=%bd ", SEC);
//    printf("\r\n"); }