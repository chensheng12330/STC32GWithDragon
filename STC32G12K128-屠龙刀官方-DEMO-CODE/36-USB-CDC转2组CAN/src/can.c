/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "can.h"
#include "usb.h"
#include "util.h"

BYTE Can1RxWptr;
BYTE Can1RxRptr;
BYTE Can2RxWptr;
BYTE Can2RxRptr;

BYTE xdata Can1RxBuffer[16][16];
BYTE xdata Can2RxBuffer[16][16];

//========================================================================
// ����: u8 ReadReg(u8 addr)
// ����: CAN���ܼĴ�����ȡ������
// ����: CAN���ܼĴ�����ַ.
// ����: CAN���ܼĴ�������.
// �汾: VER1.0
// ����: 2020-11-16
// ��ע: 
//========================================================================
u8 CanReadReg(u8 addr)
{
    CANAR = addr;
    return CANDR;
}

//========================================================================
// ����: void WriteReg(u8 addr, u8 dat)
// ����: CAN���ܼĴ������ú�����
// ����: CAN���ܼĴ�����ַ, CAN���ܼĴ�������.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-16
// ��ע: 
//========================================================================
void CanWriteReg(u8 addr, u8 dat)
{
    CANAR = addr;
    CANDR = dat;
}

//========================================================================
// ����: void CanInit()
// ����: CAN��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2023-05-30
// ��ע: 
//========================================================================
void CanInit()
{
	//-------- CAN1 --------
	P_SW1 = (P_SW1 & ~(3<<4)) | (1<<4); //�˿��л�(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1
    CANSEL = CAN1;                  //ѡ��CAN1ģ��
    CANEN = 1;                      //ʹ��CAN1ģ��
    
    CanWriteReg(MR ,0x04);          //ʹ�� Reset Mode
    CanWriteReg(ACR0, 0x00);        //�������մ���Ĵ���
    CanWriteReg(ACR1, 0x00);
    CanWriteReg(ACR2, 0x00);
    CanWriteReg(ACR3, 0x00);
    CanWriteReg(AMR0, 0xff);        //�����������μĴ���
    CanWriteReg(AMR1, 0xff);
    CanWriteReg(AMR2, 0xff);
    CanWriteReg(AMR3, 0xff);
    CanWriteReg(ISR, 0xff);         //���жϱ�־
    CanWriteReg(IMR, 0x7f);         //ʹ��ȫ���ж�
    CanWriteReg(MR, 0x00);          //�˳� Reset Mode
	CANICR = 0x02;                  //CAN�ж�ʹ��
    CanSetBaudrate(500);

	//-------- CAN2 --------
	P_SW3 = (P_SW3 & ~(3)) | (1);   //�˿��л�(CAN_Rx,CAN_Tx) 0x00:P0.2,P0.3  0x01:P5.2,P5.3  0x02:P4.6,P4.7  0x03:P7.2,P7.3
	CANSEL = CAN2;                  //ѡ��CAN2ģ��
	CAN2EN = 1;                     //CAN2ģ��ʹ��

	CanWriteReg(MR  ,0x04);         //ʹ�� Reset Mode
	CanWriteReg(ACR0,0x00);         //�������մ���Ĵ���
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);	        //�����������μĴ���
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);
	CanWriteReg(ISR ,0xff);         //���жϱ�־
	CanWriteReg(IMR ,0x7f);         //�жϼĴ���
	CanWriteReg(MR  ,0x00);         //�˳� Reset Mode
	CANICR |= 0x20;                 //CAN2�ж�ʹ��
    CanSetBaudrate(500);

    Can1RxWptr = 0;
    Can1RxRptr = 0;
    Can2RxWptr = 0;
    Can2RxRptr = 0;
}

//========================================================================
// ����: void CanSetBaudrate(u16 brt)
// ����: CAN���߲��������ú�����
// ����: brt: ������(��λ��K).
// ����: none.
// �汾: VER1.0
// ����: 2023-5-24
// ��ע: 
//========================================================================
void CanSetBaudrate(u16 brt)
{
    switch(brt)
    {
        case 20:    brt = CANBRT_20K;   break;
        case 40:    brt = CANBRT_40K;   break;
        case 50:    brt = CANBRT_50K;   break;
        case 80:    brt = CANBRT_80K;   break;
        case 100:   brt = CANBRT_100K;  break;
        case 125:   brt = CANBRT_125K;  break;
        case 200:   brt = CANBRT_200K;  break;
        case 250:   brt = CANBRT_250K;  break;
        case 400:   brt = CANBRT_400K;  break;
        case 800:   brt = CANBRT_800K;  break;
        case 1000:  brt = CANBRT_1000K; break;
        default:
        case 500:   brt = CANBRT_500K;  break;
    }
    
    CanWriteReg(MR, 0x04);          //ʹ�� Reset Mode
    CanWriteReg(BTR0, (u8)brt);
    CanWriteReg(BTR1, (u8)(brt >> 8));
    CanWriteReg(MR, 0x00);          //�˳� Reset Mode
}

