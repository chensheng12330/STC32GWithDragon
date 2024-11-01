/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "uart.h"
#include "interface.h"

static void SEG7_SetHeader()
{
    UartTxBuffer[0] = '7';
    UartTxBuffer[1] = 'S';
    UartTxBuffer[2] = 'E';
    UartTxBuffer[3] = 'G';
}

void SEG7_ShowLong(long n, char radix)
{
    SEG7_SetHeader();
    UartTxBuffer[4] = 'L';
    UartTxBuffer[5] = 0x01;
    UartTxBuffer[6] = radix;
    UartTxBuffer[7] = 0x00;
    *(long *)&UartTxBuffer[8] = n;

    uart_SendBlock(12);
}

void SEG7_ShowFloat(float f)
{
    SEG7_SetHeader();
    UartTxBuffer[4] = 'D';
    UartTxBuffer[5] = 0x01;
    UartTxBuffer[6] = 0x00;
    UartTxBuffer[7] = 0x00;
    *(float *)&UartTxBuffer[8] = f;

    uart_SendBlock(12);
}

void SEG7_ShowCode(BYTE *cod)
{
    SEG7_SetHeader();
    UartTxBuffer[4] = 'C';
    UartTxBuffer[5] = 0x00;
    UartTxBuffer[6] = 0x00;
    UartTxBuffer[7] = 0x00;
    memcpy(&UartTxBuffer[8], cod, 8);

    uart_SendBlock(16);
}

int SEG7_ShowString (const char *fmt, ...)
{
    va_list va;
    int ret;

    SEG7_SetHeader();
    UartTxBuffer[4] = 'S';
    UartTxBuffer[5] = 0x00;
    UartTxBuffer[6] = 0x00;
    UartTxBuffer[7] = 0x00;

    va_start(va, fmt);
    ret = vsprintf(&UartTxBuffer[8], fmt, va) + 8;
    va_end(va);

    uart_SendBlock((BYTE)(ret+1));

    return ret;
}

void LED40_SendData(BYTE *dat, BYTE size)
{
    if (size > 59)
        size = 59;

    UartTxBuffer[0] = 'L';
    UartTxBuffer[1] = 'E';
    UartTxBuffer[2] = 'D';
    UartTxBuffer[3] = 40;
    UartTxBuffer[4] = size;
    memcpy(&UartTxBuffer[5], dat, size);

    uart_SendBlock((BYTE)(size+5));
}

void LED64_SendData(BYTE *dat, BYTE size)
{
    if (size > 59)
        size = 59;

    UartTxBuffer[0] = 'L';
    UartTxBuffer[1] = 'E';
    UartTxBuffer[2] = 'D';
    UartTxBuffer[3] = 64;
    UartTxBuffer[4] = size;
    memcpy(&UartTxBuffer[5], dat, size);

    uart_SendBlock((BYTE)(size+5));
}

static void LCD12864_SetHeader()
{
    UartTxBuffer[0] = 'L';
    UartTxBuffer[1] = 'C';
    UartTxBuffer[2] = 'D';
    UartTxBuffer[3] = 128;
}

void LCD12864_DisplayOff()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa0;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void LCD12864_DisplayOn()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa0;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void LCD12864_CursorOff()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa1;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void LCD12864_CursorOn()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa1;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void LCD12864_CursorMoveLeft()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa2;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void LCD12864_CursorMoveRight()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa2;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void LCD12864_CursorReturnHome()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa2;
    UartTxBuffer[6] = 0x02;

    uart_SendBlock(7);
}

void LCD12864_ScrollLeft()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa3;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void LCD12864_ScrollRight()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa3;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void LCD12864_ScrollUp(BYTE line)
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 3;
    UartTxBuffer[5] = 0xa3;
    UartTxBuffer[6] = 0x02;
    UartTxBuffer[7] = line;

    uart_SendBlock(8);
}

void LCD12864_AutoWrapOff()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa4;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void LCD12864_AutoWrapOn()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa4;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void LCD12864_ReverseLine(BYTE line)
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xa5;
    UartTxBuffer[6] = line;

    uart_SendBlock(7);
}

void LCD12864_DisplayClear()
{
    LCD12864_SetHeader();
    UartTxBuffer[4] = 1;
    UartTxBuffer[5] = 0xa6;

    uart_SendBlock(6);
}

static void LCD12864_SetBuffer(BYTE *dat, WORD size)
{
    BYTE cnt;
    WORD addr;

    addr = 0;
    while (size)
    {
        cnt = 56;
        if (size < 56)
            cnt = size;

        LCD12864_SetHeader();
        UartTxBuffer[4] = cnt + 3;
        UartTxBuffer[5] = 0xaf;
        *(WORD *)&UartTxBuffer[6] = addr;
        memcpy(&UartTxBuffer[8], dat, cnt);

        uart_SendBlock((BYTE)(cnt+8));

        dat += cnt;
        addr += cnt;
        size -= cnt;
    }
}

