#include "matrix.h"
#include "menu.h"
#include "stdlib.h"

u8 xdata Ranks_Steps=2;
u8 xdata inputstr[12];
u8 xdata inputstrlength;
double xdata Ranks_Parm[5][5]=
{
    {1,2,0,0,0},
    {3,4,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},        
};

double xdata Ranks_Parm2[5][5]=
{
    {1,2,0,0,0},
    {3,4,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},        
};

double xdata Ranks_result[5][5]=
{
    {1,2,0,0,0},
    {3,4,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},        
};


//-----------------------------------------行列式矩阵-----------------------------------------
void Ranks_Set(u8 keynum)
{
    static  u8 xdata keynum_last=0;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now n=");
            Screen_ShowAsc(48,0,(u8)(Ranks_Steps%10+'0'));
            Screen_ShowStr(0,2,"end with '='");
            Screen_ShowStr(0,4,"    (1<=n<=5)");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='5' ))
                {
                    Screen_ShowAsc(32,6,keynum);
                   keynum_last = keynum;
                }
                else if( keynum == '=' )
                {
                    Ranks_Steps = (keynum_last-'0');
                    P_Index.Menu_KeyFunc=0;                    
                }
                Screen_ShowFreshen();
            }
            break; 
    }    
}

void Ranks_Amend(u8 keynum)
{
    static u8 addr_str=0;
    u8 i,j;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now A[1][1]");
            Screen_ShowStr(0,2,"= ");
            Screen_ShowNum(8,2,Ranks_Parm[0][0]);
            Screen_ShowStr(0,4," :上翻   :下翻");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;
            inputstrlength=0;
            memset(inputstr,'\0',12);
            addr_str = 0;        
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&( addr_str>0 ))
                {
                    addr_str--;
                    Screen_ShowAsc(48,0,(addr_str)/Ranks_Steps+'1');
                    Screen_ShowAsc(72,0,(addr_str)%Ranks_Steps+'1');
                    Screen_ShowStr(32,6,"            ");
                    memset(inputstr,'\0',12);
                    inputstrlength=0;
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowNum(8,2,Ranks_Parm[addr_str/Ranks_Steps][addr_str%Ranks_Steps]);
                }
                else if(( keynum==9 )&&( addr_str< (Ranks_Steps*Ranks_Steps-1)))
                {
                    addr_str++;
                    Screen_ShowAsc(48,0,(addr_str)/Ranks_Steps+'1');
                    Screen_ShowAsc(72,0,(addr_str)%Ranks_Steps+'1');
                    Screen_ShowStr(32,6,"            ");
                    memset(inputstr,'\0',12);
                    inputstrlength = 0;
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowNum(8,2,Ranks_Parm[addr_str/Ranks_Steps][addr_str%Ranks_Steps]);
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    inputstr[inputstrlength]=keynum;
                    inputstrlength++;
                }
                else if( keynum==21 )
                {
                    inputstrlength--;
                    inputstr[inputstrlength]=' '; 
                } 
                else if( keynum=='=' )
                {
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowStr(8,2,inputstr);
                    Ranks_Parm[addr_str/Ranks_Steps][addr_str%Ranks_Steps]= atof(inputstr);
                    
                    if( addr_str< (Ranks_Steps*Ranks_Steps-1) )
                    {
                        addr_str++;
                        Screen_ShowAsc(48,0,(addr_str)/Ranks_Steps+'1');
                        Screen_ShowAsc(72,0,(addr_str)%Ranks_Steps+'1');
                        Screen_ShowStr(32,6,"            ");
                        memset(inputstr,'\0',12);
                        inputstrlength=0;
                        Screen_ShowStr(8,2,"               ");
                        Screen_ShowNum(8,2,Ranks_Parm[addr_str/Ranks_Steps][addr_str%Ranks_Steps]);                        
                    }
                }
                else if( keynum==22 )
                {
                    P_Index.Menu_KeyFunc = 0;
                } 
                
                Screen_ShowStr(32,6,inputstr);
                Screen_ShowFreshen();      
            }
            
            break;   
    }    
}
double result;

