#include "STC32G_CAN.h"
#include "STC32G_NVIC.h"
#include "can.h"
#include "objdict.h"
#include "canfestival.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "System_init.h"

//CAN Bit rate = Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
#define TSG1 2 //0~15
#define TSG2 1 //0~7
#define BRP  3 //0~63
//24000000/((1+3+2)*4*2)=500KHz
#define SJW 1 //

void canInit(uint8_t CANx)
{
	CAN_InitTypeDef		CAN_InitStructure;
	
	CAN_InitStructure.CAN_Enable = ENABLE;
	CAN_InitStructure.CAN_IMR    = 0xFF;
	CAN_InitStructure.CAN_SJW    = SJW;
	CAN_InitStructure.CAN_BRP    = BRP;
	CAN_InitStructure.CAN_SAM    = 0;
	CAN_InitStructure.CAN_TSG1   = TSG1;
	CAN_InitStructure.CAN_TSG2   = TSG2;

	CAN_InitStructure.CAN_ACR0   = 0x00;
	CAN_InitStructure.CAN_ACR1   = 0x00;
	CAN_InitStructure.CAN_ACR2   = 0x00;
	CAN_InitStructure.CAN_ACR3   = 0x00;
	CAN_InitStructure.CAN_AMR0   = 0xFF;
	CAN_InitStructure.CAN_AMR1   = 0xFF;
	CAN_InitStructure.CAN_AMR2   = 0xFF;
	CAN_InitStructure.CAN_AMR3   = 0xFF;
	
	CAN_Inilize(CANx,&CAN_InitStructure);

	NVIC_CAN_Init(CANx,ENABLE,Priority_0);//intterupt, ENABLE/DISABLE; Priority(low->high) Priority_0,Priority_1,Priority_2,Priority_3

}

UNS8 canSend(CAN_PORT notused, Message *m)
{
	UNUSED(notused);
	CanSendStandardFrame(m->cob_id,m->m_data);
	return 0;
}

UNS8 canReceive(Message *m)
{
	uint8_t i;
	uint8_t buffer[16];

	CanReadFifo(buffer);
	
	m->cob_id =  ((buffer[1] << 8) + buffer[2]) >> 5;
	m->rtr    =  (buffer[0]>> 6) & 0x01;
	m->len    =  (buffer[0]>> 0) & 0x0F;
	for(i=0;i<8;i++)
	{
		m->m_data[i] = buffer[i+3];
	}
	return 0;
}

extern Message msg;
extern QueueHandle_t CAN2msgQueueHandle;
extern QueueHandle_t CAN1msgQueueHandle;

void CAN1_ISR_Handler(void) interrupt CAN1_VECTOR
{
	uint8_t isr;
	uint8_t store;
	uint8_t arTemp;

	arTemp = CANAR;     //先CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
	store = AUXR2;      //后AUXR2现场保存
	
	AUXR2 &= ~0x08;		//选择CAN1模块
	isr = CanReadReg(ISR);
//	CANAR = ISR;
//	CANDR = isr;     //写1清除标志位

	if((isr & 0x04) == 0x04)  //TI
	{
		CANAR = ISR;
		CANDR |= 0x04;    //CLR FLAG	

  }	
	if((isr & 0x08) == 0x08)  //RI
	{
		CANAR = ISR;
		CANDR |= 0x08;    //CLR FLAG

		canReceive(&msg);
		xQueueSendToBackFromISR(CAN1msgQueueHandle,&msg,0);
	}
	AUXR2 = store;     //先AUXR2现场恢复
	CANAR = arTemp;    //后CANAR现场恢复
}

void CAN2_ISR_Handler(void) interrupt CAN2_VECTOR
{
	uint8_t isr;
	uint8_t store;
	uint8_t arTemp;

	arTemp = CANAR;     //先CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
	store = AUXR2;      //后AUXR2现场保存
	
	AUXR2 |= 0x08;		//选择CAN2模块
	isr = CanReadReg(ISR);
//	CANAR = ISR;
//	CANDR = isr;        //写1清除标志位

	if((isr & 0x04) == 0x04)  //TI
	{
		CANAR = ISR;
		CANDR |= 0x04;    //CLR FLAG
		
	}	
	if((isr & 0x08) == 0x08)  //RI
	{
		CANAR = ISR;
		CANDR |= 0x08;    //CLR FLAG

		canReceive(&msg);
		xQueueSendToBackFromISR(CAN2msgQueueHandle,&msg,0);
	}

	AUXR2 = store;     //先AUXR2现场恢复
	CANAR = arTemp;    //后CANAR现场恢复
}