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

#include "stc.h"
#include "usb.h"
#include "usb_req_class.h"
#include "util.h"
#include "can.h"

LINECODING LineCoding1;
LINECODING LineCoding2;

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case SET_LINE_CODING:
        usb_set_line_coding();
        break;
    case GET_LINE_CODING:
        usb_get_line_coding();
        break;
    case SET_CONTROL_LINE_STATE:
        usb_set_ctrl_line_state();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_set_line_coding()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Interface = Setup.wIndexL;
    if (Interface == 0)
    {
        Ep0State.pData = (BYTE *)&LineCoding1;
    }
    else if (Interface == 2)
    {
        Ep0State.pData = (BYTE *)&LineCoding2;
    }
    else
    {
        usb_setup_stall();
        return;
    }
    Ep0State.wSize = Setup.wLength;

    usb_setup_out();
}

void usb_get_line_coding()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Interface = Setup.wIndexL;
    if (Interface == 0)
    {
        Ep0State.pData = (BYTE *)&LineCoding1;
    }
    else if (Interface == 2)
    {
        Ep0State.pData = (BYTE *)&LineCoding2;
    }
    else
    {
        usb_setup_stall();
        return;
    }
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}

void usb_set_ctrl_line_state()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    usb_setup_status();
}

void usb_uart_settings()
{
    if (Interface == 0)
    {
        LineCoding1.bCharFormat = 0;
        LineCoding1.bDataBits = 8;
    }
    else if (Interface == 2)
    {
        LineCoding2.bCharFormat = 0;
        LineCoding2.bDataBits = 8;
    }
}
