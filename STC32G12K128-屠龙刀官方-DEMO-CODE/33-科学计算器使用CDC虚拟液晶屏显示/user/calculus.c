#include "calculus.h"
#include "string.h"
#include "keyboard.h"
#include "fun.h"
#include "menu.h"
#include "stdlib.h"

uchar hanshu_buff[26]="x^2";//用来存函数表达式 
double hanshu_a = 1;
double hanshu_b = 2;
double hanshu_h = 0.2;
double hanshu_y = 2;

void calculus_funcWR(u8 keynum)
{
    #define IBlength 26
    static u8 addr=0;
    u8 result;
    static u8 shift_flag;
	static char input_buf[IBlength]={"    "};     //输入缓存   
    
    switch( state )
    {
        case 0://初始化
//            memset(input_buf,'\0',IBlength);
            Screen_Clear();
            Screen_ShowStr(0,(u8)(0),"右边的函数\xfd是");
            Screen_ShowStr(0,(u8)(2),input_buf);
            Screen_ShowFreshen();
            memset(input_buf,'\0',IBlength);
            addr = 0;
            state++;
            break;
     
        case 1://等待输入
            Cursor_Blink((u8)(addr*8),4,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )       //左移
                {
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )     //右移
                {
                    if( input_buf[addr]!=0 )
                        addr+=1;
                } 
                else if( keynum == 21 )     //清除
                {
                    Screen_Clear();     //后期换成清楚最后一位的函数
                    String_Delete(&input_buf,IBlength,addr);
                    if( addr>0 )
                        addr--;
                }                 
                else if( keynum == 22 )     //清屏
                {
                    memset(input_buf,'\0',IBlength);                   
                    Screen_Clear();
                    addr=0;
                }   
                else if( keynum == 17 )
                {                  
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),"1.sinh");
                    Screen_ShowStr(0,(u8)(2),"2.cosh");
                    Screen_ShowStr(0,(u8)(4),"3.tanh");
                    state = 5;
                }                   
                else if( keynum == '=' )        //结束函数输入
                {
                  //  hanshu_buff = 
                    addr = String_length(input_buf);
                    input_buf[addr]='\0';
                    P_Index.Menu_KeyFunc=0;                    
                    Screen_Clear();
                    strcpy(hanshu_buff,input_buf);

                }    
                else if( keynum == 1 )        //其它功能
                {
                    shift_flag = !shift_flag;
                }                  
                else
                {
                    if( shift_flag )
                    {
                        keynum += 0x80;
                        shift_flag = 0;
                    }
                    result=String_Insert(&input_buf,IBlength,addr,keynum);
                    if( result!=0xff )
                    {
                        addr += result;
                    }                    
                } 
                if( state==1 )
                    Screen_ShowStr(0,(u8)(4),&input_buf);
                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;


        case 5://特殊功能輸入
            if( keynum>0 )
            {
                if(( keynum=='1' )||( keynum=='2' )||( keynum=='3' ))
                {
                    addr +=String_Insert(&input_buf,IBlength,addr,120+keynum-'1');  
                    state=1;
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(4),&input_buf);
                    Screen_ShowFreshen();
                }     
            }                
            break;
        
    }
}

void calculus_funcWRa(u8 keynum)
{
    #define IBlength 26
    static u8 addr=0;
	static char input_buf[IBlength]={"    "};     //输入缓存   
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,(u8)(0),"a =");
            Screen_ShowNum(0,2,hanshu_a);
            Screen_ShowFreshen();
            memset(input_buf,'\0',IBlength);
            addr = 0;
            state++;
            break;
     
        case 1://等待输入
            Cursor_Blink((u8)(addr*8),4,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )       //左移
                {
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )     //右移
                {
                    if( input_buf[addr]!=0 )
                        addr+=1;
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    input_buf[addr]=keynum;
                    addr++;
                }
                else if( keynum==21 )
                {
                    addr--;
                    input_buf[addr]=' '; 
                } 
                else if( keynum=='=' )
                {
                    hanshu_a= atof(input_buf);
                    P_Index.Menu_KeyFunc=0;   
                }
                if( state==1 )
                    Screen_ShowStr(0,(u8)(4),&input_buf);

                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
    }
}

