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
#include "usb_desc.h"

char code DEVICEDESC[18] =
{
    0x12,                   //bLength(18);
    0x01,                   //bDescriptorType(Device);
    0x00,0x02,              //bcdUSB(2.00);
    0x00,                   //bDeviceClass(0);
    0x00,                   //bDeviceSubClass0);
    0x00,                   //bDeviceProtocol(0);
    0x40,                   //bMaxPacketSize0(64);
    0xbf,0x34,              //idVendor(34BF);
    0x04,0xff,              //idProduct(FF04);
    0x00,0x01,              //bcdDevice(1.00);
    0x01,                   //iManufacturer(1);
    0x02,                   //iProduct(2);
    0x00,                   //iSerialNumber(0);
    0x01,                   //bNumConfigurations(1);
};

char code CONFIGDESC[41] =
{
    0x09,                   //bLength(9);
    0x02,                   //bDescriptorType(Configuration);
    0x29,0x00,              //wTotalLength(41);
    0x01,                   //bNumInterfaces(1);
    0x01,                   //bConfigurationValue(1);
    0x00,                   //iConfiguration(0);
    0x80,                   //bmAttributes(BUSPower);
    0x32,                   //MaxPower(100mA);

    0x09,                   //bLength(9);
    0x04,                   //bDescriptorType(Interface);
    0x00,                   //bInterfaceNumber(0);
    0x00,                   //bAlternateSetting(0);
    0x01,                   //bNumEndpoints(1);
    0x03,                   //bInterfaceClass(HID);
    0x01,                   //bInterfaceSubClass(Boot);
    0x02,                   //bInterfaceProtocol(Mouse);
    0x00,                   //iInterface(0);

    0x09,                   //bLength(9);
    0x21,                   //bDescriptorType(HID);
    0x01,0x01,              //bcdHID(1.01);
    0x00,                   //bCountryCode(0);
    0x01,                   //bNumDescriptors(1);
    0x22,                   //bDescriptorType(HID Report);
    0x32,0x00,              //wDescriptorLength(50);

    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x81,                   //bEndpointAddress(EndPoint1 as IN);
    0x03,                   //bmAttributes(Interrupt);
    0x03,0x00,              //wMaxPacketSize(3);
    0x0a,                   //bInterval(10ms);
};

/*
Input Report:
0   Buttons (D0:LButton D1:RButton D2:MButton [D3:D7]:Pad)
1	X displacement (>0:right; <0:left)
2	Y displacement (>0:down; <0:up)
*/
char code HIDREPORTDESC[50] =
{
    0x05,0x01,              //USAGE_PAGE(Generic Desktop);
    0x09,0x02,              //USAGE(Mouse);
    0xa1,0x01,              //COLLECTION(Application);
    0x09,0x01,              //  USAGE(Pointer);
    0xa1,0x00,              //  COLLECTION(Physical);
    0x05,0x09,              //    USAGE_PAGE(Buttons);
    0x19,0x01,              //    USAGE_MINIMUM(1);
    0x29,0x03,              //    USAGE_MAXIMUM(3);
    0x15,0x00,              //    LOGICAL_MINIMUM(0);
    0x25,0x01,              //    LOGICAL_MAXIMUM(1);
    0x75,0x01,              //    REPORT_SIZE(1);
    0x95,0x03,              //    REPORT_COUNT(3);
    0x81,0x02,              //    INPUT(Data,Variable,Absolute);
    0x75,0x05,              //    REPORT_SIZE(5);
    0x95,0x01,              //    REPORT_COUNT(1);
    0x81,0x01,              //    INPUT(Constant);
    0x05,0x01,              //    USAGE_PAGE(Generic Desktop);
    0x09,0x30,              //    USAGE(X);
    0x09,0x31,              //    USAGE(Y);
    0x15,0x81,              //    LOGICAL_MINIMUM(-127);
    0x25,0x7f,              //    LOGICAL_MAXIMUM(127);
    0x75,0x08,              //    REPORT_SIZE(8);
    0x95,0x02,              //    REPORT_COUNT(2);
    0x81,0x06,              //    INPUT(Data, Variable, Relative);
    0xc0,                   //  END_COLLECTION;
    0xc0,                   //END_COLLECTION;
};

char code LANGIDDESC[4] =
{
    0x04,0x03,
    0x09,0x04,
};

char code MANUFACTDESC[8] =
{
    0x08,0x03,
    'S',0,
    'T',0,
    'C',0,
};

char code PRODUCTDESC[28] =
{
    0x1c,0x03,
    'S',0,
    'T',0,
    'C',0,
    ' ',0,
    'U',0,
    'S',0,
    'B',0,
    ' ',0,
    'M',0,
    'o',0,
    'u',0,
    's',0,
    'e',0,
};

char code PACKET0[2] = 
{
    0, 0,
};

char code PACKET1[2] = 
{
    1, 0,
};
