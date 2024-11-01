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

#include "STC32G_LIN.h"

//========================================================================
// 函数: uint8_t ReadReg(uint8_t addr)
// 描述: Lin功能寄存器读取函数。
// 参数: Lin功能寄存器地址.
// 返回: Lin功能寄存器数据.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
uint8_t LinReadReg(uint8_t addr)
{
	uint8_t dat;
	LINAR = addr;
	dat = LINDR;
	return dat;
}

//========================================================================
// 函数: void WriteReg(uint8_t addr, uint8_t dat)
// 描述: Lin功能寄存器配置函数。
// 参数: Lin功能寄存器地址, Lin功能寄存器数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinWriteReg(uint8_t addr, uint8_t dat)
{
	LINAR = addr;
	LINDR = dat;
}

//========================================================================
// 函数: void LinReadMsg(uint8_t *pdat)
// 描述: Lin读取数据函数。
// 参数: *pdat: 接收数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinReadMsg(uint8_t *pdat)
{
	uint8_t i;

	LinWriteReg(LSEL,0x80);	//地址自增，从0开始
	for(i=0;i<FRAME_LEN;i++)
	{
		pdat[i] = LinReadReg(LBUF);
	}
}

//========================================================================
// 函数: void LinSendMsg(uint8_t *pdat)
// 描述: Lin设置数据函数。
// 参数: *pdat: 设置数据缓冲区.
// 返回: Lin ID.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSendMsg(uint8_t *pdat)
{
	uint8_t i;

	LinWriteReg(LSEL,0x80);		//地址自增，从0开始
	for(i=0;i<FRAME_LEN;i++)
	{
		LinWriteReg(LBUF,pdat[i]);
	}
}

//========================================================================
// 函数: void LinSetID(uint8_t lid)
// 描述: 设置LIN ID函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSetID(uint8_t lid)
{
	LinWriteReg(LID,lid);			//设置总线ID
}

//========================================================================
// 函数: uint8_t GetLinError(void)
// 描述: 获取LIN总线错误寄存器状态。
// 参数: none.
// 返回: 错误寄存器状态.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
uint8_t GetLinError(void)
{
	uint8_t sta;
	sta = LinReadReg(LER);		//读取清除错误寄存器
	return sta;
}

//========================================================================
// 函数: uint8_t WaitLinReady(void)
// 描述: 等待LIN总线就绪。
// 参数: none.
// 返回: LIN总线状态.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
uint8_t WaitLinReady(void)
{
	uint8_t lsr;
	do{
		lsr = LinReadReg(LSR);
	}while(!(lsr & 0x02));		//判断ready状态
	return lsr;
}

//========================================================================
// 函数: void SendAbortCmd(void)
// 描述: 主模式发送Lin总线Abort函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void SendAbortCmd(void)
{
	LinWriteReg(LCR,0x80);		//主模式 Send Abort
}

//========================================================================
// 函数: void SendHeadCmd(void)
// 描述: 主模式发送Lin总线Header函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void SendHeadCmd(void)
{
	LinWriteReg(LCR,0x81);		//主模式 Send Header
}

//========================================================================
// 函数: void SendDatCmd(void)
// 描述: 主模式发送Lin总线数据函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void SendDatCmd(void)
{
	uint8_t lcr_val;
	lcr_val = 0x82+(LIN_MODE<<6)+(FRAME_LEN<<2);
	LinWriteReg(LCR,lcr_val);
}

//========================================================================
// 函数: void ResponseTxCmd(void)
// 描述: 从模式发送Lin总线Tx Response函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void ResponseTxCmd(void)
{
	uint8_t lcr_val;
	lcr_val = 0x02+(LIN_MODE<<6)+(FRAME_LEN<<2);
	LinWriteReg(LCR,lcr_val);
}

//========================================================================
// 函数: void ResponseRxCmd(void)
// 描述: 从模式发送Lin总线Rx Response函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void ResponseRxCmd(void)
{
	uint8_t lcr_val;
	lcr_val = 0x03+(LIN_MODE<<6)+(FRAME_LEN<<2);
	LinWriteReg(LCR,lcr_val);
}

