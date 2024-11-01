/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "memory.h"


#if !defined(MEMTYPE_INT) && !defined(MEMTYPE_EXT)
#error "Must define one of MEMTYPE_INT and MEMTYPE_EXT"
#endif


DWORD dwMemorySize;
WORD wSectorSize;
DWORD dwSectorNumber;


#if defined MEMTYPE_INT

void iap_init()
{
    IAP_TPS = FOSC/1000000;
    IAP_CONTR = 0x80;
    
    dwMemorySize = 64*1024;
    wSectorSize = 512;
    dwSectorNumber = dwMemorySize / wSectorSize;
}

BOOL iap_check()
{
    return 1;
}

void iap_program_byte(DWORD addr, BYTE dat)
{
    IAP_CMD = 2;
    IAP_ADDRE = 0x00;
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_DATA = dat;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void iap_erase_sector(DWORD addr)
{
    IAP_CMD = 3;
    IAP_ADDRE = 0x00;
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void iap_readsector(DWORD addr, BYTE *pdat)
{
    BYTE *ptr;
    WORD size;
    
    ptr = &IAPBASE[addr];
    for (size = wSectorSize; size; size--)
    {
        if ((DWORD)ptr >= ((DWORD)IAPBASE + dwMemorySize))
            break;
        *pdat++ = *ptr++;
    }
}

void iap_writesector(DWORD addr, BYTE *pdat)
{
    WORD size;
    
    iap_erase_sector(addr);
    for (size = wSectorSize; size; size--)
    {
        iap_program_byte(addr, *pdat++);
        addr++;
    }
}

#else

#define SFC_WREN            0x06
#define SFC_WRDI            0x04
#define SFC_RDSR            0x05
#define SFC_WRSR            0x01
#define SFC_READ            0x03
#define SFC_FASTREAD        0x0B
#define SFC_RDID            0xAB
#define SFC_PAGEPROG        0x02
#define SFC_RDCR            0xA1
#define SFC_WRCR            0xF1
#define SFC_SECTORER        0x20
#define SFC_BLOCKER         0xD8
#define SFC_CHIPER          0xC7
#define SFC_RDJID           0x9F

static BOOL fFlashOK;

sbit SPISS      =   P2^2;

void flash_init()
{
    SPI_S0 = 1;                                 //P2.2(SS_2)/P2.3(MOSI_2)/P2.4(MISO_2)/P2.5(SCLK_2)
    SPI_S1 = 0;
    SPISS = 1;
    SPCTL = 0xd0;
    SPIF = 1;
    
    fFlashOK = 0;
    flash_check();
}

static BYTE shift(BYTE dat)
{
    SPIF = 1;
    SPDAT = dat;
    while (!SPIF);
    
    return SPDAT;
}

BOOL flash_check()
{
    BYTE dat1;
    BYTE dat2;
    BYTE dat3;
    
    if (!fFlashOK)
    {
        SPISS = 0;
        shift(SFC_RDJID);
        dat1 = shift(0x00);
        dat2 = shift(0x00);
        dat3 = shift(0x00);
        SPISS = 1;
        
        //C8 40 13 (GD25Q40)
        //EF 30 13 (W25X40)
        //EF 40 13 (W25Q40)

        if (((dat1 == 0xef) && (dat2 == 0x30)) ||
            ((dat1 == 0xef) && (dat2 == 0x40)) ||
            ((dat1 == 0xc8) && (dat2 == 0x40)))
        {
            if (dat3 == 0x11)
                dwMemorySize = 128*1024;        //GD25Q10/W25X10/W25X10
            else if (dat3 == 0x12)
                dwMemorySize = 256*1024;        //GD25Q20/W25X20/W25X20
            else if (dat3 == 0x13)
                dwMemorySize = 512*1024;        //GD25Q40/W25X40/W25X40
            else if (dat3 == 0x14)
                dwMemorySize = 1024*1024;       //GD25Q80/W25X40/W25X80
    
            wSectorSize = 4*1024;
            dwSectorNumber = dwMemorySize / wSectorSize;
            
            fFlashOK = 1;
        }
        else
        {
            dwMemorySize = 0;
            wSectorSize = 0;
            dwSectorNumber = 0;
        }
    }
    
    return fFlashOK;
}

BOOL flash_isbusy()
{
    BYTE dat;
    
    SPISS = 0;
    shift(SFC_RDSR);
    dat = shift(0);
    SPISS = 1;
    
    return (dat & 0x01);
}

void flash_write_enable()
{
    while (flash_isbusy());

    SPISS = 0;
    shift(SFC_WREN);
    SPISS = 1;
}

void flash_readsector(DWORD addr, BYTE *pdat)
{
    WORD size;
    
    if (!flash_check()) return;
    
    while (flash_isbusy());
    
    SPISS = 0;
    shift(SFC_READ);
    shift((BYTE)(addr >> 16));
    shift((BYTE)(addr >> 8));
    shift((BYTE)(addr));
    for (size = wSectorSize; size; size--)
    {
        *pdat++ = shift(0);
    }
    SPISS = 1;
}

void flash_writesector(DWORD addr, BYTE *pdat)
{
    WORD size;
    
    if (!flash_check()) return;
    
    flash_write_enable();

    SPISS = 0;
    shift(SFC_SECTORER);
    shift((BYTE)(addr >> 16));
    shift((BYTE)(addr >> 8));
    shift((BYTE)(addr));
    SPISS = 1;
    
    size = wSectorSize; 
    while (size)
    {
        flash_write_enable();
    
        SPISS = 0;
        shift(SFC_PAGEPROG);
        shift((BYTE)(addr >> 16));
        shift((BYTE)(addr >> 8));
        shift((BYTE)(addr));
        while (size--)
        {
            shift(*pdat++);
            addr++;
            
            if ((BYTE)(addr) == 0x00)
                break;
        }
        SPISS = 1;
    }
}

#endif

