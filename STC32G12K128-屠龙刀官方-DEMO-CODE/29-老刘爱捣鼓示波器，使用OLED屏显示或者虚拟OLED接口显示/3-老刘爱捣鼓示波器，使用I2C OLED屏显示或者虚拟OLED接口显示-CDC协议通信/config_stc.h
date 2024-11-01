/*Register of MCU and Pins defined here*/

#ifndef CONFIG_STC_H
#define CONFIG_STC_H

#include "stc32g.h"
//#define DEBUG

#define MAIN_Fosc       35000000L   //定义主时钟
#define BAUD            (65536 - MAIN_Fosc/4/115200)

#define VirtualDevice   1    //0: 驱动硬件OLED;  1: 驱动调试接口OLED-12864虚拟设备


/* ADC采样通道设置
   ADC Channel Setup */
#define ADC_DSO ADC_CHS_14
#define ADC_BAT ADC_CHS_04

/* 采样运行指示灯
   Indicator for sampling running */
sbit P_Ready = P2 ^ 7;

/*  OLED Pin */
sbit OLED_SCLK = P2 ^ 5; //时钟（D0/SCLK）
sbit OLED_SDIN = P2 ^ 3; //数据（D1/MOSI）
sbit OLED_RST = P2 ^ 0;  //复位（RES）
sbit OLED_DC = P2 ^ 1;   //数据/命令控制（DC）
sbit OLED_CS = P2 ^ 2;   //片选（CS）

#ifdef DEBUG
sbit P15 = P1 ^ 5;
#endif

#endif