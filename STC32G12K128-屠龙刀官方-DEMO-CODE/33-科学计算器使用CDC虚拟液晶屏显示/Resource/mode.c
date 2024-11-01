#include <stdlib.h>
#include <stdio.h>
//#include "STC12C5A60S2.h"
#include "mode.h"
#include "keyboard_input.h"
#include "keyboard_device.h"
#include "alg_caculate.h"
#include "alg_linearlist.h"
//#include "lcd_12864.h"
//#include "eeprom_rw.h"
#include "normal_complex_caculate.h"
#include "function_pic.h"
#include "matrix.h"
#include "solve_equation.h"
#include "take_the_differential.h"
#include "statistics.h"
//#include "pwm.h"
#include "solve_differential_equation.h"
//#include "time.h"
//#include "change_units.h"
#include "middle.h"

#define CASE break;case

uint time_out = 0;		//节电超时

void mode_real_calculate(void);
void mode_complex_calculate(void);
void mode_change(void);
void mode_set(void);
void mode_sleep(void);

uchar mode_choose(void)
{
	switch(keyboard_output())
	{
		case '0': return 0;	//选择0

		CASE '1': return 1;	//选择1

		CASE '2': return 2;	//选择2

		CASE '3': return 3;	//选择3

		CASE '4': return 4;	//选择4

		CASE '5': return 5;	//选择5

		CASE '6': return 6;	//选择6

		CASE '7': return 7;	//选择7

		CASE '8': return 8;	//选择8

		CASE '9': return 9;	//选择9
				
		CASE 3 : return 10;	//上
		
		CASE 6 : return 11; //下
		
	}
	return 13;			//输入错误
}

void mode_select(void)
{
	uchar chProgramStatus;
	close_cursor();		//关光标

	chProgramStatus = byte_read(ADD_PROG_STATUS, 0);
	time_out = byte_read(ADD_TIMEOUT, 0);					//读取超时设定
	
	switch(chProgramStatus)
	{
		case NORMAL 		: normal_calculate();
		case COMPLEX		: complex_calculate();
		case MATRIX		 	: ju_main();
		case DRAW_PIC		: draw_pic();
		case SOLVE_EQUATION : solve_equation();
	//	case TAKE_DIFFER	: run_the_differential();
		case STASTISTICS	: sta_menu1();
		case MODE			: mode_change();
		case SET			: mode_set();
		case DIFFER_EQUATION: solve_differential_equation();
	//	case CHANGE_UNITS   : change_units();

	}
}

void mode_change(void)
{
	uchar m;
	uchar count1 = 1;
	uchar out = 0;
	uchar chStatus;
	
	while(1)
	{
		close_cursor();
		lcd_clear();

		if(count1>4)
			count1=4;
		else if(count1<1)
			count1=1;
		for(m=0;m<4;m++)
			{
				lcd_set_pos(m,0);	
				switch (m+count1)
					{
						case 1:
							lcd_write_str("1.普通计算");break;
						case 2:
							lcd_write_str("2.复数计算");break;
						case 3:
							lcd_write_str("3.矩阵运算");break;
						case 4:
							lcd_write_str("4.函数图像");break;
						case 5:
							lcd_write_str("5.解方程");break;
						case 6:
							lcd_write_str("6.统计");break;
						case 7:
							lcd_write_str("7.一阶常微分方程");break;
//						case 8:
//							lcd_write_str("8.一阶常微分方程");break;
				//		case 9:
				//			lcd_write_str("9.单位转化");break;
						}
				}	

		switch(mode_choose())
		{
			case 1 : chStatus = NORMAL;				out = 1;	break;
			case 2 : chStatus = COMPLEX;			out = 1;	break;
			case 3 : chStatus = MATRIX;				out = 1;	break;
			case 4 : chStatus = DRAW_PIC;			out = 1;	break;
			case 5 : chStatus = SOLVE_EQUATION;		out = 1;	break;
			case 6 : chStatus = STASTISTICS;		out = 1;	break;
			case 7 : chStatus = DIFFER_EQUATION;	out = 1;	break;
			case 10 : count1--;break;
			case 11 : count1++;break;
		}
		if(out == 1)
			break;
	}
	
	byte_write(ADD_PROG_STATUS, chStatus,0);		//写入当前指针
	bl_bright_change(byte_read(ADD_BRIGHT, 0));			//读取亮度
	
	reset();		//软复位
}

