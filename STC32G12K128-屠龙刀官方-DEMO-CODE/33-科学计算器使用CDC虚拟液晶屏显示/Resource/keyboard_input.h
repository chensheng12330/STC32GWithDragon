#ifndef _KEYBOARD_INPUT_H_
#define _KEYBOARD_INPUT_H_

#include "stc.h"

typedef enum
{
	INPUT_NORMAL,
	INPUT_AC,
	INPUT_EQ,
	INPUT_UP,
	INPUT_DOWN,
//	INPUT_COPY,
	INPUT_VACANT	//悬空
}INPUT_FLAG;

uchar input_to_str(uchar, char *,uchar num1);  //在第几行显示 y~（0,3）
void t0_init(void);						//定时器初始化
void close_cursor(void);		//关光标
void open_cursor(void);			//开光标

#endif
