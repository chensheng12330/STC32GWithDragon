/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAIN_Fosc               24000000L    //����Ĭ����ʱ��

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

#define EN_EP2IN                        //����1������IN�˵�
#define EN_EP3IN                        //����2�Ŀ���IN�˵�
#define EN_EP4IN                        //����1������IN�˵�
#define EN_EP5IN                        //����2�Ŀ���IN�˵�
#define EN_EP4OUT                       //����1������OUT�˵�
#define EN_EP5OUT                       //����2������OUT�˵�

#endif
