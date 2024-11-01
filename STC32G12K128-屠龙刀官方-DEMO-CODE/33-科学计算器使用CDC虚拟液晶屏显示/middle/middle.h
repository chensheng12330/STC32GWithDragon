#ifndef __MIDDLE_H
#define __MIDDLE_H

#include "stc.h"
#include "usb.h"
#include "oled.h"
#include "tim.h"
#include "key.h"
#include "eeprom.h"

extern u8 xdata state;

void Screen_ShowAsc(u8 x,u8 y,u8 asc);
void Screen_ShowNum(u8 x,u8 y,double num);
void Screen_ShowStr(u8 x,u8 y,u8 *str);
void Screen_ShowFreshen(void);
void Screen_Clear(void);
//void Cursor_Blink( u8 x,u8 y,u8 count );
void Screen_ShowLine( unsigned char x1, unsigned char y1, unsigned char x2,unsigned char y2);
void Screen_ShowPoint( unsigned char x, unsigned char y);
void Screen_Showlight(unsigned char num); //1-5
void Screen_ShowAscOpp(u8 x,u8 y,u8 asc);


void byte_write(unsigned int add,unsigned char dat,unsigned char section);	/*add地址; dat数据; section扇区(24c08取0,1,2,3)*/
unsigned char byte_read(unsigned int add, unsigned char section);	/*add地址; section扇区(24c08取0,1,2,3)*/



#endif


