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

void timer1_int (void) interrupt TMR1_VECTOR
{
	if(time_cnt == next_time)
	{
		last_time_set = time_cnt;
		TimeDispatch();
	}
	time_cnt++;
}