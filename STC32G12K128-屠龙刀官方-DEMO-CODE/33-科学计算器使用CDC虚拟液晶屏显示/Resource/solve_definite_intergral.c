#include <stdio.h>
#include <math.h> 
#include "solve_definite_intergral.h"
#include "middle.h"
#include "alg_atof.h"
#include "keyboard_device.h"
#include "keyboard_input.h"
#include "alg_caculate.h"

extern uchar num0;
extern double x;
extern uchar g_chCalcStatus;


//求定积分
void solve_definite_intergral()
{
	uchar hanshu_buff[26];//用来存函数表达式 
	uchar temp_buff[26];//用来存一些式子 
	uchar result_out[17];
	double a_shuzhi=0;
	double b_shuzhi=0; 
	double h_shuzhi=0;
	
	unsigned long int n=1;   //子区间个数
	double temp_0  =0;//临时值 
	double temp_1  =0;
	double tn,t2n;  //定积分tn和t2n
	uchar judge=0xff;//判断值
	uchar i;
	//初始化 
	for(i=0;i<26;i++)
	{
		hanshu_buff[i]=temp_buff[i]=' ';
	}
	for(i=0;i<16;i++)
		result_out[i]=' ';
	result_out[16]=0;
	
	//进入主要步骤
	g_chCalcStatus = CALC_NORMAL; 
	num0=0xff;//初始化
	
	while(1) 
	{
		judge=0xff;
		//初始化菜单显示
		lcd_clear();
		lcd_clear_pic();
		
		for(i=0;i<4;i++)
		{
			lcd_set_pos(i,0);	
			switch ('0'+i+1)
			{
				case '1':
					lcd_write_str("1.积分函数 y =");break;
				case '2':
					lcd_write_str("2.左端点a");break;
				case '3':
					lcd_write_str("3.右端点b");break;
				case '4':
					lcd_write_str("4.结果");break;
				}
		}
		//按键选择菜单选项 
		do
		{
			num0=keyboard_output();//读取键盘值, 直到按下1,2,3,4
		}while(num0!='1' && num0!='2' && num0!='3'&& num0!='4');
		
		//根据按键选择不同的功能 
		lcd_clear();
		lcd_clear_pic();
		
		switch(num0)
		{
			case '1':
				lcd_set_pos(0,0);
				lcd_write_str("积分函数 y =");
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
				lcd_write_str("左端点 a=");
				lcd_set_pos(1,0);
				sprintf( temp_buff,"%g",a_shuzhi ); 
				lcd_write_str(temp_buff);
				//读取a
//look			
				for(i=0;i<26;i++)
				{
					temp_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,temp_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					a_shuzhi = alg_atof(temp_buff);	
				}
				break;
				
			case '3': 
				lcd_set_pos(0,0);
				lcd_write_str("右端点 b=");
				lcd_set_pos(1,0);
				sprintf( temp_buff,"%g",b_shuzhi ); 
				lcd_write_str(temp_buff);
				//读取b
//look			
				for(i=0;i<26;i++)
				{
					temp_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,temp_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					b_shuzhi = alg_atof(temp_buff);	
				}
				break;
			
				
			case '4'://输出结果 
				if(a_shuzhi>b_shuzhi)
				{
					lcd_set_pos(0,0);
					lcd_write_str("a>b,wrong");
				} 
				else
				{	   
				    n=1;   //子区间个数
				    h_shuzhi=b_shuzhi-a_shuzhi;
				  //  uchar alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult);
				  
				  //t2n=h*( f(a)+f(b) )/2 ;
				  
					x=a_shuzhi;
					alg_calc_expression(hanshu_buff, &temp_0, 0);
					x=b_shuzhi;
					alg_calc_expression(hanshu_buff, &temp_1, 0);
					t2n=h_shuzhi*(temp_0+temp_1)/2;
					
				    do
				    {
				        tn=t2n;
				        h_shuzhi/=2;
				        t2n=0;
				        for(i=0;i<=n-1;i++)
				        {
				            x=a_shuzhi+h_shuzhi*(2*i+1);
				            alg_calc_expression(hanshu_buff, &temp_0, 0);
				            t2n+=temp_0;
				        }
				        t2n=tn/2+t2n*h_shuzhi;
				        n*=2;
				        if(n>16777216) //为了大运算的速度舍弃了一部分精度
				//      if(n>=268435456)
				//      if(n>2147483647/2-1)    //c51的数据类型中，unsigned long int是0~2147483647.设置一个判断，防止溢出
				            break;
				//      printf("n=%ld\n",n);//调试用的语句
				    }while(fabs(t2n-tn)>0.00001);//确保了小运算的精度
					
				    lcd_set_pos(0,0);
					lcd_write_str("积分的结果是：");
					lcd_set_pos(1,0);
					sprintf(result_out,"%g",t2n);
					lcd_write_str(result_out);
				} 
				while(keyboard_output()!=19);
				break;
		} 	
	}
	
}


