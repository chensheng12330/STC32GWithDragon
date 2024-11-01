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

#ifndef __STC32G_USART_H
#define __STC32G_USART_H	 

#include	"FreeRTOS.h"

//========================================================================
//                              定义声明
//========================================================================

#define	USART1	1
#define	USART2	2

#define FRAME_LEN       8    //数据长度: 8 字节

#define	LinMasterMode		0
#define	LinSlaveMode		1

//========================================================================
//                              USART设置
//========================================================================


//========================================================================
//                              变量声明
//========================================================================

typedef struct
{ 
	uint8_t	LIN_Enable;				//LIN总线使能  	ENABLE,DISABLE
	uint8_t	LIN_Mode;					//LIN总线模式  	LinMasterMode,LinSlaveMode
	uint8_t	LIN_AutoSync;			//自动同步使能  	ENABLE,DISABLE
	uint16_t	LIN_Baudrate;			//LIN波特率
} USARTx_LIN_InitDefine; 

//========================================================================
//                              外部声明
//========================================================================

uint8_t UASRT_LIN_Configuration(uint8_t USARTx, USARTx_LIN_InitDefine *USART);
void UsartLinSendChecksum(uint8_t USARTx, uint8_t *dat);
void UsartLinSendData(uint8_t USARTx, uint8_t *pdat);
void UsartLinSendFrame(uint8_t USARTx, uint8_t lid, uint8_t *pdat);
void UsartLinSendHeader(uint8_t USARTx, uint8_t lid);

#endif

