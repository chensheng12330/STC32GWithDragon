#include "funcpic.h"
#include "menu.h"
#include "stdlib.h"
#include "keyboard.h"
#include "fun.h"

double fUNC_X0=63;
double fUNC_Y0=31;
double fUNC_KX=1;
double fUNC_KY=1;

u8 funcstr[26];
u8 funcstrlength;
u8 funcstraddr;
u8 shift_flagf;
void FuncPic_SetX0(u8 keynum)
{
    static keynum_last;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now X0 is");
            Screen_ShowNum(0,2,(double)fUNC_X0);
            Screen_ShowStr(0,4,"end with '='");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            state = 1;
            funcstrlength  =  0;
            memset(funcstr,'\0',12);
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='9' ))
                {
                    funcstr[funcstrlength]=keynum;
                    funcstrlength++;
                    Screen_ShowStr(32,6,funcstr);
                    Screen_ShowFreshen();
                }
                else if( keynum == '=' )
                {
                    if( funcstrlength>0 )
                        fUNC_X0= atof(funcstr);   
                    P_Index.Menu_KeyFunc=0;                    
                    Screen_Clear();                    
                } 
            }
            break; 
    }    
}

void FuncPic_SetY0(u8 keynum)
{
    static keynum_last;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now Y0 is");
            Screen_ShowNum(0,2,(double)fUNC_Y0);
            Screen_ShowStr(0,4,"end with '='");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            funcstrlength  =  0;
            memset(funcstr,'\0',12);        
            state = 1;

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='9' ))
                {
                    funcstr[funcstrlength]=keynum;
                    funcstrlength++;
                    Screen_ShowStr(32,6,funcstr);
                    Screen_ShowFreshen();
                }
                else if( keynum == '=' )
                {
                    if( funcstrlength>0 )
                        fUNC_Y0= atof(funcstr);   
                    P_Index.Menu_KeyFunc=0;                    
                    Screen_Clear();                    
                } 
            }
            break; 
    }    
}

void FuncPic_SetKX(u8 keynum)
{
    static keynum_last;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now KX is");
            Screen_ShowNum(0,2,(double)fUNC_KX);
            Screen_ShowStr(0,4,"end with '='");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            funcstrlength  =  0;
            memset(funcstr,'\0',12);
            state = 1;

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='9' ))
                {
                    funcstr[funcstrlength]=keynum;
                    funcstrlength++;
                    Screen_ShowStr(32,6,funcstr);
                    Screen_ShowFreshen();
                }
                else if( keynum == '=' )
                {
                    if( funcstrlength>0 )
                        fUNC_KX= atof(funcstr);   
                    P_Index.Menu_KeyFunc=0;                    
                    Screen_Clear();                    
                } 
            }
            break; 
    }    
}


void FuncPic_SetKY(u8 keynum)
{
    static keynum_last;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"Now KY is");
            Screen_ShowNum(0,2,(double)fUNC_KY);
            Screen_ShowStr(0,4,"end with '='");
            Screen_ShowStr(0,6,"new=");
            Screen_ShowFreshen();
            funcstrlength  =  0;
            memset(funcstr,'\0',12);
            state = 1;

            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='1' )&&( keynum<='9' ))
                {
                    funcstr[funcstrlength]=keynum;
                    funcstrlength++;
                    Screen_ShowStr(32,6,funcstr);
                    Screen_ShowFreshen();
                }
                else if( keynum == '=' )
                {
                    if( funcstrlength>0 )
                        fUNC_KY= atof(funcstr);   
                    P_Index.Menu_KeyFunc=0;                    
                    Screen_Clear();                    
                } 
            }
            break; 
    }    
}

void FuncPic_Write(u8 keynum)
{
    u8 result;
    static keynum_last;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"输入函数\xfd表达式");
            Screen_ShowStr(0,2,"F(x)=");
            Screen_ShowFreshen();
            funcstrlength  =  0;
            funcstraddr = 0;
            memset(funcstr,'\0',26);
            state = 1;

            break;
     
        case 1://等待输入
