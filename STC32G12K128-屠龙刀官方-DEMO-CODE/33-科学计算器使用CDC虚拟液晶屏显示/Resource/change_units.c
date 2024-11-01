#include <stdio.h>
#include <math.h> 
#include "change_units.h"
#include "alg_atof.h"
#include "keyboard_device.h"
#include "keyboard_input.h"
#include "alg_caculate.h"
#include "middle.h"

extern uchar num0;
extern double x;
extern uchar g_chCalcStatus;


//单位转化 
void change_units()
{
	uchar temp_buff[26];//用来存一些式子 
	uchar result_output[17];
	double a_shuzhi=0;
	double b_shuzhi=0;
	
	uchar judge=0xff;//判断值 
	
	uchar m;			//用于菜单上下移屏 
	uchar count1 = 1;
	
	//初始化 
	uchar i=0;
	for(i=0;i<26;i++)
	{
		temp_buff[i]=' ';
	}
	for(i=0;i<16;i++)
		result_output[i]=' ';
	result_output[16]=0;
	
	//进入主要步骤
	g_chCalcStatus = CALC_NORMAL; 
	num0=0xff;//初始化
	
	while(1) 
	{
		judge=0xff;
		//初始化菜单显示
		lcd_clear();
		lcd_clear_pic();
		
		if(count1>3)
			count1=3;
		else if(count1<1)
			count1=1;
		for(m=0;m<4;m++)
		{
			lcd_set_pos(m,0);	
			switch ('0'+m+count1)
			{
				case '1':
					lcd_write_str("1.长度");break;
				case '2':
					lcd_write_str("2.温度");break;
				case '3':
					lcd_write_str("3.能量");break;
				case '4':
					lcd_write_str("4.速度");break;
				case '5':
					lcd_write_str("5.角度");break;
				}
		}
		//按键选择菜单选项 
		num0=0xff;
		do
		{
			num0=keyboard_output();//读取键盘值, 直到按下1,2,3,4,5,6或者↑ ↓ 
		}while(num0!='1' && num0!='2' && num0!='3'&& num0!='4' && num0!='5' && num0!=3 &&num0!=6);
		
		//根据按键选择不同的功能 
		lcd_clear();
		lcd_clear_pic();
		
		switch(num0)
		{
			case 3:
				count1--;//菜单显示上移 
				break;
			case 6:
				count1++;//菜单显示下移 
				break;
			case '1'://1英寸=1.54cm 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.厘米-");
						lcd_write_dat(0x1a);
						lcd_write_str("英寸");
					lcd_set_pos(1,0); 
						lcd_write_str("2.英寸-");
						lcd_write_dat(0x1a);
						lcd_write_str("厘米");
					lcd_set_pos(2,0); 
						lcd_write_str("3.转化结果");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("厘米:");
						
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
								b_shuzhi=a_shuzhi/2.54;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("英寸:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*2.54;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("厘米:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("英寸:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '2'://华氏，摄氏 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.华氏-");
						lcd_write_dat(0x1a);
						lcd_write_str("摄氏");
					lcd_set_pos(1,0); 
						lcd_write_str("2.摄氏-");
						lcd_write_dat(0x1a);
						lcd_write_str("华氏");
					lcd_set_pos(2,0); 
						lcd_write_str("3.转化结果");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&&num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("华氏:");
						
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
								b_shuzhi=5/9*(a_shuzhi-32);
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("摄氏:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*9/5+32;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("华氏:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("摄氏:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					}
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '3'://1卡路里=4.184千焦
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.卡路里-");
						lcd_write_dat(0x1a);
						lcd_write_str("千焦");
					lcd_set_pos(1,0); 
						lcd_write_str("2.千焦-");
						lcd_write_dat(0x1a);
						lcd_write_str("卡路里");
					lcd_set_pos(2,0); 
						lcd_write_str("3.转化结果");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("卡路里:");
						
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
								b_shuzhi=a_shuzhi*4.184;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("千焦:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi/4.184;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("卡路里:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("千焦:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '4'://1英寸=1.54cm 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.英里/h-");
						lcd_write_dat(0x1a);
						lcd_write_str("公里");
					lcd_set_pos(1,0); 
						lcd_write_str("2.公里/h-");
						lcd_write_dat(0x1a);
						lcd_write_str("英里");
					lcd_set_pos(2,0); 
						lcd_write_str("3.转化结果");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("英里/h:");
						
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
								b_shuzhi=a_shuzhi*1.6092;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("公里/h:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*0.621427;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("英里/h:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("公里/h:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '5'://°，弧度 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.角度-");
						lcd_write_dat(0x1a);
						lcd_write_str("弧度");
					lcd_set_pos(1,0); 
						lcd_write_str("2.弧度-");
						lcd_write_dat(0x1a);
						lcd_write_str("角度");
					lcd_set_pos(2,0); 
						lcd_write_str("3.转化结果");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("角度:");
						
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
								b_shuzhi=a_shuzhi*0.017453;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("弧度:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*57.29578;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("角度:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("弧度:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
		}
	
	} 	
}
	


