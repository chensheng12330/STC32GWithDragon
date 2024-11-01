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

#include	"Task.h"
#include	"APP_CAN.h"
#include	"APP_KEY.h"

//========================================================================
//                               本地变量声明
//========================================================================

static TASK_COMPONENTS Task_Comps[]=
{
//状态  计数  周期  函数
//	{0, 250, 250, Sample_Lamp},				/* task 1 Period： 250ms */
//	{0, 200, 200, Sample_ADtoUART},		/* task 2 Period： 200ms */
//	{0, 20, 20, Sample_INTtoUART},		/* task 3 Period： 20ms */
//	{0, 1, 1, Sample_RTC},						/* task 4 Period： 1ms */
//	{0, 1, 1, Sample_I2C_PS},					/* task 5 Period： 1ms */
//	{0, 1, 1, Sample_SPI_PS},					/* task 6 Period： 1ms */
//	{0, 1, 1, Sample_EEPROM},					/* task 7 Period： 1ms */
//	{0, 100, 100, Sample_WDT},				/* task 8 Period： 100ms */
//	{0, 1, 1, Sample_PWMA_Output},		/* task 9 Period： 1ms */
//	{0, 1, 1, Sample_PWMB_Output},		/* task 10 Period： 1ms */
//	{0, 500, 500, Sample_DMA_AD},			/* task 12 Period： 500ms */
//	{0, 500, 500, Sample_DMA_M2M},		/* task 13 Period： 100ms */
//	{0, 1, 1, Sample_DMA_UART},				/* task 14 Period： 1ms */
//	{0, 1, 1, Sample_DMA_SPI_PS},			/* task 15 Period： 1ms */
//	{0, 1, 1, Sample_DMA_LCM},				/* task 16 Period： 1ms */
//	{0, 1, 1, Sample_DMA_I2C},				/* task 17 Period： 1ms */
	{0, 1, 1, Sample_CAN},						/* task 18 Period： 1ms */
	{0, 100, 100, Sample_KEY},					/* task 19 Period： 100ms */
//	{0, 1, 1, Sample_LIN},						/* task 19 Period： 1ms */
//	{0, 1, 1, Sample_USART_LIN},			/* task 20 Period： 1ms */
//	{0, 1, 1, Sample_USART2_LIN},			/* task 21 Period： 1ms */
//	{0, 1, 1, Sample_HSSPI},					/* task 22 Period： 1ms */
//	{0, 1, 1, Sample_HSPWM},					/* task 22 Period： 1ms */
	/* Add new task here */
};

u8 Tasks_Max = sizeof(Task_Comps)/sizeof(Task_Comps[0]);

//========================================================================
// 函数: Task_Handler_Callback
// 描述: 任务标记回调函数.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2012-10-22
//========================================================================
void Task_Marks_Handler_Callback(void)
{
	u8 i;
	for(i=0; i<Tasks_Max; i++)
	{
		if(Task_Comps[i].TIMCount)    /* If the time is not 0 */
		{
			Task_Comps[i].TIMCount--;  /* Time counter decrement */
			if(Task_Comps[i].TIMCount == 0)  /* If time arrives */
			{
				/*Resume the timer value and try again */
				Task_Comps[i].TIMCount = Task_Comps[i].TRITime;  
				Task_Comps[i].Run = 1;    /* The task can be run */
			}
		}
	}
}

//========================================================================
// 函数: Task_Pro_Handler_Callback
// 描述: 任务处理回调函数.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2012-10-22
//========================================================================
void Task_Pro_Handler_Callback(void)
{
	u8 i;
	for(i=0; i<Tasks_Max; i++)
	{
		if(Task_Comps[i].Run) /* If task can be run */
		{
			Task_Comps[i].Run = 0;    /* Flag clear 0 */
			Task_Comps[i].TaskHook();  /* Run task */
		}
	}
}


