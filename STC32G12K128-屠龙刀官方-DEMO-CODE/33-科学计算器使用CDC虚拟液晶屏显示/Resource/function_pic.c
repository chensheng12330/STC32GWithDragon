	//#include "STC12C5A60S2.h"
	#include <math.h>
	#include <stdio.h>
	//#include "lcd_12864.h"
	#include "keyboard_input.h"
	#include "keyboard_device.h"
	#include "function_pic.h"
	#include "alg_caculate.h"
	#include "alg_linearlist.h"
	#include "middle.h"

	
	extern	uchar g_chCalcStatus;
	extern	uchar num0;
	extern int count1;	
		
	double x0=0,y0=0;
	double ky=1;
	double kx=1;
	double x,y,z;
	
	void draw_pic_input_err(void)
	{
		lcd_clear();
		lcd_set_pos(1,2);
		lcd_write_str("输入错误");

		delay_2s();
		lcd_clear();
	}
	
	void display(char *temp1)    //显示函数图像函数
	{
		uchar i;
		int c;
	
		lcd_clear_pic();
		if((x0<=127)&&(x0>=0)) 
		{
			lcd_yline(x0,0,63,1);
			if((x0<=63)&&(x0>=2))
			{
				lcd_put_point(x0-1,1);
				lcd_put_point(x0-2,2);
				}
			if((x0<=61)&&(x0>=0))
			{
				lcd_put_point(x0+1,1);
				lcd_put_point(x0+2,2);
				}
			for(i=0;i<=63;i++)
				{
						c=i-y0;
						if(c<0) c=-c;
						if(c%10==0)
							{
								if(x0==0)
									lcd_xline(0,1,i);
								else if(x0==127)
									lcd_xline(126,127,i);
								else 
									lcd_xline(x0-1,x0+1,i);
							}
				}
                OLED_BuffShow();//新增
			}
		if((y0>=0)&&(y0<=63))
		{
			lcd_xline(0,127,y0);
				if((y0<=63)&&(y0>=2))
					{
						lcd_put_point(126,y0-1);             
						lcd_put_point(125,y0-2);
						}
				if((y0<=61)&&(y0>=0))
					{
						lcd_put_point(126,y0+1);
						lcd_put_point(125,y0+2);
						}
			for(i=0;i<=127;i++)
				{
				c=i-x0;
				if(c<0) c=-c;
				if(c%10==0)
					{
						if(y0==0)
							lcd_yline(i,0,1,1);
						else if(y0==63)
							lcd_yline(i,62,63,1);
						else 
							lcd_yline(i,y0-1,y0+1,1);
					}
		}
                OLED_BuffShow();
	}
		for(i=0;i<=127;i++)
		{
			x=(i-x0)*kx;
			g_chCalcStatus = CALC_NORMAL;
			alg_calc_expression(temp1, &y, 0);
			     ////////////********************
			y *= ky;
			y=(y0-y);
			if((y<=63)&&(y>=0))
				lcd_put_point(i,y);
			}
        OLED_BuffShow();
	}
	void pic_menu()    //显示函数图像的菜单
	{			
		uchar m;
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
							lcd_write_str("1.原点坐标X0");break;
						case 2:
							lcd_write_str("2.原点坐标y0");break;
						case 3:
							lcd_write_str("3.横坐标单位长度");break;
						case 4:
							lcd_write_str("4.纵坐标单位长度");break;
						case 5:
							lcd_write_str("5.图像表达式");break;
						case 6:
							lcd_write_str("6.显示图像");break;
//						case 7:
//							lcd_write_str("0.返回");break;
						}
				}	
		}
	void pic_num()    //函数图像的相关值的改变
	{	
		char temp[13];
		uchar i=0;
		uchar d=0;
		uchar num=num0;
		for(i=0;i<13;i++)
			temp[i]=' ';
		i=0;
		lcd_clear();
		switch(num)
		{
			case '1':	
				sprintf(temp,"%g",x0);break;
			case '2':
				sprintf(temp,"%g",y0);break;
			case '3':	
				sprintf(temp,"%g",kx);break;
			case '4':	
				sprintf(temp,"%g",ky);break;
			}
		lcd_write_cmd(0x80);
		switch(num)
		{
			case '1':	
				lcd_write_str("Now x0 is");break;
			case '2':
				lcd_write_str("Now y0 is");break;
			case '3':	
				lcd_write_str("Now kx is");break;
			case '4':	
				lcd_write_str("Now ky is");break;
		}
		lcd_write_cmd(0x90);
		lcd_write_str(temp);
		lcd_write_cmd(0x88);
		lcd_write_str("end with '='");
		lcd_write_cmd(0x98);
		lcd_write_str("new=");
		while(1)
		{	
			num0=0xff;
			num0=keyboard_output();
			if(((num0<=0x39)&&(num0>=0x30))||(num0=='.')||(num0=='-'))
			{
				lcd_write_dat(num0);
				temp[i]=num0;
				i++;
				}
			else if(num0=='=')
			{			
			    temp[i]='\0';
				if((i<13)&&(i>0))
					switch(num)
					{
						case '1':	
							sscanf(temp,"%f",&x0);break;
						case '2':
							sscanf(temp,"%f",&y0);break;
						case '3':	
							sscanf(temp,"%f",&kx);break;
						case '4':	
							sscanf(temp,"%f",&ky);break;
							}
					num0=0xff;
					break;
				}
			else if(num0==18)
			{
				num0=0xff;
				if(i>0)
					i--;
				temp[i]=0;
				lcd_set_pos(3,2);
				lcd_write_str("            ");
				lcd_set_pos(3,2);
				for(d=0;d<i;d++)
					lcd_write_dat(temp[d]);
				}
			else if(num0==19)
				{
					num0=0xff;
					break;
					}
		}		
	}
	
 	void draw_pic()
	{	
		
		uchar i,flag = 0;
		char temp1[26];
		for(i=0;i<26;i++)
			temp1[i]=0;
		temp1[0]='0';
		temp1[1]='\0';	
		x0=x0+63;
		y0=y0+31;
		count1=0;
		while(1)
		{
			lcd_clear_pic();
			num0=0xff;
			pic_menu();
			num0=keyboard_output();
			if((num0=='1')||(num0=='2')||(num0=='3')||(num0=='4'))
			{
				pic_num();
				num0=0xff;
				}
			else if(num0==6)
			{
				num0=0xff;
				count1++;
				if(count1>4) count1=4;
				}
			else if(num0==3)
			{
				num0=0xff;
				count1--;
				if(count1<1) count1=1;
				}
			else if(num0=='5')         //***********************************
			{
				while(1)
				{
					num0=0xff;
					lcd_clear();
					lcd_write_cmd(0x80);
					lcd_write_str("输入函数表达式");
					lcd_write_cmd(0x90);
					lcd_write_str("F(x)=");
					
					for(i=0;i<25;i++)
						temp1[i]=' ';		//清空串
					flag = input_to_str(2, temp1, 0xff);
					if(flag == INPUT_EQ)
					{
						g_chCalcStatus = CALC_NORMAL;
						if(LArray_FALSE == alg_calc_expression(temp1, NULL, NULL))
						{	
							draw_pic_input_err();
						}
						else
						{
							break;
						}
					}
					else if(flag == INPUT_AC)
					{
						break;
					}
				
				}
				
				
			
			}
			
			else if(num0=='6')
			{
				lcd_clear();
				while(1)
				{	
					num0=0xff;
					lcd_clear_pic();
					display(temp1);
					while((num0!='1')&&(num0!='2')&&(num0!='4')&&(num0!='5')&&(num0!=3)&&(num0!=5)&&(num0!=6)&&(num0!=7)&&(num0!=19))
						num0=keyboard_output();
					if(num0=='1')
						ky=ky*2;
					else if(num0=='2')
						ky=ky/2;
					else if(num0==7)
						x0=x0-10;		
					else if(num0==5)
						x0=x0+10;	
					else if(num0==6)
						y0=y0-10;	
					else if(num0==3)
						y0=y0+10;	
					else if(num0=='4')
						kx=kx*2;	
					else if(num0=='5')
						kx=kx/2;
					else if(num0==19)
					{
						num0=0xff;
						break;
						}
				}
			}
//			else if(num0=='0' || num0==1)		//0或者mode退出
//			{
//				num0=0xff;
//				break;
//				}
		}
	}
	
	