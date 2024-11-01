#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "System_init.h"

#include "canfestival.h"
#include "objdict.h"
#include "can.h"

QueueHandle_t CAN1msgQueueHandle;
QueueHandle_t CAN2msgQueueHandle;

Message msg;
Message Can1Msg;


uint8_t i;
bit Key_Flag;
uint16_t Key_cnt;

void prvCanRxProcess( void );
void prvCan1RxProcess( void );
void KeyResetScan(void);

portTASK_FUNCTION( vCan1Task, pvParameters )
{
	UNUSED( pvParameters );
    
    canInit(CAN1);
	setNodeId(&CAN1_ObjDict_Data,0);
	setState(&CAN1_ObjDict_Data,Initialisation);

    CAN1msgQueueHandle = xQueueCreate(10,sizeof(Message));
    
	Can1Msg.cob_id =  0;  //ID=0
	Can1Msg.rtr    =  0;  //数据帧
	Can1Msg.len    =  2;  //数据长度
    Can1Msg.m_data[0] = 0x01;
    Can1Msg.m_data[1] = 0x0F;

    while(1)
    {
        CANSEL = 0;         //选择CAN1模块
        prvCan1RxProcess();
        KeyResetScan();

        vTaskDelay(1);
    }
    
    vTaskDelete(NULL);
}

portTASK_FUNCTION( vCan2Task, pvParameters )
{
	UNUSED( pvParameters );
    
    canInit(CAN2);
	setNodeId(&CAN2_ObjDict_Data,NODEID);
	setState(&CAN2_ObjDict_Data,Initialisation);
		
	CAN2msgQueueHandle = xQueueCreate(10,sizeof(Message));
    while(1)
    {
        CANSEL = 1;         //选择CAN2模块
        prvCanRxProcess();
        
        vTaskDelay(1);
    }
    
    vTaskDelete(NULL);
}

void prvCan1RxProcess( void )
{
	if( xQueueReceive(CAN1msgQueueHandle,&msg,0) == pdPASS )
	{
		portENTER_CRITICAL();
		canDispatch(&CAN1_ObjDict_Data,&msg);
		portEXIT_CRITICAL();

        printf("CAN1 Read: ID=0x%04X  ",msg.cob_id);
        for(i=0; i<msg.len; i++)    printf("0x%02X ",msg.m_data[i]);    //从串口输出收到的数据
        printf("\r\n");
	}
}

void prvCanRxProcess( void )
{
	if( xQueueReceive(CAN2msgQueueHandle,&msg,0) == pdPASS )
	{
		portENTER_CRITICAL();
		canDispatch(&CAN2_ObjDict_Data,&msg);
		portEXIT_CRITICAL();

        printf("CAN2 Read: ID=0x%04X  ",msg.cob_id);
        for(i=0; i<msg.len; i++)    printf("0x%02X ",msg.m_data[i]);    //从串口输出收到的数据
        printf("\r\n");
	}
}

void KeyResetScan(void)
{
    if(!P54)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 50)		//连续50ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发
                printf("CAN1 Send\r\n");
                
                canSend(0,&Can1Msg);

                Can1Msg.cob_id =  0x20F;  //ID=0
                Can1Msg.rtr    =  0;  //数据帧
                Can1Msg.len    =  1;  //数据长度
                Can1Msg.m_data[0]++;
                Can1Msg.m_data[1] = 0x00;
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}