#include "stc.h"
#include "usb.h"
#include <stdio.h>
#include <math.h>
#include "middle.h"
#include "keyboard_device.h"


extern uchar num0;

void er_n(int *n)
{
	uchar d=0;
	uchar i=0;
	char temp[16];
	lcd_clear();
	lcd_set_pos(0,0);//lcd_write_cmd(0x80);	
	lcd_write_str("Now n=");
	sprintf(temp,"%d",*n);
	lcd_write_str(temp);
	lcd_set_pos(0,1);//lcd_write_cmd(0x90);
	lcd_write_str("end with '='");
	lcd_set_pos(0,2);//lcd_write_cmd(0x88);
	lcd_write_str("    (2<=n<=25)");
	lcd_set_pos(0,3);//lcd_write_cmd(0x98);
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
				lcd_set_pos(4,3);lcd_write_dat(num0);//lcd_write_dat(num0);
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
			//lcd_set_pos(3,2);
			for(i=0;i<d;i++)
            {
                lcd_set_pos((uchar)(3+i),2);
                lcd_write_dat(temp[i]);//lcd_write_dat(temp[i]);
            }		
		}
	}
}

void er_num(int n,double (*a)[25],int *count2,uchar *symbol1)
{
	uchar d=0;
	uchar i=0;
	char temp[16];
	for(d=0;d<16;d++)
		temp[d]=0;
	d=0;
	lcd_clear();
	lcd_set_pos(0,0);//lcd_write_cmd(0x80);
	if((*count2)%2==1)
	{	
		lcd_write_str("now data x");
		sprintf(temp,"%d",((*count2)+1)/2);
		}
	else if((*count2)%2==0)
	{
		lcd_write_str("now data y");
		sprintf(temp,"%d",(*count2)/2);
		}
	lcd_write_str(temp);
	lcd_set_pos(0,1);//lcd_write_cmd(0x90);
	if((*count2)%2==1)
		sprintf(temp,"%g",a[((*count2)-1)/2][0]);
	else if((*count2)%2==0)
		sprintf(temp,"%g",a[((*count2)/2)-1][1]);
	lcd_write_str(temp);
	lcd_write_cmd(0x88);
	lcd_write_dat(0x1e);   //上
	lcd_write_str(":上  ");
	lcd_write_dat(0x1f);   //下
	lcd_write_str(":下");
	lcd_write_cmd(0x98);
	lcd_write_str("new=");
	while(1)
	{

		num0=0xff;
		num0=keyboard_output();
		
		if(num0==3)
		{
			num0=0xff;
			if(d!=0)
			{
				if((*count2)%2==1)
					sscanf(temp,"%g",&a[((*count2)-1)/2][0]);
				else if((*count2)%2==0)
					sscanf(temp,"%g",&a[(*count2)/2-1][1]);
				}
			(*count2)=(*count2)-1;
			if((*count2)<1)
				(*count2)=1;
		
			}
		else if((num0==6)||(num0=='='))
		{	
			num0=0xff;
			if(d!=0)
			{
				if((*count2)%2==1)
					sscanf(temp,"%g",&a[((*count2)-1)/2][0]);
				else if((*count2)%2==0)
					sscanf(temp,"%g",&a[(*count2)/2-1][1]);
				}
				(*count2)=(*count2)+1;
				if((*count2)>2*n)
					(*count2)=2*n;
		
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
			
			
		else if(num0==19)
		{
			num0=0xff;
			*symbol1=1;
			break;

			}
			
		else num0=0xff;
		}
	}
	
	
	
	void er_cal(double *s,int n,double (*a)[25])
	{
	/////////s[0]:x总和 s[1]:x的平均 s[2]:y的总和 s[3]:y的平均 s[4]:x*y的总和 s[5]:x*y的平均 
	/////////s[6]:x^2的总和 s[7]:x^2的平均 s[8]:y^2的总和 s[9]:y^2的平均 
	/////////s[10]:a s[11]:b s[12]:Lxx s[13]:Lyy s[14]:Lxy  s[15]:R
	
		uchar i;
		for(i=0;i<16;i++)
			s[i]=0;
		for(i=0;i<n;i++)
		{
			s[0]=s[0]+a[i][0];
			s[2]=s[2]+a[i][1];
			s[4]=s[4]+(a[i][0])*(a[i][1]);
			s[6]=s[6]+pow(a[i][0],2);
			s[8]=s[8]+pow(a[i][1],2);
			}
		for(i=0;i<5;i++)
			s[2*i+1]=s[2*i]/n;
		
		s[10]=(s[5]-(s[1]*s[3]))/(s[7]-pow(s[1],2));
		s[11]=s[3]-s[10]*s[1];
		
		s[12]=s[7]-pow(s[1],2);
		s[13]=s[9]-pow(s[3],2);
		s[14]=s[5]-s[1]*s[3];
		
		s[15]=s[14]/(pow(s[13]*s[12],0.5));
	
		}
		
	void er_result(uchar count,double *s)
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
							lcd_write_str("1.x 总和：");
							break;
						case 2:
							lcd_write_str("2.x 平均数：");
							break;
						case 3:
							lcd_write_str("3.y 总和:");
							break;
						case 4:
							lcd_write_str("4.y 平均数:");
							break;
						case 5:
							lcd_write_str("5.xy总和:");
							break;
						case 6:
							lcd_write_str("6.xy平均数:");
							break;
						case 7:
							lcd_write_str("7.x^2 总和:");
							break;
						case 8:
							lcd_write_str("8.x^2 平均数:");
							break;
						case 9:
							lcd_write_str("9.y^2 总和:");
							break;
						case 10:
							lcd_write_str("10.y^2平均数:");
							break;
						case 11:
							lcd_write_str("11. a: ");
							break;
						case 12:
							lcd_write_str("12. b:");
							break;
						case 13:
							lcd_write_str("13. Lxx:");
							break;
						case 14:
							lcd_write_str("14. Lyy:");
							break;
						case 15:
							lcd_write_str("14. Lxy:");
							break;
						case 16:
							lcd_write_str("14. R:");
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
	
	
	
	void er_display(double *s)
	{
		uchar count4=1;
		num0=0xff;
		while(1)
		{
			er_result(count4,s);
			num0=keyboard_output();
			if(num0==6)
			{
				num0=0xff;
				count4++;
				if(count4>29)
					count4=29;
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
			
			
		void er_menu()
		{
	
			uchar symbol1=0;
			uchar i,j;
			double a[25][2];
			double s[16];
			int n=1;
			int count2=1;
			for(i=0;i<25;i++)
				for(j=0;j<2;j++)
					a[i][j]=0;
			num0=0xff;
			while(1)
			{
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
					er_n(&n);
					}
				else if(num0=='2')
				{
					num0=0xff;
					while(symbol1==0)
						er_num(n,a,&count2,&symbol1);
					symbol1=0;
					}
				else if(num0=='3')
				{
					num0=0xff;
					er_cal(s,n,a);
					er_display(s);
					}
				else if(num0=='0')
				{
					num0=0xff;
					break;
					}
				}
			}
		
			
	