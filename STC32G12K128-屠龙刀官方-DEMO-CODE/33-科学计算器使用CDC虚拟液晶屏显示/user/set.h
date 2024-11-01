#ifndef __SET_H
#define __SET_H


#include "middle.h"

extern u8 xdata Parma[10];

void Set_Light(u8 keynum);
void Set_LightShift(u8 keynum);
void Set_Sleep(u8 keynum);
void Set_Data(u8 keynum);
void Show_time(void);
void Set_Time(u8 keynum);
void SetParm_Read(void);
        
#endif



