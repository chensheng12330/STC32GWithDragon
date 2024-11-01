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

内部集成I2C串行总线控制器做从机模式，SCL->P3.2, SDA->P3.3;
IO口模拟I2C做主机模式，SCL->P0.0, SDA->P0.1;
通过外部飞线连接 P0.0->P3.2, P0.1->P3.3，实现I2C自发自收功能。

计数器每秒钟加1, 计数范围为0~9999.
上电后主机每秒钟发送一次计数数据，从机接收到计数值后通过串口2输出.

******************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "STC32G_UART.h"
#include "STC32G_I2C.h"
#include "STC32G_Soft_I2C.h"
#include "i2c_ps.h"

static uint16_t second;
static uint8_t temp[4];


/* RTC任务函数 */
portTASK_FUNCTION( vI2C_PsTask, pvParameters )
{
    UNUSED( pvParameters );
    
    second = 0;
    while(1)
    {
        if(I2C_DisplayFlag)
        {
            I2C_DisplayFlag = 0;
            printf("UART2: %d%d%d%d\r\n", I2C_Buffer[0], I2C_Buffer[1], I2C_Buffer[2], I2C_Buffer[3]);
        }

		second++;         //秒计数+1
		if(second >= 9999)    second = 0;   //秒计数范围为0~9999

		temp[0] = second / 1000;
		temp[1] = (second % 1000) / 100;
		temp[2] = (second % 100) / 10;
		temp[3] = second % 10;

		SI2C_WriteNbyte(0, temp, 4);
        
        vTaskDelay(1000);
    }
    
    vTaskDelete(NULL);
}   
