/*
 * FreeRTOS Kernel V10.4.6
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portasm.h"

/* Constants required to setup the tick ISR. */

#define portRELOAD_VALUE                    ( 65536 - configCPU_CLOCK_HZ / configTICK_RATE_HZ )


static StackType_t data xPCL;
static StackType_t data xPCH;
static StackType_t data xPCB;


/* Setup the timer to generate the tick interrupts. */
static void prvPortSetupTimerInterrupt( void );


/*-----------------------------------------------------------*/


/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    uint32_t ulAddress;
	uint8_t index;

    /* Place a few bytes of known values on the bottom of the stack.
    This is just useful for debugging and can be uncommented if required.*/
    /*
    *pxTopOfStack = 0x11;
    pxTopOfStack++;
    *pxTopOfStack = 0x22;
    pxTopOfStack++;
    *pxTopOfStack = 0x33;
    pxTopOfStack++;
    */

    /* Simulate how the stack would look after a call to the scheduler tick
    ISR. The return address that would have been pushed by the MCU. */
    ulAddress = ( uint32_t ) pxCode;
    *pxTopOfStack = ( StackType_t ) ( ulAddress >> 16 );    /* PC[23:16] */
    pxTopOfStack++;
    *pxTopOfStack = ( StackType_t ) ( ulAddress >> 8 );     /* PC[15:8] */
    pxTopOfStack++;
    *pxTopOfStack = ( StackType_t ) ( ulAddress );          /* PC[7:0] */

	pxTopOfStack++;
	*pxTopOfStack = 0;                                      /* PSW1 */

    /* Next all the registers will have been pushed by portSAVE_CONTEXT(). */
	/* DR56 */
	pxTopOfStack++;
	*pxTopOfStack = 0;                                      /* R56 */
	pxTopOfStack++;
	*pxTopOfStack = 1;                                      /* R57(DPXL) */
	pxTopOfStack++;
	*pxTopOfStack = 0;                                      /* R58(DPH) */
	pxTopOfStack++;
	*pxTopOfStack = 0;                                      /* R59(DPL) */

	/* DR28 DR24 DR20 DR16 DR12 DR8 DR4 */
	for(index = 0; index < 28; index++)
	{
		pxTopOfStack++;
		*pxTopOfStack = 0;
	}

    /* The function parameters will be passed in the DR0 register as
    a four byte generic pointer is used. */
	pxTopOfStack++;
    ulAddress = ( uint32_t ) pvParameters;
    *pxTopOfStack = ( StackType_t ) ( ulAddress >> 24 );    /* R0 */
    pxTopOfStack++;
    *pxTopOfStack = ( StackType_t ) ( ulAddress >> 16 );    /* R1 */
    pxTopOfStack++;
    *pxTopOfStack = ( StackType_t ) ( ulAddress >> 8 );     /* R2 */
    pxTopOfStack++;
    *pxTopOfStack = ( StackType_t ) ( ulAddress );          /* R3 */
	pxTopOfStack++;
	*pxTopOfStack = ( StackType_t ) 0;                      /* PSW */

    /* we return the top of the stack */
    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
    UNUSED( pxCurrentTCB );

	/* Start the timer that generates the tick ISR. */
    prvPortSetupTimerInterrupt();

    /* Start the first task. */
    portRESTORE_SPX();
    portRESTORE_CONTEXT();
    portRETURN_TASK();

    /* Should never get here! */
    return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented for this port. */
}
/*-----------------------------------------------------------*/

/* Manual context switch.   */
void vPortYield( void )
{
    UNUSED( pxCurrentTCB );

    /* Normalize the return address of the function
    call in stack to ECALL mode, then push the current
    execution context onto the stack.*/
    portNORMALIZE_FUNCTION_STACK();
    portSAVE_CONTEXT();

    /* Saves the stack pointer for one task into its TCB, calls
    vTaskSwitchContext() to update the TCB being used, then restores the stack
    from the new TCB read to run the task. */
    portEND_SWITCHING_ISR();

	/* Restores the execution context from the stack,
    then returned using ERET */
    portRESTORE_CONTEXT();
    portRETURN_TASK();
}

/*-----------------------------------------------------------*/

/* The ISR used depends on whether the preemptive or cooperative
scheduler is being used. */

void vTimer0ISR( void )
{
    UNUSED( pxCurrentTCB );

    /* Normalize the return address of the interrupt
    call in stack to ECALL mode, then push the current
    execution context onto the stack.*/
    portNORMALIZE_INTERRUPT_STACK();
    portSAVE_CONTEXT();

#if configUSE_PREEMPTION == 1

    /* Get the scheduler to update the task states following the tick. */
    if( xTaskIncrementTick() != pdFALSE )
    {
        /* Saves the stack pointer for one task into its TCB, calls
        vTaskSwitchContext() to update the TCB being used, then restores the stack
        from the new TCB read to run the task. */
        portEND_SWITCHING_ISR();
    }

#else

    /* Same as preemptive tick, but the cooperative scheduler is being used
    so we don't have to switch in the context of the next task. */
    xTaskIncrementTick();

#endif

	/* Restores the execution context from the stack,
    then returned using ERET */
    portRESTORE_CONTEXT();
    portRELEASE_INTERRUPT();
    portRETURN_TASK();
}

/*-----------------------------------------------------------*/

/*Setup the systick timer to generate the tick interrupts at the required
frequency. */
static void prvPortSetupTimerInterrupt( void )
{
    /* Timer0 work at 1T mode. */
    AUXR |= 0x80;

    /* Timer0 work at mode0 (16-bit auto reload mode) */
    TMOD &= ~0x0f;

    /* Initial timer0 reload value (1ms per cycle). */
    TL0 = ( uint8_t )( portRELOAD_VALUE );
    TH0 = ( uint8_t )( portRELOAD_VALUE >> 8 );

    /* Interrupts are disabled when this is called so
    the timer can be started here. */
    TR0 = 1;

    /* Enable timer0 interrupts. */
    ET0 = 1;
}


    __asm   { CSEG    AT  0BH       }
#if ( configUSE_ROMHUGE == 0 )
    __asm   { JMP     vTimer0ISR?_  }
#else
    __asm   { JMP     vTimer0ISR??  }
#endif
