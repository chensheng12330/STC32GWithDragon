
#include "system.h"
#include "iic.h"
#include "u8g2_porting.h"
//==========================================================================


/*************  本地常量声明    **************/

/*************  IO口定义    **************/

/*************  本地变量声明    **************/

/*************  本地函数声明    **************/

/****************  外部函数声明和外部变量声明 *****************/

/******************** 主函数 **************************/
void main(void)
{
  WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
  EAXFR = 1; //扩展寄存器(XFR)访问使能
  CKCON = 0; //提高访问XRAM速度

  HAL_GPIO_Init();
 // HAL_Timer0_Init();
  EA = 1; //打开总中断

  UartInit();
  HAL_I2C_Init();
  DMA_Config();
  LCD_Init();
  LCD_show();

  while (1)
  {
  }
}
