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

/*************  ����˵��    **************

�����̻���STC32G����ת�Ӱ壨�����������б�д���ԡ�

CAN1(P5.0,P5.1)��CAN2(P5.2,P5.3)ͨ���շ������ӵ�ͬһ�������ϡ�

MCUÿ���Ӵ�CAN1��CAN2����һ֡���ݡ�

CAN1���͵ı��ı�CAN2���գ�CAN2���͵ı��ı�CAN1���ա�

�յ�һ����׼֡��, ��CAN֡��Ϣ������ͨ������1(P1.6,P1.7)��ӡ������

Ĭ�ϲ�����500KHz, �û��������޸ġ�

ע�⣺����CAN����ͨ�Ų���ǰ��Ҫ��R79��R80����Ͽ���

���������ʾ���ָ�λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
   (�����ϣ����λ����USB����ģʽ�Ļ������ڸ�λ�����ｫ IAP_CONTR ��bit6��0��ѡ��λ���û�������)
2. ͨ�����ء�stc_usb_hid_32.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�

����ʱ, Ĭ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "../comm/STC32G.h"  //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "../comm/usb.h"     //USB���Լ���λ����ͷ�ļ�
#include "stdio.h"
#include "intrins.h"

#define MAIN_Fosc        24000000UL

/****************************** �û������ ***********************************/
//CAN���߲�����=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
#define TSG1    2		//0~15
#define TSG2    1		//1~7 (TSG2 ��������Ϊ0)
#define BRP     3		//0~63
//24000000/((1+3+2)*4*2)=500KHz

#define SJW     0		//����ͬ����Ծ���

//���߲�����100KHz��������Ϊ 0; 100KHz��������Ϊ 1
#define SAM     0		//���ߵ�ƽ���������� 0:����1��; 1:����3��

/*****************************************************************************/


/*************  ���س�������    **************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��
#define Baudrate        115200UL
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))

#define	STANDARD_FRAME   0     //֡��ʽ����׼֡
#define	EXTENDED_FRAME   1     //֡��ʽ����չ֡

/*************  ���ر�������    **************/

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

//P3.2�ڰ�����λ�������
bit Key_Flag;
u16 Key_cnt;

typedef struct
{
	u8	DLC:4;          //���ݳ���, bit0~bit3
	u8	:2;             //������, bit4~bit5
	u8	RTR:1;          //֡����, bit6
	u8	FF:1;           //֡��ʽ, bit7
	u32	ID;             //CAN ID
	u8	DataBuffer[8];  //���ݻ���
}CAN_DataDef;

CAN_DataDef CAN1_Tx;
CAN_DataDef CAN1_Rx[8];

CAN_DataDef CAN2_Tx;
CAN_DataDef CAN2_Rx[8];

bit B_1ms;          //1ms��־
bit B_Can1Read;     //CAN �յ����ݱ�־
bit B_Can2Read;     //CAN �յ����ݱ�־
bit B_Can1Send;     //CAN �������ݱ�־
bit B_Can2Send;     //CAN �������ݱ�־
u16 msecond;

/*************  ���غ�������    **************/

void CANInit();
u8 CanReadReg(u8 addr);
u8 CanReadMsg(CAN_DataDef *CAN);
void CanSendMsg(CAN_DataDef *CAN);
void KeyResetScan(void);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
	P_SW1 = (P_SW1 & 0x3f) | 0x80;    //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;          //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1 = TM;
	TH1 = TM>>8;
	TR1 = 1;			//��ʱ��1��ʼ��ʱ
}

