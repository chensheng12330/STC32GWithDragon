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

/*************  功能说明    **************

本例程基于STC32G核心转接板（屠龙刀）进行编写测试。

CAN1(P5.0,P5.1)、CAN2(P5.2,P5.3)通过收发器连接到同一个总线上。

MCU每秒钟从CAN1、CAN2发送一帧数据。

CAN1发送的报文被CAN2接收，CAN2发送的报文被CAN1接收。

收到一个标准帧后, 将CAN帧信息、数据通过串口1(P1.6,P1.7)打印出来。

默认波特率500KHz, 用户可自行修改。

注意：进行CAN总线通信测试前需要将R79、R80电阻断开。

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 默认时钟 24MHz (用户可自行修改频率).

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件
#include "stdio.h"
#include "intrins.h"

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/
//CAN总线波特率=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
#define TSG1    2		//0~15
#define TSG2    1		//1~7 (TSG2 不能设置为0)
#define BRP     3		//0~63
//24000000/((1+3+2)*4*2)=500KHz

#define SJW     0		//重新同步跳跃宽度

//总线波特率100KHz以上设置为 0; 100KHz以下设置为 1
#define SAM     0		//总线电平采样次数： 0:采样1次; 1:采样3次

/*****************************************************************************/


/*************  本地常量声明    **************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒
#define Baudrate        115200UL
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))

#define	STANDARD_FRAME   0     //帧格式：标准帧
#define	EXTENDED_FRAME   1     //帧格式：扩展帧

/*************  本地变量声明    **************/

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

typedef struct
{
	u8	DLC:4;          //数据长度, bit0~bit3
	u8	:2;             //空数据, bit4~bit5
	u8	RTR:1;          //帧类型, bit6
	u8	FF:1;           //帧格式, bit7
	u32	ID;             //CAN ID
	u8	DataBuffer[8];  //数据缓存
}CAN_DataDef;

CAN_DataDef CAN1_Tx;
CAN_DataDef CAN1_Rx[8];

CAN_DataDef CAN2_Tx;
CAN_DataDef CAN2_Rx[8];

bit B_1ms;          //1ms标志
bit B_Can1Read;     //CAN 收到数据标志
bit B_Can2Read;     //CAN 收到数据标志
bit B_Can1Send;     //CAN 发生数据标志
bit B_Can2Send;     //CAN 发生数据标志
u16 msecond;

/*************  本地函数声明    **************/

void CANInit();
u8 CanReadReg(u8 addr);
u8 CanReadMsg(CAN_DataDef *CAN);
void CanSendMsg(CAN_DataDef *CAN);
void KeyResetScan(void);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
	P_SW1 = (P_SW1 & 0x3f) | 0x80;    //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //定时器时钟1T模式
	S1BRT = 0;          //串口1选择定时器1为波特率发生器
	TL1 = TM;
	TH1 = TM>>8;
	TR1 = 1;			//定时器1开始计时
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