//========================================================================
// 函数: void LinTxResponse(uint8_t *pdat)
// 描述: Lin从机发送应答数据，跟主机发送的Header拼成一个完整的帧。
// 参数: *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinTxResponse(uint8_t *pdat)
{
	LinSendMsg(pdat);
	ResponseTxCmd();					//TX response
	WaitLinReady();						//等待ready状态
	GetLinError();						//读取清除错误寄存器
}

//========================================================================
// 函数: void LinReadFrame(uint8_t *pdat)
// 描述: Lin主机发送完整帧函数。
// 参数: lid: Lin ID; *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinReadFrame(uint8_t *pdat)
{
	ResponseRxCmd();					//RX response
	WaitLinReady();						//等待ready状态
	GetLinError();						//读取清除错误寄存器

	LinReadMsg(pdat);					//接收Lin总线数据
}

//========================================================================
// 函数: void LinSendFrame(uint8_t lid, uint8_t *pdat)
// 描述: Lin主机发送完整帧函数。
// 参数: lid: Lin ID; *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSendFrame(uint8_t lid, uint8_t *pdat)
{
	LinSetID(lid);						//设置总线ID
	LinSendMsg(pdat);

	SendHeadCmd();						//主模式 Send Seader
	WaitLinReady();						//等待ready状态
	GetLinError();						//读取清除错误寄存器

	SendDatCmd();							//Send Data
	WaitLinReady();						//等待ready状态
	GetLinError();						//读取清除错误寄存器
}

//========================================================================
// 函数: void LinSendHeaderRead(uint8_t lid, uint8_t *pdat)
// 描述: Lin主机发送Header，由从机发送应答数据，拼成一个完整的帧。
// 参数: lid: 发送应答从机的总线ID; *pdat: 接收数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSendHeaderRead(uint8_t lid, uint8_t *pdat)
{
	LinSetID(lid);						//设置发送Response从机总线ID

	SendHeadCmd();						//主模式 send header
	WaitLinReady();						//等待ready状态
	GetLinError();						//读取清除错误寄存器
	
	ResponseRxCmd();					//RX response
	WaitLinReady();						//等待ready状态
	GetLinError();						//读取清除错误寄存器

	LinReadMsg(pdat);				//接收Lin总线从机发送的应答数据
}

//========================================================================
// 函数: void LinSetBaudrate(uint16_t brt)
// 描述: Lin总线波特率设置函数。
// 参数: brt: 波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSetBaudrate(uint16_t brt)
{
	uint16_t tmp;
	tmp = (MAIN_Fosc >> 4) / brt;
	LinWriteReg(DLH,(uint8_t)(tmp>>8));
	LinWriteReg(DLL,(uint8_t)tmp);
}

//========================================================================
// 函数: void LinSetHeadDelay(uint8_t base_ms, uint8_t prescaler)
// 描述: Lin总线设置帧头延时函数。
// 参数: base_ms:延时计数, prescaler:延时分频.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSetHeadDelay(uint8_t base_ms, uint8_t prescaler)
{
	uint16_t tmp;
	tmp = (MAIN_Fosc * base_ms) / 1000;
	LinWriteReg(HDRH,(uint8_t)(tmp>>8));
	LinWriteReg(HDRL,(uint8_t)tmp);		//设置帧头延时计数

	LinWriteReg(HDP,prescaler);		//设置帧头延时分频
}

//========================================================================
// 函数: void LIN_Inilize(LIN_InitTypeDef *LIN)
// 描述: LIN 初始化程序.
// 参数: LIN: 结构参数,请参考LIN.h里的定义.
// 返回: none.
// 版本: V1.0, 2021-06-02
//========================================================================
void LIN_Inilize(LIN_InitTypeDef *LIN)
{
	if(LIN->LIN_Enable == ENABLE)	LINEN = 1;		//使能LIN模块
	else								LINEN = 0;		//关闭LIN模块

	GetLinError();						//读取清除错误寄存器
	LinWriteReg(LIE,LIN->LIN_IE);		//LIE中断使能寄存器
	LinSetBaudrate(LIN->LIN_Baudrate);	//设置波特率
	LinSetHeadDelay(LIN->LIN_HeadDelay,LIN->LIN_HeadPrescaler);	//设置帧头延时
}
