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

#ifndef PORT_ASM_H
#define PORT_ASM_H


/* We require the address of the pxCurrentTCB variable, 
but don't want to know any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * pxCurrentTCB;

/*
 * Saves the stack pointer for one task into its TCB, calls
 * vTaskSwitchContext() to update the TCB being used, then restores the stack
 * from the new TCB read to run the task.
 */
//void portEND_SWITCHING_ISR( void );
//void vPortEND_SWITCHING_ISR( void )
//{
//	portEND_SWITCHING_ISR();
//}	

/* Saves the stack pointer for one task into its TCB. */
#define portSAVE_SPX()                      \
{                                           \
    __asm   { MOV   DR0,DR60    }           \
    __asm   { MOV   DR4,pxCurrentTCB    }   \
    __asm   { MOV   @WR6+0x2,WR2        }   \
}

/* Restores the stack from the new TCB read to 
run the task. */
#define portRESTORE_SPX()                   \
{                                           \
    __asm   { MOV   DR4,pxCurrentTCB    }   \
    __asm   { MOV   WR2,@WR6+0x2        }   \
    __asm   { XRL   WR0,WR0     }           \
    __asm   { MOV   DR60,DR0    }           \
}

/* Saves the stack pointer for one task into its TCB, calls
vTaskSwitchContext() to update the TCB being used, then restores the stack
from the new TCB read to run the task. */
#define portEND_SWITCHING_ISR()             \
{                                           \
extern void vTaskSwitchContext(void);       \
                                            \
    portSAVE_SPX();                         \
    vTaskSwitchContext();                   \
    portRESTORE_SPX();                      \
}

/* Macro to push the current execution context 
onto the stack. */
#define portSAVE_CONTEXT()                  \
{                                           \
    __asm   { PUSH  DR56    }               \
    __asm   { PUSH  DR28    }               \
    __asm   { PUSH  DR24    }               \
    __asm   { PUSH  DR20    }               \
    __asm   { PUSH  DR16    }               \
    __asm   { PUSH  DR12    }               \
    __asm   { PUSH  DR8     }               \
    __asm   { PUSH  DR4     }               \
    __asm   { PUSH  DR0     }               \
    __asm   { PUSH  PSW     }               \
}

/* Macro that restores the execution context 
from the stack. */
#define portRESTORE_CONTEXT()               \
{                                           \
    __asm   { POP   PSW     }               \
    __asm   { POP   DR0     }               \
    __asm   { POP   DR4     }               \
    __asm   { POP   DR8     }               \
    __asm   { POP   DR12    }               \
    __asm   { POP   DR16    }               \
    __asm   { POP   DR20    }               \
    __asm   { POP   DR24    }               \
    __asm   { POP   DR28    }               \
    __asm   { POP   DR56    }               \
}

/* Normalize the return address of the function 
call in stack to ECALL mode. */
#if ( configUSE_ROMHUGE == 0 )
    #define portNORMALIZE_FUNCTION_STACK()  \
    {                                       \
        __asm   { CLR   EA      }           \
        __asm   { POP   xPCH    }           \
        __asm   { POP   xPCL    }           \
        __asm   { PUSH  #BYTE0 0FFH }       \
        __asm   { PUSH  xPCH    }           \
        __asm   { PUSH  xPCL    }           \
        __asm   { PUSH  PSW1    }           \
    }
#else
    #define portNORMALIZE_FUNCTION_STACK()  \
    {                                       \
        __asm   { CLR   EA      }           \
        __asm   { PUSH  PSW1    }           \
    }
#endif

/* Normalize the return address of the interrupt 
call in stack to ECALL mode. */
#define portNORMALIZE_INTERRUPT_STACK()     \
{                                           \
    __asm   { POP   xPCH    }               \
    __asm   { POP   xPCL    }               \
    __asm   { POP   xPCB    }               \
    __asm   { POP   PSW1    }               \
    __asm   { PUSH  xPCB    }               \
    __asm   { PUSH  xPCH    }               \
    __asm   { PUSH  xPCL    }               \
    __asm   { PUSH  PSW1    }               \
}

/* Clear the interrupt priority status trigger 
while keeping the stack data unchanged */
#define portRELEASE_INTERRUPT()             \
{                                           \
    __asm   { CLR   EA      }               \
    __asm   { PUSH  PSW1    }               \
    __asm   { PUSH  #BYTE2 ($+10)   }       \
    __asm   { PUSH  #BYTE0 ($+7)    }       \
    __asm   { PUSH  #BYTE1 ($+4)    }       \
    __asm   { RETI          }               \
}

/* All function call and interrupt are returned 
using ERET consistently. */
#define portRETURN_TASK()                   \
{                                           \
    __asm   { POP   PSW1    }               \
    __asm   { SETB  EA      }               \
    __asm   { DB    0AAH    }   /* ERET */  \
}


#endif

