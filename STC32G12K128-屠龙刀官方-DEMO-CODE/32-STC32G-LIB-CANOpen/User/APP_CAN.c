/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "APP_CAN.h"
#include "STC32G_CAN.h"
#include "STC32G_GPIO.h"
#include "STC32G_NVIC.h"
#include "STC32G_Switch.h"

#include "canfestival.h"
#include "objdict.h"

//========================================================================
//                               本地常量声明	
//========================================================================

#define	SetExtendedFrame	0    //0: 标准帧通信; 1: 扩展帧通信

#define NODEID 0x0F

//========================================================================
//                               本地变量声明
//========================================================================

Message msg;
Message Can1Msg;

bit Key_Flag;
u16 Key_cnt;

//========================================================================
//                               本地函数声明
//========================================================================

void KeyResetScan(void);

//========================================================================
//                            外部函数和变量声明
//========================================================================

extern bit B_Can1Read;     //CAN 收到数据标志
extern bit B_Can2Read;     //CAN 收到数据标志
extern bit B_Can1Send;     //CAN 发生数据标志
extern bit B_Can2Send;     //CAN 发生数据标志

extern u16 msecond;

/********************** 等待函数 **********************/
void WaitCan1Send(u8 i)
{
	 while((--i) && (B_Can1Send));
}

void WaitCan2Send(u8 i)
{
	 while((--i) && (B_Can2Send));
}

//========================================================================
// 函数: CAN_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2022-03-27
//========================================================================
void CAN_init(void)
{
	CAN_InitTypeDef	CAN_InitStructure;					//结构定义

	CAN_InitStructure.CAN_Enable = ENABLE;		//CAN功能使能   ENABLE或DISABLE
	CAN_InitStructure.CAN_IMR    = CAN_ALLIM;	//CAN中断寄存器 	CAN_DOIM,CAN_BEIM,CAN_TIM,CAN_RIM,CAN_EPIM,CAN_EWIM,CAN_ALIM,CAN_ALLIM,DISABLE
	CAN_InitStructure.CAN_SJW    = 0;					//重新同步跳跃宽度  0~3
	CAN_InitStructure.CAN_SAM    = 0;					//总线电平采样次数  0:采样1次; 1:采样3次

	//CAN总线波特率=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
	CAN_InitStructure.CAN_TSG1   = 2;					//同步采样段1       0~15
	CAN_InitStructure.CAN_TSG2   = 1;					//同步采样段2       1~7 (TSG2 不能设置为0)
	CAN_InitStructure.CAN_BRP    = 3;					//波特率分频系数    0~63
	//24000000/((1+3+2)*4*2)=500KHz

	CAN_InitStructure.CAN_ACR0    = 0x00;			//总线验收代码寄存器 0~0xFF
	CAN_InitStructure.CAN_ACR1    = 0x00;
	CAN_InitStructure.CAN_ACR2    = 0x00;
	CAN_InitStructure.CAN_ACR3    = 0x00;
	CAN_InitStructure.CAN_AMR0    = 0xff;			//总线验收屏蔽寄存器 0~0xFF
	CAN_InitStructure.CAN_AMR1    = 0xff;
	CAN_InitStructure.CAN_AMR2    = 0xff;
	CAN_InitStructure.CAN_AMR3    = 0xff;
	CAN_Inilize(CAN1,&CAN_InitStructure);			//CAN1 初始化
	CAN_Inilize(CAN2,&CAN_InitStructure);			//CAN2 初始化
	
	NVIC_CAN_Init(CAN1,ENABLE,Priority_1);		//中断使能, CAN1/CAN2; ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	NVIC_CAN_Init(CAN2,ENABLE,Priority_1);		//中断使能, CAN1/CAN2; ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	
	P5_MODE_IO_PU(GPIO_Pin_LOW);		//P5.0~P5.3 设置为准双向口

	CAN1_SW(CAN1_P50_P51);				//CAN1_P00_P01,CAN1_P50_P51,CAN1_P42_P45,CAN1_P70_P71
	CAN2_SW(CAN2_P52_P53);				//CAN2_P02_P03,CAN2_P52_P53,CAN2_P46_P47,CAN2_P72_P73

	setNodeId(&CAN1_ObjDict_Data,0);
	setState(&CAN1_ObjDict_Data,Initialisation);
	setNodeId(&CAN2_ObjDict_Data,NODEID);
	setState(&CAN2_ObjDict_Data,Initialisation);

	Can1Msg.cob_id =  0;  //ID=0
	Can1Msg.rtr    =  0;  //数据帧
	Can1Msg.len    =  2;  //数据长度
    Can1Msg.m_data[0] = 0x01;
    Can1Msg.m_data[1] = 0x0F;
}

//========================================================================
// 函数: Sample_CAN
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2022-03-27
//========================================================================
void Sample_CAN(void)
{
    u8 i;

    KeyResetScan();

	if(B_Can1Read)
	{
		B_Can1Read = 0;
		
		CANSEL = CAN1;		//选择CAN1模块

        canReceive(&msg);
        canDispatch(&CAN1_ObjDict_Data,&msg);

        printf("CAN1 Read: ID=0x%04X  ",msg.cob_id);
        for(i=0; i<msg.len; i++)    printf("0x%02X ",msg.m_data[i]);    //从串口输出收到的数据
        printf("\r\n");
	}

	if(B_Can2Read)
	{
		B_Can2Read = 0;
		
		CANSEL = CAN2;		//选择CAN2模块

        canReceive(&msg);
        canDispatch(&CAN2_ObjDict_Data,&msg);

        printf("CAN2 Read: ID=0x%04X  ",msg.cob_id);
        for(i=0; i<msg.len; i++)    printf("0x%02X ",msg.m_data[i]);    //从串口输出收到的数据
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
            if(Key_cnt >= 50)		//连续50ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发
                printf("CAN1 Send\r\n");
                
                CANSEL = CAN1;        //选择CAN1模块
                canSend(0,&Can1Msg);

                Can1Msg.cob_id =  0x20F;  //ID=0
                Can1Msg.rtr    =  0;  //数据帧
                Can1Msg.len    =  1;  //数据长度
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
