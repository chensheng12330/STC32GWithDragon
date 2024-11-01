#include "set.h"
#include "rtc.h"
#include "menu.h"

u8 xdata Parma[10]={3,3,1,4,0,0,0,0,0,0};

void SetParm_Write(void)
{
    u8 xdata i;
    IapErase(0x0000);
    Parma[9] = 0xc8;
    for(i=0;i<10;i++)
    {
        IapProgram(0x0000+i, Parma[i]);  
    }        
}

void SetParm_Read(void)
{
    u8 xdata i;
    for(i=0;i<10;i++)
       Parma[i] = IapRead(0x0000+i); //P1=0x12
    if( Parma[9]!=0xc8 )    //强制初始化
    {
        Parma[9] = 0xc8;
        Parma[0]=3;
        Parma[1]=3;
        Parma[2]=1;
        Parma[3]=4;
        SetParm_Write();
    }
}  

void Set_Light(u8 keynum)
{
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,2,"屏幕亮度");
            Screen_ShowAscOpp(80,2,(u8)(Parma[0]+'0'));
            Screen_ShowFreshen();
            state = 1;   
            //break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&(Parma[0]>0))
                {
                    Parma[0]--;
                    SetParm_Write();
                }
                else if(( keynum==9 )&&(Parma[0]<5))
                {
                    Parma[0]++;
                    SetParm_Write();
                }  
                else if(( keynum==22 )||( keynum=='=' ))
                {
                    P_Index.Menu_KeyFunc = 0;
                }
                Screen_Showlight(Parma[0]);
                Screen_ShowAscOpp(80,2,Parma[0]+'0');
                Screen_ShowFreshen();      
            }
            break;   
    }    
}

void Set_LightShift(u8 keynum)
{
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,2,"shift提示");
            Screen_ShowAscOpp(80,2,Parma[1]+'1');
            Screen_ShowFreshen();
            state = 1;   
            //break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&(Parma[1]>1))
                {
                    Parma[1]--;
                    SetParm_Write();
                }
                else if(( keynum==9 )&&(Parma[1]<5))
                {
                    Parma[1]++;
                    SetParm_Write();
                } 
                else if(( keynum==22 )||( keynum=='=' ))
                {
                    P_Index.Menu_KeyFunc = 0;
                }                
                //Screen_Showlight(Parma[0]);
                Screen_ShowAscOpp(80,2,Parma[1]+'0');
                Screen_ShowFreshen();      
            }
            break;   
    }    
}

void Set_Sleep(u8 keynum)
{
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(0,0,"节电模式");
            
            Screen_ShowStr(8,4,"无操作  分钟后");
        
            Screen_ShowAscOpp(56,4,Parma[2]+'1');
            Screen_ShowAscOpp(64,4,' ');
            Screen_ShowFreshen();
            state = 1;   
            //break;
     
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum==4 )&&(Parma[2]>1))
                {
                    Parma[2]--;
                    SetParm_Write();
                }
                else if(( keynum==9 )&&(Parma[2]<9))
                {
                    Parma[2]++;
                    SetParm_Write();
                } 
                else if(( keynum==22 )||( keynum=='=' ))
                {
                    P_Index.Menu_KeyFunc = 0;
                }                
                Screen_Showlight(Parma[0]);
                Screen_ShowAscOpp(56,4,Parma[2]+'0');
                Screen_ShowAscOpp(64,4,' ');
                Screen_ShowFreshen();      
            }
            break;   
    }    
}

