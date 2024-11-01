/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stc32g.h"
#include "intrins.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/*-----------------------------------------------------------
 * 版本更新历史
 *
 * V1.0.2 (2022-4-27)
 *     修正任务参数pvParameters初始压栈错误的问题
 *     (注意pvParameters参数传地址或者立即数时只有低24位是有效位，最高8位编译时会自动填0)
 *
 * V1.0.1 (2022-4-26)
 *     修正无法正常使用xdata的问题
 *
 * V1.0.0 (2022-4-24)
 *     移植成功并发布初版
 *----------------------------------------------------------*/


/***************************************************************************************************************/
/*                                        STC32-251基础配置配置选项                                            */
/***************************************************************************************************************/
#define configPORT_VERSION_NUMBER               "V1.0.2"                //STC移植代码版本

#define configUSE_ROMHUGE                       0                       //1 ROM使用HUGE模式，0使用LARGE模式
#define MAIN_Fosc                               ( 24000000UL )          //系统工作频率

#if ( configUSE_ROMHUGE == 0 )
    #pragma ROM ( LARGE )
#else
    #pragma ROM ( HUGE )
#endif

#pragma WARNING DISABLE = 47                                            //关闭47号警告: unreferenced local variable
#pragma WARNING DISABLE = 138                                           //关闭138号警告: expression with possibly no effect
#pragma WARNING DISABLE = 183                                           //关闭183号警告: dead assignment eliminated

#ifndef configUSE_ROMHUGE
    #error Missing definition:  configUSE_ROMHUGE must be defined in FreeRTOSConfig.h as either 1 or 0.
#endif

//#define UNUSED(expr)      (void)(expr)
#define UNUSED(expr)        if ((expr) == 0)


/***************************************************************************************************************/
/*                                        FreeRTOS基础配置配置选项                                             */
/***************************************************************************************************************/

#define configUSE_PREEMPTION                    1                       //1使用抢占式内核，0使用协程
#define configUSE_TIME_SLICING                  1                       //1使能时间片调度(默认式使能的)
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	0                       //1启用特殊方法来选择下一个要运行的任务
                                                                        //一般是硬件计算前导零指令，如果所使用的
                                                                        //MCU没有这些硬件指令的话此宏应该设置为0！
#define configUSE_TICKLESS_IDLE                 0                       //1启用低功耗tickless模式
#define configUSE_QUEUE_SETS                    0                       //为1时启用队列
#define configCPU_CLOCK_HZ                      (MAIN_Fosc)             //CPU频率
#define configTICK_RATE_HZ                      ((TickType_t)1000)      //时钟节拍频率，这里设置为1000,周期就是1ms
#define configMAX_PRIORITIES                    (32)                    //可使用的最大优先级
#define configDEFAULT_PRIORITIES                (tskIDLE_PRIORITY + 1)  //任务默认优先级
#define configMINIMAL_STACK_SIZE                ((unsigned short)256)   //空闲任务使用的堆栈大小
#define configDEFAULT_STACK_SIZE                ((unsigned short)256)   //任务默认堆栈大小
#define configMAX_TASK_NAME_LEN                 (8)                     //任务名字字符串长度
#define configUSE_IDLE_HOOK						0                       //1使用空闲钩子,0不使用 /*void vApplicationIdleHook( void );*/
#define configUSE_TICK_HOOK						0                       //1使用时间片钩子,0不使用 /*void vApplicationTickHook( void );*/
#define configSUPPORT_DYNAMIC_ALLOCATION        1                       //支持动态内存申请
#define configTOTAL_HEAP_SIZE					((size_t)(2*1024))      // 系统所有总的堆大小

#define configUSE_16_BIT_TICKS                  0                       //系统节拍计数器变量数据类型，
                                                                        //1表示为16位无符号整形，0表示为32位无符号整形
#define configIDLE_SHOULD_YIELD                 1                       //为1时空闲任务放弃CPU使用权给其他同优先级的用户任务
#define configUSE_TASK_NOTIFICATIONS            1                       //为1时开启任务通知功能，默认开启
#define configUSE_MUTEXES                       1                       //为1时使用互斥信号量
#define configQUEUE_REGISTRY_SIZE               0                       //不为0时表示启用队列记录，具体的值是可以
                                                                        //记录的队列和信号量最大数目。
#define configCHECK_FOR_STACK_OVERFLOW          0                       //大于0时启用堆栈溢出检测功能，如果使用此功能
                                                                        //用户必须提供一个栈溢出钩子函数，如果使用的话
                                                                        //此值可以为1或者2，因为有两种栈溢出检测方法。
#define configUSE_RECURSIVE_MUTEXES             0                       //为1时使用递归互斥信号量
#define configUSE_MALLOC_FAILED_HOOK            0                       //1使用内存申请失败钩子函数
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_COUNTING_SEMAPHORES           0                       //为1时使用计数信号量

#define configGENERATE_RUN_TIME_STATS	        0                       //为1时启用运行时间统计功能
#define configUSE_TRACE_FACILITY				0                       //为1启用可视化跟踪调试
#define configUSE_STATS_FORMATTING_FUNCTIONS	0                       //与宏configUSE_TRACE_FACILITY同时为1时会编译下面3个函数
                                                                        //prvWriteNameToBuffer(),vTaskList(),
                                                                        //vTaskGetRunTimeStats()
/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 			        0                       //为1时启用协程，启用协程以后必须添加文件croutine.c
#define configMAX_CO_ROUTINE_PRIORITIES         (2)                     //协程的有效优先级数目

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_xTaskGetSchedulerState          0
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xTimerPendFunctionCall          0



#endif /* FREERTOS_CONFIG_H */
