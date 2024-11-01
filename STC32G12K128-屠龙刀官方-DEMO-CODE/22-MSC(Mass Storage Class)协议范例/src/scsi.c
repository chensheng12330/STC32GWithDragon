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
#include "usb_desc.h"
#include "scsi.h"
#include "util.h"
#include "memory.h"

#define LBA                 (MEMORYSIZE/SECTORSIZE - 1)

static CBW cbw;
static CSW csw;
static BYTE status;
static BYTE sensekey;
static BYTE senseasc;
static DWORD residue;
static BYTE xdata scsibuf[4096];

void scsi_process()
{
    BYTE cnt;
    BYTE key;
    BYTE asc;
    
    cnt = usb_bulk_intr_out((BYTE *)&cbw, 1);
    if ((cnt == sizeof(CBW)) &&
        (cbw.dCBWSignature == CBW_SIGNATURE))
    {
        residue = reverse4(cbw.dCBWDataLength);
        status = SCSI_PASSED;
        key = SENSE_NOSENSE;
        asc = 0;
       
        switch (cbw.bScsiOpcode)
        {
        case SCSI_TESTUNITREADY:
            if (!scsi_test_unit_ready())
            {
                status = SCSI_FAILED;
                key = SENSE_NOTREADY;
                asc = ASC_MEDIUMNOTPRESENT;
            }
            break;
        case SCCI_STARTSTOPUNIT:
            scsi_start_stop_unit();
            break;
        case SCSI_MEDIAREMOVAL:
            scsi_media_removal();
            break;
        case SCSI_INQUIRY:
            scsi_inquiry();
            break;
        case SCSI_READCAPACITY10:
            scsi_read_capacity10();
            break;
        case SCSI_FORMATCAPACITY:
            scsi_format_capacity();
            break;
        case SCSI_REQUESTSENSE:
            scsi_request_sense();
            break;
        case SCSI_MODESENSE6:
            scsi_mode_sense6();
            break;
        case SCSI_MODESENSE10:
            scsi_mode_sense10();
            break;
        case SCSI_READ10:
            scsi_read10();
            break;
        case SCSI_WRITE10:
            scsi_write10();
            break;
        case SCSI_VERIFY10:
            scsi_verify10();
            break;
        default:
            status = SCSI_FAILED;
            key = SENSE_ILLEGALREQUEST;
            asc = ASC_INVALIDCOMMAND;
            break;
        }
        sensekey = key;
        senseasc = asc;
        
        csw.dCSWSignature = CSW_SIGNATURE;
        csw.dCSWTag = cbw.dCBWTag;
        csw.dCSWDataResidue = reverse4(residue);
        csw.bCSWStatus = status;
        
        scsi_in(NULL, 0);
    }
    else
    {
        usb_write_reg(INCSR1, INSDSTL);
        usb_write_reg(OUTCSR1, OUTSDSTL);
    }
}

void scsi_in(BYTE *pData, int nSize)
{
    BYTE cnt;
    
    if (pData == NULL)
    {
        pData = (BYTE *)&csw;
        nSize = sizeof(CSW);
    }
    else
    {
        if (nSize > residue)
        {
            nSize = residue;
        }
        residue -= nSize;
    }
    
    while (nSize)
    {
        cnt = EP1IN_SIZE;
        if (nSize < EP1IN_SIZE) cnt = nSize;
        
        while (usb_bulk_intr_in_busy());
        usb_bulk_intr_in(pData, cnt, 1);
        
        nSize -= cnt;
        pData += cnt;
    }
}

void scsi_out(BYTE *pData, int nSize)
{
    BYTE cnt;
    
    if (nSize > residue)
    {
        nSize = residue;
    }
    residue -= nSize;
    
    while (nSize)
    {
        while (!usb_bulk_intr_out_ready());
        cnt = usb_bulk_intr_out(pData, 1);
        
        nSize -= cnt;
        pData += cnt;
    }
}

BOOL scsi_test_unit_ready()
{
    return memory_check();
}

void scsi_start_stop_unit()
{
}

void scsi_media_removal()
{
}

