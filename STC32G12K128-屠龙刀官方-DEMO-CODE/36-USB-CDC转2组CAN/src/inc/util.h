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

#ifndef __UTIL_H__
#define __UTIL_H__

//为节省代码空间，
//常数字节交换建议使用反转宏
//变量字节交换建议使用反转函数

#define REV4(d)     ((((DWORD)(d) & 0x000000ff) << 24) | \
                     (((DWORD)(d) & 0x0000ff00) << 8)  | \
                     (((DWORD)(d) & 0x00ff0000) >> 8)  | \
                     (((DWORD)(d) & 0xff000000) >> 24))

#define REV2(d)     ((((WORD)(d) & 0x00ff) << 8) | \
                     (((WORD)(d) & 0xff00) >> 8))
                    
DWORD reverse4(DWORD d);
WORD reverse2(WORD w);

int printf_cdc1 (const char *fmt, ...);
int printf_cdc2 (const char *fmt, ...);

#endif