/********************* 主函数 *************************/
void main(void)
{
    u8 i,j,n,sr;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    RSTFLAG |= 0x04;   //设置硬件复位后需要检测P3.2的状态选择运行区域，否则硬件复位后进入USB下载模式

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    usb_init();

    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    CANInit();
    UartInit();

    EUSB = 1;   //IE2相关的中断使能后，需要重新设置EUSB
    EA = 1;     //打开总中断

	//====初始化数据=====
	CAN1_Tx.FF = STANDARD_FRAME;    //标准帧
	CAN1_Tx.RTR = 0;                //0：数据帧，1：远程帧
	CAN1_Tx.DLC = 0x08;             //数据长度
	CAN1_Tx.ID = 0x0567;            //CAN ID
	CAN1_Tx.DataBuffer[0] = 0x11;   //数据内容
	CAN1_Tx.DataBuffer[1] = 0x12;
	CAN1_Tx.DataBuffer[2] = 0x13;
	CAN1_Tx.DataBuffer[3] = 0x14;
	CAN1_Tx.DataBuffer[4] = 0x15;
	CAN1_Tx.DataBuffer[5] = 0x16;
	CAN1_Tx.DataBuffer[6] = 0x17;
	CAN1_Tx.DataBuffer[7] = 0x18;
	
	CAN2_Tx.FF = EXTENDED_FRAME;    //扩展帧
	CAN2_Tx.RTR = 0;                //0：数据帧，1：远程帧
	CAN2_Tx.DLC = 0x08;             //数据长度
	CAN2_Tx.ID = 0x03456789;        //CAN ID
	CAN2_Tx.DataBuffer[0] = 0x21;   //数据内容
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
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
            
            if(++msecond >= 1000)   //1秒到
            {
                msecond = 0;

                //------------------处理CAN1模块-----------------------
                CANSEL = 0;         //选择CAN1模块
                sr = CanReadReg(SR);

                if(sr & 0x01)		//判断是否有 BS:BUS-OFF状态
                {
                    CANAR = MR;
                    CANDR &= ~0x04;  //清除 Reset Mode, 从BUS-OFF状态退出
                }
                else
                {
                    CanSendMsg(&CAN1_Tx);   //发送一帧数据
                }

                //------------------处理CAN2模块-----------------------
                CANSEL = 1;         //选择CAN2模块
                sr = CanReadReg(SR);

                if(sr & 0x01)		//判断是否有 BS:BUS-OFF状态
                {
                    CANAR = MR;
                    CANDR &= ~0x04;  //清除 Reset Mode, 从BUS-OFF状态退出
                }
                else
                {
                    CanSendMsg(&CAN2_Tx);   //发送一帧数据
                }
            }
        }

        if(B_Can1Read)
        {
            B_Can1Read = 0;
            
            CANSEL = 0;         //选择CAN1模块
            n = CanReadMsg(CAN1_Rx);    //读取接收内容
            if(n>0)
            {
                for(i=0;i<n;i++)
                {
//                    CanSendMsg(&CAN1_Rx[i]);  //CAN总线原样返回
                    printf("CAN1:ID=0x%08lX DLC=%d FF=%d RTR=%d ",CAN1_Rx[i].ID,CAN1_Rx[i].DLC,CAN1_Rx[i].FF,CAN1_Rx[i].RTR);   //串口打印帧信息
                    for(j=0;j<CAN1_Rx[i].DLC;j++)
                    {
                        printf("0x%02X ",CAN1_Rx[i].DataBuffer[j]);    //从串口输出收到的数据
                    }
                    printf("\r\n");
                }
            }
        }

        if(B_Can2Read)
        {
            B_Can2Read = 0;
            
            CANSEL = 1;         //选择CAN2模块
            n = CanReadMsg(CAN2_Rx);    //读取接收内容
            if(n>0)
            {
                for(i=0;i<n;i++)
                {
//                    CanSendMsg(&CAN2_Rx[i]);  //CAN总线原样返回
                    printf("CAN2:ID=0x%08lX DLC=%d FF=%d RTR=%d ",CAN2_Rx[i].ID,CAN2_Rx[i].DLC,CAN2_Rx[i].FF,CAN2_Rx[i].RTR);   //串口打印帧信息
                    for(j=0;j<CAN2_Rx[i].DLC;j++)
                    {
                        printf("0x%02X ",CAN2_Rx[i].DataBuffer[j]);    //从串口输出收到的数据
                    }
                    printf("\r\n");
                }
            }
        }

        if (bUsbOutReady) //USB调试及复位所需代码
        {
            usb_OUT_done();
        }
    }
}


/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: u8 ReadReg(u8 addr)
// 描述: CAN功能寄存器读取函数。
// 参数: CAN功能寄存器地址.
// 返回: CAN功能寄存器数据.
// 版本: VER1.0
// 日期: 2020-11-16
// 备注: 
//========================================================================
u8 CanReadReg(u8 addr)
{
	u8 dat;
	CANAR = addr;
	dat = CANDR;
	return dat;
}

