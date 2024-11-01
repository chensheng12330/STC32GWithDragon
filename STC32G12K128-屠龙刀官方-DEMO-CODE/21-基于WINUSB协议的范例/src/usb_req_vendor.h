#ifndef __USB_REQ_VENDOR_H__
#define __USB_REQ_VENDOR_H__

#define MS_VENDORCODE           0x01
#define VENDOR_INPUT            0x02
#define VENDOR_OUTPUT           0x03

void usb_req_vendor();
void usb_vendor_polling();

void usb_vendor_code();
void usb_vendor_in();
void usb_vendor_out();

#endif
