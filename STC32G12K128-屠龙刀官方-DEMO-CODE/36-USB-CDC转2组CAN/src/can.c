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
    CANAR = addr;
    return CANDR;
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
// 函数: void CanInit()
// 描述: CAN初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2023-05-30
// 备注: 
//========================================================================
void CanInit()
{
	//-------- CAN1 --------
	P_SW1 = (P_SW1 & ~(3<<4)) | (1<<4); //端口切换(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1
    CANSEL = CAN1;                  //选择CAN1模块
    CANEN = 1;                      //使能CAN1模块
    
    CanWriteReg(MR ,0x04);          //使能 Reset Mode
    CanWriteReg(ACR0, 0x00);        //总线验收代码寄存器
    CanWriteReg(ACR1, 0x00);
    CanWriteReg(ACR2, 0x00);
    CanWriteReg(ACR3, 0x00);
    CanWriteReg(AMR0, 0xff);        //总线验收屏蔽寄存器
    CanWriteReg(AMR1, 0xff);
    CanWriteReg(AMR2, 0xff);
    CanWriteReg(AMR3, 0xff);
    CanWriteReg(ISR, 0xff);         //清中断标志
    CanWriteReg(IMR, 0x7f);         //使能全部中断
    CanWriteReg(MR, 0x00);          //退出 Reset Mode
	CANICR = 0x02;                  //CAN中断使能
    CanSetBaudrate(500);

	//-------- CAN2 --------
	P_SW3 = (P_SW3 & ~(3)) | (1);   //端口切换(CAN_Rx,CAN_Tx) 0x00:P0.2,P0.3  0x01:P5.2,P5.3  0x02:P4.6,P4.7  0x03:P7.2,P7.3
	CANSEL = CAN2;                  //选择CAN2模块
	CAN2EN = 1;                     //CAN2模块使能

	CanWriteReg(MR  ,0x04);         //使能 Reset Mode
	CanWriteReg(ACR0,0x00);         //总线验收代码寄存器
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);	        //总线验收屏蔽寄存器
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);
	CanWriteReg(ISR ,0xff);         //清中断标志
	CanWriteReg(IMR ,0x7f);         //中断寄存器
	CanWriteReg(MR  ,0x00);         //退出 Reset Mode
	CANICR |= 0x20;                 //CAN2中断使能
    CanSetBaudrate(500);

    Can1RxWptr = 0;
    Can1RxRptr = 0;
    Can2RxWptr = 0;
    Can2RxRptr = 0;
}

//========================================================================
// 函数: void CanSetBaudrate(u16 brt)
// 描述: CAN总线波特率设置函数。
// 参数: brt: 波特率(单位：K).
// 返回: none.
// 版本: VER1.0
// 日期: 2023-5-24
// 备注: 
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
    
    CanWriteReg(MR, 0x04);          //使能 Reset Mode
    CanWriteReg(BTR0, (u8)brt);
    CanWriteReg(BTR1, (u8)(brt >> 8));
    CanWriteReg(MR, 0x00);          //退出 Reset Mode
}

//========================================================================
// 函数: void Can1ReadMsg(void)
// 描述: CAN1接收数据函数。
// 参数: none.
// 返回: none.
// 版本: VER2.0
// 日期: 2023-5-24
// 备注: 
//========================================================================
void Can1ReadMsg()
{
    while (CanReadReg(SR) & 0x80)   //判断接收缓冲区里是否还有数据
    {
        u8 *pdat;
        u8 start;
        u8 remain;
        u8 i;
    
        if ((((Can1RxWptr + 1) ^ Can1RxRptr) & 0x0f) == 0)
            break;                  //缓冲区已满,暂停读取CAN数据

        pdat = &Can1RxBuffer[Can1RxWptr++ & 0x0f][2];
        i = 0;
        *pdat++ = start = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
        remain = (start & 0x0f) + (start & 0x80 ? 4 : 2);
    
        while (remain--)            //读取有效数据
            *pdat++ = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
    
        remain = 13 - i;            //一帧最多13个字节，不足补0
        while (remain--)
            *pdat++ = 0;                                    
        
        while (i & 3)               //判断已读数据长度是否4的整数倍
            CanReadReg((u8)(RX_BUF0 + (i++ & 3)));          
    }
}

//========================================================================
// 函数: void Can2ReadMsg(void)
// 描述: CAN2接收数据函数。
// 参数: none.
// 返回: none.
// 版本: VER2.0
// 日期: 2023-5-24
// 备注: 
//========================================================================
void Can2ReadMsg()
{
    while (CanReadReg(SR) & 0x80)   //判断接收缓冲区里是否还有数据
    {
        u8 *pdat;
        u8 start;
        u8 remain;
        u8 i;
    
        if ((((Can2RxWptr + 1) ^ Can2RxRptr) & 0x0f) == 0)
            break;                  //缓冲区已满,暂停读取CAN数据

        pdat = &Can2RxBuffer[Can2RxWptr++ & 0x0f][2];
        i = 0;
        *pdat++ = start = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
        remain = (start & 0x0f) + (start & 0x80 ? 4 : 2);
    
        while (remain--)            //读取有效数据
            *pdat++ = CanReadReg((u8)(RX_BUF0 + (i++ & 3)));
    
        remain = 13 - i;            //一帧最多13个字节，不足补0
        while (remain--)
            *pdat++ = 0;                                    
        
        while (i & 3)               //判断已读数据长度是否4的整数倍
            CanReadReg((u8)(RX_BUF0 + (i++ & 3)));          
    }
}