u8 xdata saddr = 0;
u8 xdata sdata[6] ;
void Set_Data(u8 keynum)
{
    u8 i;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(16,0,"20");
            Screen_ShowStr(48,0,"年");
            Screen_ShowStr(48,2,"月");
            Screen_ShowStr(48,4,"日");        
            Screen_ShowFreshen();
            state = 1; 
            keynum = 8;  
            saddr = 0;  
            sdata[0] = YEAR/10+'0';
            sdata[1] = YEAR%10+'0';
        
            sdata[2] = MONTH/10+'0';
            sdata[3] = MONTH%10+'0'; 

            sdata[4] = DAY/10+'0';
            sdata[5] = DAY%10+'0'; 
        
        
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='0' )&&( keynum<='9' ))
                {
                    if( saddr<6 )
                        sdata[saddr++] = keynum;
                }
                else if(( keynum==8 )&&(saddr>0))
                {
                    saddr--;
                }
                else if(( keynum==10 )&&(saddr<6))
                {
                    saddr++;
                } 
                else if( keynum=='=' )
                {
                      INIYEAR = (u8)((sdata[0]-'0')*10+(sdata[1]-'0'));   //继承当前年月日
                    INIMONTH = (u8)((sdata[2]-'0')*10+(sdata[3]-'0'));
                    INIDAY = (u8)((sdata[4]-'0')*10+(sdata[5]-'0'));                       
                     INIHOUR = HOUR;   //修改时分秒
                    INIMIN = MIN;
                    INISEC = SEC;   
RTCCFG |= 0x01;   //触发RTC寄存器初始化                   
                    P_Index.Menu_KeyFunc = 0;
                }
                else if(( keynum==22 ))
                {
                    P_Index.Menu_KeyFunc = 0;
                }                
                for( i=0;i<6;i++ )
                {
                    if( i!=saddr )
                    {
                        Screen_ShowAsc(32+8*(i%2),2*(i/2),sdata[i]);
                    }
                    else
                        Screen_ShowAscOpp(32+8*(i%2),2*(i/2),sdata[i]);
                }
              
                Screen_ShowFreshen();      
            }
            break;   
    }    
}

void Set_Time(u8 keynum)
{
    u8 i;
    switch( state )
    {
        case 0://初始化
            Screen_Clear();
            Screen_ShowStr(48,0,"时");
            Screen_ShowStr(48,2,"分");
            Screen_ShowStr(48,4,"秒");        
            Screen_ShowFreshen();
            state = 1; 
            keynum = 8;  
            saddr = 0;  
            sdata[0] = HOUR/10+'0';
            sdata[1] = HOUR%10+'0';
        
            sdata[2] = MIN/10+'0';
            sdata[3] = MIN%10+'0'; 

            sdata[4] = SEC/10+'0';
            sdata[5] = SEC%10+'0'; 
        
        
        case 1://等待输入
            if( keynum>0 )
            {
                if(( keynum>='0' )&&( keynum<='9' ))
                {
                    if( saddr<6 )
                        sdata[saddr++] = keynum;
                }
                else if(( keynum==8 )&&(saddr>0))
                {
                    saddr--;
                }
                else if(( keynum==10 )&&(saddr<6))
                {
                    saddr++;
                } 
                else if( keynum=='=' )
                {
                      INIYEAR = YEAR;   //继承当前年月日
                    INIMONTH = MONTH;
                    INIDAY = DAY;                       
                     INIHOUR = (u8)((sdata[0]-'0')*10+(sdata[1]-'0'));   //修改时分秒
                    INIMIN = (u8)((sdata[0]-'0')*10+(sdata[1]-'0'));
                    INISEC = (u8)((sdata[0]-'0')*10+(sdata[1]-'0'));   
RTCCFG |= 0x01;   //触发RTC寄存器初始化                    
                    P_Index.Menu_KeyFunc = 0;
                }
                else if(( keynum==22 ))
                {
                    P_Index.Menu_KeyFunc = 0;
                }                
                for( i=0;i<6;i++ )
                {
                    if( i!=saddr )
                    {
                        Screen_ShowAsc(32+8*(i%2),2*(i/2),sdata[i]);
                    }
                    else
                        Screen_ShowAscOpp(32+8*(i%2),2*(i/2),sdata[i]);
                }
              
                Screen_ShowFreshen();      
            }
            break;   
    }    
}


void Show_time(void)
{
    char xdata showdat[16];
	uchar xdata key = 0;
	static bit shake=0;
    if( B1S_Flag==1 )
    {
        B1S_Flag = 0;
        sprintf(showdat,"20%02d-%02d-%02d",YEAR,MONTH,DAY);
        Screen_ShowStr(0,0,showdat);        //显示年月日
        
        OLED_BuffShowAscll32(0,2,HOUR/10);
        OLED_BuffShowAscll32(16,2,HOUR%10);
        
       
        OLED_BuffShowAscll32(48,2,MIN/10);
        OLED_BuffShowAscll32(64,2,MIN%10);  
        
        OLED_BuffShowAscll32(96,2,SEC/10);
        OLED_BuffShowAscll32(112,2,SEC%10);          
       if( shake==1 )
       {
          OLED_BuffShowAscll32(32,2,10);   
          OLED_BuffShowAscll32(80,2,10);
       }
       else
       {
          OLED_BuffShowAscll32(32,2,11);   
          OLED_BuffShowAscll32(80,2,11);
       }           
        shake = !shake;
        Screen_ShowFreshen();
    }
}
