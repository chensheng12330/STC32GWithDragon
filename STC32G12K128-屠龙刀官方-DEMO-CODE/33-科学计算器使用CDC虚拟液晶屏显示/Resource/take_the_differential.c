#include <stdio.h>
#include <math.h> 
#include "take_the_differential.h"
#include "middle.h"
#include "alg_atof.h"
#include "keyboard_device.h"
#include "keyboard_input.h"
#include "alg_caculate.h"

#define PRECISION 0.01
extern uchar num0;
extern double x;
extern uchar g_chCalcStatus;
void take_the_derivative( uchar *hanshu_buff, double x0 , uchar *result_output) ;//传进 函数    一个的指针 ( 用来返回结果 ) ;

//求导函数 
void run_the_differential()
{
	uchar hanshu_buff[26];//用来存函数表达式 
	uchar x0_buff[26];//用来存函数表达式 
	uchar result_output[17];//存放结果
	double x0=0;
	uchar judge=0xff;
	//初始化 
	uchar i=0;
	for(i=0;i<26;i++)
	{
		hanshu_buff[i]=x0_buff[i]=' ';
	}
	for(i=0;i<17;i++)
	{
		result_output[i]=' ';
	}
	//进入主要步骤
	g_chCalcStatus = CALC_NORMAL; 
	num0=0xff;//初始化
	while(1) 
	{
		judge=0xff;
		//初始化菜单显示
		lcd_clear();
		lcd_clear_pic();
		lcd_set_pos(0,0);
		lcd_write_str("1 输入函数"); 
		lcd_set_pos(1,0);
		lcd_write_str("2 输入x0"); 
		lcd_set_pos(2,0);
		lcd_write_str("3 求导结果"); 
//		
		num0=0xff;
		do
		{
			num0=keyboard_output();//读取键盘值, 直到按下1,2,3
		}while(num0!='1' && num0!='2' && num0!='3' );
		
		
		lcd_clear();
		lcd_clear_pic();
		
		switch(num0)
		{
			case '1':
				lcd_set_pos(0,0);
				lcd_write_str("y =");
				lcd_set_pos(1,0);
				for(i=0; i<16 && hanshu_buff[i]!=0 ; i++ )
				{
					lcd_write_dat( hanshu_buff[i] ) ;
					if(i==15)
					{
						lcd_set_pos(2,0);
						for(i=16; i<25 && hanshu_buff[i]!=0 ; i++ )
						{
							lcd_write_dat( hanshu_buff[i] ) ;
						}
					}	
				} 
				//读取函数
			
						//在第3行输入函数 
				do
				{
					for(i=0;i<26;i++)
					{
						hanshu_buff[i]=0;
					} 
					judge = input_to_str(3,hanshu_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				
				break;
				
				
			case '2': 
				lcd_set_pos(0,0);
				lcd_write_str("x0 =");
				lcd_set_pos(1,0);
				sprintf( x0_buff,"%g",x0 ); 
				lcd_write_str(x0_buff);
				//读取x0 
//look			
				for(i=0;i<26;i++)
				{
					x0_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,x0_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					x0 = alg_atof(x0_buff);	
				}
				break;
				
			case '3':
				lcd_set_pos(0,0);
				lcd_write_str("x0 =");
				lcd_set_pos(1,0);
				sprintf( x0_buff,"%g",x0 ); 
				lcd_write_str(x0_buff);
				lcd_set_pos(2,0);
				lcd_write_str("y' =");
				//运行求导程序 
				take_the_derivative( hanshu_buff, x0, result_output );
				while(keyboard_output()!=19);
				
				break;
		} 	
	}
	
	 
	
}


	
/*************求导子函数*********************
//传入：double x0 ，存放函数的字符串							    	*/ 	

/*********调试思路 ******** 
//假设函数是 y = 1/x ；

//判断

//1 是否在定义域内 

//2 不可导？
//	a 判断的时候，加上dx之后根本就没有这个值 ——>还需要再调用判断函数，判断加上dx之后是不是在定义域内
//	b 正负不等->相差过大	//用两个导数的差值 除以 他们的和，判断一下差值占比大小，太大就是不可导 

//3 还不够精确？ 
// 精度所限，直接用最高精度的dx去算就好了，没办法解决 

	//interesting ,/0 bug,/0.0可通过			*/
	 
void take_the_derivative( uchar *hanshu_buff, double x0 , uchar *result_output) //传进 函数    一个的指针 ( 用来返回结果 ) 
{
    
	double dx= 0.000001; 
	
	double positive_derivative = 0.0;
	double negative_derivative = 0.0;
	
	double differential_result = 0.0;
	
	uchar flag_wrong  = 0;
	uchar i=0;
	double temp_0=0;
	double temp_1=0;
	

//	uchar result_output[17];//用来存放结果 
	for(i=0;i<16;i++)
	{
		result_output[i]=' ';
	}
	result_output[16]=0;
	
//    alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult);
		x = x0; 
		if( alg_calc_expression( hanshu_buff , 0 , 0 )==1 )
		{
			x = x0 + dx;
			if( alg_calc_expression( hanshu_buff , 0 , 0 )==1 )
			{
				x = x0 - dx;
				if( alg_calc_expression( hanshu_buff , 0 , 0 )==1 )
					;
				else
					flag_wrong=1;
			}
			else
				flag_wrong=1;
		}
		else 
			flag_wrong=1;
			
		if(flag_wrong==0)
		{
			x = x0;
			alg_calc_expression( hanshu_buff , &temp_1  , 0 );
			
			x = x0 + dx;
			alg_calc_expression( hanshu_buff , &temp_0  , 0 );
			
			positive_derivative = ( temp_0 - temp_1  )/dx; 
			
			x = x0 - dx;
			alg_calc_expression( hanshu_buff , &temp_0  , 0 );
			
			negative_derivative = ( temp_0 - temp_1  )/(-dx);
		
			
//			if(  fabs(   ( positive_derivative - negative_derivative )
//		    	 	/( positive_derivative + negative_derivative ) ) < PRECISION )
//								//这里的精度用来判断是否正负两个导数相差过大，即判断是否该点可导 
//			{
				differential_result = ( positive_derivative + negative_derivative ) /2;
				
				sprintf(result_output, "%g",differential_result);
				lcd_set_pos(3,0);
				lcd_write_str( result_output);
//			}
//			else
//			{
//				//提示错误 	
//				lcd_set_pos(3,0);
//				lcd_write_str("不可导");	
//			}
		} 
//		else if(flag_wrong==1)	//提示错误
//		{
//			lcd_set_pos(3,0);
//			lcd_write_str( "输入不合法" );
//		}
		
	
}


///*************判断是否在定义域内*************
//传入：f(x)的运算结果 double f
//返回：TRUE 
//	  FALSE							    	*/ 
//	  
//int judge_domain_of_definition( double f ) 
//{
//	char str[8];
//	char str_error[]="1.#INF";	//错误值 
//	char str_error0[]="-1.#INF";
//	gcvt( f, 7,str);			//利用gcvt函数，把f转化成字符串，与错误值对比
//	if(strcmp(str,str_error)==0 ||strcmp(str,str_error0)==0 )//判断一下是否在定义域内，如果不在定义域内，会返回0，并且提示 "您输入的x不在此函数的定义域内"  
//	{
//		printf("您输入的x不在此函数的定义域内\n");
//		return FALSE;
//	}
//	else
//		return TRUE;	
//} 

