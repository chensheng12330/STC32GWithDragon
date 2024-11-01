#ifndef __OLED_H
#define __OLED_H


#define Drive_Func  0  //0:CDC虚拟串口   1：硬件SPI（暂无实现）

extern  unsigned char xdata ShowBUFF[8][128];

void OLED_Init(void);
void OLED_DisplayOn(void);                                          //开显示
void OLED_DisplayOff(void);                                         //关显示
void OLED_LightSet(unsigned char num);     //亮度设置
    
void OLED_ShowChar(unsigned char x,unsigned char y,char asc);       //OLED显示一个8*16字符
void OLED_ShowGBK(unsigned char x,unsigned char y,char *gbk);       //OLED显示一个16*16字符
void OLED_ShowString(unsigned char x,unsigned char y,char *s);

    
void OLED_BuffClear(void);     //清屏(CDC未实现)
void OLED_BuffShow(void)     ;
void OLED_BuffShowChar(unsigned char x,unsigned char y,char asc);     //OLED显示一个8*16字符
void OLED_BuffShowGBK(unsigned char x,unsigned char y,char *gbk);     //OLED显示一个16*16字符
void OLED_BuffShowString(unsigned char x,unsigned char y,char *s);
void OLED_BuffShowPoint(unsigned char x,unsigned char y);     //OLED显示一个8*16字符
void OLED_BuffShowLine( unsigned char x1, unsigned char y1, unsigned char x2,unsigned char y2);
void OLED_BuffShowAscll32(unsigned char  x,unsigned char  y,unsigned char num);
void OLED_BuffShowCharOpp(unsigned char x,unsigned char y,char asc) ;
void Cursor_Blink( unsigned char x,unsigned char y,unsigned char count );
#endif