void LCD12864_ShowString(BYTE x, BYTE y, char *str)
{
    LCD12864_SetBuffer(str, strlen(str) + 1);

    LCD12864_SetHeader();
    UartTxBuffer[4] = 3;
    UartTxBuffer[5] = 0xa7;
    UartTxBuffer[6] = x;
    UartTxBuffer[7] = y;

    uart_SendBlock(8);
}

void LCD12864_ShowPicture(BYTE x, BYTE y, BYTE cx, BYTE cy, BYTE *dat)
{
    LCD12864_SetBuffer(dat, (WORD)(2UL*cx*cy));

    LCD12864_SetHeader();
    UartTxBuffer[4] = 5;
    UartTxBuffer[5] = 0xa8;
    UartTxBuffer[6] = x;
    UartTxBuffer[7] = y;
    UartTxBuffer[8] = cx;
    UartTxBuffer[9] = cy;

    uart_SendBlock(10);
}

static void OLED12864_SetHeader()
{
    UartTxBuffer[0] = 'O';
    UartTxBuffer[1] = 'L';
    UartTxBuffer[2] = 'E';
    UartTxBuffer[3] = 128;
}

void OLED12864_DisplayOff()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc0;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void OLED12864_DisplayOn()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc0;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void OLED12864_DisplayContent()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc1;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void OLED12864_DisplayEntire()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc1;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void OLED12864_HorizontalMirror()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc2;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

void OLED12864_VerticalMirror()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc2;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void OLED12864_DisplayReverse()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 1;
    UartTxBuffer[5] = 0xc3;

    uart_SendBlock(6);
}

void OLED12864_SetContrast(BYTE bContrast)
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc4;
    UartTxBuffer[6] = bContrast;

    uart_SendBlock(7);
}

void OLED12864_SetAddressMode(BYTE bMode)
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc5;
    UartTxBuffer[6] = bMode;

    uart_SendBlock(7);
}

void OLED12864_ScrollLeft(BYTE bPageStart, BYTE bPageEnd, WORD nInterval)
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 6;
    UartTxBuffer[5] = 0xc6;
    UartTxBuffer[6] = 0x00;
    UartTxBuffer[7] = bPageStart;
    UartTxBuffer[8] = bPageEnd;
    *(WORD *)&UartTxBuffer[9] = nInterval;

    uart_SendBlock(11);
}

void OLED12864_ScrollRight(BYTE bPageStart, BYTE bPageEnd, WORD nInterval)
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 6;
    UartTxBuffer[5] = 0xc6;
    UartTxBuffer[6] = 0x01;
    UartTxBuffer[7] = bPageStart;
    UartTxBuffer[8] = bPageEnd;
    *(WORD *)&UartTxBuffer[9] = nInterval;

    uart_SendBlock(11);
}

void OLED12864_ScrollUp(BYTE bPageStart, BYTE bPageEnd, WORD nInterval)
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 6;
    UartTxBuffer[5] = 0xc6;
    UartTxBuffer[6] = 0x02;
    UartTxBuffer[7] = bPageStart;
    UartTxBuffer[8] = bPageEnd;
    *(WORD *)&UartTxBuffer[9] = nInterval;

    uart_SendBlock(11);
}

void OLED12864_ScrollStart()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc7;
    UartTxBuffer[6] = 0x01;

    uart_SendBlock(7);
}

void OLED12864_ScrollStop()
{
    OLED12864_SetHeader();
    UartTxBuffer[4] = 2;
    UartTxBuffer[5] = 0xc7;
    UartTxBuffer[6] = 0x00;

    uart_SendBlock(7);
}

static void OLED12864_SetBuffer(BYTE *dat, WORD size)
{
    BYTE cnt;
    WORD addr;

    addr = 0;
    while (size)
    {
        cnt = 56;
        if (size < 56)
            cnt = size;

        OLED12864_SetHeader();
        UartTxBuffer[4] = cnt + 3;
        UartTxBuffer[5] = 0xcf;
        *(WORD *)&UartTxBuffer[6] = addr;
        memcpy(&UartTxBuffer[8], dat, cnt);

        uart_SendBlock((BYTE)(cnt+8));

        dat += cnt;
        addr += cnt;
        size -= cnt;
    }
}

void OLED12864_ShowPicture(BYTE x, BYTE y, BYTE cx, BYTE cy, BYTE *dat)
{
    OLED12864_SetBuffer(dat, (WORD)cx*cy);

    OLED12864_SetHeader();
    UartTxBuffer[4] = 5;
    UartTxBuffer[5] = 0xc8;
    UartTxBuffer[6] = x;
    UartTxBuffer[7] = y;
    UartTxBuffer[8] = cx;
    UartTxBuffer[9] = cy;

    uart_SendBlock(10);
}