void scsi_inquiry()
{
    static char code INQUIRY_DATA[36] =
    {
        0x00,                   //DeviceType(Direct-access device);
        0x80,                   //RemovableMedia(Removable);
        0x00,                   //Versions(ANSI X3.131:1994);
        0x00,                   //ResponseDataFormat(Standard);
        0x1f,                   //AdditionalLength(31);
        0x00,0x00,0x00,         //Reserved;
        'S','T','C',' ',' ',' ',' ',' ',
        'U','S','B','-','M','a','s','s','-','S','t','o','r','a','g','e',
        '1','.','0','0',
    };

    scsi_in(INQUIRY_DATA, sizeof(INQUIRY_DATA));
}

void scsi_read_capacity10()
{
    static char code CAPACITY_DATA[8] =
    {
        0x00,0x00,0x00,0x00,    //LastLogicalBlockAddress(LBA)
        0x00,0x00,0x00,0x00,    //BytesPerBlock(SECTORSIZE);
    };

    memcpy(scsibuf, CAPACITY_DATA, sizeof(CAPACITY_DATA));
    *(DWORD *)&scsibuf[0] = dwSectorNumber - 1;
    *(WORD *)&scsibuf[6] = wSectorSize;
    
    scsi_in(scsibuf, sizeof(CAPACITY_DATA));
}

void scsi_format_capacity()
{
    static char code FORMATCAPACITY_DATA[12] =
    {
        0x00,0x00,0x00,0x08,    //CapacityListLength(8)
        0x00,0x00,0x00,0x00,    //NumberOfBlocks(LBA)
        0x02,                   //DescriptorTypes(Formatted Media)
        0x00,0x00,0x00,         //BytesPerBlock(SECTORSIZE);
    };

    memcpy(scsibuf, FORMATCAPACITY_DATA, sizeof(FORMATCAPACITY_DATA));
    *(DWORD *)&scsibuf[4] = dwSectorNumber - 1;
    *(WORD *)&scsibuf[9] = wSectorSize;
    
    scsi_in(scsibuf, sizeof(FORMATCAPACITY_DATA));
}

void scsi_request_sense()
{
    static char code REQUESTSENSE_DATA[18] =
    {
        0x70,                   //ErrorCode(0x70);
        0x00,                   //Reserved;
        0x00,                   //SenseKey;
        0x00,0x00,0x00,0x00,    //Information;
        0x0a,                   //AdditionalSenseLength(10);
        0x00,0x00,0x00,0x00,    //Reserved;
        0x00,                   //ASC;
        0x00,                   //ASCQ;
        0x00,0x00,0x00,0x00,    //Reserved;
    };

    memcpy(scsibuf, REQUESTSENSE_DATA, sizeof(REQUESTSENSE_DATA));
    scsibuf[2] = sensekey;
    scsibuf[12] = senseasc;

    scsi_in(scsibuf, sizeof(REQUESTSENSE_DATA));
}

void scsi_mode_sense6()
{
    static char code MODESENSE6_DATA[4] =
    {
        0x03,0x00,0x00,0x00,
    };

    scsi_in(MODESENSE6_DATA, sizeof(MODESENSE6_DATA));
}

void scsi_mode_sense10()
{
    static char code MODESENSE10_DATA[8] =
    {
        0x00,0x06,0x00,0x80,
        0x00,0x00,0x00,0x00,
    };

    scsi_in(MODESENSE10_DATA, sizeof(MODESENSE10_DATA));
}

void scsi_read10()
{
    DWORD lba;
    
    lba = *(DWORD *)&cbw.ScsiParam[1];
    lba *= wSectorSize;

    while (residue)
    {
        memory_readsector(lba, scsibuf);
        scsi_in(scsibuf, wSectorSize);
        lba += wSectorSize;
    }
}

void scsi_write10()
{
    DWORD lba;
    
    lba = *(DWORD *)&cbw.ScsiParam[1];
    lba *= wSectorSize;

    while (residue)
    {
        scsi_out(scsibuf, wSectorSize);
        memory_writesector(lba, scsibuf);
        lba += wSectorSize;
    }
}

void scsi_verify10()
{
}


