#ifndef _KEYBOARD_DEVICE_H_
#define _KEYBOARD_DEVICE_H_

#include "stc.h"

#define KEY_X P3		/*键盘x轴挂在P2*/
#define KEY_Y P2		/*键盘y轴挂在P0*/

#define KEY_X_IS_HIGH	/*y轴放在高5位*/
//#define LEY_X_IS_LOW	/*y轴放在低5位*/


/** =========== 按键静态扫描 =========== */
uchar keyboard(void);

/** =========== 按键动态唤醒 =========== */
uchar keyboard_wake(void);

/** =========== 按键扫描值处理 =========== */
//返回 uchar number0,可以查表，根据返回值进行对应操作 
uchar keyboard_output();

//关于
void about();

void delay_2s(void);	//延时2s

#endif