/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于STC32G核心转接板（屠龙刀）进行编写测试。

通过USB连接STC-ISP(V6.90以后版本);

STC-ISP调试接口里面的虚拟键盘上点击按键发送相应的键值到MCU;

MCU收到键值执行对应的功能;

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32g.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 24MHZ (用户可自行修改频率)。

******************************************/

#define PRINTF_SEGLED           //printf输出重定向到ISP下载软件中的7段数码管

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件
#include "vk.h"
#include "pic.h"

#define MAIN_Fosc       24000000L   //定义主时钟

void sys_init();
void DelayXms(int n);

char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

BYTE xdata cod[8];

void delay_ms(u8 ms);
void KeyResetScan(void);

void main()
{
    sys_init();
    usb_init();
    EA = 1;

    while (1)
    {
        delay_ms(1);
        KeyResetScan();   //长按P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码

        if (DeviceState != DEVSTATE_CONFIGURED)        //判断USB设备识别是否完成
            continue;

        if (bUsbOutReady)
        {
            if ((UsbOutBuffer[0] == 'K') &&
                (UsbOutBuffer[1] == 'E') &&
                (UsbOutBuffer[2] == 'Y') &&
                (UsbOutBuffer[3] == 'P'))
            {
                switch (UsbOutBuffer[5])
                {
                case VK_DIGIT_1:
                    printf("%08lx", 0x1234abcdL);      //在数码管上显示字符串
                    break;
                case VK_DIGIT_2:
                    SEG7_ShowLong(0x98765432, 16);     //在数码管上显示4字节长整型数
                    break;
                case VK_DIGIT_3:
                    SEG7_ShowFloat(3.1415);            //在数码管上显示IEEE754格式单精度浮点数
                    break;
                case VK_DIGIT_4:
                    cod[0] = 0x3f;
                    cod[1] = 0x06;
                    cod[2] = 0x5b;
                    cod[3] = 0x4f;
                    cod[4] = 0x66;
                    cod[5] = 0x6d;
                    cod[6] = 0x7d;
                    cod[7] = 0x27;
                    SEG7_ShowCode(cod);                //在数码管上直接显示所给的段码
                    break;
                case VK_DIGIT_5:
                    cod[0] = 0x01;    //屏蔽位，P0有效
                    cod[1] = 0xaa;    //P0状态
                    LED40_SendData(cod, 2);            //控制DIP40的各个管脚上LED的状态
                    break;
                case VK_DIGIT_6:
                    cod[0] = 0x0f;    //屏蔽位，P0~P3有效
                    cod[1] = 0x12;    //P0状态
                    cod[2] = 0x34;    //P1状态
                    cod[3] = 0x56;    //P2状态
                    cod[4] = 0x78;    //P3状态
                    LED40_SendData(cod, 5);            //控制DIP40的各个管脚上LED的状态
                    break;
                case VK_DIGIT_7:
                    cod[0] = 0x01;    //屏蔽位，P0有效
                    cod[1] = 0xaa;    //P0状态
                    LED64_SendData(cod, 2);            //控制DIP64的各个管脚上LED的状态
                    break;
                case VK_DIGIT_8:
                    cod[0] = 0x0f;    //屏蔽位，P0~P3有效
                    cod[1] = 0x12;    //P0状态
                    cod[2] = 0x34;    //P1状态
                    cod[3] = 0x56;    //P2状态
                    cod[4] = 0x78;    //P3状态
                    LED64_SendData(cod, 5);            //控制DIP64的各个管脚上LED的状态
                    break;
                case VK_ALPHA_A:
                    LCD12864_DisplayOff();             //关闭LCD12864的显示
                    break;
                case VK_ALPHA_B:
                    LCD12864_DisplayOn();              //打开LCD12864的显示
                    break;
                case VK_ALPHA_C:
                    LCD12864_CursorOff();              //隐藏LCD12864的光标
                    break;
                case VK_ALPHA_D:
                    LCD12864_CursorOn();               //显示LCD12864的光标
                    break;
                case VK_ALPHA_E:
                    LCD12864_CursorMoveLeft();         //光标向左移动一个单位（16个像素）
                    break;
                case VK_ALPHA_F:
                    LCD12864_CursorMoveRight();        //光标向右移动一个单位（16个像素）
                    break;
                case VK_ALPHA_G:
                    LCD12864_CursorReturnHome();       //光标回到左上角初始位置
                    break;
                case VK_ALPHA_H:
                    LCD12864_ScrollLeft();             //屏幕向左滚动一个单位（16个像素）
                    break;
                case VK_ALPHA_I:
                    LCD12864_ScrollRight();            //屏幕向右滚动一个单位（16个像素）
                    break;
                case VK_ALPHA_J:
                    LCD12864_ScrollUp(16);             //屏幕向上滚动一个单位（16个像素）
                    break;
                case VK_ALPHA_K:
                    LCD12864_AutoWrapOff();            //关闭自动换行功能
                    break;
                case VK_ALPHA_L:
                    LCD12864_AutoWrapOn();             //开启自动换行功能
                    break;
                case VK_ALPHA_M:
                    LCD12864_ReverseLine(1);           //反白显示指定的行
                    break;
                case VK_ALPHA_N:
                    LCD12864_DisplayClear();           //清除显示
                    break;
                case VK_ALPHA_O:
                    LCD12864_ShowString(0, 0, "深圳国芯人工智能有限公司");  //在LCD12864上显示ASCII码和简体中文字符
                    break;
                case VK_ALPHA_P:
                    LCD12864_ShowPicture(0, 0, 8, 64, (BYTE *)&LCD12864_IMG);  //在LCD12864屏幕上显示图片
                    break;
                case VK_ALPHA_Q:
                    OLED12864_DisplayOff();            //关闭OLED12864的显示
                    break;
                case VK_ALPHA_R:
                    OLED12864_DisplayOn();             //打开OLED12864的显示
                    break;
                case VK_ALPHA_S:
                    OLED12864_DisplayContent();        //显示屏幕内容
                    break;
                case VK_ALPHA_T:
                    OLED12864_DisplayEntire();         //全屏点亮（用于测试）
                    break;
                case VK_ALPHA_U:
                    OLED12864_HorizontalMirror();      //水平镜像显示
                    break;
                case VK_ALPHA_V:
                    OLED12864_VerticalMirror();        //垂直镜像显示
                    break;
                case VK_ALPHA_W:
                    OLED12864_DisplayReverse();        //反白显示
                    break;
                case VK_ALPHA_X:
                    OLED12864_SetContrast(0x10);       //亮度值（范围0～255）
                    break;
                case VK_ALPHA_Y:
                    OLED12864_SetContrast(0x80);       //亮度值（范围0～255）
                    break;
                case VK_ALPHA_Z:
                    OLED12864_SetContrast(0xf0);       //亮度值（范围0～255）
                    break;
                case VK_UP:
                    //参数1：参与滚屏的起始页（范围0～7）
                    //参数2：参与滚屏的结束页（范围0～7）
                    //参数3：滚屏的周期值（周期单位为毫秒）
                    OLED12864_ScrollUp(1, 6, 200);     //设置好方向后通过“OLED12864_ScrollStart”指令开始执行
                    break;
                case VK_DOWN:
                    OLED12864_ScrollStart();           //开始执行滚动操作
                    break;
                case VK_LEFT:
                    //参数1：参与滚屏的起始页（范围0～7）
                    //参数2：参与滚屏的结束页（范围0～7）
                    //参数3：滚屏的周期值（周期单位为毫秒）
                    OLED12864_ScrollLeft(0, 2, 200);   //设置好方向后通过“OLED12864_ScrollStart”指令开始执行
                    break;
                case VK_RIGHT:
                    //参数1：参与滚屏的起始页（范围0～7）
                    //参数2：参与滚屏的结束页（范围0～7）
                    //参数3：滚屏的周期值（周期单位为毫秒）
                    OLED12864_ScrollRight(0, 2, 200);  //设置好方向后通过“OLED12864_ScrollStart”指令开始执行
                    break;
                case VK_HOME:
                    OLED12864_SetAddressMode(0);       //寻址模式（0：水平寻址； 1：垂直寻址； 2：页寻址）
                    break;
                case VK_END:
                    OLED12864_SetAddressMode(1);       //寻址模式（0：水平寻址； 1：垂直寻址； 2：页寻址）
                    break;
                case VK_PRIOR:    //PageUp
                    OLED12864_ScrollStop();            //停止执行滚动操作
                    break;
                case VK_NEXT:     //PageDown
                    OLED12864_ShowPicture(0, 0, 128, 8, (BYTE *)&OLED12864_IMG);  //在OLED12864屏幕上显示图片
                    break;
                }
            }
            else
            {
                USB_SendData(UsbOutBuffer,64);    //发送数据缓冲区，长度
            }
            usb_OUT_done();
        }
    }
}

void sys_init()
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-3-9
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
     u16 i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);   //6T per loop
     }while(--ms);
}

//========================================================================
// 函数: void KeyResetScan(void)
// 描述: P3.2口按键长按1秒触发软件复位，进入USB下载模式。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//连续1000ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

                USBCON = 0x00;      //清除USB设置
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
                while (1);
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}