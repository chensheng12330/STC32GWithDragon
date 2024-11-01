#ifndef __EEPROM_H
#define __EEPROM_H

#include "stc32g.h" //头文件见下载软件
#include "intrins.h"

void IapIdle();
char IapRead(unsigned long addr); 
void IapProgram(unsigned long addr, char dat);
void IapErase(unsigned long addr);


#endif