void UartPutc(unsigned char dat)
{
	SBUF = dat; 
	while(TI==0);
	TI = 0;
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/********************* ������ *************************/
void main(void)
{
    u8 i,j,n,sr;

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    RSTFLAG |= 0x04;   //����Ӳ����λ����Ҫ���P3.2��״̬ѡ���������򣬷���Ӳ����λ�����USB����ģʽ

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    usb_init();

    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    CANInit();
    UartInit();

    EUSB = 1;   //IE2��ص��ж�ʹ�ܺ���Ҫ��������EUSB
    EA = 1;     //�����ж�

	//====��ʼ������=====
	CAN1_Tx.FF = STANDARD_FRAME;    //��׼֡
	CAN1_Tx.RTR = 0;                //0������֡��1��Զ��֡
	CAN1_Tx.DLC = 0x08;             //���ݳ���
	CAN1_Tx.ID = 0x0567;            //CAN ID
	CAN1_Tx.DataBuffer[0] = 0x11;   //��������
	CAN1_Tx.DataBuffer[1] = 0x12;
	CAN1_Tx.DataBuffer[2] = 0x13;
	CAN1_Tx.DataBuffer[3] = 0x14;
	CAN1_Tx.DataBuffer[4] = 0x15;
	CAN1_Tx.DataBuffer[5] = 0x16;
	CAN1_Tx.DataBuffer[6] = 0x17;
	CAN1_Tx.DataBuffer[7] = 0x18;
	
	CAN2_Tx.FF = EXTENDED_FRAME;    //��չ֡
	CAN2_Tx.RTR = 0;                //0������֡��1��Զ��֡
	CAN2_Tx.DLC = 0x08;             //���ݳ���
	CAN2_Tx.ID = 0x03456789;        //CAN ID
	CAN2_Tx.DataBuffer[0] = 0x21;   //��������
	CAN2_Tx.DataBuffer[1] = 0x22;
	CAN2_Tx.DataBuffer[2] = 0x23;
	CAN2_Tx.DataBuffer[3] = 0x24;
	CAN2_Tx.DataBuffer[4] = 0x25;
	CAN2_Tx.DataBuffer[5] = 0x26;
	CAN2_Tx.DataBuffer[6] = 0x27;
	CAN2_Tx.DataBuffer[7] = 0x28;

    B_Can1Send = 0;
    B_Can2Send = 0;
    while(1)
    {
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            KeyResetScan();   //P3.2�ڰ������������λ������USB����ģʽ������Ҫ�˹��ܿ�ɾ�����д���
            
            if(++msecond >= 1000)   //1�뵽
            {
                msecond = 0;

                //------------------����CAN1ģ��-----------------------
                CANSEL = 0;         //ѡ��CAN1ģ��
                sr = CanReadReg(SR);

                if(sr & 0x01)		//�ж��Ƿ��� BS:BUS-OFF״̬
                {
                    CANAR = MR;
                    CANDR &= ~0x04;  //��� Reset Mode, ��BUS-OFF״̬�˳�
                }
                else
                {
                    CanSendMsg(&CAN1_Tx);   //����һ֡����
                }

                //------------------����CAN2ģ��-----------------------
                CANSEL = 1;         //ѡ��CAN2ģ��
                sr = CanReadReg(SR);

                if(sr & 0x01)		//�ж��Ƿ��� BS:BUS-OFF״̬
                {
                    CANAR = MR;
                    CANDR &= ~0x04;  //��� Reset Mode, ��BUS-OFF״̬�˳�
                }
                else
                {
                    CanSendMsg(&CAN2_Tx);   //����һ֡����
                }
            }
        }

        if(B_Can1Read)
        {
            B_Can1Read = 0;
            
            CANSEL = 0;         //ѡ��CAN1ģ��
            n = CanReadMsg(CAN1_Rx);    //��ȡ��������
            if(n>0)
            {
                for(i=0;i<n;i++)
                {
//                    CanSendMsg(&CAN1_Rx[i]);  //CAN����ԭ������
                    printf("CAN1:ID=0x%08lX DLC=%d FF=%d RTR=%d ",CAN1_Rx[i].ID,CAN1_Rx[i].DLC,CAN1_Rx[i].FF,CAN1_Rx[i].RTR);   //���ڴ�ӡ֡��Ϣ
                    for(j=0;j<CAN1_Rx[i].DLC;j++)
                    {
                        printf("0x%02X ",CAN1_Rx[i].DataBuffer[j]);    //�Ӵ�������յ�������
                    }
                    printf("\r\n");
                }
            }
        }

        if(B_Can2Read)
        {
            B_Can2Read = 0;
            
            CANSEL = 1;         //ѡ��CAN2ģ��
            n = CanReadMsg(CAN2_Rx);    //��ȡ��������
            if(n>0)
            {
                for(i=0;i<n;i++)
                {
//                    CanSendMsg(&CAN2_Rx[i]);  //CAN����ԭ������
                    printf("CAN2:ID=0x%08lX DLC=%d FF=%d RTR=%d ",CAN2_Rx[i].ID,CAN2_Rx[i].DLC,CAN2_Rx[i].FF,CAN2_Rx[i].RTR);   //���ڴ�ӡ֡��Ϣ
                    for(j=0;j<CAN2_Rx[i].DLC;j++)
                    {
                        printf("0x%02X ",CAN2_Rx[i].DataBuffer[j]);    //�Ӵ�������յ�������
                    }
                    printf("\r\n");
                }
            }
        }

        if (bUsbOutReady) //USB���Լ���λ�������
        {
            usb_OUT_done();
        }
    }
}


