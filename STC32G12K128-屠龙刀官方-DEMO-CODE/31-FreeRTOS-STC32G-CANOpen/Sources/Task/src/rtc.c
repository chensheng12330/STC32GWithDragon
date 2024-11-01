/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于STC32G为主控芯片的实验箱进行编写测试。

用STC的MCU的IO方式驱动8位数码管。

显示效果为: 模拟数码时钟.

左边4位LED显示时间(小时,分钟).

******************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "rtc.h"
#include "display.h"

static void prvRtcInit( void );
static void prvRtcCounter( void );

void vRtcUpdateDisplay( void );

static uint8_t ucDotState;                              //秒闪显示状态

uint8_t ucHour;                                         //时钟变量(时)
uint8_t ucMinute;                                       //时钟变量(分)
uint8_t ucSecond;                                       //时钟变量(秒)

/* RTC任务函数 */
portTASK_FUNCTION( vRtcTask, pvParameters )
{
    
    UNUSED( pvParameters );
    
    prvRtcInit();
    while(1)
    {
        prvRtcCounter();
        vRtcUpdateDisplay();
        vTaskDelay(500);
    }
    
    vTaskDelete(NULL);
}   

static void prvRtcInit( void )
{
    ucHour   = 12;                                      //初始化时间值
    ucMinute = 0;
    ucSecond = 0;
    ucDotState = 0;
}


/********************** 显示时钟函数 ************************/
void vRtcUpdateDisplay( void )
{
    pucLEDBuffer[0] = ucHour >= 10 ? ucHour / 10 : DIS_BLACK;
    pucLEDBuffer[1] = ucHour % 10;
    pucLEDBuffer[2] = ucMinute / 10;
    pucLEDBuffer[3] = ucMinute % 10;
    
    if (ucDotState)
        pucLEDBuffer[1] |= DIS_DOT;                     //小时后的小数点做秒闪
}

/********************** 时钟演示函数 ************************/
static void prvRtcCounter( void )
{
    ucDotState = !ucDotState;
    
    if (!ucDotState) return;

    if(++ucSecond >= 60)
    {
        ucSecond = 0;
        if(++ucMinute >= 60)
        {
            ucMinute = 0;
            if(++ucHour >= 24)
                ucHour = 0;
        }
    }
}
