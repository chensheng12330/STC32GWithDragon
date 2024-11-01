#ifndef __KEYBOARD_H
#define __KEYBOARD_H 


#include "middle.h"

u8 String_length(char* str);
u8 String_Insert(char *str,u8 leng,u8 addr,u8 keynuml);
void String_Delete(char *str,u8 length,u8 addr);
void String_Clear(char *str,u8 length);



#endif