//========================================================================
// 函数: void WriteReg(u8 addr, u8 dat)
// 描述: CAN功能寄存器配置函数。
// 参数: CAN功能寄存器地址, CAN功能寄存器数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-16
// 备注: 
//========================================================================
void CanWriteReg(u8 addr, u8 dat)
{
	CANAR = addr;
	CANDR = dat;
}

//========================================================================
// 函数: void CanReadFifo(CAN_DataDef *CANx)
// 描述: 读取CAN缓冲区数据函数。
// 参数: *CANx: 存放CAN总线读取数据.
// 返回: none.
// 版本: VER2.0
// 日期: 2023-01-31
// 备注: 
//========================================================================
void CanReadFifo(CAN_DataDef *CAN)
{
    u8 i;
    u8 pdat[5];
    u8 RX_Index=0;

    pdat[0] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));

    if(pdat[0] & 0x80)  //判断是标准帧还是扩展帧
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //扩展帧ID占4个字节
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[3] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[4] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = (((u32)pdat[1] << 24) + ((u32)pdat[2] << 16) + ((u32)pdat[3] << 8) + pdat[4]) >> 3;
    }
    else
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //标准帧ID占2个字节
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = ((pdat[1] << 8) + pdat[2]) >> 5;
    }
    
    CAN->FF = pdat[0] >> 7;     //帧格式
    CAN->RTR = pdat[0] >> 6;    //帧类型
    CAN->DLC = pdat[0];         //数据长度

    for(i=0;((i<CAN->DLC) && (i<8));i++)        //读取数据长度为len，最多不超过8
    {
        CAN->DataBuffer[i] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //读取有效数据
    }
    while(RX_Index&3)   //判断已读数据长度是否4的整数倍
    {
        CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));  //读取填充数据，一帧数据占据4的整数倍缓冲区空间，不足补0
    }
}

//========================================================================
// 函数: u8 CanReadMsg(void)
// 描述: CAN接收数据函数。
// 参数: *CANx: 存放CAN总线读取数据.
// 返回: 帧个数.
// 版本: VER2.0
// 日期: 2023-01-31
// 备注: 
//========================================================================
u8 CanReadMsg(CAN_DataDef *CAN)
{
    u8 i;
    u8 n=0;

    do{
        CanReadFifo(&CAN[n++]);  //读取接收缓冲区数据
        i = CanReadReg(SR);
    }while(i&0x80);     //判断接收缓冲区里是否还有数据，有的话继续读取

    return n;   //返回帧个数
}

//========================================================================
// 函数: void CanSendMsg(CAN_DataDef *CAN)
// 描述: CAN发送标准帧函数。
// 参数: *CANx: 存放CAN总线发送数据..
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CanSendMsg(CAN_DataDef *CAN)
{
    u32 CanID;
    u8 RX_Index,i;

    if(CANSEL)  //判断是否CAN2
    {
        i = 200;
        while((--i) && (B_Can2Send));  //等待CAN2上次发送完成
    }
    else
    {
        i = 200;
        while((--i) && (B_Can1Send));  //等待CAN1上次发送完成
    }

    if(CAN->FF)     //判断是否扩展帧
    {
        CanID = CAN->ID << 3;
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6)|0x80);	//bit7: 标准帧(0)/扩展帧(1), bit6: 数据帧(0)/远程帧(1), bit3~bit0: 数据长度(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>24));
        CanWriteReg(TX_BUF2,(u8)(CanID>>16));
        CanWriteReg(TX_BUF3,(u8)(CanID>>8));

        CanWriteReg(TX_BUF0,(u8)CanID);

        RX_Index = 1;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //数据长度为DLC，最多不超过8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //写入有效数据
        }
        while(RX_Index&3)   //判断已读数据长度是否4的整数倍
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //写入填充数据，一帧数据占据4的整数倍缓冲区空间，不足补0
        }
    }
    else    //发送标准帧
    {
        CanID = (u16)(CAN->ID << 5);
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6));  //bit7: 标准帧(0)/扩展帧(1), bit6: 数据帧(0)/远程帧(1), bit3~bit0: 数据长度(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>8));
        CanWriteReg(TX_BUF2,(u8)CanID);

        RX_Index = 3;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //数据长度为DLC，最多不超过8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //写入有效数据
        }
        while(RX_Index&3)   //判断已读数据长度是否4的整数倍
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //写入填充数据，一帧数据占据4的整数倍缓冲区空间，不足补0
        }
    }
	CanWriteReg(CMR ,0x04);		//发起一次帧传输
    
    if(CANSEL)  //判断是否CAN2
    {
        B_Can2Send = 1;     //设置CAN2发送忙标志
    }
    else
    {
        B_Can1Send = 1;     //设置CAN1发送忙标志
    }
}

