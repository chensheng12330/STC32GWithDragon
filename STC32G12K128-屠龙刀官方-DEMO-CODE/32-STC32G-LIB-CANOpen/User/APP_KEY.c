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

#include "APP_KEY.h"
#include "STC32G_GPIO.h"
#include "STC32G_NVIC.h"

#include "objdict.h"

//========================================================================
//                               本地常量声明	
//========================================================================


//========================================================================
//                               本地变量声明
//========================================================================

uint8_t  input_val[1];
uint8_t output_val[1];

//========================================================================
//                               本地函数声明
//========================================================================


//========================================================================
//                            外部函数和变量声明
//========================================================================

uint8_t get_input(void)
{
	uint8_t val = 0;
	
	val = P3;
	val = ~(val>>2)&0x0F;

	return val;
}

void set_output(uint8_t val)
{
	P6 = ~val;
}

unsigned char digital_input_handler(CO_Data* d, unsigned char *newInput, unsigned char size)
{
	unsigned char loops, i, input, transmission = 0;

	loops = (sizeof(Read_Inputs_8_Bit) <= size) ? sizeof(Read_Inputs_8_Bit) : size;
	for (i=0; i < loops; i++)
	{
		input = *newInput ^ Polarity_Input_8_Bit[i];

		if (Read_Inputs_8_Bit[i] != input)
		{
			if (Global_Interrupt_Enable_Digital)
			{
				if ((Interrupt_Mask_Any_Change_8_Bit[i]  &(Read_Inputs_8_Bit[i] ^ input))||
				     (Interrupt_Mask_Low_to_High_8_Bit[i] & ~Read_Inputs_8_Bit[i] & input)||
				     (Interrupt_Mask_High_to_Low_8_Bit[i] & Read_Inputs_8_Bit[i] & ~input))
				{
					transmission = 1;
				}
			}
			Read_Inputs_8_Bit[i] = input;

		}
		newInput++;
	}

	if (transmission)
	{
		/* force emission of PDO by artificially changing last emitted*/
		d->PDO_status[0].last_message.cob_id = 0;
		sendPDOevent(d);
	}

	return 1;
}

unsigned char digital_output_handler(CO_Data* d, unsigned char *newOutput, unsigned char size)
{
	unsigned char loops, i, error;//, type;
	unsigned int varsize = 1;

	loops = (sizeof(Write_Outputs_8_Bit) <= size) ? sizeof(Write_Outputs_8_Bit) : size;

	for (i=0; i < loops; i++)
	{
//		getODentry(d, 0x1001, 0x0, &error, &varsize, &type, RO);
		if ((getState(d) == Stopped) || (error != 0))	// node stopped or error
		{
			Write_Outputs_8_Bit[i] &= (~Error_Mode_Outputs_8_Bit[i] | Error_Value_Outputs_8_Bit[i]);
			Write_Outputs_8_Bit[i] |= (Error_Mode_Outputs_8_Bit[i] & Error_Value_Outputs_8_Bit[i]);
		}
		*newOutput = Write_Outputs_8_Bit[i] ^ Change_Polarity_Outputs_8_Bit[i];
		newOutput++;
	}
	return 1;
}

//========================================================================
// 函数: Sample_KEY
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2022-03-27
//========================================================================
void Sample_KEY(void)
{
	input_val[0] = get_input();
	digital_input_handler(&CAN2_ObjDict_Data, input_val, sizeof(input_val));
	digital_output_handler(&CAN2_ObjDict_Data, output_val, sizeof(output_val));
	set_output(output_val[0]);
}

