#ifndef CAN_H
#define CAN_H

#include "STC32G_CAN.h"

/* 显示任务函数 */
portTASK_FUNCTION( vCan1Task, pvParameters );
portTASK_FUNCTION( vCan2Task, pvParameters );


#endif