//========================================================================
// 函数: void CANSetBaudrate()
// 描述: CAN总线波特率设置函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CANSetBaudrate()
{
	CanWriteReg(BTR0,(SJW << 6) + BRP);
	CanWriteReg(BTR1,(SAM << 7) + (TSG2 << 4) + TSG1);
}

//========================================================================
// 函数: void CANInit()
// 描述: CAN初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CANInit()
{
	//-------- CAN1 --------
	CANEN = 1;          //CAN1模块使能
	CANSEL = 0;         //选择CAN1模块
	P_SW1 = (P_SW1 & ~(3<<4)) | (1<<4); //端口切换(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1

	CanWriteReg(MR  ,0x04);		//使能 Reset Mode
	CANSetBaudrate();	//设置波特率
	
	CanWriteReg(ACR0,0x00);		//总线验收代码寄存器
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);		//总线验收屏蔽寄存器
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);

	CanWriteReg(IMR ,0xff);		//中断寄存器
	CanWriteReg(ISR ,0xff);		//清中断标志
	CanWriteReg(MR  ,0x00);		//退出 Reset Mode
	CANICR = 0x02;				//CAN中断使能

	//-------- CAN2 --------
	CAN2EN = 1;         //CAN2模块使能
	CANSEL = 1;         //选择CAN2模块
	P_SW3 = (P_SW3 & ~(3)) | (1);       //端口切换(CAN_Rx,CAN_Tx) 0x00:P0.2,P0.3  0x01:P5.2,P5.3  0x02:P4.6,P4.7  0x03:P7.2,P7.3

	CanWriteReg(MR  ,0x04);		//使能 Reset Mode
	CANSetBaudrate();					//设置波特率

	CanWriteReg(ACR0,0x00);		//总线验收代码寄存器
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);		//总线验收屏蔽寄存器
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);

	CanWriteReg(IMR ,0xff);		//中断寄存器
	CanWriteReg(ISR ,0xff);		//清中断标志
	CanWriteReg(MR  ,0x00);		//退出 Reset Mode
	CANICR |= 0x20;				//CAN2中断使能
}

//========================================================================
// 函数: void CANBUS1_Interrupt(void) interrupt CAN1_VECTOR
// 描述: CAN总线中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-03-24
// 备注: 
//========================================================================
void CANBUS1_Interrupt(void) interrupt CAN1_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;     //先CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
	store = AUXR2;      //后AUXR2现场保存
	
	AUXR2 &= ~0x08;		//选择CAN1模块
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
		CANDR &= ~0x04;  //清除 Reset Mode, 从BUS-OFF状态退出
		
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

	AUXR2 = store;     //先AUXR2现场恢复
	CANAR = arTemp;    //后CANAR现场恢复
}

//========================================================================
// 函数: void CANBUS2_Interrupt(void) interrupt CAN2_VECTOR
// 描述: CAN总线中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CANBUS2_Interrupt(void) interrupt CAN2_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;     //先CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
	store = AUXR2;      //后AUXR2现场保存
	
	AUXR2 |= 0x08;		//选择CAN2模块
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
		CANDR &= ~0x04;  //清除 Reset Mode, 从BUS-OFF状态退出
		
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

	AUXR2 = store;     //先AUXR2现场恢复
	CANAR = arTemp;    //后CANAR现场恢复
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
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
// 函数: void KeyResetScan(void)
// 描述: P3.2口按键长按1秒触发软件复位，进入USB下载模式。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//连续1000ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

                USBCON = 0x00;      //清除USB设置
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
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
