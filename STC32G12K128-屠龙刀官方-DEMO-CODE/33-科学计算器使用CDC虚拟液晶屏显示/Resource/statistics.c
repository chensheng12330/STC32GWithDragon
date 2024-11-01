	#include <math.h>
	#include <middle.h>
#include "middle.h"
	#include "keyboard_device.h"



	extern uchar num0;
	
	void sta_delay()
	{
		uint i,j;
		for(i=0;i<1000;i++)
			for(j=0;j<1000;j++);
		}
	
	void cal_data(double (*a)[5],double (*c)[5],double *s,int n)
	{
////总和，平均数，中位数，最大值，最小值 ，极差 ，方差，修正方差，标准差，修正标准差
////变异系数，平均偏差，调和平均数，几何平均数
		uchar i,j;
		double t;
		double sum;
/*****************************************************/		
		sum=0;
		for(i=0;i<n;i++)
			sum=sum+*(*a+i);
		s[0]=sum;										 //求和
		s[1]=s[0]/n;								 //平均数
		
/*****************************************************/	
		for(i=0;i<n;i++)
			*(*c+i)=*(*a+i);
		for(i=0;i<n;i++)
			for(j=0;j<n-i-1;j++)
			{
				if(*(*c+j)>*(*c+j+1))
				{
					t=*(*c+j);
					*(*c+j)=*(*c+j+1);
					*(*c+j+1)=t;
					}
				}
		s[3]=*(*c+n-1);                 //最大值
		s[4]=*(*c);										  //最小值
		s[5]=s[3]-s[4];									//极差
		if(n%2==0)
			s[2]=(*(*c-1+n/2)+*(*c+n/2))/2;
		else if(n%2==1)
			s[2]=*(*c+(n-1)/2);           //中位数

/*****************************************************/

		sum=0;
		for(i=0;i<n;i++)
			sum=sum+1/(*(*a+i));
		s[12]=n/sum;                       //调和平均数

/*****************************************************/
		
		sum=1;
		for(i=0;i<n;i++)
			sum=sum*(*(*a+i));
		s[13]=pow(sum,1.0/(double)n);      //几何平均数     
		
/*****************************************************/	
	
		sum=0;
		for(i=0;i<n;i++)
			sum=sum+pow((*(*a+i)-s[1]),2);
		s[6]=sum/n;                        //方差
		s[7]=sum/(n-1);										 //修正方差
		s[8]=pow(s[6],0.5);									 //标准差
		s[9]=pow(s[7],0.5);								 //修正标准差
		s[10]=s[8]/s[1];                     //变异系数
	
/*****************************************************/

		t=0,sum=0;
		for(i=0;i<n;i++)
		{
			t=*(*a+i)-s[1];
			if(t<0)
				t=-t;
			sum=sum+t;
			}
		s[11]=sum/(float)n;                //平均偏差
		
/*****************************************************/

	}
	
	
	void mode_num(double (*a)[5],double (*c)[5],double (*b)[5],uchar *index,int n,int *count1)                      //众数
	{
		uchar i;
		int count=1;
    for(i=0;i<n-1;) 
		{
			while(*(*c+i)==*(*c+i+1)) 
			{
				count++;
				i++;
        }
			if(count>(*count1))
			{	
				(*count1)=count;
				(*index)=0;
				*(*b+*index)=*(*a+i);
        }
			else if(count==(*count1)) 
			{
        (*index)++;
				*(*b+*index)=*(*a+i);
				}
			count=1;
			i++;
			}

		}
		
		void sta_result(uchar count,double *s)
		{	
			char temp[16];
			uchar m,i,j;
			lcd_clear();
			for(m=0;m<4;m++)
			{
				i=m+count;
				lcd_set_pos(m,0);
				if(i%2==1)
				{
					j=(i+1)/2;
						switch (j) 
						{
							case 1:
								lcd_write_str("1.总和：");
								break;
							case 2:
								lcd_write_str("2.平均数：");
								break;
							case 3:
								lcd_write_str("3.中位数：");
								break;
							case 4:
								lcd_write_str("4.最大值：");
								break;
							case 5:
								lcd_write_str("5.最小值：");
								break;
							case 6:
								lcd_write_str("6.极差：");
								break;
							case 7:
								lcd_write_str("7.方差：");
								break;
							case 8:
								lcd_write_str("8.修正方差：");
								break;
							case 9:
								lcd_write_str("9.标准差：");
								break;
							case 10:
								lcd_write_str("10. 修正标准差：");
								break;
							case 11:
								lcd_write_str("11. 变异系数：");
								break;
							case 12:
								lcd_write_str("12. 平均偏差:");
								break;
							case 13:
								lcd_write_str("13. 调和平均数:");
								break;
							case 14:
								lcd_write_str("14. 几何平均数:");
								break;
							}
					}
				else if(i%2==0)
				{
						sprintf(temp,"%g",s[i/2-1]);
						lcd_write_str(temp);
					}
				}  
			}  
			
			
		void n_input(int *n)
		{
			uchar d=0;
			uchar i=0;
			char temp[16];
			lcd_clear();
			lcd_write_cmd(0x80);	
			lcd_write_str("Now n=");
			sprintf(temp,"%d",*n);
			lcd_write_str(temp);
			lcd_write_cmd(0x90);
			lcd_write_str("end with '='");
			lcd_write_cmd(0x88);
			lcd_write_str("    (2<=n<=25)");
			lcd_write_cmd(0x98);
			lcd_write_str("new=");
			for(d=0;d<16;d++)
				temp[d]=0;
			d=0;
			while(1)
				{
					num0=0xff;
					num0=keyboard_output();
					if((num0<=0x39)&&(num0>=0x30)&&(d<16))
						{
							temp[d]=num0;
							lcd_write_dat(num0);
							num0=0xff;
							d++;
							}
					else if((num0=='=')||(num0==19))
						{	
							num0=0xff;
							sscanf(temp,"%d",n);
							break;
							}
					else if(num0==18)
					{
						num0=0xff;
						if(d>0)
							d--;
						temp[d]=0;
						lcd_set_pos(3,2);
						lcd_write_str("            ");
						lcd_set_pos(3,2);
						for(i=0;i<d;i++)
							lcd_write_dat(temp[i]);
						}
					}
			}
			
		
		void num_input(double (*a)[5],int n,uchar *symbol1,int *count2)
		{
			uchar d=0;
			uchar i=0;
			char temp[16];
			lcd_clear();
			lcd_write_cmd(0x80);
			lcd_write_str("now data ");
			sprintf(temp,"%d",*count2);
			lcd_write_str(temp);
			lcd_write_cmd(0x90);
			sprintf(temp,"%g",*(*a+*count2-1));
			lcd_write_str(temp);
			lcd_write_cmd(0x88);
			lcd_write_dat(0x1e);   //上
			lcd_write_str(":上翻  ");
			lcd_write_dat(0x1f);   //下
			lcd_write_str(":下翻");
			lcd_write_cmd(0x98);
			lcd_write_str("new=");
			for(d=0;d<16;d++)
				temp[d]=0;
			d=0;
			num0=0xff;
			while(1)
			{
				num0=keyboard_output();
				if(num0==3)
					{
						num0=0xff;
						if(d!=0)
							sscanf(temp,"%g",(*a+(*count2)-1));
						(*count2)=(*count2)-1;
						if((*count2)<1)
							(*count2)=1;
						break;
					}
				else if((num0==6)||(num0=='='))
					{	
						num0=0xff;
						if(d!=0)
							sscanf(temp,"%g",(*a+(*count2)-1));
						(*count2)=(*count2)+1;
						if((*count2)>n)
							(*count2)=n;
						break;
					}
				else if(num0==19)
					{
						num0=0xff;
						(*symbol1)=1;
						break;
						}
				else if(num0==18)
				{
					num0=0xff;
					if(d>0)
						d--;
					temp[d]=0;
					lcd_set_pos(3,2);
					lcd_write_str("            ");
					lcd_set_pos(3,2);
					for(i=0;i<d;i++)
						lcd_write_dat(temp[i]);
				}
				else if(((num0<=0x39)&&(num0>=0x30))||(num0=='.')||(num0=='-'))
					{
						lcd_write_dat(num0);
						temp[d]=num0;
						num0=0xff;
						d++;
						}
				else num0=0xff;
				}
		}
			
		
		void sta_mode_display(double (*b)[5],uchar index,int *count1)
		{
			uchar temp[16];
			uchar count5=0;
			uchar i,j;
			num0=0xff;
			lcd_clear();
			for(i=0;i<16;i++)
				temp[i]=0;
				
				if((*count1)==1)
				{
					lcd_set_pos(1,2);
					lcd_write_str("没有众数");
					lcd_set_pos(2,2);
					lcd_write_str("没有众数");
					sta_delay();
					lcd_clear();
					}
				else if((*count1)>1)
				{
					i=index+1;
					while(1)
					{
						if(i<=4)
						{
							for(j=0;j<i;j++)
							{
								lcd_set_pos(j,0);
								sprintf(temp,"%g",*(*b+j));
								lcd_write_str(temp);
								}
							}
						else if(i>4)
						{
							for(j=0;j<4;j++)
							{	
								lcd_set_pos(j,0);
								sprintf(temp,"%g",*(*b+j+count5));
								lcd_write_str(temp);
								}
							}
						num0=0xff;
						num0=keyboard_output();
						if(num0==19)
						{
							num0=0xff;
							break;
							}
						else if(num0==3)
						{
							num0=0xff;
							if(count5>0)
								count5--;
							}
						else if(num0==6)
						{
							num0=0xff;
							count5++;
							if(count5>i-4)
								count5=i-4;
							}
						}
				}
			
		
			}
		
		void sta_result_display(double *s)
		{
			uchar count4=1;
			num0=0xff;
			while(1)
			{

				sta_result(count4,s);
				num0=keyboard_output();
				if(num0==6)
				{
					num0=0xff;
					count4++;
					if(count4>25)
						count4=25;
					}
				else if(num0==3)
				{
					num0=0xff;
					count4--;
					if(count4<1)
						count4=1;
					}
				else if(num0==19)
				{
					num0=0xff;
					break;
					}
				}
			}
			
			
		
		void sta_menu1()
		{
			double a[5][5],b[5][5],c[5][5];
			double s[14];
			uchar index;
			uchar i=0;
			uchar j=0;
			int n=1;
			int count1=1;
			int count2=1;
			uchar symbol1=0;
			for(i=0;i<5;i++)
				for(j=0;j<5;j++)
					a[i][j]=0;
			num0=0xff;
				while(1)
				{
					num0=0xff;
					lcd_clear();
					lcd_write_cmd(0x80);
					lcd_write_str("1.输入数据个数");
					lcd_write_cmd(0x90);
					lcd_write_str("2.输入数据");
					lcd_write_cmd(0x88);
					lcd_write_str("3.计算");
					lcd_write_cmd(0x98);
					lcd_write_str("0.返回");
					num0=keyboard_output();
					if(num0=='1')
					{	
						num0=0xff;
						n_input(&n);
						while((n>25)||(n<2))					
							n_input(&n);
							}
						
					else if(num0=='2')
					{
						num0=0xff;
						while(symbol1==0)
							num_input(a,n,&symbol1,&count2);
						symbol1=0;
						}
					else if(num0=='3')
					{
						num0=0xff;
						cal_data(a,c,s,n);
						mode_num(a,c,b,&index,n,&count1);
						while(1)
						{
							lcd_clear();
							lcd_write_cmd(0x80);
							lcd_write_str("1.查看数据结果");
							lcd_write_cmd(0x90);
							lcd_write_str("2.查看众数");
							lcd_write_cmd(0x88);
							lcd_write_str("0.返回");
							num0=keyboard_output();
							if(num0=='1')
							{
								num0=0xff;
								lcd_clear();
								sta_result_display(s);
								}
							else if(num0=='2')
							{
								num0=0xff;
								lcd_clear();
								sta_mode_display(b,index,&count1);
								}
							else if(num0=='0')
							{	
								lcd_clear();
								num0=0xff;
								break;
								}
							}
							}
						else if(num0=='0')
						{
							num0=0xff;
							break;
						
						}
					}
				}
			
			
		
			