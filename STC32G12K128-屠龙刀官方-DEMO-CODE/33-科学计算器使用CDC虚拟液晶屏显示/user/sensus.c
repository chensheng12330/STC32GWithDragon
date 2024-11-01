#include "Sensus.h"
#include "menu.h"
#include "stdlib.h"
#include "fun.h"
#include <math.h>

u8 sensustotal = 5;
u8 sensusstr[12];
double sensusnum[25]={3,3,3,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

u8 sensusstrlength=0;
u8 sensusstrnum=0;

void Sensus_Set(u8 keynum)
{
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now n=");
            Screen_ShowNum(48,0,sensustotal);
            Screen_ShowStr(0,2,"end with '='");
            Screen_ShowStr(0,4,"    (2<=n<=25)");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;
            sensusstrlength = 0;
            memset(sensusstr,'\0',12);

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='0' )&&( keynum<='9' ))
                {
                    if(sensusstrlength<2)
                    {
                        Screen_ShowAsc(32+8*sensusstrlength,6,keynum);
                        sensusstr[sensusstrlength++] = keynum;
                    }

                }
                else if( keynum == '=' )
                {
                    sensustotal = (u8)atof(sensusstr);
                    if( sensustotal>25 )
                        sensustotal = 25;
                    if(sensustotal<2 )
                        sensustotal = 2; 
                    P_Index.Menu_KeyFunc=0;                    
                }
                Screen_ShowFreshen();
            }
            break; 
    }    
}

void Sensus_Amend(u8 keynum)
{
    static u8 addr_str=0;
    u8 i,j;
    switch( state )
    {
        case 0://初始化
            
            Screen_Clear();
            sensusstrnum = 0;
            Screen_ShowStr(0,0,"Now data ");
            Screen_ShowNum(72,0,sensusstrnum+1);
            Screen_ShowNum(0,2,sensusnum[sensusstrnum]);
            Screen_ShowStr(0,4," :上翻   :下翻");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;
            sensusstrlength=0;
            memset(sensusstr,'\0',12);
            sensusstrnum = 0;        
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&( sensusstrnum>0 ))
                {
                    sensusstrnum--;
                    Screen_ShowStr(0,2,"               ");
                    Screen_ShowStr(72,0,"  ");
                    Screen_ShowNum(72,0,sensusstrnum+1);
                    Screen_ShowNum(0,2,sensusnum[sensusstrnum]);
                    Screen_ShowStr(32,6,"            ");
                    sensusstrlength=0;
                    memset(sensusstr,'\0',12);                    
                }
                else if(( keynum==9 )&&( sensusstrnum<sensustotal-1))
                {
                    sensusstrnum++;
                    Screen_ShowStr(0,2,"               ");
                    Screen_ShowStr(72,0,"  ");
                    Screen_ShowNum(72,0,sensusstrnum+1);
                    Screen_ShowNum(0,2,sensusnum[sensusstrnum]);
                    Screen_ShowStr(32,6,"            ");
                    sensusstrlength=0;
                    
                    memset(sensusstr,'\0',12);   
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    sensusstr[sensusstrlength]=keynum;
                    sensusstrlength++;
                }
                else if( keynum==21 )
                {
                    sensusstrlength--;
                    sensusstr[sensusstrlength]=' '; 
                } 
                else if( keynum=='=' )
                {
                    if( sensusstrnum<sensustotal-1 )
                    {
                        sensusnum[sensusstrnum] = atof(sensusstr);
                        sensusstrnum++;
                        Screen_ShowStr(0,2,"            ");
                        Screen_ShowStr(72,0,"  ");
                        Screen_ShowNum(72,0,sensusstrnum+1);
                        Screen_ShowNum(0,2,sensusnum[sensusstrnum]);
                        Screen_ShowStr(32,6,"            ");
                        sensusstrlength=0;
                        memset(sensusstr,'\0',12); 
                        
                    }
                    else
                    {
                       sensusnum[sensusstrnum] = atof(sensusstr); 
                       Screen_ShowNum(0,2,sensusnum[sensusstrnum]);
                    }
                }
                Screen_ShowStr(32,6,sensusstr);
                Screen_ShowFreshen();      
            }
            break;   
    }    
}
double xdata a[25],b[25],c[25];
double xdata s[14];

	void cal_data(double *a,double *c,double *s,int n)
	{
////总和，平均数，中位数，最大值，最小值 ，极差 ，方差，修正方差，标准差，修正标准差
////变异系数，平均偏差，调和平均数，几何平均数
		uchar i,j;
		double t;
		double sum;
/*****************************************************/		
		sum=0;
		for(i=0;i<n;i++)
			sum=sum+(*a+i);
		s[0]=sum;										 //求和
		s[1]=s[0]/n;								 //平均数
		
/*****************************************************/	
		for(i=0;i<n;i++)
			(*(c+i))=(*(a+i));
		for(i=0;i<n;i++)
			for(j=0;j<n-i-1;j++)
			{
				if((*(c+j))>(*(c+j+1)))
				{
					t=(*(c+j));
					(*(c+j))=(*(c+j+1));
					(*(c+j+1))=t;
					}
				}
		s[3]=(*(c+n-1));                 //最大值
		s[4]=(*c);										  //最小值
		s[5]=s[3]-s[4];									//极差
		if(n%2==0)
			s[2]=((*(c-1+n/2))+(*(c+n/2)))/2;
		else if(n%2==1)
			s[2]=(*c+(n-1)/2);           //中位数

/*****************************************************/

		sum=0;
		for(i=0;i<n;i++)
			sum=sum+1/((*a+i));
		s[12]=n/sum;                       //调和平均数

/*****************************************************/
		
		sum=1;
		for(i=0;i<n;i++)
			sum=sum*((*a+i));
		s[13]=pow(sum,1.0/(double)n);      //几何平均数     
		
/*****************************************************/	
	
		sum=0;
		for(i=0;i<n;i++)
			sum=sum+pow(((*a+i)-s[1]),2);
		s[6]=sum/n;                        //方差
		s[7]=sum/(n-1);										 //修正方差
		s[8]=pow(s[6],0.5);									 //标准差
		s[9]=pow(s[7],0.5);								 //修正标准差
		s[10]=s[8]/s[1];                     //变异系数
	
/*****************************************************/

		t=0,sum=0;
		for(i=0;i<n;i++)
		{
			t=(*a+i)-s[1];
			if(t<0)
				t=-t;
			sum=sum+t;
			}
		s[11]=sum/(float)n;                //平均偏差
		
/*****************************************************/

	}
