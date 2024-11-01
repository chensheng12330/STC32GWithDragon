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

#include "STC32G_PWM.h"

//========================================================================
// 函数: PWM_Configuration
// 描述: PWM初始化程序.
// 参数: PWMx: 结构参数,请参考PWM.h里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2012-10-22
//========================================================================
uint8_t PWM_Configuration(uint8_t PWM, PWMx_InitDefine *PWMx)
{
	if(PWM == PWM1)
	{
		PWMA_CC1E_Disable();		//关闭输入捕获/比较输出
		PWMA_CC1NE_Disable();		//关闭比较输出
		PWMA_CC1S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC1ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO1P)
		{
			PWMA_CC1E_Enable();			//开启输入捕获/比较输出
			PWMA_ENO |= ENO1P;
		}
		else
		{
			PWMA_CC1E_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO1P;
		}
		if(PWMx->PWM_EnoSelect & ENO1N)
		{
			PWMA_CC1NE_Enable();		//开启输入捕获/比较输出
			PWMA_ENO |= ENO1N;
		}
		else
		{
			PWMA_CC1NE_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO1N;
		}
		
		PWMA_Duty1(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM2)
	{
		PWMA_CC2E_Disable();		//关闭输入捕获/比较输出
		PWMA_CC2NE_Disable();		//关闭比较输出
		PWMA_CC2S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC2ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO2P)
		{
			PWMA_CC2E_Enable();			//开启输入捕获/比较输出
			PWMA_ENO |= ENO2P;
		}
		else
		{
			PWMA_CC2E_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO2P;
		}
		if(PWMx->PWM_EnoSelect & ENO2N)
		{
			PWMA_CC2NE_Enable();		//开启输入捕获/比较输出
			PWMA_ENO |= ENO2N;
		}
		else
		{
			PWMA_CC2NE_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO2N;
		}
		
		PWMA_Duty2(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM3)
	{
		PWMA_CC3E_Disable();		//关闭输入捕获/比较输出
		PWMA_CC3NE_Disable();		//关闭比较输出
		PWMA_CC3S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC3ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO3P)
		{
			PWMA_CC3E_Enable();			//开启输入捕获/比较输出
			PWMA_ENO |= ENO3P;
		}
		else
		{
			PWMA_CC3E_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO3P;
		}
		if(PWMx->PWM_EnoSelect & ENO3N)
		{
			PWMA_CC3NE_Enable();		//开启输入捕获/比较输出
			PWMA_ENO |= ENO3N;
		}
		else
		{
			PWMA_CC3NE_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO3N;
		}
		
		PWMA_Duty3(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM4)
	{
		PWMA_CC4E_Disable();		//关闭输入捕获/比较输出
		PWMA_CC4NE_Disable();		//关闭比较输出
		PWMA_CC4S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC4ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO4P)
		{
			PWMA_CC4E_Enable();			//开启输入捕获/比较输出
			PWMA_ENO |= ENO4P;
		}
		else
		{
			PWMA_CC4E_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO4P;
		}
		if(PWMx->PWM_EnoSelect & ENO4N)
		{
			PWMA_CC4NE_Enable();		//开启输入捕获/比较输出
			PWMA_ENO |= ENO4N;
		}
		else
		{
			PWMA_CC4NE_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO4N;
		}
		
		PWMA_Duty4(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWMA)
	{
//		PWMA_OC1_ReloadEnable(PWMx->PWM_Reload);	//输出比较的预装载使能
//		PWMA_OC1_FastEnable(PWMx->PWM_Fast);		//输出比较快速功能使能
//		PWMA_CCPCAPreloaded(PWMx->PWM_PreLoad);	//捕获/比较预装载控制位(该位只对具有互补输出的通道起作用)
//		PWMA_BrakeEnable(PWMx->PWM_BrakeEnable);	//开启/禁止刹车输入

		PWMA_DeadTime(PWMx->PWM_DeadTime);	//死区发生器设置
		PWMA_AutoReload(PWMx->PWM_Period);	//周期设置
		PWMA_BrakeOutputEnable(PWMx->PWM_MainOutEnable);	//主输出使能
		PWMA_CEN_Enable(PWMx->PWM_CEN_Enable);	//使能计数器
		return	SUCCESS;
	}

	if(PWM == PWM5)
	{
		PWMB_CC5E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC5S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC5ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO5P)
		{
			PWMB_CC5E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO5P;
		}
		else
		{
			PWMB_CC5E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO5P;
		}
		
		PWMB_Duty5(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM6)
	{
		PWMB_CC6E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC6S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC6ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO6P)
		{
			PWMB_CC6E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO6P;
		}
		else
		{
			PWMB_CC6E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO6P;
		}
		
		PWMB_Duty6(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM7)
	{
		PWMB_CC7E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC7S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC7ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO7P)
		{
			PWMB_CC7E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO7P;
		}
		else
		{
			PWMB_CC7E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO7P;
		}
		
		PWMB_Duty7(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM8)
	{
		PWMB_CC8E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC8S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC8ModeSet(PWMx->PWM_Mode);		//设置输出比较模式

		if(PWMx->PWM_EnoSelect & ENO8P)
		{
			PWMB_CC8E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO8P;
		}
		else
		{
			PWMB_CC8E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO8P;
		}
		
		PWMB_Duty8(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWMB)
	{
//		PWMB_OC5_ReloadEnable(PWMx->PWM_Reload);	//输出比较的预装载使能
//		PWMB_OC5_FastEnable(PWMx->PWM_Fast);		//输出比较快速功能使能
//		PWMB_CCPCBPreloaded(PWMx->PWM_PreLoad);	//捕获/比较预装载控制位(该位只对具有互补输出的通道起作用)
//		PWMB_BrakeEnable(PWMx->PWM_BrakeEnable);	//开启/禁止刹车输入

		PWMB_DeadTime(PWMx->PWM_DeadTime);	//死区发生器设置
		PWMB_AutoReload(PWMx->PWM_Period);	//周期设置
		PWMB_BrakeOutputEnable(PWMx->PWM_MainOutEnable);	//主输出使能
		PWMB_CEN_Enable(PWMx->PWM_CEN_Enable);	//使能计数器
		return	SUCCESS;
	}

	return	FAIL;	//错误
}

//========================================================================
// 函数: UpdatePwm
// 描述: PWM占空比更新程序.
// 参数: PWM: PWM通道/组号, PWMx结构参数,请参考PWM.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-10-22
//========================================================================
void UpdatePwm(uint8_t PWM, PWMx_Duty *PWMx)
{
	switch(PWM)
	{
		case PWM1:
			PWMA_Duty1(PWMx->PWM1_Duty);
		break;

		case PWM2:
			PWMA_Duty2(PWMx->PWM2_Duty);
		break;

		case PWM3:
			PWMA_Duty3(PWMx->PWM3_Duty);
		break;

		case PWM4:
			PWMA_Duty4(PWMx->PWM4_Duty);
		break;

		case PWM5:
			PWMB_Duty5(PWMx->PWM5_Duty);
		break;

		case PWM6:
			PWMB_Duty6(PWMx->PWM6_Duty);
		break;

		case PWM7:
			PWMB_Duty7(PWMx->PWM7_Duty);
		break;

		case PWM8:
			PWMB_Duty8(PWMx->PWM8_Duty);
		break;

		case PWMA:
			PWMA_Duty1(PWMx->PWM1_Duty);
			PWMA_Duty2(PWMx->PWM2_Duty);
			PWMA_Duty3(PWMx->PWM3_Duty);
			PWMA_Duty4(PWMx->PWM4_Duty);
		break;

		case PWMB:
			PWMB_Duty5(PWMx->PWM5_Duty);
			PWMB_Duty6(PWMx->PWM6_Duty);
			PWMB_Duty7(PWMx->PWM7_Duty);
			PWMB_Duty8(PWMx->PWM8_Duty);
		break;
	}
}

//========================================================================
// 函数: ReadPWMA(void)
// 描述: 异步读取PWMA特殊功能寄存器函数. 
// 参数: addr: 读取特殊功能寄存器地址.
// 返回: dat:  读取特殊功能寄存器内容.
// 版本: V1.0, 2022-03-16
//========================================================================
uint8_t ReadPWMA(uint8_t addr)
{
	uint8_t dat;
	
	while (HSPWMA_ADR &	0x80); //等待前一个异步读写完成
	HSPWMA_ADR = addr |	0x80;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMA_ADDR寄存器的最高位写1,表示读数据
	while (HSPWMA_ADR &	0x80); //等待当前异步读取完成
	dat	= HSPWMA_DAT;		   //读取异步数据
	
	return dat;
}

//========================================================================
// 函数: ReadPWMB(void)
// 描述: 异步读取PWMB特殊功能寄存器函数. 
// 参数: addr: 读取特殊功能寄存器地址.
// 返回: dat:  读取特殊功能寄存器内容.
// 版本: V1.0, 2022-03-16
//========================================================================
uint8_t ReadPWMB(uint8_t addr)
{
	uint8_t dat;
	
	while (HSPWMB_ADR &	0x80); //等待前一个异步读写完成
	HSPWMB_ADR = addr |	0x80;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMB_ADDR寄存器的最高位写1,表示读数据
	while (HSPWMB_ADR &	0x80); //等待当前异步读取完成
	dat	= HSPWMB_DAT;		   //读取异步数据
	
	return dat;
}

//========================================================================
// 函数: WritePWMA(void)
// 描述: 异步读取PWMA特殊功能寄存器函数. 
// 参数: addr: 写入特殊功能寄存器地址.
// 参数: dat:  写入特殊功能寄存器内容.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void WritePWMA(uint8_t	addr, uint8_t dat)
{
	while (HSPWMA_ADR &	0x80); //等待前一个异步读写完成
	HSPWMA_DAT = dat;		   //准备需要写入的数据
	HSPWMA_ADR = addr &	0x7f;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMA_ADDR寄存器的最高位写0,表示写数据
}

//========================================================================
// 函数: WritePWMB(void)
// 描述: 异步读取PWMB特殊功能寄存器函数. 
// 参数: addr: 写入特殊功能寄存器地址.
// 参数: dat:  写入特殊功能寄存器内容.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void WritePWMB(uint8_t	addr, uint8_t dat)
{
	while (HSPWMB_ADR &	0x80); //等待前一个异步读写完成
	HSPWMB_DAT = dat;		   //准备需要写入的数据
	HSPWMB_ADR = addr &	0x7f;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMB_ADDR寄存器的最高位写0,表示写数据
}

//========================================================================
// 函数: HSPWM_Configuration
// 描述: HSPWM初始化程序.
// 参数: PWMx: 结构参数,请参考PWM.h里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2022-04-05
//========================================================================
void HSPWM_Configuration(uint8_t PWM, HSPWMx_InitDefine *PWMx, PWMx_Duty *DUTYx)
{
	uint8_t tmpENO,tmpCCER1,tmpCCER2;
	
	if(PWM == PWMA)
	{
		HSPWMA_CFG = 0x03;			//使能PWMA相关寄存器异步访问功能

		if(PWMx->PWM_EnoSelect & ENO1P)
		{
			tmpENO |= ENO1P;
			tmpCCER1 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO1P;
			tmpCCER1 &= ~0x01;
		}
		if(PWMx->PWM_EnoSelect & ENO1N)
		{
			tmpENO |= ENO1N;
			tmpCCER1 |= 0x04;
		}
		else
		{
			tmpENO &= ~ENO1N;
			tmpCCER1 &= ~0x04;
		}
		if(PWMx->PWM_EnoSelect & ENO2P)
		{
			tmpENO |= ENO2P;
			tmpCCER1 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO2P;
			tmpCCER1 &= ~0x10;
		}
		if(PWMx->PWM_EnoSelect & ENO2N)
		{
			tmpENO |= ENO2N;
			tmpCCER1 |= 0x40;
		}
		else
		{
			tmpENO &= ~ENO2N;
			tmpCCER1 &= ~0x40;
		}
		if(PWMx->PWM_EnoSelect & ENO3P)
		{
			tmpENO |= ENO3P;
			tmpCCER2 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO3P;
			tmpCCER2 &= ~0x01;
		}
		if(PWMx->PWM_EnoSelect & ENO3N)
		{
			tmpENO |= ENO3N;
			tmpCCER2 |= 0x04;
		}
		else
		{
			tmpENO &= ~ENO3N;
			tmpCCER2 &= ~0x04;
		}
		if(PWMx->PWM_EnoSelect & ENO4P)
		{
			tmpENO |= ENO4P;
			tmpCCER2 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO4P;
			tmpCCER2 &= ~0x10;
		}
		if(PWMx->PWM_EnoSelect & ENO4N)
		{
			tmpENO |= ENO4N;
			tmpCCER2 |= 0x40;
		}
		else
		{
			tmpENO &= ~ENO4N;
			tmpCCER2 &= ~0x40;
		}
		
		//通过异步方式设置PWMA的相关寄存器
		WritePWMA((uint8_t)&PWMA_CCER1, 0x00);
		WritePWMA((uint8_t)&PWMA_CCER2, 0x00);
		WritePWMA((uint8_t)&PWMA_CCMR1, CCMRn_PWM_MODE1);			//通道模式配置
		WritePWMA((uint8_t)&PWMA_CCMR2, CCMRn_PWM_MODE1);
		WritePWMA((uint8_t)&PWMA_CCMR3, CCMRn_PWM_MODE1);
		WritePWMA((uint8_t)&PWMA_CCMR4, CCMRn_PWM_MODE1);
		WritePWMA((uint8_t)&PWMA_CCER1, tmpCCER1);			//配置通道输出使能和极性
		WritePWMA((uint8_t)&PWMA_CCER2, tmpCCER2);
		WritePWMA((uint8_t)&PWMA_ENO, tmpENO);	//使能PWM信号输出端口
		WritePWMA((uint8_t)&PWMA_CCR1H, (uint8_t)(DUTYx->PWM1_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((uint8_t)&PWMA_CCR1L, (uint8_t)DUTYx->PWM1_Duty);
		WritePWMA((uint8_t)&PWMA_CCR2H, (uint8_t)(DUTYx->PWM2_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((uint8_t)&PWMA_CCR2L, (uint8_t)DUTYx->PWM2_Duty);
		WritePWMA((uint8_t)&PWMA_CCR3H, (uint8_t)(DUTYx->PWM3_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((uint8_t)&PWMA_CCR3L, (uint8_t)DUTYx->PWM3_Duty);
		WritePWMA((uint8_t)&PWMA_CCR4H, (uint8_t)(DUTYx->PWM4_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((uint8_t)&PWMA_CCR4L, (uint8_t)DUTYx->PWM4_Duty);
		WritePWMA((uint8_t)&PWMA_ARRH,	(uint8_t)(PWMx->PWM_Period >> 8));	//设置输出PWM的周期
		WritePWMA((uint8_t)&PWMA_ARRL,	(uint8_t)(PWMx->PWM_Period));
		WritePWMA((uint8_t)&PWMA_DTR, PWMx->PWM_DeadTime);					//设置互补对称输出PWM的死区
		WritePWMA((uint8_t)&PWMA_BRK, PWMx->PWM_MainOutEnable<<7);	//使能主输出
		WritePWMA((uint8_t)&PWMA_CR1, PWMx->PWM_CEN_Enable);				//开始PWM计数
	}
	else if(PWM == PWMB)
	{
		HSPWMB_CFG = 0x03;			//使能PWMB相关寄存器异步访问功能

		if(PWMx->PWM_EnoSelect & ENO5P)
		{
			tmpENO |= ENO5P;
			tmpCCER1 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO5P;
			tmpCCER1 &= ~0x01;
		}
		if(PWMx->PWM_EnoSelect & ENO6P)
		{
			tmpENO |= ENO6P;
			tmpCCER1 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO6P;
			tmpCCER1 &= ~0x10;
		}
		if(PWMx->PWM_EnoSelect & ENO7P)
		{
			tmpENO |= ENO7P;
			tmpCCER2 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO7P;
			tmpCCER2 &= ~0x01;
		}
		if(PWMx->PWM_EnoSelect & ENO8P)
		{
			tmpENO |= ENO8P;
			tmpCCER2 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO8P;
			tmpCCER2 &= ~0x10;
		}
		
		//通过异步方式设置PWMA的相关寄存器
		WritePWMB((uint8_t)&PWMB_CCER1, 0x00);
		WritePWMB((uint8_t)&PWMB_CCER2, 0x00);
		WritePWMB((uint8_t)&PWMB_CCMR1, CCMRn_PWM_MODE1);			//通道模式配置
		WritePWMB((uint8_t)&PWMB_CCMR2, CCMRn_PWM_MODE1);
		WritePWMB((uint8_t)&PWMB_CCMR3, CCMRn_PWM_MODE1);
		WritePWMB((uint8_t)&PWMB_CCMR4, CCMRn_PWM_MODE1);
		WritePWMB((uint8_t)&PWMB_CCER1, tmpCCER1);			//配置通道输出使能和极性
		WritePWMB((uint8_t)&PWMB_CCER2, tmpCCER2);
		WritePWMB((uint8_t)&PWMB_ENO, tmpENO);	//使能PWM信号输出端口
		WritePWMB((uint8_t)&PWMB_CCR5H, (uint8_t)(DUTYx->PWM5_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((uint8_t)&PWMB_CCR5L, (uint8_t)DUTYx->PWM5_Duty);
		WritePWMB((uint8_t)&PWMB_CCR6H, (uint8_t)(DUTYx->PWM6_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((uint8_t)&PWMB_CCR6L, (uint8_t)DUTYx->PWM6_Duty);
		WritePWMB((uint8_t)&PWMB_CCR7H, (uint8_t)(DUTYx->PWM7_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((uint8_t)&PWMB_CCR7L, (uint8_t)DUTYx->PWM7_Duty);
		WritePWMB((uint8_t)&PWMB_CCR8H, (uint8_t)(DUTYx->PWM8_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((uint8_t)&PWMB_CCR8L, (uint8_t)DUTYx->PWM8_Duty);
		WritePWMB((uint8_t)&PWMB_ARRH,	(uint8_t)(PWMx->PWM_Period >> 8));	//设置输出PWM的周期
		WritePWMB((uint8_t)&PWMB_ARRL,	(uint8_t)(PWMx->PWM_Period));
		WritePWMB((uint8_t)&PWMB_DTR, PWMx->PWM_DeadTime);					//设置互补对称输出PWM的死区
		WritePWMB((uint8_t)&PWMB_BRK, PWMx->PWM_MainOutEnable<<7);	//使能主输出
		WritePWMB((uint8_t)&PWMB_CR1, PWMx->PWM_CEN_Enable);				//开始PWM计数
	}
}

//========================================================================
// 函数: UpdateHSPwm
// 描述: HSPWM占空比更新程序.
// 参数: PWM: PWM通道/组号, PWMx结构参数,请参考PWM.h里的定义.
// 返回: none.
// 版本: V1.0, 2022-04-05
//========================================================================
void UpdateHSPwm(uint8_t PWM, PWMx_Duty *PWMx)
{
	switch(PWM)
	{
		case PWM1:
			WritePWMA((uint8_t)&PWMA_CCR1H, (uint8_t)(PWMx->PWM1_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR1L, (uint8_t)PWMx->PWM1_Duty);
		break;

		case PWM2:
			WritePWMA((uint8_t)&PWMA_CCR2H, (uint8_t)(PWMx->PWM2_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR2L, (uint8_t)PWMx->PWM2_Duty);
		break;

		case PWM3:
			WritePWMA((uint8_t)&PWMA_CCR3H, (uint8_t)(PWMx->PWM3_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR3L, (uint8_t)PWMx->PWM3_Duty);
		break;

		case PWM4:
			WritePWMA((uint8_t)&PWMA_CCR4H, (uint8_t)(PWMx->PWM4_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR4L, (uint8_t)PWMx->PWM4_Duty);
		break;

		case PWM5:
			WritePWMB((uint8_t)&PWMB_CCR5H, (uint8_t)(PWMx->PWM5_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR5L, (uint8_t)PWMx->PWM5_Duty);
		break;

		case PWM6:
			WritePWMB((uint8_t)&PWMB_CCR6H, (uint8_t)(PWMx->PWM6_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR6L, (uint8_t)PWMx->PWM6_Duty);
		break;

		case PWM7:
			WritePWMB((uint8_t)&PWMB_CCR7H, (uint8_t)(PWMx->PWM7_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR7L, (uint8_t)PWMx->PWM7_Duty);
		break;

		case PWM8:
			WritePWMB((uint8_t)&PWMB_CCR8H, (uint8_t)(PWMx->PWM8_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR8L, (uint8_t)PWMx->PWM8_Duty);
		break;

		case PWMA:
			WritePWMA((uint8_t)&PWMA_CCR1H, (uint8_t)(PWMx->PWM1_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR1L, (uint8_t)PWMx->PWM1_Duty);
			WritePWMA((uint8_t)&PWMA_CCR2H, (uint8_t)(PWMx->PWM2_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR2L, (uint8_t)PWMx->PWM2_Duty);
			WritePWMA((uint8_t)&PWMA_CCR3H, (uint8_t)(PWMx->PWM3_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR3L, (uint8_t)PWMx->PWM3_Duty);
			WritePWMA((uint8_t)&PWMA_CCR4H, (uint8_t)(PWMx->PWM4_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((uint8_t)&PWMA_CCR4L, (uint8_t)PWMx->PWM4_Duty);
		break;

		case PWMB:
			WritePWMB((uint8_t)&PWMB_CCR5H, (uint8_t)(PWMx->PWM5_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR5L, (uint8_t)PWMx->PWM5_Duty);
			WritePWMB((uint8_t)&PWMB_CCR6H, (uint8_t)(PWMx->PWM6_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR6L, (uint8_t)PWMx->PWM6_Duty);
			WritePWMB((uint8_t)&PWMB_CCR7H, (uint8_t)(PWMx->PWM7_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR7L, (uint8_t)PWMx->PWM7_Duty);
			WritePWMB((uint8_t)&PWMB_CCR8H, (uint8_t)(PWMx->PWM8_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((uint8_t)&PWMB_CCR8L, (uint8_t)PWMx->PWM8_Duty);
		break;
	}
}

/*********************************************************/
