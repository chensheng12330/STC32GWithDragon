#ifndef __INTERFACE_H__
#define __INTERFACE_H__

int SEG7_ShowString(const char *fmt, ...);
void SEG7_ShowLong(long n, char radix);
void SEG7_ShowFloat(float f);
void SEG7_ShowCode(BYTE *cod);

void LED40_SendData(BYTE *dat, BYTE size);
void LED64_SendData(BYTE *dat, BYTE size);

void LCD12864_DisplayOff();
void LCD12864_DisplayOn();
void LCD12864_CursorOff();
void LCD12864_CursorOn();
void LCD12864_CursorMoveLeft();
void LCD12864_CursorMoveRight();
void LCD12864_CursorReturnHome();
void LCD12864_ScrollLeft();
void LCD12864_ScrollRight();
void LCD12864_ScrollUp(BYTE line);
void LCD12864_AutoWrapOff();
void LCD12864_AutoWrapOn();
void LCD12864_ReverseLine(BYTE line);
void LCD12864_DisplayClear();
void LCD12864_ShowString(BYTE x, BYTE y, char *str);
void LCD12864_ShowPicture(BYTE x, BYTE y, BYTE cx, BYTE cy, BYTE *dat);

void OLED12864_DisplayOff();
void OLED12864_DisplayOn();
void OLED12864_DisplayContent();
void OLED12864_DisplayEntire();
void OLED12864_HorizontalMirror();
void OLED12864_VerticalMirror();
void OLED12864_DisplayReverse();
void OLED12864_SetContrast(BYTE bContrast);
void OLED12864_SetAddressMode(BYTE bMode);
void OLED12864_ScrollLeft(BYTE bPageStart, BYTE bPageEnd, WORD nInterval);
void OLED12864_ScrollRight(BYTE bPageStart, BYTE bPageEnd, WORD nInterval);
void OLED12864_ScrollUp(BYTE bPageStart, BYTE bPageEnd, WORD nInterval);
void OLED12864_ScrollStart();
void OLED12864_ScrollStop();
void OLED12864_ShowPicture(BYTE x, BYTE y, BYTE cx, BYTE cy, BYTE *dat);

#endif
