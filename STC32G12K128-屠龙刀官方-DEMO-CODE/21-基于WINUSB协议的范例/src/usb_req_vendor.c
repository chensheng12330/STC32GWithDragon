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
#include "usb_req_vendor.h"
#include "usb_desc.h"

void usb_req_vendor()
{
    switch (Setup.bRequest)
    {
    case MS_VENDORCODE:
        usb_vendor_code();
        break;
    case VENDOR_INPUT:
        usb_vendor_in();
        break;
    case VENDOR_OUTPUT:
        usb_vendor_out();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_vendor_code()
{
    if (Setup.bmRequestType != (IN_DIRECT | VENDOR_REQUEST | DEVICE_RECIPIENT))
    {
        usb_setup_stall();
        return;
    }
    
    switch (Setup.wIndexL)
    {
    case 0x04:
        Ep0State.pData = COMPATIBLEIDDESC;
        Ep0State.wSize = sizeof(COMPATIBLEIDDESC);
        break;
    case 0x05:
        Ep0State.pData = EXTPROPERTYDESC;
        Ep0State.wSize = sizeof(EXTPROPERTYDESC);
        break;
    default:
        usb_setup_stall();
        return;
    }
    
    if (Ep0State.wSize > Setup.wLength)
    {
        Ep0State.wSize = Setup.wLength;
    }

    usb_setup_in();
}

void usb_vendor_in()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | VENDOR_REQUEST | DEVICE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}

void usb_vendor_out()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | VENDOR_REQUEST | DEVICE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_out();
}

void usb_vendor_polling()
{
    BYTE cnt;
    
    if (DeviceState != DEVSTATE_CONFIGURED)  //如果USB配置没有完成，就直接退出
        return;

    if (!UsbInBusy && (Rptr != Wptr))  //判断USB是否空闲，以及接收数据个数是否等于发送数据个数
    {
        EUSB = 0;
        UsbInBusy = 1;
        usb_write_reg(INDEX, 1);
        cnt = 0;
        while (Rptr != Wptr)
        {
            usb_write_reg(FIFO1, UsbBuffer[Rptr++]);  //发送接收到的数据
            cnt++;
            if (cnt == EP1IN_SIZE) break;
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }
    
    if (UsbOutBusy)
    {
        EUSB = 0;
        if (Wptr - Rptr < 256 - EP1OUT_SIZE)
        {
            UsbOutBusy = 0;
            usb_write_reg(INDEX, 1);
            usb_write_reg(OUTCSR1, 0);
        }
        EUSB = 1;
    }
}