char cal_hanglieshi(double (*a)[5])          //计算行列式   //参数: a[5][5]
{	
    uchar z,j,i;
    double k;
    double d[5][5];
    for(i=0;i<5;i++)
        for(j=0;j<5;j++)
            d[i][j]=a[i][j];
    result=1.0;
    for(z=0;z<Ranks_Steps-1;z++)
        for(i=z;i<Ranks_Steps-1;i++)
         {    
            if(d[z][z]==0)
                for(i=z;d[z][z]==0;i++)
                {
                    for(j=0;j<Ranks_Steps;j++)
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
             for(j=z;j<Ranks_Steps;j++)
            d[i+1][j]=k*(d[z][j])+d[i+1][j];
         }
        
    for(z=0;z<Ranks_Steps;z++)
            result=result*(d[z][z]);
    
    return 0;
}
    
void Ranks_Caculate(u8 keynum)
{
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"the result is:");
            cal_hanglieshi(Ranks_Parm);
            if(result != 0)
            {
                Screen_ShowNum(0,2,result);
            }
            else
            {
                Screen_ShowStr(0,4,"数\xfd据错误");
            }
            Screen_ShowStr(0,4,"请按任意键返回");
            Screen_ShowFreshen();  
            state = 1;

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                P_Index.Menu_KeyFunc=0;                    
                Screen_Clear();
            }
            break; 
    }        

}

u8 hang1=2,lie1=2;    //行列式 或第一个矩阵的阶
u8 hang2=2,lie2=2;  //第二个矩阵的阶

//-----------------------------------------加减乘法举证-----------------------------------------
void Ranks_2_4Set(u8 keynum)
{
    static keynum_last;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0," A's hang=(<=5)");
            Screen_ShowNum(0,2,hang1);
            Screen_ShowStr(0,4,"end with '='");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='5' ))
                {
                    Screen_ShowAsc(32,6,keynum);
                    keynum_last = keynum;
                }
                else if( keynum == '=' )
                {
                    hang1 = (keynum_last-'0');
                    //P_Index.Menu_KeyFunc=0;  
                    state = 2;
                }
                Screen_ShowFreshen();
            }
            break; 
            
        case 2://初始化
            Screen_Clear();
            Screen_ShowStr(0,0," A's lie=(<=5)");
            Screen_ShowNum(0,2,lie1);
            Screen_ShowStr(0,4,"end with '='");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 3;

            break;
     
        case 3://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='5' ))
                {
                    Screen_ShowAsc(32,6,keynum);
                    keynum_last = keynum;
                }
                else if( keynum == '=' )
                {
                    lie1 = (keynum_last-'0');
                    P_Index.Menu_KeyFunc=0;  
                    hang2 = hang1;
                    lie2 = lie1;
//Screen_Clear();                    
                }
                Screen_ShowFreshen();
            }
            break;             
    }    
}

void Ranks_AmendA(u8 keynum)
{
    static u8 addr_str=0;
    u8 i,j;
    switch( state )
    {
        case 0://初始化
            
            Screen_Clear();
            Screen_ShowStr(0,0,"Now A[1][1]");
            Screen_ShowStr(0,2,"= ");
            Screen_ShowNum(8,2,Ranks_Parm[0][0]);
            Screen_ShowStr(0,4," :上翻   :下翻");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;
            inputstrlength=0;
            memset(inputstr,'\0',12);
            addr_str = 0;        
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&( addr_str>0 ))
                {
                    addr_str--;
                    Screen_ShowAsc(48,0,(addr_str)/lie1+'1');
                    Screen_ShowAsc(72,0,(addr_str)%lie1+'1');
                    Screen_ShowStr(32,6,"            ");
                    memset(inputstr,'\0',12);
                    inputstrlength=0;
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowNum(8,2,Ranks_Parm[addr_str/lie1][addr_str%lie1]);
                }
                else if(( keynum==9 )&&( addr_str< (lie1*hang1-1)))
                {
                    addr_str++;
                    Screen_ShowAsc(48,0,(addr_str)/lie1+'1');
                    Screen_ShowAsc(72,0,(addr_str)%lie1+'1');
                    Screen_ShowStr(32,6,"            ");
                    memset(inputstr,'\0',12);
                    inputstrlength = 0;
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowNum(8,2,Ranks_Parm[addr_str/lie1][addr_str%lie1]);
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    inputstr[inputstrlength]=keynum;
                    inputstrlength++;
                }
                else if( keynum==21 )
                {
                    inputstrlength--;
                    inputstr[inputstrlength]=' '; 
                } 
                else if( keynum=='=' )
                {
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowStr(8,2,inputstr);
                    Ranks_Parm[addr_str/lie1][addr_str%lie1]= atof(inputstr);
                }
                Screen_ShowStr(32,6,inputstr);
                Screen_ShowFreshen();      
            }
            break;   
    }    
}

