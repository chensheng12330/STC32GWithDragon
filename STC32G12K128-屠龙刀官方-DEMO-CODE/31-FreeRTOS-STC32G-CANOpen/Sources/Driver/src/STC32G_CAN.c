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

#include "STC32G_CAN.h"

//========================================================================
// 函数: uint8_t ReadReg(uint8_t addr)
// 描述: CAN功能寄存器读取函数。
// 参数: CAN功能寄存器地址.
// 返回: CAN功能寄存器数据.
// 版本: VER1.0
// 日期: 2020-11-16
// 备注: 
//========================================================================
uint8_t CanReadReg(uint8_t addr)
{
	uint8_t dat;
	CANAR = addr;
	dat = CANDR;
	return dat;
}

//========================================================================
// 函数: void WriteReg(uint8_t addr, uint8_t dat)
// 描述: CAN功能寄存器配置函数。
// 参数: CAN功能寄存器地址, CAN功能寄存器数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-16
// 备注: 
//========================================================================
void CanWriteReg(uint8_t addr, uint8_t dat)
{
	CANAR = addr;
	CANDR = dat;
}

//========================================================================
// 函数: void CAN_Inilize(uint8_t CANx, CAN_InitTypeDef *CAN)
// 描述: CAN 初始化程序.
// 参数: CAN: 结构参数,请参考CAN.h里的定义.
// 返回: none.
// 版本: V1.0, 2021-06-02
//========================================================================
void CAN_Inilize(uint8_t CANx, CAN_InitTypeDef *CAN)
{
	if(CANx == CAN1)
	{
		CANSEL = CAN1;		//选择CAN1模块
		CanWriteReg(MR  ,0x04);		//使能 Reset Mode

		if(CAN->CAN_Enable == ENABLE)	CANEN = 1;		//使能CAN1模块
		else								CANEN = 0;		//关闭CAN1模块
		
		CanWriteReg(BTR0,(uint8_t)((CAN->CAN_SJW << 6) + CAN->CAN_BRP));
		CanWriteReg(BTR1,(uint8_t)((CAN->CAN_SAM << 7) + (CAN->CAN_TSG2 << 4) + CAN->CAN_TSG1));

		CanWriteReg(ACR0,CAN->CAN_ACR0);		//总线验收代码寄存器
		CanWriteReg(ACR1,CAN->CAN_ACR1);
		CanWriteReg(ACR2,CAN->CAN_ACR2);
		CanWriteReg(ACR3,CAN->CAN_ACR3);
		CanWriteReg(AMR0,CAN->CAN_AMR0);		//总线验收屏蔽寄存器
		CanWriteReg(AMR1,CAN->CAN_AMR1);
		CanWriteReg(AMR2,CAN->CAN_AMR2);
		CanWriteReg(AMR3,CAN->CAN_AMR3);

		CanWriteReg(ISR ,0xff);		//清中断标志
		CanWriteReg(IMR ,CAN->CAN_IMR);			//中断寄存器设置
		CanWriteReg(MR  ,0x00);		//退出 Reset Mode
	}
	else if(CANx == CAN2)
	{
		CANSEL = CAN2;		//选择CAN2模块
		
		CanWriteReg(MR  ,0x04);		//使能 Reset Mode

		if(CAN->CAN_Enable == ENABLE)	CAN2EN = 1;		//使能CAN2模块
		else								CAN2EN = 0;		//关闭CAN2模块
		
		CanWriteReg(BTR0,(uint8_t)((CAN->CAN_SJW << 6) + CAN->CAN_BRP));
		CanWriteReg(BTR1,(uint8_t)((CAN->CAN_SAM << 7) + (CAN->CAN_TSG2 << 4) + CAN->CAN_TSG1));

		CanWriteReg(ACR0,CAN->CAN_ACR0);		//总线验收代码寄存器
		CanWriteReg(ACR1,CAN->CAN_ACR1);
		CanWriteReg(ACR2,CAN->CAN_ACR2);
		CanWriteReg(ACR3,CAN->CAN_ACR3);
		CanWriteReg(AMR0,CAN->CAN_AMR0);		//总线验收屏蔽寄存器
		CanWriteReg(AMR1,CAN->CAN_AMR1);
		CanWriteReg(AMR2,CAN->CAN_AMR2);
		CanWriteReg(AMR3,CAN->CAN_AMR3);

		CanWriteReg(ISR ,0xff);		//清中断标志
		CanWriteReg(IMR ,CAN->CAN_IMR);			//中断寄存器设置
		CanWriteReg(MR  ,0x00);		//退出 Reset Mode
	}
}

