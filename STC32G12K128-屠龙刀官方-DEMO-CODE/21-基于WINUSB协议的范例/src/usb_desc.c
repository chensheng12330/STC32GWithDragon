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
#include "usb_req_vendor.h"

char code DEVICEDESC[18] =
{
    0x12,                   //bLength(18);
    0x01,                   //bDescriptorType(Device);
    0x00,0x02,              //bcdUSB(2.00);
    0xff,                   //bDeviceClass();
    0xff,                   //bDeviceSubClass();
    0xff,                   //bDeviceProtocol();
    0x40,                   //bMaxPacketSize0(64);
    0xbf,0x34,              //idVendor(34BF);
    0x05,0xff,              //idProduct(FF05);
    0x00,0x01,              //bcdDevice(1.00);
    0x01,                   //iManufacturer(1);
    0x02,                   //iProduct(2);
    0x00,                   //iSerialNumber(0);
    0x01,                   //bNumConfigurations(1);
};

char code CONFIGDESC[32] =
{
    0x09,                   //bLength(9);
    0x02,                   //bDescriptorType(Configuration);
    0x20,0x00,              //wTotalLength(32);
    0x01,                   //bNumInterfaces(1);
    0x01,                   //bConfigurationValue(1);
    0x00,                   //iConfiguration(0);
    0x80,                   //bmAttributes(BUSPower);
    0x32,                   //MaxPower(100mA);

    0x09,                   //bLength(9);
    0x04,                   //bDescriptorType(Interface);
    0x00,                   //bInterfaceNumber(0);
    0x00,                   //bAlternateSetting(0);
    0x02,                   //bNumEndpoints(2);
    0xff,                   //bInterfaceClass();
    0xff,                   //bInterfaceSubClass();
    0xff,                   //bInterfaceProtocol();
    0x00,                   //iInterface(0);

    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x81,                   //bEndpointAddress(EndPoint1 as IN);
    0x02,                   //bmAttributes(Bulk);
    0x40,0x00,              //wMaxPacketSize(64);
    0x00,                   //bInterval(Ignored);

    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x01,                   //bEndpointAddress(EndPoint1 as OUT);
    0x02,                   //bmAttributes(Bulk);
    0x40,0x00,              //wMaxPacketSize(64);
    0x00,                   //bInterval(Ignored);
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

char code PRODUCTDESC[22] =
{
    0x16,0x03,
    'S',0,
    'T',0,
    'C',0,
    ' ',0,
    'W',0,
    'I',0,
    'N',0,
    'U',0,
    'S',0,
    'B',0,
};

char code PACKET0[2] = 
{
    0, 0,
};

char code PACKET1[2] = 
{
    1, 0,
};

char code OSDESC[18] = 
{
    0x12,                   //bLength(18)
    0x03,                   //bDescriptorType(3)
    'M',0,                  //Signature('MSFT100')
    'S',0,
    'F',0,
    'T',0,
    '1',0,
    '0',0,
    '0',0,
    MS_VENDORCODE,          //bMS_VendorCode(1)
    0,                      //bPad
};

char code COMPATIBLEIDDESC[40] = 
{
    //Header Section
    0x28,0x00,0x00,0x00,    //dwLength(40)
    0x00,0x01,              //bcdVersion(1.00)
    0x04,0x00,              //wIndex(4)
    0x01,                   //bCount(1)
    0,0,0,0,0,0,0,          //RESERVED

    //Function Section
    0x00,                   //bFirstInterfaceNumber
    0x01,                   //RESERVED
    'W',                    //compatibleID
    'I',
    'N',
    'U',
    'S',
    'B',
    0,0,
    0,0,0,0,0,0,0,0,        //subCompatibleID
    0,0,0,0,0,0,            //RESERVED
};

char code EXTPROPERTYDESC[144] =
{
    //Header Section
    0x90,0x00,0x00,0x00,    //dwTotalSize(144)
    0x00,0x01,              //bcdVersion(1.00)
    0x05,0x00,              //wIndex(5)
    0x01,0x00,              //wCount(1)
  
    //Property Section
    0x86,0x00,0x00,0x00,    //dwSize(134)
    0x01,0x00,0x00,0x00,    //dwPropertyDataType(REG_SZ)
    0x2a,0x00,              //wPropertyNameLength(42)
    'D',0,                  //bProperytName(L"DeviceInterfaceGUIDs")
    'e',0,
    'v',0,
    'i',0,
    'c',0,
    'e',0,
    'I',0,
    'n',0,
    't',0,
    'e',0,
    'r',0,
    'f',0,
    'a',0,
    'c',0,
    'e',0,
    'G',0,
    'U',0,
    'I',0,
    'D',0,
    's',0,
    0,0,
    0x4e,0x00,0x00,0x00,    //dwPropertyDataLength (78)
    '{',0,                  //bProperytName(L"{F72FE0D4-CBCB-407D-8814-9ED673D0DD6B}")
    'F',0,
    '7',0,
    '2',0,
    'F',0,
    'E',0,
    '0',0,
    'D',0,
    '4',0,
    '-',0,
    'C',0,
    'B',0,
    'C',0,
    'B',0,
    '-',0,
    '4',0,
    '0',0,
    '7',0,
    'D',0,
    '-',0,
    '8',0,
    '8',0,
    '1',0,
    '4',0,
    '-',0,
    '9',0,
    'E',0,
    'D',0,
    '6',0,
    '7',0,
    '3',0,
    'D',0,
    '0',0,
    'D',0,
    'D',0,
    '6',0,
    'B',0,
    '}',0,
    0,0,
};
