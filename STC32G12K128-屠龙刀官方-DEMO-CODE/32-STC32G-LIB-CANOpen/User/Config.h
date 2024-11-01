/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#ifndef		__CONFIG_H
#define		__CONFIG_H

//========================================================================
//                               ��ʱ�Ӷ���
//========================================================================

//#define MAIN_Fosc		22118400L	//������ʱ��
//#define MAIN_Fosc		12000000L	//������ʱ��
//#define MAIN_Fosc		11059200L	//������ʱ��
//#define MAIN_Fosc		 5529600L	//������ʱ��
#define MAIN_Fosc		24000000L	//������ʱ��

//========================================================================
//                                ͷ�ļ�
//========================================================================

#include "type_def.h"
#include "stc32g.h"
#include <stdlib.h>
#include <stdio.h>

//========================================================================
//                             �ⲿ�����ͱ�������
//========================================================================

#define CAN_BAUDRATE    1000

// Needed defines by Canfestival lib
#define MAX_CAN_BUS_ID 1
#define SDO_MAX_LENGTH_TRANSFER 32
#define SDO_BLOCK_SIZE 16
#define SDO_MAX_SIMULTANEOUS_TRANSFERS 4
#define NMT_MAX_NODE_ID 128
#define SDO_TIMEOUT_MS 3000U
#define MAX_NB_TIMER 8
#define MIN_TIMER_TIMEOUT_US 5U

//#define SDO_DYNAMIC_BUFFER_ALLOCATION
//#define SDO_DYNAMIC_BUFFER_ALLOCATION_SIZE 	(1024)
//#define SDO_MAX_LENGTH_TRANSFERT 						32
//#define SDO_MAX_SIMULTANEOUS_TRANSFERTS 		5

// CANOPEN_BIG_ENDIAN is not defined
#define CANOPEN_LITTLE_ENDIAN 1

#define US_TO_TIMEVAL_FACTOR 8

#define REPEAT_SDO_MAX_SIMULTANEOUS_TRANSFERS_TIMES(repeat)\
repeat
#define REPEAT_NMT_MAX_NODE_ID_TIMES(repeat)\
repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat

#define EMCY_MAX_ERRORS 8
#define REPEAT_EMCY_MAX_ERRORS_TIMES(repeat)\
repeat repeat repeat repeat repeat repeat repeat repeat


#endif
