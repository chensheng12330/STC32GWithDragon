#ifndef __HX711_H__
#define __HX711_H__

#include <STC32G.h>
#include "config.h"

sbit HX711_DOUT = P0^0; 
sbit HX711_SCK = P0^1; 

u32 HX711_Read(void);

#endif