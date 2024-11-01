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
#include "usb.h"
#include "usb_req_class.h"

BYTE bHidIdle;
BYTE bKeyCode = 0;
BYTE bKeyDebounce = 0;
BOOL fKeyOK;

sbit LED_NUM        = P2^7;
sbit LED_CAPS       = P2^6;
sbit LED_SCROLL     = P2^5;

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case GET_REPORT:
        usb_get_report();
        break;
    case SET_REPORT:
        usb_set_report();
        break;
    case GET_IDLE:
        usb_get_idle();
        break;
    case SET_IDLE:
        usb_set_idle();
        break;
    case GET_PROTOCOL:
        usb_get_protocol();
        break;
    case SET_PROTOCOL:
        usb_set_protocol();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_get_report()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}

void usb_set_report()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_out();
}

void usb_get_idle()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = &bHidIdle;
    Ep0State.wSize = 1;

    usb_setup_in();
}

void usb_set_idle()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    bHidIdle = Setup.wValueH;

    usb_setup_status();
}

void usb_get_protocol()
{
    usb_setup_stall();
}

void usb_set_protocol()
{
    usb_setup_stall();
}

void usb_class_out()
{
    BYTE led;
    
    if (usb_bulk_intr_out(UsbBuffer, 1) == 1)
    {
        led = UsbBuffer[0];
        LED_NUM = !(led & 0x01);    //获取 NUM 指示灯状态
        LED_CAPS = !(led & 0x02);   //获取 CAPS 指示灯状态
        LED_SCROLL = !(led & 0x04); //获取 SCROLL 指示灯状态
    }
}

void usb_class_in()
{
    BYTE key[8];
    BYTE i;
    
    if (DeviceState != DEVSTATE_CONFIGURED)  //如果USB配置没有完成，就直接退出
        return;

    if (!UsbInBusy && fKeyOK)  //判断USB是否空闲，以及是否有按键按下
    {
        fKeyOK = 0;

        key[0] = 0;  //清除发送缓冲区
        key[1] = 0;
        key[2] = 0;
        key[3] = 0;
        key[4] = 0;
        key[5] = 0;
        key[6] = 0;
        key[7] = 0;

        switch (bKeyCode)
        {
        case 0x0e:  key[2] = 0x1e; break;  //0x1e -> "1"
        case 0x0d:  key[2] = 0x1f; break;  //0x1f -> "2"
        case 0x0b:  key[2] = 0x20; break;  //0x20 -> "3"
        case 0x07:  key[2] = 0x21; break;  //0x21 -> "4"
        }
        
        EUSB = 0;
        UsbInBusy = 1;
        usb_write_reg(INDEX, 1);
        for (i=0; i<8; i++)
        {
            usb_write_reg(FIFO1, key[i]);  //发送按键码
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }
}

void scan_key()
{
    BYTE key;
    
    key = 0;
    P3 |= 0x3c;
    _nop_();
    _nop_();
    key = (P3 & 0x3c) >> 2;  //获取P3.2~P3.5按键状态
    
    if (key != bKeyCode)
    {
        bKeyCode = key;
        bKeyDebounce = 20;  //设置防抖时间 20 * 1ms = 20ms
    }
    else
    {
        if (bKeyDebounce)
        {
            bKeyDebounce--;
            if (bKeyDebounce == 0)
            {
                fKeyOK = 1;  //检测到按键状态发送变化
            }
        }
    }
    
}