char  code todm[][16]=
{
    "1.总和:",
    "2.平均数\xfd:",
    "3.中位数\xfd:",
    "4.最大值:",
    "5.最小值:",
    "6.极差:",
    "7.方差:",
    "8.修正\xfd方差:",
    "9.标准差:",
    "10.修正\xfd标准差:",
    "11.变异系数\xfd:",
    "12.平均偏差:",
    "13.调和平均数\xfd:",
    "14.几何平均数\xfd:",
}	;
    
void Sensus_Check(u8 keynum)        //数据查看
{
    static u8 find_str=0;
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
			find_str = 0;
            state = 1;   
			cal_data(sensusnum,c,s,sensustotal);
        
            Screen_ShowStr(0,0,(char*)todm[find_str]);
            sprintf(sensusstr,"%g",s[find_str]);
			Screen_ShowStr(0,2,sensusstr);
        
            Screen_ShowStr(0,4,(char*)todm[find_str+1]);
            sprintf(sensusstr,"%g",s[find_str+1]);
			Screen_ShowStr(0,6,sensusstr);
        
            Screen_ShowFreshen();  
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&( find_str>0 ))
                {
                    find_str--;
                    Screen_Clear();
                    Screen_ShowStr(0,0,(char*)todm[find_str]);
                    sprintf(sensusstr,"%g",s[find_str]);
                    Screen_ShowStr(0,2,sensusstr);
                
                    Screen_ShowStr(0,4,(char*)todm[find_str+1]);
                    sprintf(sensusstr,"%g",s[find_str+1]);
                    Screen_ShowStr(0,6,sensusstr);
                
                    Screen_ShowFreshen();                      
                }
                else if(( keynum==9 )&&( find_str<12))
                {
                    find_str++;
                    Screen_Clear();
                    Screen_ShowStr(0,0,(char*)todm[find_str]);
                    sprintf(sensusstr,"%g",s[find_str]);
                    Screen_ShowStr(0,2,sensusstr);
                
                    Screen_ShowStr(0,4,(char*)todm[find_str+1]);
                    sprintf(sensusstr,"%g",s[find_str+1]);
                    Screen_ShowStr(0,6,sensusstr);
                
                    Screen_ShowFreshen();  
                }  
            }
            break;   
    }    
}

