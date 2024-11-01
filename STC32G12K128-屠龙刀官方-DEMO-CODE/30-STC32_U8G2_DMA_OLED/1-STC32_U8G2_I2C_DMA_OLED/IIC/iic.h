#ifndef __IIC_H__
#define __IIC_H__

#include "system.h"

extern uint8_t xdata DmaTxBuffer[32];

extern void HAL_I2C_Init();
extern void HAL_I2C_Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

extern void DMA_Config(void);
extern void UartInit(void);

#endif /*__IIC_H__*/

