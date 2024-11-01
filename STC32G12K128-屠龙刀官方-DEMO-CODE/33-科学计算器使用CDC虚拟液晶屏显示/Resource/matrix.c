	//#include "STC12C5A60S2.h"
	#include <stdio.h>
	//#include "lcd_12864.h"
	#include "keyboard_device.h"
	#include "middle.h"
	
	extern uchar num0;		 //输入的键值
	extern int	 count1;       //翻页计数
	
	uchar hang1=1,lie1=1;    //行列式 或第一个矩阵的阶
	uchar hang2=1,lie2=1;  //第二个矩阵的阶
	bit symbol1=0;         //矩阵/行列式输入元素标志位
	double result=1.0;						 //行列式的值
	bit flag_jia=0,flag_jian=0,flag_cheng=0;  //矩阵加、减、乘标志位
	bit flag_kcheng=0,flag_ni=0,flag_zhi=0,flag_zhuan=0,flag_cifang=0;     //矩阵数乘、求逆、求秩、转置标志位、次方
	 
	void set_hanglieshiarray()    //设置行列式的阶  
	{	
		uchar d=0;
		lcd_write_cmd(0x80);	
		lcd_write_str("Now n=");
		lcd_write_dat((uchar)(hang1+0x30));
		lcd_write_cmd(0x90);
		lcd_write_str("end with '='");
		lcd_write_cmd(0x88);
		lcd_write_str("    (1<=n<=5)");
		lcd_write_cmd(0x98);
		lcd_write_str("new=");
	
		while(1)
			{
				num0=keyboard_output();
				if((num0<=0x35)&&(num0>=0x31))
					{
						if(d==0)
						{
							lcd_write_dat(num0);
							hang1=num0-0x30;
							}
						num0=0xff;
						d++;
					}
				else if((num0=='=')||(num0==19))
					{	
						num0=0xff;
						lie1=hang1;
						break;
						}
				else if(num0==18)
				{
					d=0;
					num0=0xff;
					lcd_set_pos(3,2);
					lcd_write_dat(' ');
					lcd_set_pos(3,2);
					}
			}
	}
	
	
	char cal_hanglieshi(double (*a)[5])          //计算行列式   //参数: a[5][5]
	{	
		uchar z,j,i;
		double k;
		double d[5][5];
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
				d[i][j]=a[i][j];
		lcd_clear();
		result=1.0;
		lcd_write_cmd(0x80);
		lcd_write_str("the result is:");
		lcd_write_cmd(0x90);
		for(z=0;z<hang1-1;z++)
			for(i=z;i<hang1-1;i++)
			 {    
				if(d[z][z]==0)
					for(i=z;d[z][z]==0;i++)
					{
						for(j=0;j<hang1;j++)
							d[z][j]=d[z][j]+d[i+1][j];
						if(d[z][z]!=0)
							break;
						else  
						{	
							result=0;
							return 0;
							}
					}
				 k=-d[i+1][z]/d[z][z];
				 for(j=z;j<hang1;j++)
				d[i+1][j]=k*(d[z][j])+d[i+1][j];
			 }
			
		for(z=0;z<hang1;z++)
				result=result*(d[z][z]);
		
		return 0;
	}
	
	
	void simple_num(bit flag,double (*a)[5],double (*b)[5])           //对单个元素查看并修改   //参数:矩阵标志位,第一个矩阵,第二个矩阵
	{		
		uchar d=0,i;
		int hang3,lie3;
		char temp[16];	
		if(flag==0)
		{
			hang3=(count1/lie1)+1;
			lie3=(count1%lie1)+1;
			}
		else 
		{
			hang3=(count1/lie2)+1;
			lie3=(count1%lie2)+1;
			}
			
		lcd_write_cmd(0x80);
		if(flag==0)		
			lcd_write_str("now A[");
		else 
			lcd_write_str("now B[");
		lcd_write_dat((uchar)(hang3+0x30));
		lcd_write_str("][");
		lcd_write_dat((uchar)(lie3+0x30));
		lcd_write_dat(']');
		lcd_write_cmd(0x90);
		lcd_write_dat('=');
		if(flag==0)
			sprintf(temp,"%g",a[hang3-1][lie3-1]);
		else 
			sprintf(temp,"%g",b[hang3-1][lie3-1]);
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
			if((num0==3)||(num0==5))
				{
					
					if(num0==3)
						count1=count1-1;
					else if((num0==5)&&(flag==0))
						count1=count1-lie1;
					else if((num0==5)&&(flag==1))
						count1=count1-lie2;	
					if(count1<0)
						count1=0;
					if(d!=0)
					{
						if(flag==0)
							sscanf(temp,"%g",&a[hang3-1][lie3-1]);
						else
							sscanf(temp,"%g",&b[hang3-1][lie3-1]);
					}
					num0=0xff;
					break;
				}
			else if((num0==6)||(num0==7)||(num0=='='))
				{	
					
					if((num0==6)||(num0=='='))
						count1=count1+1;
					else if((num0==7)&&(flag==0))
						count1=count1+lie1;
					else if((num0==7)&&(flag==1))
						count1=count1+lie2;
					if(flag==0)
					{
						if(count1>lie1*hang1-1)
							count1=lie1*hang1-1;
							}
					else 
					{
						if(count1>lie2*hang2-1)
							count1=lie2*hang2-1;
							}
					if(d!=0)
					{
						if(flag==0)
							sscanf(temp,"%g",&a[hang3-1][lie3-1]);
						else
							sscanf(temp,"%g",&b[hang3-1][lie3-1]);
					}
					num0=0xff;
					break;
				}
			
			else if(num0==19)
				{
					num0=0xff;
					symbol1=1;
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
	
	char hanglieshi(double (*a)[5],double (*b)[5])           //行列式的菜单跟接口
	{
	char temp[16];	
		while(1)
		{
			uchar i=0,j=0;
			lcd_clear();
			lcd_write_cmd(0x80);
			lcd_write_str("1.行列式阶");
			lcd_write_cmd(0x90);
			lcd_write_str("2.查看并更改");
			lcd_write_cmd(0x88);
			lcd_write_str("3.计算行列式");
			lcd_write_cmd(0x98);
			lcd_write_str("0.返回");
			num0=keyboard_output();
			switch (num0)
				{	
					
					case '1':
					{	
						num0=0xff;
						lcd_clear();
						set_hanglieshiarray();
						break;
							}
					case '2':
					{	
						num0=0xff;
						count1=0;
						while(1)
						{
							lcd_clear();
							simple_num(0,a,b);
							if(symbol1==1)
								{
								symbol1=0;
								break;
									}
							}
						break;
						}
					case '3':
					{	
						num0=0xff;
						lcd_clear();
						cal_hanglieshi(a);
						if(result!=0)
						{
							sprintf(temp,"%g",result);
							lcd_write_str(temp);
							}
						else lcd_write_dat(0x30);
						lcd_write_cmd(0x88);
						lcd_write_str("请按任意键返回");
						num0=keyboard_output();
						break;
							}
					case '0':        
					{	
						num0=0xff;
						lcd_clear();
						return 0;
							}
				}
			
			}
	}	
	
	
	
	void ju_sethang(bit flag)
	{
		uchar d;
		lcd_clear();
		lcd_write_cmd(0x80);
		if(flag==0)
			lcd_write_str(" A's hang=(<=5)");
		else 
			lcd_write_str(" B's hang=(<=5)");
		lcd_write_cmd(0x90);
		if(flag==0)
			lcd_write_dat((uchar)(hang1+0x30));
		else 
			lcd_write_dat((uchar)(hang2+0x30));
		lcd_write_cmd(0x88);
		lcd_write_str("end with '='");
		lcd_write_cmd(0x98);
		lcd_write_str("new hang=");
		d=0;
		while(1)
		{
			num0=keyboard_output();
			if((num0<=0x35)&&(num0>=0x31))
			{
				if(d==0)
				{
					lcd_write_dat(num0);
					if(flag==0)
						hang1=num0-0x30;
					else 
						hang2=num0-0x30;
						}
				d++;
				}
			else if((num0=='=')||(num0==19))
			{
				num0=0xff;
				break;
				}
			else if(num0==18)
			{
				d=0;
				lcd_set_pos(3,4);
				lcd_write_dat(' ');
				lcd_set_pos(3,4);
				num0=0xff;
				}
		}
	}
	
	
	void  ju_setlie(bit flag)
	{
		uchar d;
		lcd_clear();
		lcd_write_cmd(0x80);
		if(flag==0)
			lcd_write_str(" A's lie=(<=5)");
		else 
			lcd_write_str(" B's lie=(<=5)");
		lcd_write_cmd(0x90);
		if(flag==0)
			lcd_write_dat((uchar)(lie1+0x30));
		else 
			lcd_write_dat((uchar)(lie2+0x30));
		lcd_write_cmd(0x88);
		lcd_write_str("end with '='");
		lcd_write_cmd(0x98);
		lcd_write_str("new lie=");
		d=0;
		while(1)
		{
			num0=keyboard_output();
			if((num0<=0x35)&&(num0>=0x31))
			{
				if(d==0)
				{
					lcd_write_dat(num0);
					if(flag==0)
						lie1=num0-0x30;
					else 
						lie2=num0-0x30;
					}
				d++;
				}
			else if((num0=='=')||(num0==19))
			{
				num0=0xff;
				break;
				}
			else if(num0==18)
			{
				d=0;
				lcd_set_pos(3,4);
				lcd_write_dat(' ');
				lcd_set_pos(3,4);
				num0=0xff;
				}
		}
	}
	
	

	
	char ju_display(uchar h1,uchar l1,double (*c)[5])    //显示矩阵元素计算结果
	{
		char temp[16];
		uchar i,j,k,m,n;
		count1=0;
		while(1)
		{	
			lcd_clear();
			i=(count1/l1)+1;
			j=(count1%l1)+1;
			for(k=0;k<2;k++)
			{
				m=(k+j-1)/l1;
				n=((k+j-1)%l1)+1;
				if(m>h1)
					break;
				lcd_set_pos((uchar)(2*k),0);
				lcd_write_str("C[");
				lcd_write_dat((uchar)(i+m+0x30));
				lcd_write_str("][");
				lcd_write_dat((uchar)(n+0x30));
				lcd_write_str("]=");
				lcd_set_pos((uchar)(2*k+1),0);
				sprintf(temp,"%g",c[i+m-1][n-1]);
				lcd_write_str(temp);
			}	
			num0=0xff;
			num0=keyboard_output();
			switch	(num0)
				{
					case 3:	
					{	
						num0=0xff;
						count1--;
						if(count1<0)
							count1=0;
						break;
						}
						
					case 6:
					{
						num0=0xff;
						count1++;
						if(count1>h1*l1-2)
							count1=h1*l1-2;
						break;
						}
					case 19:
					{
						num0=0xff;
						return 0;
						}
				}
		}
	}
	
	
	
	void ju_jiajian(double (*a)[5],double (*b)[5],double (*c)[5])     //矩阵加减运算
	{
		uchar i,j;
		if(flag_jia==1)
			for(i=0;i<hang1;i++)
				for(j=0;j<lie1;j++)
					c[i][j]=a[i][j]+b[i][j];
		else if(flag_jian==1)
			for(i=0;i<hang2;i++)
				for(j=0;j<lie2;j++)
					c[i][j]=a[i][j]-b[i][j];
		
		ju_display(hang1,lie1,c);
	}	
	
	
	
	void ju_cheng(double (*a)[5],double (*b)[5],double (*c)[5])       //矩阵乘法
	{
		uchar i,j,k;
		double d[5][5];
		double e[5][5];
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
			{
				d[i][j]=a[i][j];
				e[i][j]=b[i][j];
				}
		for(i=0;i<hang1;i++)
			for(j=0;j<lie2;j++)
			{
				result=0;
				for(k=0;k<lie1;k++)
					result=result+d[i][k]*e[k][j];
				c[i][j]=result;
			}
		ju_display(hang1,lie2,c);
	}
	
	
		
	
	void ju_zhuan(double (*a)[5],double (*c)[5])     //矩阵转置
 	{
		uchar i,j;
		for(i=0;i<hang1;i++)
			for(j=0;j<lie1;j++)
				c[j][i]=a[i][j];
		ju_display(lie1,hang1,c);
	}
	
	
	
	void ju_ni(double (*a)[5],double (*b)[5],double (*c)[5])    //矩阵求逆
	{
		char i,j,m;
		float t;
		double d[5][5];
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
				d[i][j]=a[i][j];
		for(i=0;i<hang1;i++)
			for(j=0;j<hang1;j++)
				b[i][j]=0;
		for(i=0;i<hang1;i++)
			b[i][i]=1;
		for(m=0;m<hang1;m++)          
		{ 
			t=d[m][m];                 
			i=m;                        
			while(d[m][m]==0)
			{    
				d[m][m]=d[i+1][m];
				i++;
				}
			if(i>m)
			{	
				d[i][m]=t;                
				for(j=0;j<hang1;j++)                     
				{
					t=d[m][j];
					d[m][j]=d[i][j];
					d[i][j]=t;
					}
				for(j=0;j<hang1;j++)                     
				{
					t=b[m][j];
					b[m][j]=b[i][j];
					b[i][j]=t;
					}
			}
		for(i=m+1;i<hang1;i++)
     for(j=hang1-1;j>=0;j--)
			b[i][j]-=b[m][j]*d[i][m]/d[m][m];
		for(i=m+1;i<hang1;i++)
     for(j=hang1-1;j>=m;j--)
			d[i][j]-=d[m][j]*d[i][m]/d[m][m]; 
		for(j=hang1-1;j>=0;j--)
			b[m][j]/=d[m][m];   
		for(j=hang1-1;j>=m;j--)
			d[m][j]/=d[m][m];
	}

	m=hang1-1;
	while(m>0)
	 {
		for(i=0;i<m;i++)
		{
			for(j=hang1-1;j>=0;j--)
				b[i][j]-=d[i][m]*b[m][j];
			for(j=hang1-1;j>=m;j--)          
				d[i][j]-=d[i][m]*d[m][j];
			}
		 m--;
	 }
    
	 for(i=0;i<hang1;i++)                        
		for(j=0;j<hang1;j++)
		 c[i][j]=b[i][j];
		ju_display(hang1,lie1,c);
	}
	
	
	void ju_kcheng(float k,double (*a)[5],double (*c)[5])    //矩阵数乘
	{
		uchar i,j;
		
		for(i=0;i<hang1;i++)
		 for(j=0;j<lie1;j++)
			 c[i][j]=k*a[i][j];
		ju_display(hang1,lie1,c);

	}
	
	
	
	
	void ju_cifang(uchar n,double (*a)[5],double (*b)[5],double (*c)[5])      //矩阵的次方运算 A^2 A^3 A^4……
	{
		float s=0;
		uchar i,j,m,k;
		double d[5][5];
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
				d[i][j]=a[i][j];
		if(n>1)
		{
			for(i=0;i<hang1;i++)
				for(j=0;j<hang1;j++)
					b[i][j]=d[i][j];
			for(m=1;m<n;m++)
			{
			 for(i=0;i<hang1;i++)
				for(j=0;j<hang1;j++)
				{
				 s=0;
				 for(k=0;k<hang1;k++)
					s=s+d[i][k]*b[k][j];
				c[i][j]=s;
				}
			for(i=0;i<hang1;i++)
				for(j=0;j<hang1;j++)
					d[i][j]=c[i][j];
			}
		}
		if(n==1)
			for(i=0;i<hang1;i++)
				for(j=0;j<hang1;j++)
					c[i][j]=d[i][j];
		ju_display(hang1,hang1,c);
	}
	
	
	char ju_zhi(double (*a)[5],double (*c)[5])   //矩阵求秩
	{
		char temp[16];
		float t;
		char i,j;
		char ri,ci;  
		bit f_z; 
		double d[5][5];
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
				d[i][j]=a[i][j];
		for(ri=ci=0;ci<lie1;ci++)
		{
			f_z=1;
			for(i=ri;i<hang1;i++)
				if(d[i][ci]!=0)
				{
					if(i!=ri) 
						if(f_z)
							for(j=0;j<lie1-ci;j++)
							 { 
								 t=d[ri][ci+j];	
								 d[ri][ci+j]=d[i][ci+j];
								 d[i][ci+j]=t;
							 }
						else 
						{ 
							t=d[i][ci];
							for(j=0;j<lie1-ci;j++)
								d[i][ci+j]*=d[ri][ci];
							for(j=0;j<lie1-ci;j++)
								d[i][ci+j]+=d[ri][ci+j]*(-t);
						}
					f_z=0; 
				} 
		 if(!f_z) ri++; 
   }
		for(i=0;i<hang1;i++)
			for(j=0;j<lie1;j++)
				c[i][j]=d[i][j];
		result=ri;
		
		while(1)
		{	
			lcd_clear();
			lcd_write_cmd(0x80);
			lcd_write_str("1.查看行列式的秩");
			lcd_write_cmd(0x90);
			lcd_write_str("2.查看变换后矩阵");
			lcd_write_cmd(0x88);
			lcd_write_str("0.返回");
			num0=keyboard_output();
			switch (num0)
			{
				case '2':
				{
					num0=0xff;
					ju_display(hang1,lie1,c);
					lcd_clear();
					break;	
					}
				case '0':
				{
					num0=0xff;
					return 0;
					}
				case '1':
				{
						num0=0xff;
						lcd_clear();
						lcd_write_cmd(0x80);
						lcd_write_str("该矩阵的秩为：");
						lcd_write_cmd(0x90);
						sprintf(temp,"%g",result);
						lcd_write_str(temp);
						if(hang1==lie1)
						{
							lcd_write_cmd(0x88);
							if(result==hang1)
									lcd_write_str("该矩阵满秩！");
							else 
								lcd_write_str("该矩阵退化矩阵！");
							}
						num0=0xff;
						while(num0!=19)
							num0=keyboard_output();
						break;
					}
			}
  
		}
	}
	
	
	
	void ju_menu1(uchar count)
	{
		uchar m;
		lcd_clear();
		for(m=0;m<4;m++)
			{
				lcd_set_pos(m,0);
				switch (count+m)
				{
					case 1:
						lcd_write_str("1.矩阵的阶");
						break;
					case 2:
						lcd_write_str("2.修改第一矩阵");
						break;
					case 3:
						lcd_write_str("3.修改第二矩阵");
						break;
					case 4:
						lcd_write_str("4.计算矩阵");
						break;
					case 5:
						lcd_write_str("0.返回");
						break;
					}
			}
	}
	
	
	
	
	char ju_couple(double (*a)[5],double (*b)[5],double (*c)[5])     //矩阵双元计算
	{
		char count3=1;//翻主页
		lcd_clear();
		while(1)
		{
			ju_menu1(count3);
			num0=keyboard_output();
			switch (num0)
			{	
					case '1':
					{	
						lcd_clear();
						num0=0xff;
						ju_sethang(0);
						ju_setlie(0);
						if(flag_cheng==1)
						{	
							ju_setlie(1);
							hang2=lie1;
							}
						else if((flag_jia==1)||(flag_jian==1))
						{	
							hang2=hang1;
							lie2=lie1;
							}
						break;
						}
					case '2':
					{
						num0=0xff;
						count1=0;
						while(1)
						{
							lcd_clear();
							simple_num(0,a,b);
							if(symbol1==1)
								{
									symbol1=0;
									break;
									}
						}
						break;
							}
					case '3':        
					{
						num0=0xff;
						count1=0;
						while(1)
						{
							lcd_clear();
							simple_num(1,a,b);
							if(symbol1==1)
								{
									symbol1=0;
									break;
									}
						}
						break;
							}
					case '4':
					{
						lcd_clear();
						num0=0xff;
						if((flag_jia==1)||(flag_jian==1))
							{
								ju_jiajian(a,b,c);
								break;
								}
						else if(flag_cheng==1)
							{
								ju_cheng(a,b,c);
								break;
								}
						}
					case '0':
						{
							num0=0xff;
							return 0;
							}
					case 6:
						{
							num0=0xff;
							count3++;
							if(count3>2)
								count3=2;
							break;
							}
					case 3:
						{
							num0=0xff;
							count3--;
							if(count3<1)
								count3=1;
							break;
							}
				}
			
			}

	}
	
	
	
	char ju_simple(double (*a)[5],double (*b)[5],double (*c)[5])
	{
		uchar i;
		char temp[16];
		uchar d=0;
		int n=1;
		double k=1;
		while(1)
		{	
			lcd_clear();
			lcd_write_cmd(0x80);
			lcd_write_str("1.设置矩阵行列");
			lcd_write_cmd(0x90);
			lcd_write_str("2.查看或修改矩阵");
			lcd_write_cmd(0x88);
			if((flag_zhuan==1)||(flag_ni==1))
				lcd_write_str("3.查看转换后矩阵");
			else if(flag_cifang==1)
				lcd_write_str("3.输入次方n");
			else if(flag_ni==1)
				lcd_write_str("3.查看矩阵的逆");
			else if(flag_kcheng==1)
				lcd_write_str("3.输入数乘系数k");
			else if(flag_zhi==1)
				lcd_write_str("3.查看矩阵的秩");
			lcd_write_cmd(0x98);
			if((flag_cifang==1)||(flag_kcheng==1))
				lcd_write_str("4.计算  0.返回");
	    else if((flag_ni==1)||(flag_zhi==1)||(flag_zhuan==1))
				lcd_write_str("0.返回");
			num0=keyboard_output();
			if(num0=='1')
				{
					num0=0xff;
					lcd_clear();
					if((flag_cifang==1)||(flag_ni==1))
						set_hanglieshiarray();
					else if((flag_kcheng==1)||(flag_zhuan==1)||(flag_zhi==1))
					{
						ju_sethang(0);
						ju_setlie(0);
						}
					
				}
			else if(num0=='2')
				{
					num0=0xff;
					count1=0;
					while(1)
					{
						lcd_clear();
						simple_num(0,a,b);
						if(symbol1==1)
							{
								symbol1=0;
								break;
							}
						}
				}
			else if(num0=='3')
				{
					num0=0xff;
					lcd_clear();
					if(flag_ni==1)
						ju_ni(a,b,c);
					else if(flag_zhuan==1)
						ju_zhuan(a,c);
					else if(flag_zhi==1)
						ju_zhi(a,c);
					else if((flag_cifang==1)||(flag_kcheng==1))
						{
							lcd_clear();
							lcd_write_cmd(0x80);
							lcd_write_str("now the n is");
							lcd_set_pos(1,3);
							if(flag_cifang==1)
								lcd_write_dat((uchar)(n+0x30));
							else 
							{
								sprintf(temp,"%g",k);
								lcd_write_str(temp);
								}
							lcd_write_cmd(0x88);
							lcd_write_str("end with '='");
							lcd_write_cmd(0x98);
							lcd_write_str("new=");
							for(d=0;d<16;d++)
								temp[d]=0;
							d=0;
							while(1)
								{
									num0=keyboard_output();
									if(((num0<=0x39)&&(num0>=0x30))||(num0=='.')||(num0=='-'))
										{
												temp[d]=num0;
												lcd_write_dat(num0);
												num0=0xff;
												d++;
											}
									else if(num0==18)
									{
										num0=0xff;
										d--;
										if(d<0) d=0;
										temp[d]=0;
										lcd_set_pos(3,2);
										lcd_write_str("            ");
										for(i=0;i<d;i++)
											lcd_write_dat(temp[i]);
										}
									else if((num0=='=')||(num0==19))
										{
											if(d!=0)
											{
												if(flag_cifang==1)
													sscanf(temp,"%d",&n);
												else 
													sscanf(temp,"%g",&k);
													}
											num0=0xff;
											d=0;
											break;
										}
									else if(num0=='3')
									{
										num0=0xff;
										if(d!=0)
										{
											if(flag_cifang==1)
												sscanf(temp,"%d",&n);
											else 
												sscanf(temp,"%g",&k);
											d=0;
											}
										break;
										}
								}
						}

				}
			else if(num0=='4')
				{
					num0=0xff;
					lcd_clear();
					if(flag_kcheng==1)
							ju_kcheng(k,a,c);
					else if(flag_cifang==1)
							ju_cifang(n,a,b,c);
				}
			else if(num0=='0')
				{	
					num0=0xff;
					lcd_clear();
					return 0;
				}
		}
	}
	
	void ju_menu2(uchar count)
	{	
		uchar m;
		lcd_clear();
		for(m=0;m<4;m++)
		{
			lcd_set_pos(m,0);
			switch (m+count)
			{
				case 1:
					lcd_write_str("1.计算行列式");
					break;
				case 2:
					lcd_write_str("2.计算矩阵加法");
					break;
				case 3:
					lcd_write_str("3.计算矩阵减法");
					break;
				case 4:
					lcd_write_str("4.计算矩阵乘法");
					break;
				case 5:
					lcd_write_str("5.计算方阵高次幂");
					break;
				case 6:
					lcd_write_str("6.求矩阵的逆");
					break;
				case 7:
					lcd_write_str("7.求转置矩阵");
					break;
				case 8:
					lcd_write_str("8.求矩阵的秩");
					break;
				case 9:
					lcd_write_str("9.计算矩阵的数乘");
					break;
				}
			}
	}
	
	
	void ju_main()
	{
		double a[5][5],b[5][5],c[5][5];
		char count4=1;
		uchar i,j;
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
			{
				a[i][j]=0;
		    b[i][j]=0;
				c[i][j]=0;
				}
		symbol1=0;
		result=1.0;
		while(1)
		{	
			lcd_clear();
			ju_menu2(count4);
			num0=keyboard_output();
			if(num0=='1')
			{
				num0=0xff;
				hanglieshi(a,b);
				}
			else if(num0=='2')
			{
				num0=0xff;
				flag_jia=1;
				ju_couple(a,b,c);
				flag_jia=0;
				}
			else if(num0=='3')
			{
				num0=0xff;
				flag_jian=1;
				ju_couple(a,b,c);
				flag_jian=0;
				}
			else if(num0=='4')
			{
				num0=0xff;
				flag_cheng=1;
				ju_couple(a,b,c);
				flag_cheng=0;
				}
			else if(num0=='5')
			{
				num0=0xff;
				flag_cifang=1;
				ju_simple(a,b,c);
				flag_cifang=0;
				}
			else if(num0=='6')
			{
				num0=0xff;
				flag_ni=1;
				ju_simple(a,b,c);
				flag_ni=0;
				}
			else if(num0=='7')
			{
				num0=0xff;
				flag_zhuan=1;
				ju_simple(a,b,c);
				flag_zhuan=0;
				}
			else if(num0=='8')
			{
				num0=0xff;
				flag_zhi=1;
				ju_simple(a,b,c);
				flag_zhi=0;
				}
			else if(num0=='9')
			{
				num0=0xff;
				flag_kcheng=1;
				ju_simple(a,b,c);
				flag_kcheng=0;
				}
			else if(num0==6)
			{	
				num0=0xff;
				count4++;
				if(count4>6)
					count4=6;
				}
			else if(num0==3)
			{
				num0=0xff;
				count4--;
				if(count4<1)
					count4=1;
				}
		}
	}
	