//void mode_num(double *a,double *c,double *b,uchar *index,int n,int *count1)                      //众数
//{
//    uchar i;
//    int count=1;
//    for(i=0;i<n-1;) 
//    {
//        while((*c+i)==(*c+i+1)) 
//        {
//            count++;
//            i++;
//        }
//        if(count>(*count1))
//        {	
//            (*count1)=count;
//            (*index)=0;
//            (*(b+*index))=(*a+i);
//        }
//        else if(count==(*count1)) 
//        {
//            (*index)++;
//            (*(b+*index))=(*a+i);
//        }
//        count=1;
//        i++;
//    }

//}
void mode_num(double (*a),double (*c),double (*b),uchar *index,int n,int *count1)                      //众数
{
    u8 i=1;
    u8 j=0;
    static u8 xdata countdata[25];
    memset( countdata,1,25 );
      
    *count1 = 0;
    for(i=1;i<n;i++)    //遍历数组
    {
//        Screen_ShowNum(i*16,2,a[i]); 
        for(j=0;j<i;j++)    //从头到当前目标查找有无重复数字
        {   
            if( a[j]== a[i] )      //当前值等于前面某个值
            {
                countdata[j]++;
                break;
            }
        }  
    }
//    for( i=0;i<n;i++ )
//    {
//        Screen_ShowNum(i*16,2,countdata[i]);
//    }
    
    j = 0;
    for( i=0;i<n;i++ )      //查找最多的是几次
    {
        if( countdata[i]>j )
            j = countdata[i];
    }
//    Screen_ShowNum(0,4,j);
    for( i=0;i<n;i++ )
    {
        if( countdata[i]==j )       //最多次数的数字
        {
            *(b+*count1) = *(a+i);
            (*count1)++;
        }
    }
}
    
void Sensus_Check2(u8 keynum)        //数据查看
{
    static u8 find_str=0;
    static uchar index=0;
	static int count1=1;
	static int count2=1;
    u8 i;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
			find_str = 0;
            state = 1;   
            //cal_data(sensusnum,c,s,sensustotal);
			mode_num(sensusnum,c,b,&index,sensustotal,&count1);
        
            if( count1==0 )
            {
                Screen_ShowStr(0,0,"没有众数\xfd");
                Screen_ShowFreshen();              
            }
            else if( count1>0  )
            {
                for(i=0;i<4;i++)
                {
                    if( i<count1 )
                    {
                        sprintf(sensusstr,"%g",(*(b+i))); 
                        Screen_ShowStr(0,i*2,sensusstr);
                    }                        
                }                        
            }
            keynum = 4;
//            b[0]=1;b[1]=2;b[2]=3;b[3]=4;b[04]=5;b[5]=6;count1 = 6;
//            
//            Screen_ShowFreshen();  
//            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&( find_str>0 ))
                {
                    find_str--;
                }
                else if(( keynum==9 )&&( find_str+4<count1))
                {
                    find_str++;
                } 
                else if( keynum=='=' )      //返回菜单
                {
                    P_Index.Menu_KeyFunc = 0; 
                }
                if( count1>0 )
                {
                    Screen_Clear();
                    for(i=0;i<4;i++)
                    {
                        if( (i+find_str)<count1 )
                        {
                            sprintf(sensusstr,"众数\xfd%d:%g",i+find_str+1,(*(b+i+find_str))); 
                            Screen_ShowStr(0,i*2,sensusstr);
                        }                        
                    }                        
                    Screen_ShowFreshen(); 
                }
            }
            break;   
    }    
}