void calculus_funcWRb(u8 keynum)
{
    #define IBlength 26
    static u8 addr=0;
	static char input_buf[IBlength]={"    "};     //输入缓存   
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,(u8)(0),"b =");
            Screen_ShowNum(0,2,hanshu_b);
            Screen_ShowFreshen();
            memset(input_buf,'\0',IBlength);
            addr = 0;
            state++;
            break;
     
        case 1://等待输入
            Cursor_Blink((u8)(addr*8),4,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )       //左移
                {
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )     //右移
                {
                    if( input_buf[addr]!=0 )
                        addr+=1;
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    input_buf[addr]=keynum;
                    addr++;
                }
                else if( keynum==21 )
                {
                    addr--;
                    input_buf[addr]=' '; 
                } 
                else if( keynum=='=' )
                {
                    hanshu_b= atof(input_buf);
                    P_Index.Menu_KeyFunc=0;   
                }
                if( state==1 )
                    Screen_ShowStr(0,(u8)(4),&input_buf);

                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
    }
}
void calculus_funcWRh(u8 keynum)
{
    #define IBlength 26
    static u8 addr=0;
	static char input_buf[IBlength]={"    "};     //输入缓存   
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,(u8)(0),"h =");
            Screen_ShowNum(0,2,hanshu_h);
            Screen_ShowFreshen();
            memset(input_buf,'\0',IBlength);
            addr = 0;
            state++;
            break;
     
        case 1://等待输入
            Cursor_Blink((u8)(addr*8),4,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )       //左移
                {
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )     //右移
                {
                    if( input_buf[addr]!=0 )
                        addr+=1;
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    input_buf[addr]=keynum;
                    addr++;
                }
                else if( keynum==21 )
                {
                    addr--;
                    input_buf[addr]=' '; 
                } 
                else if( keynum=='=' )
                {
                    hanshu_h= atof(input_buf);
                    P_Index.Menu_KeyFunc=0;   
                }
                if( state==1 )
                    Screen_ShowStr(0,(u8)(4),&input_buf);

                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
    }
}

void calculus_funcWRy(u8 keynum)
{
    #define IBlength 26
    static u8 addr=0;
	static char input_buf[IBlength]={"    "};     //输入缓存   
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,(u8)(0),"右端点y的值=");
            Screen_ShowNum(0,2,hanshu_y);
            Screen_ShowFreshen();
            memset(input_buf,'\0',IBlength);
            addr = 0;
            state++;
            break;
     
        case 1://等待输入
            Cursor_Blink((u8)(addr*8),4,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )       //左移
                {
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )     //右移
                {
                    if( input_buf[addr]!=0 )
                        addr+=1;
                }  
                else if((( keynum>='0' )&&(keynum<='9'))||( keynum=='.' )||( keynum=='-' ))
                {
                    input_buf[addr]=keynum;
                    addr++;
                }
                else if( keynum==21 )
                {
                    addr--;
                    input_buf[addr]=' '; 
                } 
                else if( keynum=='=' )
                {
                    hanshu_y= atof(input_buf);
                    P_Index.Menu_KeyFunc=0;   
                }
                if( state==1 )
                    Screen_ShowStr(0,(u8)(4),&input_buf);

                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
    }
}



void calculus_funcShow(u8 keynum)
{
    static int n=0;
    u8 i= 0;
    char dat[16];
    static u8 addr_str=0;
    double x_array[10];//微分方程的解的列表 
	double y_array[10];
    double temp_0  =0;//临时值 
    double temp_1  =0;
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            addr_str = 0;
            n = (int)(hanshu_b-hanshu_a)/hanshu_h;
        
            if( hanshu_a>hanshu_b )
                Screen_ShowStr(0,(u8)(0),"a>b,wrong");
            else if( hanshu_h<0 )
                Screen_ShowStr(0,(u8)(0),"h<0,wrong");            
            else if( n<0 )
            {
                Screen_ShowStr(0,(u8)(0),"请尝试缩小(b-a)");  
                Screen_ShowStr(0,(u8)(2),"或者增大h");
            }                
            else
            {
                x_array[0]=hanshu_a;
                y_array[0]=hanshu_y;        
                for(i=0;i<n+1;i++)
                {
                
                    x_array[i+1]=x_array[i]+hanshu_h;
                    
                //  uchar alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult);
                //  y[i+1]=y[i]+h * g(x[i],y[i]);
                    x=x_array[i];
                    y=y_array[i];
                    alg_calc_expression(hanshu_buff, &temp_0, 0);
                    y_array[i+1]=y_array[i] + hanshu_h * temp_0;
                //  y[i+1]=y[i]+h/2*( g( x[i],y[i] ) + g( x[i+1],y[i+1] ) );
                    x=x_array[i+1];
                    y=y_array[i+1];
                    alg_calc_expression(hanshu_buff, &temp_1, 0);
                    y_array[i+1]=y_array[i]+hanshu_h/2*( temp_0+ temp_1 );
                }
                i=0;                
            }
            Screen_ShowFreshen();

            state++;
            keynum = 4;
            //break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&( addr_str>0 ))
                {
                    addr_str--;
                      
                }
                else if(( keynum==9 )&&( addr_str<n))
                {
                    addr_str++;

                }  
                //P_Index.Menu_KeyFunc=0;   
                Screen_Clear();
                sprintf(dat,"x[%d]=",addr_str);
                Screen_ShowStr(0,(u8)(0),dat);
                Screen_ShowNum(0,2,x_array[addr_str]);
                sprintf(dat,"y[%d]=",addr_str);
                Screen_ShowStr(0,(u8)(4),dat);
                Screen_ShowNum(0,6,y_array[addr_str]);                
                keynum = 0;
                Screen_ShowFreshen();
            }
            
            break;
    }
}