void mode_set(void)
{
	uchar m;
	uchar count1 = 1;
	uchar out = 0;
	
	uchar set_value;
	
	while(1)
	{
		close_cursor();
		lcd_clear();

		if(count1>3)
			count1=3;
		else if(count1<1)
			count1=1;
		for(m=0;m<4;m++)
			{
				lcd_set_pos(m,0);	
				switch (m+count1)
					{
						case 1:
							lcd_write_str("1.屏幕亮度");break;
						case 2:
							lcd_write_str("2.shift 提示");break;
						case 3:
							lcd_write_str("3.节电选项");break;
						case 4:
							lcd_write_str("4.日期设置");break;
						case 5:
							lcd_write_str("5.时钟设置");break;
						case 6:
							lcd_write_str("6.关于");break;
//						case 7:
//							lcd_write_str("7.返回");break;
						}
				}	
		
		
		switch(mode_choose())
		{
			case 10 : count1--;break;
			case 11 : count1++;break;				//移动屏幕
			
			case 1 : 
					
					while(1)
					{
						lcd_clear();
						lcd_inverse(1, 5, 1);							//反白
						lcd_set_pos(1,0);
						lcd_write_str("屏幕亮度");
						
						set_value = byte_read(ADD_BRIGHT, 0);			//读亮度
						bl_bright_change(set_value);
						
						lcd_set_pos(1,5);
						lcd_write_dat((uchar)(set_value+'0'));
						
						if(set_value > 1)
						{
							lcd_set_pos(2,5);
							lcd_write_dat(0x19);					//下箭头
						}
						if(set_value < 5)
						{
							lcd_set_pos(0,5);
							lcd_write_dat(0x18);					//上箭头
						}
						switch(keyboard())
						{
							case 4	:	if(set_value < 5)//上
											set_value ++;
										break;
										
							case 9	:	if(set_value>1)//下
											set_value --;
										break;
							case 25	:			//AC
							case 40	:	out = 1;//=
										break;
						}
						byte_write(ADD_BRIGHT, set_value, 0);			//写亮度
						if(out == 1)
						{
							out = 0;
							break;
						}
							
					}

					lcd_clear_pic();								//清反白
					break;

			case 2 : 
					while(1)
					{
						lcd_clear();
						lcd_inverse(1, 6, 1);							//反白
						lcd_set_pos(1,0);
						lcd_write_str(" shift提示");
						lcd_set_pos(3,1);
						lcd_write_str("通过改变亮度");
						
						set_value = byte_read(ADD_SHIFTSIGN, 0);			//读亮度
						bl_bright_change(set_value);
						
						lcd_set_pos(1,6);
						lcd_write_dat((uchar)(set_value+'0'));
						
						if(set_value > 1)
						{
							lcd_set_pos(2,6);
							lcd_write_dat(0x19);					//下箭头
						}
						if(set_value < 5)
						{
							lcd_set_pos(0,6);
							lcd_write_dat(0x18);					//上箭头
						}
						switch(keyboard())
						{
							case 4	:	if(set_value < 5)//上
											set_value ++;
										break;
										
							case 9	:	if(set_value>1)//下
											set_value --;
										break;
							case 25	:			//AC
							case 40	:	out = 1;//=
						}
						
						byte_write(ADD_SHIFTSIGN, set_value, 0);			//写亮度
						
						if(out == 1)
						{
							out = 0;
							break;
						}
							
					}
					bl_bright_change(byte_read(ADD_BRIGHT, 0));			//还原原来亮度
					lcd_clear_pic();								//清反白
					break;
					
			case 3 : 
					while(1)
					{
						lcd_clear();
						lcd_inverse(2, 4, 1);							//反白
						lcd_set_pos(0,0);
						lcd_write_str("节电模式");
						lcd_set_pos(2,1);
						lcd_write_str("无操作");
						lcd_set_pos(2,5);
						lcd_write_str("分钟后");
						
						set_value = byte_read(ADD_TIMEOUT, 0);			//读时间
						
						lcd_set_pos(2,4);
						lcd_write_dat((uchar)(set_value+'0'));
						
						if(set_value > 1)
						{
							lcd_set_pos(3,4);
							lcd_write_dat(0x19);					//下箭头
						}
						if(set_value < 9)
						{
							lcd_set_pos(1,4);
							lcd_write_dat(0x18);					//上箭头
						}
						switch(keyboard())
						{
							case 4	:	if(set_value < 9)//上
											set_value ++;
										break;
										
							case 9	:	if(set_value>1)//下
											set_value --;
										break;
							case 25	:			//AC
							case 40	:	out = 1;//=
										break;
						}
						
						byte_write(ADD_TIMEOUT, set_value, 0);			//写时间
						if(out == 1)
						{
							out = 0;
							break;
						}
							
					}
					lcd_clear_pic();								//清反白
					break;
			case 4 : general_dateset();			out = 1;	break;
			case 5 : general_timeset();			out = 1;	break;
			
			case 6 : about();					out = 1;	break;
//			case 7 : chProgramStatus = STASTISTICS;		out = 1;	break;
//			case 10 : count1--;break;
//			case 11 : count1++;break;
		}
	}
}

void mode_sleep(void)	//低功耗模式
{
	close_cursor();
	lcd_clear();
	bl_bright_change(1); //降低亮度
	page_time();
}


void reset(void)
{
	IAP_CONTR = 0x20;			//送0x20，软复位到程序区
}