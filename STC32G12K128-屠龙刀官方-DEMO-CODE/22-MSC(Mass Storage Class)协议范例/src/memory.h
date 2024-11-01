#ifndef __MEMORY_H__
#define __MEMORY_H__


#if defined MEMTYPE_INT

#define IAPBASE             ((BYTE far *)(0xfe0000))

void iap_init();
BOOL iap_check();
void iap_readsector(DWORD addr, BYTE *pdat);
void iap_writesector(DWORD addr, BYTE *pdat);

void iap_program_byte(DWORD addr, BYTE dat);
void iap_erase_sector(DWORD addr);

#define memory_init         iap_init
#define memory_check        iap_check
#define memory_readsector   iap_readsector
#define memory_writesector  iap_writesector

#else

void flash_init();
BOOL flash_check();
void flash_readsector(DWORD addr, BYTE *pdat);
void flash_writesector(DWORD addr, BYTE *pdat);

#define memory_init         flash_init
#define memory_check        flash_check
#define memory_readsector   flash_readsector
#define memory_writesector  flash_writesector

#endif

extern DWORD dwMemorySize;
extern WORD wSectorSize;
extern DWORD dwSectorNumber;

#endif