//========================================================================
// 函数: void CanReadFifo(uint8_t *pdat)
// 描述: 读取CAN缓冲区数据函数。
// 参数: *pdat: 存放CAN缓冲区数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-16
// 备注: 
//========================================================================
void CanReadFifo(uint8_t *pdat)
{
	pdat[0]  = CanReadReg(RX_BUF0);
	pdat[1]  = CanReadReg(RX_BUF1);
	pdat[2]  = CanReadReg(RX_BUF2);
	pdat[3]  = CanReadReg(RX_BUF3);

	pdat[4]  = CanReadReg(RX_BUF0);
	pdat[5]  = CanReadReg(RX_BUF1);
	pdat[6]  = CanReadReg(RX_BUF2);
	pdat[7]  = CanReadReg(RX_BUF3);

	pdat[8]  = CanReadReg(RX_BUF0);
	pdat[9]  = CanReadReg(RX_BUF1);
	pdat[10] = CanReadReg(RX_BUF2);
	pdat[11] = CanReadReg(RX_BUF3);

	pdat[12]  = CanReadReg(RX_BUF0);
	pdat[13]  = CanReadReg(RX_BUF1);
	pdat[14]  = CanReadReg(RX_BUF2);
	pdat[15]  = CanReadReg(RX_BUF3);
}

//========================================================================
// 函数: uint16_t CanReadStandardFrame(uint8_t *pdat)
// 描述: CAN读取标准帧函数。
// 参数: *pdat: 接收数据缓冲区.
// 返回: CAN ID.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
uint16_t CanReadStandardFrame(uint8_t *pdat)
{
	uint8_t i;
	uint16_t CanID;
	uint8_t buffer[16];

	CanReadFifo(buffer);
	CanID = ((buffer[1] << 8) + buffer[2]) >> 5;
	for(i=0;i<8;i++)
	{
		pdat[i] = buffer[i+3];
	}
	return CanID;
}

//========================================================================
// 函数: uint32_t CanReadExtendedFrame(uint8_t *pdat)
// 描述: CAN读取扩展帧函数。
// 参数: *pdat: 接收数据缓冲区.
// 返回: CAN ID.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
uint32_t CanReadExtendedFrame(uint8_t *pdat)
{
	uint8_t i;
	uint32_t CanID;
	uint8_t buffer[16];

	CanReadFifo(buffer);
	CanID = (((uint32_t)buffer[1] << 24) + ((uint32_t)buffer[2] << 16) + ((uint32_t)buffer[3] << 8) + buffer[4]) >> 3;
	for(i=0;i<8;i++)
	{
		pdat[i] = buffer[i+5];
	}
	return CanID;
}

//========================================================================
// 函数: void CanSendStandardFrame(uint16_t canid, uint8_t *pdat)
// 描述: CAN发送标准帧函数。
// 参数: canid: CAN ID; *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CanSendStandardFrame(uint16_t canid, uint8_t *pdat)
{
	uint16_t CanID;

	CanID = canid << 5;
	CanWriteReg(TX_BUF0,0x08);	//bit7: 标准帧(0)/扩展帧(1), bit6: 数据帧(0)/远程帧(1), bit3~bit0: 数据长度(DLC)
	CanWriteReg(TX_BUF1,(uint8_t)(CanID>>8));
	CanWriteReg(TX_BUF2,(uint8_t)CanID);
	CanWriteReg(TX_BUF3,pdat[0]);

	CanWriteReg(TX_BUF0,pdat[1]);
	CanWriteReg(TX_BUF1,pdat[2]);
	CanWriteReg(TX_BUF2,pdat[3]);
	CanWriteReg(TX_BUF3,pdat[4]);

	CanWriteReg(TX_BUF0,pdat[5]);
	CanWriteReg(TX_BUF1,pdat[6]);
	CanWriteReg(TX_BUF2,pdat[7]);
	
	CanWriteReg(TX_BUF3,0x00);
	CanWriteReg(CMR ,0x04);		//发起一次帧传输
}

//========================================================================
// 函数: void CanSendExtendedFrame(uint32_t canid, uint8_t *pdat)
// 描述: CAN发送扩展帧函数。
// 参数: canid: CAN ID; *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CanSendExtendedFrame(uint32_t canid, uint8_t *pdat)
{
	uint32_t CanID;

	CanID = canid << 3;
	CanWriteReg(TX_BUF0,0x88);	//bit7: 标准帧(0)/扩展帧(1), bit6: 数据帧(0)/远程帧(1), bit3~bit0: 数据长度(DLC)
	CanWriteReg(TX_BUF1,(uint8_t)(CanID>>24));
	CanWriteReg(TX_BUF2,(uint8_t)(CanID>>16));
	CanWriteReg(TX_BUF3,(uint8_t)(CanID>>8));

	CanWriteReg(TX_BUF0,(uint8_t)CanID);
	CanWriteReg(TX_BUF1,pdat[0]);
	CanWriteReg(TX_BUF2,pdat[1]);
	CanWriteReg(TX_BUF3,pdat[2]);

	CanWriteReg(TX_BUF0,pdat[3]);
	CanWriteReg(TX_BUF1,pdat[4]);
	CanWriteReg(TX_BUF2,pdat[5]);
	CanWriteReg(TX_BUF3,pdat[6]);

	CanWriteReg(TX_BUF0,pdat[7]);
	CanWriteReg(TX_BUF1,0x00);
	CanWriteReg(TX_BUF2,0x00);
	CanWriteReg(TX_BUF3,0x00);

	CanWriteReg(CMR ,0x04);		//发起一次帧传输
}

