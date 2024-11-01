#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "normal_complex_caculate.h"
#include "middle.h"
#include "keyboard_input.h"
#include "keyboard_device.h"
#include "alg_caculate.h"
#include "alg_linearlist.h"
//#include "eeprom_rw.h"

#define HISTORY_SIZE 60

uchar code history_add[] = {0,26,52,78,104,130,156,182,208};		//历史纪录回调地址

uchar g_chCalcStatus;
double g_ans;

void push_history(char *pExpression);
void pop_history(char *pExpression, char loc);


void normal_calculate(void)
{
	uchar i,flag;
	char input_buf[26];
	char output_buf[16];
	uchar chInputStatus = INPUT_VACANT;
	
	char top,bottom,current;	//用于历史纪录浏览
	double re_result;
		
	uchar num1 = 0xff;

	top = 0;
	bottom = 0;
	
	lcd_clear();
	g_chCalcStatus = CALC_NORMAL;
	
	while(1)
	{
		if(INPUT_VACANT == chInputStatus)		//悬空则需要输入
		{
			for(i=0;i<25;i++)
				input_buf[i]=' ';		//清空串
			chInputStatus = input_to_str(0,input_buf,num1);
		}		
		
		if(INPUT_EQ == chInputStatus)
		{
			chInputStatus = INPUT_VACANT;	//悬空
			if(LArray_TRUE == alg_calc_expression(input_buf, &re_result, NULL))
			{
				sprintf(output_buf,"%g",re_result);
				g_ans = re_result;					//更新ans
				lcd_set_pos(3,0);
				lcd_write_str("                ");
				lcd_set_pos(3,0);
				lcd_write_str(output_buf);
				push_history(input_buf);			//压入历史纪录
				current = byte_read(ADD_TOP_HISTORY, 0);	//更新当前位置
			}
			else 
			{
				sprintf(output_buf,"INPUT ERROR");
				lcd_set_pos(3,0);
				lcd_write_str("                ");
				lcd_set_pos(3,0);
				lcd_write_str(output_buf);
			}
			
		}

		else if(INPUT_UP == chInputStatus || INPUT_DOWN == chInputStatus)	//上下翻页
		{
			flag = 0xff;
			if(INPUT_UP == chInputStatus)		//上翻页
			{
				bottom = byte_read(ADD_BOTTOM_HISTORY, 0);		//读取当前顶部和底部序号
				if(current-1!=bottom || (current == 0 && bottom != HISTORY_SIZE-1))					//没有翻到		访问current-1
				{	
					if(current == 0)
						current = HISTORY_SIZE-1;
					else
						current -= 1;
					
					flag = 1;
				}
				else 
					chInputStatus = input_to_str(0,input_buf,0xff);		//写表达式
			}
			
			else if(INPUT_DOWN == chInputStatus)
			{
				top = byte_read(ADD_TOP_HISTORY, 0);
				if(current!=top)					//没有翻到底
				{	
					if(current == HISTORY_SIZE-1)
						current = 0;
					else
						current += 1;
				}
				flag = 1;
			}
			
			if(flag == 1)
			{
				flag = 0xff;
				for(i=0;i<26;i++)
					input_buf[i]=' ';		//清空串
				pop_history(input_buf, current);	//读取值
				lcd_clear();						//清屏
				if(LArray_TRUE == alg_calc_expression(input_buf, &re_result, NULL))		//显示结果
				{
					sprintf(output_buf,"%g",re_result);
					lcd_set_pos(3,0);
					lcd_write_str("                ");
					lcd_set_pos(3,0);
					lcd_write_str(output_buf);
				}
				
				chInputStatus = input_to_str(0,input_buf,0xff);		//写表达式
			}
		}

		
				
		if(INPUT_AC == chInputStatus)
		{
			lcd_clear();
			num1 = 0xff;
			chInputStatus = INPUT_VACANT;	//指针悬空
			current = byte_read(ADD_TOP_HISTORY, 0);	//历史归位
			if(current == HISTORY_SIZE-1)
				current = 0;
			else
				current += 1;
			continue;
		}
		
		if(INPUT_VACANT == chInputStatus)
		{
			num1=keyboard_output();
			lcd_clear();				//清屏
		}
	}
}

void complex_calculate(void)
{
	uchar i;
	char input_buf[26];
	char output_buf[16];
	uchar chInputStatus;
	double re_result,im_result;
	uchar num1 = 0xff;
	lcd_clear();
	g_chCalcStatus = CALC_COMPLEX;
	
	while(1)
	{
		for(i=0;i<25;i++)
			input_buf[i]=' ';		//清空串
		do
		{
			chInputStatus = input_to_str(0, input_buf,num1);			//输入串
		}while(INPUT_EQ != chInputStatus && INPUT_AC != chInputStatus);
		
		if(INPUT_EQ == chInputStatus)
		{
			if(LArray_TRUE == alg_calc_expression(input_buf, &re_result, &im_result))
			{
				if(im_result <0.0)
					sprintf(output_buf,"%g%gi",re_result,im_result);
				else
					sprintf(output_buf,"%g+%gi",re_result,im_result);
			}
			else 
				sprintf(output_buf,"INPUT ERROR");
			lcd_set_pos(3,0);
			lcd_write_str("                ");
			lcd_set_pos(3,0);
			lcd_write_str(output_buf);
		}
		
		else if(INPUT_AC == chInputStatus)
		{
			lcd_clear();
			num1 = 0xff;
			continue;
		}
		
		num1=keyboard_output();
		lcd_clear();

	}
}

void push_history(char *pExpression)
{
	char top, bottom;
	char i;
	
	top = byte_read(ADD_TOP_HISTORY, 0);
	bottom = byte_read(ADD_BOTTOM_HISTORY, 0);		//读取当前顶部和底部序号

	if(bottom == 0 && top == HISTORY_SIZE-1)	//当前已经被写满
	{
		bottom += 1;
		top = 0;
	}
	else if(bottom - top == 1)					//当前已经被写满							
	{
		if(bottom == HISTORY_SIZE-1)
			bottom = 0;
		else 
			bottom += 1;
		if(top == HISTORY_SIZE-1)
			top = 0;
		else
			top += 1;
	}
	else
	{
		top += 1;
	}
	
	for(i=0;i<26;i++)
	{
		byte_write(history_add[top%9]+i, pExpression[i], top/9+1);		//写入表达式
		if(pExpression[i] == '\0')
			break;
	}
	
	byte_write(ADD_TOP_HISTORY, top, 0);
	byte_write(ADD_BOTTOM_HISTORY, bottom, 0);		//写回
}

void pop_history(char *pExpression,char loc)
{
	char i;
		
	for(i=0;i<26;i++)
	{
		pExpression[i] = byte_read(history_add[loc%9]+i, loc/9+1);		//写入表达式
		if(pExpression[i] == '\0')
			break;
	}

}