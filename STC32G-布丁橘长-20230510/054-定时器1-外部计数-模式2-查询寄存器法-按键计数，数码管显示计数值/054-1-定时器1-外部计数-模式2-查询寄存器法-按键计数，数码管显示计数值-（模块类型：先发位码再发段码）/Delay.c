#include <delay.h>

void  delayms(u16 ms)		//简单延时函数，自适应主频，1毫秒*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}