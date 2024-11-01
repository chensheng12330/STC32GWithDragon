/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAIN_Fosc               24000000L    //定义默认主时钟

#define EP0_SIZE                64
#define EP1IN_SIZE              64
#define EP2IN_SIZE              64
#define EP3IN_SIZE              64
#define EP4IN_SIZE              64
#define EP5IN_SIZE              64
#define EP1OUT_SIZE             64
#define EP2OUT_SIZE             64
#define EP3OUT_SIZE             64
#define EP4OUT_SIZE             64
#define EP5OUT_SIZE             64

#define EN_EP2IN                        //串口1的数据IN端点
#define EN_EP3IN                        //串口2的控制IN端点
#define EN_EP4IN                        //串口1的数据IN端点
#define EN_EP5IN                        //串口2的控制IN端点
#define EN_EP4OUT                       //串口1的数据OUT端点
#define EN_EP5OUT                       //串口2的数据OUT端点

#endif
