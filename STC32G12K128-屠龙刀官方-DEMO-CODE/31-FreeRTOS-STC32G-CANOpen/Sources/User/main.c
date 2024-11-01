
/*
 * FreeRTOS STC port V1.0.2
 * 
 * STC32G12K128功能简介：
 *    2组 CAN, 3组Lin, USB, 32位 8051, RMB5,
 *    高精准12位ADC, DMA 支持(TFT彩屏, ADC, 4组串口, SPI, I2C )
 *  
 *  各位用户在后续的使用过程中，如果发现有不正确的地方或者
 *  有可改善的地方，请和STC联系，由STC官方进行统一修改并更新版本
 *
 *  若您的代码或文章中需要引用本程序包中的部分移植代码，请在您的
 *  代码或文章中注明参考或使用了STC的FreeRTOS的移植代码
*/

#include "FreeRTOS.h"
#include "task.h"
#include "System_init.h"

#include "io.h"
#include "can.h"

#include "canfestival.h"
#include "timer.h"
#include "ObjDict.h"

void vSystemInit( void );



void main( void )
{
	/* 系统初始化 */
    vSystemInit();
	initTimer();
	
	printf("Hi,FreeRTOS\r\n");
	
	/* 创建任务 */
    xTaskCreate((TaskFunction_t )vIOTask,
                (const char*    )"IO",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);

    xTaskCreate((TaskFunction_t )vCan1Task,
                (const char*    )"TX_CAN",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES + 1),
                (TaskHandle_t*  )NULL);

    xTaskCreate((TaskFunction_t )vCan2Task,
                (const char*    )"RX_CAN",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES + 1),
                (TaskHandle_t*  )NULL);

    /* 启动任务调度器开始任务调度 */
    vTaskStartScheduler();
								
    /* 正常情况下不会运行到此处 */
    while (1);
}
