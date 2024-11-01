
/*
 * FreeRTOS STC port V1.0.2
 * 
 * STC32G12K128���ܼ�飺
 *    2�� CAN, 3��Lin, USB, 32λ 8051, RMB5,
 *    �߾�׼12λADC, DMA ֧��(TFT����, ADC, 4�鴮��, SPI, I2C )
 *  
 *  ��λ�û��ں�����ʹ�ù����У���������в���ȷ�ĵط�����
 *  �пɸ��Ƶĵط������STC��ϵ����STC�ٷ�����ͳһ�޸Ĳ����°汾
 *
 *  �����Ĵ������������Ҫ���ñ�������еĲ�����ֲ���룬��������
 *  �����������ע���ο���ʹ����STC��FreeRTOS����ֲ����
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
	/* ϵͳ��ʼ�� */
    vSystemInit();
	initTimer();
	
	printf("Hi,FreeRTOS\r\n");
	
	/* �������� */
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

    /* ���������������ʼ������� */
    vTaskStartScheduler();
								
    /* ��������²������е��˴� */
    while (1);
}
