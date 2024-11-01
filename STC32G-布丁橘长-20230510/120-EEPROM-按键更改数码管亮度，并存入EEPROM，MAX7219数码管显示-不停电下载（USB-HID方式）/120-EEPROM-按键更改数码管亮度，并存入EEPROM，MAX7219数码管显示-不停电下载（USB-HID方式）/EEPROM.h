#ifndef __EEPROM_H__
#define __EEPROM_H__

//EEPROM1-16K，32个扇区起始地址定义
#define SECTOR0 	0x000000
#define SECTOR1 	0x000200
#define SECTOR2 	0x000400
#define SECTOR3 	0x000600
#define SECTOR4 	0x000800
#define SECTOR5 	0x000A00
#define SECTOR6 	0x000C00
#define SECTOR7 	0x000E00
#define SECTOR8 	0x001000
#define SECTOR9 	0x001200
#define SECTOR10	0x001400
#define SECTOR11	0x001600
#define SECTOR12 	0x001800
#define SECTOR13	0x001A00
#define SECTOR14	0x001C00
#define SECTOR15	0x001E00
#define SECTOR16	0x002000
#define SECTOR17	0x002200
#define SECTOR18	0x002400
#define SECTOR19	0x002600
#define SECTOR20	0x002800
#define SECTOR21	0x002A00
#define SECTOR22 	0x002C00
#define SECTOR23	0x002E00
#define SECTOR24	0x003000
#define SECTOR25	0x003200
#define SECTOR26	0x003400
#define SECTOR27	0x003600
#define SECTOR28	0x003800
#define SECTOR29	0x003A00
#define SECTOR30	0x003C00
#define SECTOR31	0x003E00

char IapRead(unsigned long addr);
void IapProgram(unsigned long addr, char dat);
void IapErase(unsigned long addr);
void IapIdle();
	
#endif