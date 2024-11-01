/*
    adc_stc.h

    Sampling with ADC built in MCU
    
    Copyright (c) 2020 Creative Lau (creativelaulab@gmail.com)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef ADC_STC_H
#define ADC_STC_H

#include "config_stc.h"
#include "intrins.h"
#include "delay.h"
#include "chart.h"
#include <string.h>

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

#define ADC_SPEED_512 0x0F //512个时钟
#define ADC_SPEED_480 0x0E //480个时钟
#define ADC_SPEED_448 0x0D //448个时钟
#define ADC_SPEED_416 0x0C //416个时钟
#define ADC_SPEED_384 0x0B //384个时钟
#define ADC_SPEED_352 0x0A //352个时钟
#define ADC_SPEED_320 0x09 //320个时钟
#define ADC_SPEED_288 0x08 //288个时钟
#define ADC_SPEED_256 0x07 //256个时钟
#define ADC_SPEED_224 0x06 //224个时钟
#define ADC_SPEED_192 0x05 //192个时钟
#define ADC_SPEED_160 0x04 //160个时钟
#define ADC_SPEED_128 0x03 //128个时钟
#define ADC_SPEED_96 0x02  //96个时钟
#define ADC_SPEED_64 0x01  //64个时钟
#define ADC_SPEED_32 0x00  //32个时钟

#define ADC_CHS_10 0x00    //将P1.0的模拟值输入ADC模拟通道
#define ADC_CHS_11 0x01    //将P1.1的模拟值输入ADC模拟通道
#define ADC_CHS_12 0x02    //将P1.2的模拟值输入ADC模拟通道
#define ADC_CHS_13 0x03    //将P1.3的模拟值输入ADC模拟通道
#define ADC_CHS_14 0x04    //将P1.4的模拟值输入ADC模拟通道
#define ADC_CHS_15 0x05    //将P1.5的模拟值输入ADC模拟通道
#define ADC_CHS_16 0x06    //将P1.6的模拟值输入ADC模拟通道
#define ADC_CHS_17 0x07    //将P1.7的模拟值输入ADC模拟通道
#define ADC_CHS_00 0x08    //将P0.0的模拟值输入ADC模拟通道
#define ADC_CHS_01 0x09    //将P0.1的模拟值输入ADC模拟通道
#define ADC_CHS_02 0x0A    //将P0.2的模拟值输入ADC模拟通道
#define ADC_CHS_03 0x0B    //将P0.3的模拟值输入ADC模拟通道
#define ADC_CHS_04 0x0C    //将P0.4的模拟值输入ADC模拟通道
#define ADC_CHS_05 0x0D    //将P0.5的模拟值输入ADC模拟通道
#define ADC_CHS_06 0x0E    //将P0.6的模拟值输入ADC模拟通道
#define ADC_CHS_BG 0x0F    //将第16通道（内部BandGap参考电压）的模拟值输入ADC模拟通道

uint16 GetADC_CHX(uint8 chx);                 //获得chx通道ADC
uint16 GetVoltage(uint8 chx, uint16 lsb);     //获得chx通道电压值 mV
uint16 GetBGV(void);                          //获得内部参考电压BGV mV
uint16 *GetWaveADC(uint8 chx, uint8 scale_h); //获得波形采样

#endif