a:          Cursor_Blink((u8)(funcstraddr*8),4,50); 
        
            if( keynum>0 )
            {
                if( keynum == 8 )           //光标向前
                {
                    if(funcstraddr>=1)
                        funcstraddr-=1;
                }
                else if( keynum == 10 )     //光标向后
                {
                    if( funcstr[funcstraddr]!='\0' )
                        funcstraddr+=1;
                } 
                else if( keynum == 21 )     //清除显示
                {
                    Screen_Clear();
                    Screen_ShowStr(0,0,"输入函数\xfd表达式");
                    Screen_ShowStr(0,2,"F(x)=");                    
                    String_Delete(&funcstr,26,funcstraddr);
                    if( funcstraddr>0 )
                        funcstraddr--;
                }                 
                else if( keynum == 22 )
                {
                    memset(funcstr,'\0',26);                 
                    Screen_Clear();
                    Screen_ShowStr(0,0,"输入函数\xfd表达式");
                    Screen_ShowStr(0,2,"F(x)=");    
                    funcstraddr=0;
                }                     
                else if( keynum == '=' )        //計算結果
                {
                    Screen_Clear();
                    state = 2;
                }  
                else if( keynum == 1 )        //其它功能
                {
                    shift_flagf = !shift_flagf;
                }                     
                else
                {
                    if( shift_flagf )
                    {
                        keynum += 0x80;
                        shift_flagf = 0;
                    }                   
                    result=String_Insert(&funcstr,26,funcstraddr,keynum);
                    if( result!=0xff )
                    {
                        funcstraddr += result;
                    }                    
                } 
                if( state==1 )
                    Screen_ShowStr(0,(u8)(4),&funcstr);
                Screen_ShowFreshen();
                keynum = 0;
            }
            break; 
        case 2://初始化
                if(0 == alg_calc_expression(funcstr, NULL, NULL))
                {	
                    Screen_ShowStr(0,(u8)(0),"1输入错误");
                }
                else
                {
                    //Screen_ShowStr(0,(u8)(0),"2输入正确");
                    P_Index.Menu_KeyFunc=0;     
                } 
                Screen_ShowFreshen();
                
                state = 3;
            break; 
        case 3://初始化


            break;                    
    }    
}

void display(char *temp1)    //显示函数图像函数
{
    uchar i;
    int c;
//    double x,y,z;
    
    Screen_Clear();
    
    if((fUNC_X0<=127)&&(fUNC_X0>=0)) 
    {
        Screen_ShowLine(fUNC_X0,0,fUNC_X0,63);
        if((fUNC_X0<=63)&&(fUNC_X0>=2))
        {
            Screen_ShowPoint(fUNC_X0-1,1);
            Screen_ShowPoint(fUNC_X0-2,2);
        }
        if((fUNC_X0<=61)&&(fUNC_X0>=0))
        {
            Screen_ShowPoint(fUNC_X0+1,1);
            Screen_ShowPoint(fUNC_X0+2,2);
        }
        for(i=0;i<=63;i++)
        {
            c=i-fUNC_Y0;
            if(c<0) c=-c;
            if(c%10==0)
            {
                if(fUNC_X0==0)
                    Screen_ShowLine(0,i,1,i);//lcd_xline(0,1,i);
                else if(fUNC_X0==127)
                    Screen_ShowLine(126,i,127,i);//lcd_xline(126,127,i);
                else 
                    Screen_ShowLine(fUNC_X0-1,i,fUNC_X0+1,i);//lcd_xline(fUNC_X0-1,fUNC_X0+1,i);
            }
        }
    }
    
    if((fUNC_Y0>=0)&&(fUNC_Y0<=63))
    {
        Screen_ShowLine(0,fUNC_Y0,127,fUNC_Y0);//lcd_xline(0,127,fUNC_Y0);
        if((fUNC_Y0<=63)&&(fUNC_Y0>=2))
        {
            Screen_ShowPoint(126,fUNC_Y0-1);             
            Screen_ShowPoint(125,fUNC_Y0-2);
        }
        if((fUNC_Y0<=61)&&(fUNC_Y0>=0))
        {
            Screen_ShowPoint(126,fUNC_Y0+1);
            Screen_ShowPoint(125,fUNC_Y0+2);
        }
        for(i=0;i<=127;i++)
        {
            c=i-fUNC_X0;
            if(c<0) 
                c=-c;
            if(c%10==0)
            {
                if(fUNC_Y0==0)
                    Screen_ShowLine(i,0,i,1);//lcd_yline(i,0,1,1);
                else if(fUNC_Y0==63)
                    Screen_ShowLine(i,62,i,63);//lcd_yline(i,62,63,1);
                else 
                    Screen_ShowLine(i,fUNC_Y0-1,i,fUNC_Y0-1);//lcd_yline(i,fUNC_Y0-1,fUNC_Y0+1,1);
            }
        }
    }
    for(i=0;i<=127;i++)
    {
        x=(i-fUNC_X0)*fUNC_KX;
        alg_calc_expression(temp1, &y, 0);
        ////////////********************
        y *= fUNC_KY;
        y=(fUNC_Y0-y);
        if((y<=63)&&(y>=0))
            Screen_ShowPoint(i,y);
    }
    Screen_ShowFreshen();
}
    
void FuncPic_ShowPic(u8 keynum)
{
    static keynum_last;
    
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            display(funcstr);
            state = 1;
            break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                switch( keynum )
                {
                    case '1':   fUNC_KY = fUNC_KY*2; break;
                    case '2':   fUNC_KY = fUNC_KY/2; break;
                    case '4':   fUNC_KX = fUNC_KX*2; break;
                    case '5':   fUNC_KX = fUNC_KX/2; break;  
                    case 4:     fUNC_Y0 = fUNC_Y0+10; break;   //上
                    case 9:     fUNC_Y0 = fUNC_Y0-10; break;   //下
                    case 8:     fUNC_X0 = fUNC_X0+10; break;   //左
                    case 10:    fUNC_X0 = fUNC_X0-10; break;                      
                }
                state = 0;
            }
            break; 
    }    
}


