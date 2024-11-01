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

#ifndef	__STC32G_LCM_H
#define	__STC32G_LCM_H

#include	"config.h"

//========================================================================
//                              脚位定义
//========================================================================

sbit LCD_RS = P4^5;      //数据/命令切换
sbit LCD_WR = P4^2;      //写控制
sbit LCD_RD = P4^4;      //读控制
sbit LCD_CS = P3^4;      //片选
sbit LCD_RESET = P4^3;   //复位

//========================================================================
//                              定义声明
//========================================================================

#define LCM_WRITE_CMD()					LCMIFCR = ((LCMIFCR & ~0x07) | 0x84)
#define LCM_WRITE_DAT()					LCMIFCR = ((LCMIFCR & ~0x07) | 0x85)
#define LCM_READ_CMD()					LCMIFCR = ((LCMIFCR & ~0x07) | 0x86)
#define LCM_READ_DAT()					LCMIFCR = ((LCMIFCR & ~0x07) | 0x87)

#define SET_LCM_DAT_LOW(n)			LCMIFDATL = (n)
#define SET_LCM_DAT_HIGH(n)			LCMIFDATH = (n)

//========================================================================
//                              常量声明
//========================================================================

#define	MODE_I8080			0	//I8080模式
#define	MODE_M6800			1	//M6800模式

#define	BIT_WIDE_8			0	//8位数据宽度
#define	BIT_WIDE_16			2	//16位数据宽度

//========================================================================
//                              变量声明
//========================================================================

typedef struct
{
	u8	LCM_Enable;					//LCM接口使能  	ENABLE,DISABLE
	u8	LCM_Mode;						//LCM接口模式  	MODE_I8080,MODE_M6800
	u8	LCM_Bit_Wide;				//LCM数据宽度  	BIT_WIDE_8,BIT_WIDE_16
	u8	LCM_Setup_Time;			//LCM通信数据建立时间  	0~7
	u8	LCM_Hold_Time;			//LCM通信数据保持时间  	0~3
} LCM_InitTypeDef;


//========================================================================
//                              外部声明
//========================================================================
extern bit LcmFlag;
extern u16 LCM_Cnt;

void LCM_Inilize(LCM_InitTypeDef *LCM);

#endif
