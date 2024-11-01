#include "FreeRTOS.h"
#include "task.h"

#include "io.h"
#include "canfestival.h"
#include "timer.h"
#include "ObjDict.h"

uint8_t  input_val[1];
uint8_t output_val[1];



/* IOÈÎÎñº¯Êý */
portTASK_FUNCTION( vIOTask, pvParameters )
{
	UNUSED( pvParameters );
    
    prvIOInit();
    while(1)
    {
        prvIOUpdate();
        
        vTaskDelay(100);
    }
    
    vTaskDelete(NULL);
}   


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

void prvIOInit( void )
{
    P40 = 0;
}

void prvIOUpdate( void )
{
	input_val[0] = get_input();
	digital_input_handler(&CAN2_ObjDict_Data, input_val, sizeof(input_val));
	digital_output_handler(&CAN2_ObjDict_Data, output_val, sizeof(output_val));
	set_output(output_val[0]);
}