/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}

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
	u8 dat;
	CANAR = addr;
	dat = CANDR;
	return dat;
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
// ����: void CanReadFifo(CAN_DataDef *CANx)
// ����: ��ȡCAN���������ݺ�����
// ����: *CANx: ���CAN���߶�ȡ����.
// ����: none.
// �汾: VER2.0
// ����: 2023-01-31
// ��ע: 
//========================================================================
void CanReadFifo(CAN_DataDef *CAN)
{
    u8 i;
    u8 pdat[5];
    u8 RX_Index=0;

    pdat[0] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));

    if(pdat[0] & 0x80)  //�ж��Ǳ�׼֡������չ֡
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //��չ֡IDռ4���ֽ�
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[3] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[4] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = (((u32)pdat[1] << 24) + ((u32)pdat[2] << 16) + ((u32)pdat[3] << 8) + pdat[4]) >> 3;
    }
    else
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //��׼֡IDռ2���ֽ�
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = ((pdat[1] << 8) + pdat[2]) >> 5;
    }
    
    CAN->FF = pdat[0] >> 7;     //֡��ʽ
    CAN->RTR = pdat[0] >> 6;    //֡����
    CAN->DLC = pdat[0];         //���ݳ���

    for(i=0;((i<CAN->DLC) && (i<8));i++)        //��ȡ���ݳ���Ϊlen����಻����8
    {
        CAN->DataBuffer[i] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //��ȡ��Ч����
    }
    while(RX_Index&3)   //�ж��Ѷ����ݳ����Ƿ�4��������
    {
        CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));  //��ȡ������ݣ�һ֡����ռ��4���������������ռ䣬���㲹0
    }
}

//========================================================================
// ����: u8 CanReadMsg(void)
// ����: CAN�������ݺ�����
// ����: *CANx: ���CAN���߶�ȡ����.
// ����: ֡����.
// �汾: VER2.0
// ����: 2023-01-31
// ��ע: 
//========================================================================
u8 CanReadMsg(CAN_DataDef *CAN)
{
    u8 i;
    u8 n=0;

    do{
        CanReadFifo(&CAN[n++]);  //��ȡ���ջ���������
        i = CanReadReg(SR);
    }while(i&0x80);     //�жϽ��ջ��������Ƿ������ݣ��еĻ�������ȡ

    return n;   //����֡����
}

