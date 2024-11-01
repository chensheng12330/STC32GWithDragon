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

#ifndef	__STC32G_CAN_H
#define	__STC32G_CAN_H

#include	"FreeRTOS.h"

//========================================================================
//                              定义声明
//========================================================================

#define	CAN1	0
#define	CAN2	1

//========================================================================
//                              常量声明
//========================================================================

#define CAN_DOIM     0x01  //接收溢出中断
#define CAN_BEIM     0x02  //总线错位中断
#define CAN_TIM      0x04  //发送中断
#define CAN_RIM      0x08  //接收中断
#define CAN_EPIM     0x10  //被动错位中断
#define CAN_EWIM     0x20  //错位警告中断
#define CAN_ALIM     0x40  //仲裁丢失中断
#define CAN_ALLIM    0x7F  //所有中断

//========================================================================
//                              变量声明
//========================================================================

typedef struct
{
	uint8_t	CAN_Enable;					//CAN功能使能  	ENABLE,DISABLE
	uint8_t	CAN_IMR;						//CAN中断寄存器 	CAN_DOIM,CAN_BEIM,CAN_TIM,CAN_RIM,CAN_EPIM,CAN_EWIM,CAN_ALIM,CAN_ALLIM,DISABLE
	uint8_t	CAN_SJW;						//重新同步跳跃宽度  0~3
	uint8_t	CAN_BRP;						//波特率分频系数    0~63
	uint8_t	CAN_SAM;						//总线电平采样次数  0:采样1次; 1:采样3次
	uint8_t	CAN_TSG1;						//同步采样段1       0~15
	uint8_t	CAN_TSG2;						//同步采样段2       1~7 (TSG2 不能设置为0)

	uint8_t	CAN_ACR0;						//总线验收代码寄存器 0~0xFF
	uint8_t	CAN_ACR1;						//总线验收代码寄存器 0~0xFF
	uint8_t	CAN_ACR2;						//总线验收代码寄存器 0~0xFF
	uint8_t	CAN_ACR3;						//总线验收代码寄存器 0~0xFF
	uint8_t	CAN_AMR0;						//总线验收屏蔽寄存器 0~0xFF
	uint8_t	CAN_AMR1;						//总线验收屏蔽寄存器 0~0xFF
	uint8_t	CAN_AMR2;						//总线验收屏蔽寄存器 0~0xFF
	uint8_t	CAN_AMR3;						//总线验收屏蔽寄存器 0~0xFF
} CAN_InitTypeDef;


//========================================================================
//                              外部声明
//========================================================================

uint8_t CanReadReg(uint8_t addr);
void CanReadFifo(uint8_t *pdat);
void CAN_Inilize(uint8_t CANx, CAN_InitTypeDef *CAN);
uint16_t CanReadStandardFrame(uint8_t *pdat);
uint32_t CanReadExtendedFrame(uint8_t *pdat);
void CanSendStandardFrame(uint16_t canid, uint8_t *pdat);
void CanSendExtendedFrame(uint32_t canid, uint8_t *pdat);

#endif