void Ranks_AmendB(u8 keynum)
{
    static u8 addr_str=0;
    u8 i,j;
    switch( state )
    {
        case 0://初始化
            
            Screen_Clear();
            Screen_ShowStr(0,0,"Now B[1][1]");
            Screen_ShowStr(0,2,"= ");
            Screen_ShowNum(8,2,Ranks_Parm2[0][0]);
            Screen_ShowStr(0,4," :上翻   :下翻");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;
            inputstrlength=0;
            memset(inputstr,'\0',12);
            addr_str = 0;        
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&( addr_str>0 ))
                {
                    addr_str--;
                    Screen_ShowAsc(48,0,(addr_str)/lie1+'1');
                    Screen_ShowAsc(72,0,(addr_str)%lie1+'1');
                    Screen_ShowStr(32,6,"            ");
                    memset(inputstr,'\0',12);
                    inputstrlength=0;
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowNum(8,2,Ranks_Parm2[addr_str/lie1][addr_str%lie1]);
                }
                else if(( keynum==9 )&&( addr_str< (lie1*hang1-1)))
                {
                    addr_str++;
                    Screen_ShowAsc(48,0,(addr_str)/lie1+'1');
                    Screen_ShowAsc(72,0,(addr_str)%lie1+'1');
                    Screen_ShowStr(32,6,"            ");
                    memset(inputstr,'\0',12);
                    inputstrlength = 0;
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowNum(8,2,Ranks_Parm2[addr_str/lie1][addr_str%lie1]);
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    inputstr[inputstrlength]=keynum;
                    inputstrlength++;
                }
                else if( keynum==21 )
                {
                    inputstrlength--;
                    inputstr[inputstrlength]=' '; 
                } 
                else if( keynum=='=' )
                {
                    Screen_ShowStr(8,2,"               ");
                    Screen_ShowStr(8,2,inputstr);
                    Ranks_Parm2[addr_str/lie1][addr_str%lie1]= atof(inputstr);
                }
                Screen_ShowStr(32,6,inputstr);
                Screen_ShowFreshen();      
            }
            break;   
    }    
}

void ju_jiajian(double (*a)[5],double (*b)[5],double (*c)[5],u8 mode)     //矩阵加减运算
{
    u8 i,j,k;
	double xdata d[5][5];
	double xdata e[5][5];
    
    if(mode==1)         //矩阵加法
    {
        for(i=0;i<hang1;i++)
            for(j=0;j<lie1;j++)
                c[i][j]=a[i][j]+b[i][j];
    }
    else if(mode==2) //矩阵减法
    {
        for(i=0;i<hang2;i++)
            for(j=0;j<lie2;j++)
                c[i][j]=a[i][j]-b[i][j];
    }
    else if( mode==3 )//矩阵乘法
    {
		for(i=0;i<5;i++)
        {
			for(j=0;j<5;j++)
			{
				d[i][j]=a[i][j];
				e[i][j]=b[i][j];
				}
            }
            for(i=0;i<hang1;i++)
            {
                for(j=0;j<lie2;j++)
                {
                    result=0;
                    for(k=0;k<lie1;k++)
                        result=result+d[i][k]*e[k][j];
                    c[i][j]=result;
                }    
            }    
    }
}	

