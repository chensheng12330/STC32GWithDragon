#include "middle.h"
#include "set.h"

unsigned char xdata pos_x,pos_y;      //OLED显示调用的地址变量
u8 xdata state=1;

void Screen_ShowAsc(u8 x,u8 y,u8 asc)
{
    OLED_BuffShowChar(x,y,(asc));
}
void Screen_ShowAscOpp(u8 x,u8 y,u8 asc)
{
    OLED_BuffShowCharOpp(x,y,(asc));
}

void Screen_ShowNum(u8 x,u8 y,double num)
{
    u8 xdata str[10];
    memset(str,0,10);
    sprintf(str,"%g",num);
    OLED_BuffShowString(x,y,(u8*)str);
}

void Screen_ShowStr(u8 x,u8 y,u8 *str)
{
    OLED_BuffShowString(x,y,(u8*)str);
}

void Screen_ShowFreshen(void)
{
    OLED_BuffShow();
}

void Screen_Clear(void)
{
    OLED_BuffClear();
}

void Screen_ShowLine( unsigned char x1, unsigned char y1, unsigned char x2,unsigned char y2)
{
    OLED_BuffShowLine(x1,y1,x2,y2);
}

void Screen_ShowPoint( unsigned char x, unsigned char y)
{
    OLED_BuffShowPoint(x,y);     //OLED显示一个dian
}

void Screen_Showlight(unsigned char num) //1-5
{
    OLED_LightSet((u8)(10 + 45* num));     //亮度设置
}
        