//========================================================================
// 函数: void CanSendMsg(u8 *CAN)
// 描述: CAN发送标准帧函数。
// 参数: *CAN: 存放CAN总线发送数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2023-5-24
// 备注: 
//========================================================================
void CanSendMsg(u8 *CAN)
{
    u8 start;
    u8 remain;
    u8 i;

    i = 0;
    start = CAN[0];                 //起始字节
    remain = (start & 0x0f) + (start & 0x80 ? 4 : 2) + 1;   

    while (remain--)                //写入有效数据
        CanWriteReg((u8)(TX_BUF0 + (i++ & 3)), CAN[i-1]);     

    while (i & 3)                   //写入填充数据，一帧数据占据4的整数倍缓冲区空间，不足补0
        CanWriteReg((u8)(TX_BUF0 + (i++ & 3)), 0);          
    
	CanWriteReg(CMR, 0x04);	        //发起一次帧传输
}

void CanPolling()
{
    u8 *pdat;
    u8 sum;
    u8 i;

    if (DeviceState != DEVSTATE_CONFIGURED)
        return;

    //判断缓冲队列是否有待发数据帧
    if ((Can1RxRptr ^ Can1RxWptr) & 0x0f)
    {
        //获取队列首地址
        pdat = Can1RxBuffer[Can1RxRptr++ & 0x0f];
        sum = pdat[0] = CAN1;    //设置通道号
        pdat[1] = CANDATA;    //设置数据包头
        for (i = 1; i < 15; i++)
            sum += pdat[i];
        pdat[15] = (u8)(256 - sum); //计算校验和
        
        CDC1_SendData(pdat, 16);
    }

    //判断缓冲队列是否有待发数据帧
    if ((Can2RxRptr ^ Can2RxWptr) & 0x0f)
    {
        //获取队列首地址
        pdat = Can2RxBuffer[Can2RxRptr++ & 0x0f];
        sum = pdat[0] = CAN2;    //设置通道号
        pdat[1] = CANDATA;    //设置数据包头
        for (i = 1; i < 15; i++)
            sum += pdat[i];
        pdat[15] = (u8)(256 - sum); //计算校验和
        
        CDC2_SendData(pdat, 16);
    }

    if (bCDC1OutReady)
    {
        CANSEL = CAN1;		//选择CAN1模块
        if(CDC1OutBuffer[1] == CANDATA)     //发送数据
        {
            CanSendMsg(&CDC1OutBuffer[2]);   //发送一帧数据
        }
        else if(CDC1OutBuffer[1] == CANBAUD)   //设置波特率
        {
            CanSetBaudrate(((u16)CDC1OutBuffer[5]<<8)+CDC1OutBuffer[6]);
        }

        CDC1_OUT_done();    //接收应答（固定格式），告诉上位机数据已经处理完成，可以接收下一个数据包了
    }
    
    if (bCDC2OutReady)
    {
        CANSEL = CAN2;		//选择CAN2模块
        if(CDC2OutBuffer[1] == CANDATA)     //发送数据
        {
            CanSendMsg(&CDC2OutBuffer[2]);   //发送一帧数据
        }
        else if(CDC2OutBuffer[1] == CANBAUD)   //设置波特率
        {
            CanSetBaudrate(((u16)CDC2OutBuffer[5]<<8)+CDC2OutBuffer[6]);
        }

        CDC2_OUT_done();    //接收应答（固定格式），告诉上位机数据已经处理完成，可以接收下一个数据包了
    }
}

void CAN1_ISR_Handler (void) interrupt CAN1_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;                 //先CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
	store = AUXR2;                  //后AUXR2现场保存
	
	AUXR2 &= ~0x08;		            //选择CAN1模块
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
		CANDR &= ~0x04;             //清除 Reset Mode, 从BUS-OFF状态退出
		
		CANAR = ISR;
		CANDR = 0x20;               //CLR FLAG
	}	

	AUXR2 = store;                  //先AUXR2现场恢复
	CANAR = arTemp;                 //后CANAR现场恢复
}

void CAN2_ISR_Handler (void) interrupt CAN2_VECTOR
{
	u8 isr;
	u8 store;
	u8 arTemp;

	arTemp = CANAR;                 //先CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
	store = AUXR2;                  //后AUXR2现场保存
	
	AUXR2 |= 0x08;                  //选择CAN2模块
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
		CANDR &= ~0x04;             //清除 Reset Mode, 从BUS-OFF状态退出
		
		CANAR = ISR;
		CANDR = 0x20;               //CLR FLAG
	}	

	AUXR2 = store;                  //先AUXR2现场恢复
	CANAR = arTemp;                 //后CANAR现场恢复
}
