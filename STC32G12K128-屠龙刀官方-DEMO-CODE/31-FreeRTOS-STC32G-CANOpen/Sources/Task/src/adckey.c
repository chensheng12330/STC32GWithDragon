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

通过ADC采集按键口ADC值，根据ADC值设置键码为1~16.

按键只支持单键按下, 不支持多键同时按下, 那样将会有不可预知的结果.

键按下超过1秒后,将以10键/秒的速度提供重键输出. 用户只需要检测KeyCode是否非0来判断键是否按下.

调整时间键:
键码1: 小时+.
键码2: 小时-.
键码3: 分钟+.
键码4: 分钟-.

******************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "STC32G_ADC.h"

#include "adckey.h"
#include "rtc.h"
//#include "display.h"

#define ADCKEY_CHANNEL          0                       //ADC按键的输入通道（P1.0）
#define ADC_OFFSET              64                      //ADC按键扫描时的偏差范围


static void prvAdcKeyInit( void );
static void prvAdcCalculateKey( void );
static void prvAdcKeyEvent( void );


static uint8_t ucKeyState;                              //ADC按键状态
static uint8_t ucKeyState1;
static uint8_t ucKeyState2;
static uint8_t ucKeyState3;
static uint8_t ucKeyHoldCnt;                            //键按下计时

uint8_t ucKeyCode;                                      //给用户使用的键码, 1~16有效

/* ADC按键扫描任务函数 */
portTASK_FUNCTION( vAdcKeyTask, pvParameters )
{
    
	UNUSED( pvParameters );

    prvAdcKeyInit();
    while(1)
    {
        prvAdcCalculateKey();
        prvAdcKeyEvent();

        vTaskDelay(10);
    }
    
    vTaskDelete(NULL);
}   

static void prvAdcKeyInit( void )
{
//    P1M1 |= 0x01;                                       //设置 P1.0 为 ADC 输入口
//    P1M0 &= ~0x01;

    ucKeyState  = 0;
    ucKeyState1 = 0;
    ucKeyState2 = 0;
    ucKeyState3 = 0;                                    //键状态
    ucKeyHoldCnt = 0;                                   //键按下计时
    ucKeyCode = 0;                                      //给用户使用的键码, 1~16有效
}

/***************** ADC键盘计算键码 *****************************
电路和软件算法设计: Coody
本ADC键盘方案在很多实际产品设计中, 验证了其稳定可靠, 即使按键使用导电膜,都很可靠.
16个键,理论上各个键对应的ADC值为 (4096 / 16) * k = 256 * k, k = 1 ~ 16, 特别的, k=16时,对应的ADC值是4095.
但是实际会有偏差,则判断时限制这个偏差, ADC_OFFSET为+-偏差, 则ADC值在 (256*k-ADC_OFFSET) 与 (256*k+ADC_OFFSET)之间为键有效.
间隔一定的时间,就采样一次ADC,比如10ms.
为了避免偶然的ADC值误判, 或者避免ADC在上升或下降时误判, 使用连续3次ADC值均在偏差范围内时, ADC值才认为有效.
以上算法, 能保证读键非常可靠.
**********************************************/
static void prvAdcCalculateKey( void )
{
    uint16_t usResult;
    uint8_t i;
    uint16_t j;
    
    usResult = Get_ADCResult(ADCKEY_CHANNEL);
    
    if(usResult < (256 - ADC_OFFSET))
    {
        ucKeyState = 0;                                 //键状态归0
        ucKeyHoldCnt = 0;
    }
    
    j = 256;
    for (i = 1; i <= 16; i++)
    {
        if ((usResult >= (j - ADC_OFFSET)) &&
            (usResult <= (j + ADC_OFFSET)))
           break;                                       //判断是否在偏差范围内
        j += 256;
    }
    
    ucKeyState3 = ucKeyState2;
    ucKeyState2 = ucKeyState1;
    
    if (i > 16)
        ucKeyState1 = 0;                                //键无效
    else                                                //键有效
    {
        ucKeyState1 = i;
        if ((ucKeyState3 == ucKeyState2) &&
            (ucKeyState2 == ucKeyState1) &&
            (ucKeyState3 > 0) &&
            (ucKeyState2 > 0) &&
            (ucKeyState1 > 0))
        {
            if (ucKeyState == 0)                        //第一次检测到
            {
                ucKeyCode = i;                          //保存键码
                ucKeyState = i;                         //保存键状态
                ucKeyHoldCnt = 0;
            }
            
            if (ucKeyState == i)                        //连续检测到同一键按着
            {
                if (++ucKeyHoldCnt >= 100)              //按下1秒后,以10次每秒的速度Repeat Key
                {
                    ucKeyHoldCnt = 90;
                    ucKeyCode  = i;                     //保存键码
                }
            }
            else
                ucKeyHoldCnt = 0;                       //按下时间计数归0
        }
    }
}

static void prvAdcKeyEvent( void )
{
    uint8_t ucCode;
    
    ucCode = ucKeyCode;
    ucKeyCode = 0;
    
    if (ucCode > 0)                                     //有键按下
    {
//        pucLEDBuffer[6] = ucCode / 10;                  //显示键码
//        pucLEDBuffer[7] = ucCode % 10;                  //显示键码

        switch (ucCode)
        {
        case 1:                                         //hour +1
            if (++ucHour >= 24)
                ucHour = 0;
            break;
        case 2:                                         //hour -1
            if (--ucHour >= 24)
                ucHour = 23;
            break;
        case 3:                                         //minute +1
            ucSecond = 0;
            if (++ucMinute >= 60)
                ucMinute = 0;
            break;
        case 4:                                         //minute -1
            ucSecond = 0;
            if (--ucMinute >= 60)
                ucMinute = 59;
            break;
        default:
            return;
        }
        
        vRtcUpdateDisplay();
    }
}


