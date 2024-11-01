/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include "APP_CAN.h"
#include "STC32G_CAN.h"
#include "STC32G_GPIO.h"
#include "STC32G_NVIC.h"
#include "STC32G_Switch.h"

#include "canfestival.h"
#include "objdict.h"

//========================================================================
//                               ���س�������	
//========================================================================

#define	SetExtendedFrame	0    //0: ��׼֡ͨ��; 1: ��չ֡ͨ��

#define NODEID 0x0F

//========================================================================
//                               ���ر�������
//========================================================================

Message msg;
Message Can1Msg;

bit Key_Flag;
u16 Key_cnt;

//========================================================================
//                               ���غ�������
//========================================================================

void KeyResetScan(void);

//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================

extern bit B_Can1Read;     //CAN �յ����ݱ�־
extern bit B_Can2Read;     //CAN �յ����ݱ�־
extern bit B_Can1Send;     //CAN �������ݱ�־
extern bit B_Can2Send;     //CAN �������ݱ�־

extern u16 msecond;

/********************** �ȴ����� **********************/
void WaitCan1Send(u8 i)
{
	 while((--i) && (B_Can1Send));
}

void WaitCan2Send(u8 i)
{
	 while((--i) && (B_Can2Send));
}

//========================================================================
// ����: CAN_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void CAN_init(void)
{
	CAN_InitTypeDef	CAN_InitStructure;					//�ṹ����

	CAN_InitStructure.CAN_Enable = ENABLE;		//CAN����ʹ��   ENABLE��DISABLE
	CAN_InitStructure.CAN_IMR    = CAN_ALLIM;	//CAN�жϼĴ��� 	CAN_DOIM,CAN_BEIM,CAN_TIM,CAN_RIM,CAN_EPIM,CAN_EWIM,CAN_ALIM,CAN_ALLIM,DISABLE
	CAN_InitStructure.CAN_SJW    = 0;					//����ͬ����Ծ���  0~3
	CAN_InitStructure.CAN_SAM    = 0;					//���ߵ�ƽ��������  0:����1��; 1:����3��

	//CAN���߲�����=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
	CAN_InitStructure.CAN_TSG1   = 2;					//ͬ��������1       0~15
	CAN_InitStructure.CAN_TSG2   = 1;					//ͬ��������2       1~7 (TSG2 ��������Ϊ0)
	CAN_InitStructure.CAN_BRP    = 3;					//�����ʷ�Ƶϵ��    0~63
	//24000000/((1+3+2)*4*2)=500KHz

	CAN_InitStructure.CAN_ACR0    = 0x00;			//�������մ���Ĵ��� 0~0xFF
	CAN_InitStructure.CAN_ACR1    = 0x00;
	CAN_InitStructure.CAN_ACR2    = 0x00;
	CAN_InitStructure.CAN_ACR3    = 0x00;
	CAN_InitStructure.CAN_AMR0    = 0xff;			//�����������μĴ��� 0~0xFF
	CAN_InitStructure.CAN_AMR1    = 0xff;
	CAN_InitStructure.CAN_AMR2    = 0xff;
	CAN_InitStructure.CAN_AMR3    = 0xff;
	CAN_Inilize(CAN1,&CAN_InitStructure);			//CAN1 ��ʼ��
	CAN_Inilize(CAN2,&CAN_InitStructure);			//CAN2 ��ʼ��
	
	NVIC_CAN_Init(CAN1,ENABLE,Priority_1);		//�ж�ʹ��, CAN1/CAN2; ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	NVIC_CAN_Init(CAN2,ENABLE,Priority_1);		//�ж�ʹ��, CAN1/CAN2; ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	
	P5_MODE_IO_PU(GPIO_Pin_LOW);		//P5.0~P5.3 ����Ϊ׼˫���

	CAN1_SW(CAN1_P50_P51);				//CAN1_P00_P01,CAN1_P50_P51,CAN1_P42_P45,CAN1_P70_P71
	CAN2_SW(CAN2_P52_P53);				//CAN2_P02_P03,CAN2_P52_P53,CAN2_P46_P47,CAN2_P72_P73

	setNodeId(&CAN1_ObjDict_Data,0);
	setState(&CAN1_ObjDict_Data,Initialisation);
	setNodeId(&CAN2_ObjDict_Data,NODEID);
	setState(&CAN2_ObjDict_Data,Initialisation);

	Can1Msg.cob_id =  0;  //ID=0
	Can1Msg.rtr    =  0;  //����֡
	Can1Msg.len    =  2;  //���ݳ���
    Can1Msg.m_data[0] = 0x01;
    Can1Msg.m_data[1] = 0x0F;
}

//========================================================================
// ����: Sample_CAN
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void Sample_CAN(void)
{
    u8 i;

    KeyResetScan();

	if(B_Can1Read)
	{
		B_Can1Read = 0;
		
		CANSEL = CAN1;		//ѡ��CAN1ģ��

        canReceive(&msg);
        canDispatch(&CAN1_ObjDict_Data,&msg);

        printf("CAN1 Read: ID=0x%04X  ",msg.cob_id);
        for(i=0; i<msg.len; i++)    printf("0x%02X ",msg.m_data[i]);    //�Ӵ�������յ�������
        printf("\r\n");
	}

	if(B_Can2Read)
	{
		B_Can2Read = 0;
		
		CANSEL = CAN2;		//ѡ��CAN2ģ��

        canReceive(&msg);
        canDispatch(&CAN2_ObjDict_Data,&msg);

        printf("CAN2 Read: ID=0x%04X  ",msg.cob_id);
        for(i=0; i<msg.len; i++)    printf("0x%02X ",msg.m_data[i]);    //�Ӵ�������յ�������
        printf("\r\n");
	}
}

void KeyResetScan(void)
{
    if(!P54)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 50)		//����50ms��Ч�������
            {
                Key_Flag = 1;		//���ð���״̬����ֹ�ظ�����
                printf("CAN1 Send\r\n");
                
                CANSEL = CAN1;        //ѡ��CAN1ģ��
                canSend(0,&Can1Msg);

                Can1Msg.cob_id =  0x20F;  //ID=0
                Can1Msg.rtr    =  0;  //����֡
                Can1Msg.len    =  1;  //���ݳ���
                Can1Msg.m_data[0]++;
                Can1Msg.m_data[1] = 0x00;
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}
