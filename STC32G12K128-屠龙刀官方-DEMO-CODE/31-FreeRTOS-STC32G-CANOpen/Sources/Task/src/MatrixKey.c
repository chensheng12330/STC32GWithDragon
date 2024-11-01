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

本例程基于STC32G为主控芯片的实验箱9进行编写测试.

通过P5.4口输出低电平驱动有源蜂鸣器.

行列扫描按键每个按键按下后蜂鸣器响一声.

******************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "MatrixKey.h"

/****************************** 用户定义宏 ***********************************/

sbit BEEP = P5^4;

uint8_t cntms;
uint8_t KeyCode;    //给用户使用的键码, 1~16有效
uint8_t IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //行列键盘变量

void MatrixKeyScan(void);

/* RTC任务函数 */
portTASK_FUNCTION( vMatrixKeyTask, pvParameters )
{
    UNUSED( pvParameters );
    
    P5M1 &= ~0x10;   P5M0 |= 0x10;   //设置P5.4为推挽输出
    BEEP = 1;  //蜂鸣器关闭
    while(1)
    {
        if(cntms > 0)
        {
            cntms--;
        }
        else
        {
            BEEP = 1;    //蜂鸣器关闭
        }

        MatrixKeyScan();
        if(KeyCode > 0)  //有键按下
        {
            KeyCode = 0;
            BEEP = 0;    //蜂鸣器响起
            cntms = 5;   //持续时间 5*50ms
        }
        
        vTaskDelay(50);
    }
    
    vTaskDelete(NULL);
}   

/*****************************************************
    行列键扫描程序
    使用XY查找4x4键的方法，只能单键，速度快

   Y     P04      P05      P06      P07
          |        |        |        |
X         |        |        |        |
P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/

uint8_t code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

void IO_KeyDelay(void)
{
    uint8_t i;
    i = 60;
    while(--i)  ;
}

void MatrixKeyScan(void)    //50ms call
{
    uint8_t  j;

    j = IO_KeyState1;   //保存上一次状态

    P0 = 0xf0;  //X低，读Y
    IO_KeyDelay();
    IO_KeyState1 = P0 & 0xf0;

    P0 = 0x0f;  //Y低，读X
    IO_KeyDelay();
    IO_KeyState1 |= (P0 & 0x0f);
    IO_KeyState1 ^= 0xff;   //取反
    
    if(j == IO_KeyState1)   //连续两次读相等
    {
        j = IO_KeyState;
        IO_KeyState = IO_KeyState1;
        if(IO_KeyState != 0)    //有键按下
        {
            F0 = 0;
            if(j == 0)  F0 = 1; //第一次按下
            else if(j == IO_KeyState)
            {
                if(++IO_KeyHoldCnt >= 20)   //1秒后重键
                {
                    IO_KeyHoldCnt = 18;
                    F0 = 1;
                }
            }
            if(F0)
            {
                j = T_KeyTable[IO_KeyState >> 4];
                if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                    KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;    //计算键码，17~32
            }
        }
        else    IO_KeyHoldCnt = 0;
    }
    P0 = 0xff;
}