//========================================================================
// ����: void CanSendMsg(CAN_DataDef *CAN)
// ����: CAN���ͱ�׼֡������
// ����: *CANx: ���CAN���߷�������..
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CanSendMsg(CAN_DataDef *CAN)
{
    u32 CanID;
    u8 RX_Index,i;

    if(CANSEL)  //�ж��Ƿ�CAN2
    {
        i = 200;
        while((--i) && (B_Can2Send));  //�ȴ�CAN2�ϴη������
    }
    else
    {
        i = 200;
        while((--i) && (B_Can1Send));  //�ȴ�CAN1�ϴη������
    }

    if(CAN->FF)     //�ж��Ƿ���չ֡
    {
        CanID = CAN->ID << 3;
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6)|0x80);	//bit7: ��׼֡(0)/��չ֡(1), bit6: ����֡(0)/Զ��֡(1), bit3~bit0: ���ݳ���(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>24));
        CanWriteReg(TX_BUF2,(u8)(CanID>>16));
        CanWriteReg(TX_BUF3,(u8)(CanID>>8));

        CanWriteReg(TX_BUF0,(u8)CanID);

        RX_Index = 1;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //���ݳ���ΪDLC����಻����8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //д����Ч����
        }
        while(RX_Index&3)   //�ж��Ѷ����ݳ����Ƿ�4��������
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //д��������ݣ�һ֡����ռ��4���������������ռ䣬���㲹0
        }
    }
    else    //���ͱ�׼֡
    {
        CanID = (u16)(CAN->ID << 5);
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6));  //bit7: ��׼֡(0)/��չ֡(1), bit6: ����֡(0)/Զ��֡(1), bit3~bit0: ���ݳ���(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>8));
        CanWriteReg(TX_BUF2,(u8)CanID);

        RX_Index = 3;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //���ݳ���ΪDLC����಻����8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //д����Ч����
        }
        while(RX_Index&3)   //�ж��Ѷ����ݳ����Ƿ�4��������
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //д��������ݣ�һ֡����ռ��4���������������ռ䣬���㲹0
        }
    }
	CanWriteReg(CMR ,0x04);		//����һ��֡����
    
    if(CANSEL)  //�ж��Ƿ�CAN2
    {
        B_Can2Send = 1;     //����CAN2����æ��־
    }
    else
    {
        B_Can1Send = 1;     //����CAN1����æ��־
    }
}

//========================================================================
// ����: void CANSetBaudrate()
// ����: CAN���߲��������ú�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CANSetBaudrate()
{
	CanWriteReg(BTR0,(SJW << 6) + BRP);
	CanWriteReg(BTR1,(SAM << 7) + (TSG2 << 4) + TSG1);
}

//========================================================================
// ����: void CANInit()
// ����: CAN��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CANInit()
{
	//-------- CAN1 --------
	CANEN = 1;          //CAN1ģ��ʹ��
	CANSEL = 0;         //ѡ��CAN1ģ��
	P_SW1 = (P_SW1 & ~(3<<4)) | (1<<4); //�˿��л�(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1

	CanWriteReg(MR  ,0x04);		//ʹ�� Reset Mode
	CANSetBaudrate();	//���ò�����
	
	CanWriteReg(ACR0,0x00);		//�������մ���Ĵ���
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);		//�����������μĴ���
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);

	CanWriteReg(IMR ,0xff);		//�жϼĴ���
	CanWriteReg(ISR ,0xff);		//���жϱ�־
	CanWriteReg(MR  ,0x00);		//�˳� Reset Mode
	CANICR = 0x02;				//CAN�ж�ʹ��

	//-------- CAN2 --------
	CAN2EN = 1;         //CAN2ģ��ʹ��
	CANSEL = 1;         //ѡ��CAN2ģ��
	P_SW3 = (P_SW3 & ~(3)) | (1);       //�˿��л�(CAN_Rx,CAN_Tx) 0x00:P0.2,P0.3  0x01:P5.2,P5.3  0x02:P4.6,P4.7  0x03:P7.2,P7.3

	CanWriteReg(MR  ,0x04);		//ʹ�� Reset Mode
	CANSetBaudrate();					//���ò�����

	CanWriteReg(ACR0,0x00);		//�������մ���Ĵ���
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);		//�����������μĴ���
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);

	CanWriteReg(IMR ,0xff);		//�жϼĴ���
	CanWriteReg(ISR ,0xff);		//���жϱ�־
	CanWriteReg(MR  ,0x00);		//�˳� Reset Mode
	CANICR |= 0x20;				//CAN2�ж�ʹ��
}