//========================================================================
// ����: void Can1ReadMsg(void)
// ����: CAN1�������ݺ�����
// ����: none.
// ����: none.
// �汾: VER2.0
// ����: 2023-5-24
// ��ע: 
//========================================================================
void Can1ReadMsg()
{
    while (CanReadReg(SR) & 0x80)   //�жϽ��ջ��������Ƿ�������
    {
        u8 *pdat;
        u8 start;
        u8 remain;
        u8 i;
    
        if ((((Can1RxWptr + 1) ^ Can1RxRptr) & 0x0f) == 0)
            break;                  //����������,��ͣ��ȡCAN����

        pdat = &Can1RxBuffer[Can1RxWptr++ & 0x0f][2];
        i = 0;
        *pdat++ = start = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
        remain = (start & 0x0f) + (start & 0x80 ? 4 : 2);
    
        while (remain--)            //��ȡ��Ч����
            *pdat++ = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
    
        remain = 13 - i;            //һ֡���13���ֽڣ����㲹0
        while (remain--)
            *pdat++ = 0;                                    
        
        while (i & 3)               //�ж��Ѷ����ݳ����Ƿ�4��������
            CanReadReg((u8)(RX_BUF0 + (i++ & 3)));          
    }
}

//========================================================================
// ����: void Can2ReadMsg(void)
// ����: CAN2�������ݺ�����
// ����: none.
// ����: none.
// �汾: VER2.0
// ����: 2023-5-24
// ��ע: 
//========================================================================
void Can2ReadMsg()
{
    while (CanReadReg(SR) & 0x80)   //�жϽ��ջ��������Ƿ�������
    {
        u8 *pdat;
        u8 start;
        u8 remain;
        u8 i;
    
        if ((((Can2RxWptr + 1) ^ Can2RxRptr) & 0x0f) == 0)
            break;                  //����������,��ͣ��ȡCAN����

        pdat = &Can2RxBuffer[Can2RxWptr++ & 0x0f][2];
        i = 0;
        *pdat++ = start = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
        remain = (start & 0x0f) + (start & 0x80 ? 4 : 2);
    
        while (remain--)            //��ȡ��Ч����
            *pdat++ = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
    
        remain = 13 - i;            //һ֡���13���ֽڣ����㲹0
        while (remain--)
            *pdat++ = 0;                                    
        
        while (i & 3)               //�ж��Ѷ����ݳ����Ƿ�4��������
            CanReadReg((u8)(RX_BUF0 + (i++ & 3)));          
    }
}

//========================================================================
// ����: void CanSendMsg(u8 *CAN)
// ����: CAN���ͱ�׼֡������
// ����: *CAN: ���CAN���߷�������.
// ����: none.
// �汾: VER1.0
// ����: 2023-5-24
// ��ע: 
//========================================================================
void CanSendMsg(u8 *CAN)
{
    u8 start;
    u8 remain;
    u8 i;

    i = 0;
    start = CAN[0];                 //��ʼ�ֽ�
    remain = (start & 0x0f) + (start & 0x80 ? 4 : 2) + 1;   

    while (remain--)                //д����Ч����
        CanWriteReg((u8)(TX_BUF0 + (i++ & 3)), CAN[i-1]);     

    while (i & 3)                   //д��������ݣ�һ֡����ռ��4���������������ռ䣬���㲹0
        CanWriteReg((u8)(TX_BUF0 + (i++ & 3)), 0);          
    
	CanWriteReg(CMR, 0x04);	        //����һ��֡����
}