void Ranks_Caculate2(u8 keynum)
{
    static u8 cal_mode; 	//当前页面的模式 1：加法 2：减法 3：乘法
    static u8 addr_str=0;
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            cal_mode = P_Index.Menu_Pilot[P_Index.Menu_Depth-1];
            ju_jiajian(Ranks_Parm,Ranks_Parm2,Ranks_result,cal_mode);
            Screen_ShowStr(0,0,"c[ ][ ]=");
            Screen_ShowStr(0,4,"c[ ][ ]=");
//            Screen_ShowAsc(100,0,cal_mode+'0');
            Screen_ShowFreshen(); 
            keynum = 1 ;       
            state = 1;
            addr_str = 0;
            //break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                //P_Index.Menu_KeyFunc=0;  
//                Screen_Clear();
                if(( keynum==4 )&&( addr_str>0 ))
                {
                    addr_str--;
                }
                else if(( keynum==9 )&&( addr_str< (lie1*hang1-2)))
                {
                    addr_str++;
                } 
                Screen_ShowAsc(16,0,(addr_str)/lie1+'1');
                Screen_ShowAsc(40,0,(addr_str)%lie1+'1');
                Screen_ShowStr(0,2,"                ");
                Screen_ShowNum(0,2,Ranks_result[addr_str/lie1][addr_str%lie1]);
                
                Screen_ShowAsc(16,4,(addr_str+1)/lie1+'1');
                Screen_ShowAsc(40,4,(addr_str+1)%lie1+'1');
                Screen_ShowStr(0,6,"                ");
                Screen_ShowNum(0,6,Ranks_result[(addr_str+1)/lie1][(addr_str+1)%lie1]);                
                Screen_ShowFreshen();
            }
            break; 
    }        
}
u8 Ranks_power= 2;
//----------------------------------设置次方----------------------------------
void Ranks_Setpower(u8 keynum)
{
    static keynum_last;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now the n is:");
            Screen_ShowAsc(120,0,(u8)(Ranks_power%10+'0'));
            Screen_ShowStr(0,2,"end with '='");
            Screen_ShowStr(0,4,"    (1<=n<10)");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='9' ))
                {
                    Screen_ShowAsc(32,6,keynum);
                   keynum_last = keynum;
                }
                else if( keynum == '=' )
                {
                    Ranks_power = (keynum_last-'0');
                    P_Index.Menu_KeyFunc=0;                    
                }
                Screen_ShowFreshen();
            }
            break; 
    }    
}
void ju_cifang(uchar n,double (*a)[5],double (*b)[5],double (*c)[5])      //矩阵的次方运算 A^2 A^3 A^4……
{
    float xdata s=0;
    uchar xdata i,j,m,k;
    double xdata d[5][5];
    
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
	}
 	void ju_zhuan(double (*a)[5],double (*c)[5])     //矩阵转置
 	{
		uchar i,j;
		for(i=0;i<hang1;i++)
			for(j=0;j<lie1;j++)
				c[j][i]=a[i][j];
	}
 	void ju_kcheng(float k,double (*a)[5],double (*c)[5])    //矩阵数乘
	{
		uchar i,j;
		
		for(i=0;i<hang1;i++)
		 for(j=0;j<lie1;j++)
			 c[i][j]=k*a[i][j];
	}
	
    
void Ranks_Caculate3(u8 keynum)
{
    static u8 cal_mode; 	//当前页面的模式 4:次方 
    static u8 addr_str=0;
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            cal_mode = P_Index.Menu_Pilot[P_Index.Menu_Depth-1];
            
            if( cal_mode == 4 )
                ju_cifang(Ranks_power,Ranks_Parm,Ranks_Parm2,Ranks_result);
            else if( cal_mode == 5 )
                ju_ni(Ranks_Parm,Ranks_Parm2,Ranks_result);           
            else if( cal_mode == 6 )
                ju_zhuan(Ranks_Parm,Ranks_result);
            else if( cal_mode == 8 )
                ju_kcheng(Ranks_power,Ranks_Parm,Ranks_result);
                
            
            Screen_ShowStr(0,0,"c[ ][ ]=");
            if(  (hang1+lie1)>1 )
                Screen_ShowStr(0,4,"c[ ][ ]=");
            Screen_ShowFreshen(); 
            keynum = 1 ;       
            state = 1;
            addr_str = 0;
            //break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                //P_Index.Menu_KeyFunc=0;  
//                Screen_Clear();
                if(( keynum==4 )&&( addr_str>0 ))
                {
                    addr_str--;
                }
                else if(( keynum==9 )&&( addr_str< (lie1*hang1-2)))
                {
                    addr_str++;
                } 
                Screen_ShowAsc(16,0,(addr_str)/lie1+'1');
                Screen_ShowAsc(40,0,(addr_str)%lie1+'1');
                Screen_ShowStr(0,2,"                ");
                Screen_ShowNum(0,2,Ranks_result[addr_str/lie1][addr_str%lie1]);
                
                if(  (hang1+lie1)>1 )
                {
                    Screen_ShowAsc(16,4,(addr_str+1)/lie1+'1');
                    Screen_ShowAsc(40,4,(addr_str+1)%lie1+'1');
                    Screen_ShowStr(0,6,"                ");
                    Screen_ShowNum(0,6,Ranks_result[(addr_str+1)/lie1][(addr_str+1)%lie1]);    
                }                    
                Screen_ShowFreshen();
            }
            break; 
    }        
}

