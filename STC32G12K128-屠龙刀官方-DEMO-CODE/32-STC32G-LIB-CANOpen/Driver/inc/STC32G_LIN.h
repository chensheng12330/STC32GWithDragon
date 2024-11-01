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

#ifndef	__STC32G_LIN_H
#define	__STC32G_LIN_H

#include	"config.h"

//========================================================================
//                              定义声明
//========================================================================

#define LIN_MODE        1    //0: LIN2.1(增强校验);  1: LIN1.3
#define FRAME_LEN       8    //数据长度: 8 字节

//========================================================================
//                              常量声明
//========================================================================

#define LIN_LIDE     0x01  //Head中断
#define LIN_RDYE     0x02  //Ready中断
#define LIN_ERRE     0x04  //错误中断
#define LIN_ABORTE   0x08  //终止中断
#define LIN_ALLIE    0x0F  //所有中断

//========================================================================
//                              变量声明
//========================================================================

typedef struct
{
	u8	LIN_Enable;					//LIN功能使能  	ENABLE,DISABLE
	u16	LIN_Baudrate;				//LIN波特率
	u8	LIN_IE;							//LIN中断使能  	LIN_LIDE/LIN_RDYE/LIN_ERRE/LIN_ABORTE/LIN_ALLIE,DISABLE
	u8	LIN_HeadDelay;			//帧头延时计数  	0~(65535*1000)/MAIN_Fosc
	u8	LIN_HeadPrescaler;	//帧头延时分频  	0~63
} LIN_InitTypeDef;


//========================================================================
//                              外部声明
//========================================================================

void LIN_Inilize(LIN_InitTypeDef *LIN);
u8 LinReadReg(u8 addr);
void LinReadFrame(u8 *pdat);
void LinSendFrame(u8 lid, u8 *pdat);
void LinSendHeaderRead(u8 lid, u8 *pdat);
void LinTxResponse(u8 *pdat);

#endif
