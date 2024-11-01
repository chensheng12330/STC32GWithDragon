#ifndef __TIM_H
#define __TIM_H

#include "stc.h"

extern u32 xdata Sys_Time_10ms ;
extern u32 xdata Time_sleep;
#define Timer0_Reload   (MAIN_Fosc / 100)      //Timer 0 中断频率, 10次/秒 100hz的速度
#define Timer1_Reload   (MAIN_Fosc / 100)      //Timer 1 中断频率, 2000次/秒

void Timer0_init(void);
    
#endif

