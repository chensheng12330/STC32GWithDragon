
#ifndef RTC_H
#define RTC_H


/* RTC任务函数 */
portTASK_FUNCTION( vRtcTask, pvParameters );

void vRtcUpdateDisplay( void );

extern uint8_t ucHour;                                  //RTC变量(时)
extern uint8_t ucMinute;                                //RTC变量(分)
extern uint8_t ucSecond;                                //RTC变量(秒)


#endif