void CanPolling()
{
    u8 *pdat;
    u8 sum;
    u8 i;

    if (DeviceState != DEVSTATE_CONFIGURED)
        return;

    //�жϻ�������Ƿ��д�������֡
    if ((Can1RxRptr ^ Can1RxWptr) & 0x0f)
    {
        //��ȡ�����׵�ַ
        pdat = Can1RxBuffer[Can1RxRptr++ & 0x0f];
        sum = pdat[0] = CAN1;    //����ͨ����
        pdat[1] = CANDATA;    //�������ݰ�ͷ
        for (i = 1; i < 15; i++)
            sum += pdat[i];
        pdat[15] = (u8)(256 - sum); //����У���
        
        CDC1_SendData(pdat, 16);
    }

    //�жϻ�������Ƿ��д�������֡
    if ((Can2RxRptr ^ Can2RxWptr) & 0x0f)
    {
        //��ȡ�����׵�ַ
        pdat = Can2RxBuffer[Can2RxRptr++ & 0x0f];
        sum = pdat[0] = CAN2;    //����ͨ����
        pdat[1] = CANDATA;    //�������ݰ�ͷ
        for (i = 1; i < 15; i++)
            sum += pdat[i];
        pdat[15] = (u8)(256 - sum); //����У���
        
        CDC2_SendData(pdat, 16);
    }

    if (bCDC1OutReady)
    {
        CANSEL = CAN1;		//ѡ��CAN1ģ��
        if(CDC1OutBuffer[1] == CANDATA)     //��������
        {
            CanSendMsg(&CDC1OutBuffer[2]);   //����һ֡����
        }
        else if(CDC1OutBuffer[1] == CANBAUD)   //���ò�����
        {
            CanSetBaudrate(((u16)CDC1OutBuffer[5]<<8)+CDC1OutBuffer[6]);
        }

        CDC1_OUT_done();    //����Ӧ�𣨹̶���ʽ����������λ�������Ѿ�������ɣ����Խ�����һ�����ݰ���
    }
    
    if (bCDC2OutReady)
    {
        CANSEL = CAN2;		//ѡ��CAN2ģ��
        if(CDC2OutBuffer[1] == CANDATA)     //��������
        {
            CanSendMsg(&CDC2OutBuffer[2]);   //����һ֡����
        }
        else if(CDC2OutBuffer[1] == CANBAUD)   //���ò�����
        {
            CanSetBaudrate(((u16)CDC2OutBuffer[5]<<8)+CDC2OutBuffer[6]);
        }

        CDC2_OUT_done();    //����Ӧ�𣨹̶���ʽ����������λ�������Ѿ�������ɣ����Խ�����һ�����ݰ���
    }
}

void CAN1_ISR_Handler (void) interrupt CAN1_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;                 //��CANAR�ֳ����棬������ѭ����д�� CANAR ������жϣ����ж����޸��� CANAR ����
	store = AUXR2;                  //��AUXR2�ֳ�����
	
	AUXR2 &= ~0x08;		            //ѡ��CAN1ģ��
	isr = CanReadReg(ISR);

	if (isr & 0x04)                 //TI
	{
		CANAR = ISR;
		CANDR = 0x04;
    }	
	if (isr & 0x08)                 //RI
	{
		CANAR = ISR;
		CANDR = 0x08;
	
		Can1ReadMsg();
	}

	if((isr & 0x40) == 0x40)        //ALI
	{
		CANAR = ISR;
		CANDR = 0x40;
	}	

	if((isr & 0x10) == 0x10)        //EPI
	{
		CANAR = ISR;
		CANDR = 0x10;           
	}	

	if((isr & 0x02) == 0x02)        //BEI
	{
		CANAR = ISR;
		CANDR = 0x02;    
	}	

	if((isr & 0x01) == 0x01)        //DOI
	{
		CANAR = ISR;
		CANDR = 0x01;    
	}	

	if((isr & 0x20) == 0x20)        //EWI
	{
		CANAR = MR;
		CANDR &= ~0x04;             //��� Reset Mode, ��BUS-OFF״̬�˳�
		
		CANAR = ISR;
		CANDR = 0x20;               //CLR FLAG
	}	

	AUXR2 = store;                  //��AUXR2�ֳ��ָ�
	CANAR = arTemp;                 //��CANAR�ֳ��ָ�
}

void CAN2_ISR_Handler (void) interrupt CAN2_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;                 //��CANAR�ֳ����棬������ѭ����д�� CANAR ������жϣ����ж����޸��� CANAR ����
	store = AUXR2;                  //��AUXR2�ֳ�����
	
	AUXR2 |= 0x08;                  //ѡ��CAN2ģ��
	isr = CanReadReg(ISR);

	if (isr & 0x04)                 //TI
	{
		CANAR = ISR;
		CANDR = 0x04;
    }	
	if (isr & 0x08)                 //RI
	{
		CANAR = ISR;
		CANDR = 0x08;
	
		Can2ReadMsg();
	}

	if((isr & 0x40) == 0x40)        //ALI
	{
		CANAR = ISR;
		CANDR = 0x40;
	}	

	if((isr & 0x10) == 0x10)        //EPI
	{
		CANAR = ISR;
		CANDR = 0x10;           
	}	

	if((isr & 0x02) == 0x02)        //BEI
	{
		CANAR = ISR;
		CANDR = 0x02;    
	}	

	if((isr & 0x01) == 0x01)        //DOI
	{
		CANAR = ISR;
		CANDR = 0x01;    
	}	

	if((isr & 0x20) == 0x20)        //EWI
	{
		CANAR = MR;
		CANDR &= ~0x04;             //��� Reset Mode, ��BUS-OFF״̬�˳�
		
		CANAR = ISR;
		CANDR = 0x20;               //CLR FLAG
	}	

	AUXR2 = store;                  //��AUXR2�ֳ��ָ�
	CANAR = arTemp;                 //��CANAR�ֳ��ָ�
}