double xdata zhiresult=1.0;						 //行列式的值

char ju_zhi(double (*a)[5],double (*c)[5])   //矩阵求秩
	{
		char xdata  temp[16];
		float xdata  t;
		char xdata  i,j;
		char xdata  ri,ci;  
		bit xdata  f_z; 
		double  xdata d[5][5];
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
		zhiresult=ri;
	}
    

void Ranks_changeCaculate(u8 keynum)
{
    static  u8 xdata cal_mode; 	//当前页面的模式 4:次方 
    static  u8 xdata addr_str=0;
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            ju_zhi(Ranks_Parm,Ranks_result);
            Screen_ShowStr(0,0,"1.查看行列式的秩");
            Screen_ShowStr(0,2,"2.查看变换后矩阵");
            Screen_ShowStr(0,4,"0.返回");
            Screen_ShowFreshen(); 
 //           keynum = 1 ;       
            state = 1;
            addr_str = 0;
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if( keynum=='1' )
                {
                   state=2;
                    goto state2;
                }
                else if( keynum=='2' )
                {
                    state=3;
                    Screen_Clear();
                    Screen_ShowStr(0,0,"c[ ][ ]=");
                    if(  (hang1+lie1)>1 )
                        Screen_ShowStr(0,4,"c[ ][ ]=");
                    Screen_ShowFreshen(); 
                    keynum = 1 ;       
                    addr_str = 0; 
                    goto state4;                       
                }
                else if( keynum=='0' )
                    P_Index.Menu_KeyFunc=0;   
            }
            break;
            
        case 2:
state2:     Screen_Clear();
            Screen_ShowStr(0,0,"该矩阵的秩为:");
            Screen_ShowNum(0,2,zhiresult);    
        
            if(hang1==lie1)
            {
                if(zhiresult==hang1)
                    Screen_ShowStr(0,4,"该矩阵满秩!");
                else 
                    Screen_ShowStr(0,4,"该矩阵退化矩阵!");
            }
            Screen_ShowStr(0,6,"按'='键返回");
            Screen_ShowFreshen();
            state = 4;
            break;
        case 4:
            if( keynum>0 )
            {
                if( keynum == '=' )
                   state=0;
            }  
            break;
            
        case 3:
            if( keynum>0 )
            {
                
state4:         //Screen_Clear();
//                Screen_ShowNum(0,2,zhiresult);    

                if(( keynum==4 )&&( addr_str>0 ))
                {
                    addr_str--;
                }
                else if(( keynum==9 )&&( addr_str< (lie1*hang1-2)))
                {
                    addr_str++;
                } 
                else if( keynum=='=' )
                    P_Index.Menu_KeyFunc=0;   
                                
                Screen_ShowAsc(16,0,(addr_str)/lie1+'1');
                Screen_ShowAsc(40,0,(addr_str)%lie1+'1');
                Screen_ShowStr(0,2,"                ");
                Screen_ShowNum(0,2,Ranks_result[addr_str/lie1][addr_str%lie1]);
                
                if(  (hang1+lie1)>1 )
                {
                    Screen_ShowAsc(16,4,(addr_str+1)/lie1+'1');
                    Screen_ShowAsc(40,4,(addr_str+1)%lie1+'1');
                    Screen_ShowStr(0,6,"                ");
                    Screen_ShowNum(0,6,Ranks_result[(addr_str+1)/lie1][(addr_str+1)%lie1]);    
                }                    
                Screen_ShowFreshen();
            }
            break; 
    }        
}