//========================================================================
// ����: void CANBUS1_Interrupt(void) interrupt CAN1_VECTOR
// ����: CAN�����жϺ�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-03-24
// ��ע: 
//========================================================================
void CANBUS1_Interrupt(void) interrupt CAN1_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;     //��CANAR�ֳ����棬������ѭ����д�� CANAR ������жϣ����ж����޸��� CANAR ����
	store = AUXR2;      //��AUXR2�ֳ�����
	
	AUXR2 &= ~0x08;		//ѡ��CAN1ģ��
	isr = CanReadReg(ISR);

	if((isr & 0x04) == 0x04)  //TI
	{
		CANAR = ISR;
		CANDR = 0x04;    //CLR FLAG
		
		B_Can1Send = 0;
	}	
	if((isr & 0x08) == 0x08)  //RI
	{
		CANAR = ISR;
		CANDR = 0x08;    //CLR FLAG
	
		B_Can1Read = 1;
	}

	if((isr & 0x40) == 0x40)  //ALI
	{
		CANAR = ISR;
		CANDR = 0x40;    //CLR FLAG
	}	

	if((isr & 0x20) == 0x20)  //EWI
	{
		CANAR = MR;
		CANDR &= ~0x04;  //��� Reset Mode, ��BUS-OFF״̬�˳�
		
		CANAR = ISR;
		CANDR = 0x20;    //CLR FLAG
	}	

	if((isr & 0x10) == 0x10)  //EPI
	{
		CANAR = ISR;
		CANDR = 0x10;    //CLR FLAG
	}	

	if((isr & 0x02) == 0x02)  //BEI
	{
		CANAR = ISR;
		CANDR = 0x02;    //CLR FLAG
	}	

	if((isr & 0x01) == 0x01)  //DOI
	{
		CANAR = ISR;
		CANDR = 0x01;    //CLR FLAG
	}	

	AUXR2 = store;     //��AUXR2�ֳ��ָ�
	CANAR = arTemp;    //��CANAR�ֳ��ָ�
}

//========================================================================
// ����: void CANBUS2_Interrupt(void) interrupt CAN2_VECTOR
// ����: CAN�����жϺ�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CANBUS2_Interrupt(void) interrupt CAN2_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;     //��CANAR�ֳ����棬������ѭ����д�� CANAR ������жϣ����ж����޸��� CANAR ����
	store = AUXR2;      //��AUXR2�ֳ�����
	
	AUXR2 |= 0x08;		//ѡ��CAN2ģ��
	isr = CanReadReg(ISR);

	if((isr & 0x04) == 0x04)  //TI
	{
		CANAR = ISR;
		CANDR = 0x04;    //CLR FLAG
		
		B_Can2Send = 0;
	}	
	if((isr & 0x08) == 0x08)  //RI
	{
		CANAR = ISR;
		CANDR = 0x08;    //CLR FLAG

		B_Can2Read = 1;
	}

	if((isr & 0x40) == 0x40)  //ALI
	{
		CANAR = ISR;
		CANDR = 0x40;    //CLR FLAG
	}	

	if((isr & 0x20) == 0x20)  //EWI
	{
		CANAR = MR;
		CANDR &= ~0x04;  //��� Reset Mode, ��BUS-OFF״̬�˳�
		
		CANAR = ISR;
		CANDR = 0x20;    //CLR FLAG
	}	

	if((isr & 0x10) == 0x10)  //EPI
	{
		CANAR = ISR;
		CANDR = 0x10;    //CLR FLAG
	}	

	if((isr & 0x02) == 0x02)  //BEI
	{
		CANAR = ISR;
		CANDR = 0x02;    //CLR FLAG
	}	

	if((isr & 0x01) == 0x01)  //DOI
	{
		CANAR = ISR;
		CANDR = 0x01;    //CLR FLAG
	}	

	AUXR2 = store;     //��AUXR2�ֳ��ָ�
	CANAR = arTemp;    //��CANAR�ֳ��ָ�
}

//========================================================================
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
	u16 i;
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i);
	}while(--ms);
}

//========================================================================
// ����: void KeyResetScan(void)
// ����: P3.2�ڰ�������1�봥�������λ������USB����ģʽ��
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//����1000ms��Ч�������
            {
                Key_Flag = 1;		//���ð���״̬����ֹ�ظ�����

                USBCON = 0x00;      //���USB����
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //���������λ����ISP��ʼִ��
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
