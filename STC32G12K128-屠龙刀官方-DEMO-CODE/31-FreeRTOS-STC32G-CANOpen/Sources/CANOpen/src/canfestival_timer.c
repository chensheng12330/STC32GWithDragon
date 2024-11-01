#include "STC32G_Timer.h"
#include "STC32G_NVIC.h"

#include "canfestival.h"
#include "timer.h"

UNS16 xdata time_cnt = 0;
UNS16 xdata next_time = 0;
TIMEVAL xdata last_time_set = TIMEVAL_MAX;

void setTimer(TIMEVAL value)
{
	next_time = (time_cnt + value);
}

TIMEVAL getElapsedTime(void)
{
	UNS16 ret = 0;
	ret = time_cnt - last_time_set;
	return ret;
}

void initTimer(void)
{
	TIM_InitTypeDef		TIM_InitStructure;
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//指定时钟源, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = (uint16_t)(65536UL - (MAIN_Fosc / 1000UL));			//中断频率, 1000次/秒
	TIM_InitStructure.TIM_Run       = ENABLE;					//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);					//初始化Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer1_Init(ENABLE,Priority_0);
	
}

void timer1_int (void) interrupt TMR1_VECTOR
{
	time_cnt++;
	if(time_cnt == next_time)
	{
		last_time_set = time_cnt;
		TimeDispatch();
	}
}