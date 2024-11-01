#include "nor_cal.h"
#include "middle.h"
#include "string.h"
#include "keyboard.h"
#include "fun.h"

#define NULL ((void *) 0)       //空指针
#define IBlength 26             //输入数据个数的上限
u8 xdata SaveBuff[512] ;        //历史记录缓存数组
 

void normal_calculate(u8 keynum)        //普通计算
{
    static u8 xdata addr=0;                     //当前光标显示地址
    static u8 xdata momory = 0;                 //记忆组数地址
    u8 xdata result=0;                          //键盘输入字符串长度结果 0xff表示其它指令 
    static u8 xdata shift_flag=0;               //第二功能标志位
    double xdata re_result;                     //计算结果
	static char xdata input_buf[IBlength+1];    //输入缓存
	char xdata output_buf[16];                  //输出缓存   
    u16 xdata i=0;
    
    switch( state )
    {
        case 0:                                 //初始化
            memset(input_buf,'\0',IBlength);
            memset(output_buf,'\0',16);
            Screen_Clear();
 
        //-----------------------读取掉电参数--------------------
            for(i=0;i<512;i++)
                SaveBuff[i] = IapRead(0x0200+i); //P1=0x12
            if( SaveBuff[0]!= 0x05 )
            {
                memset(SaveBuff,0,512);
                SaveBuff[0] = 0x05;
            }
            
            addr = 0;
            state++;
            break;
     
        case 1://等待输入
a:          Cursor_Blink((u8)(addr*8),0,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )
                {
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )
                {
                    if( input_buf[addr]!=0 )
                        addr+=1;
                }
                else if( keynum == 4 )  //历史记录上翻
                {
                    state = 10;
                    momory = 0;
                    goto ad;
                }   
                else if( keynum == 9 )  //历史记录下翻
                {
                    state = 10;
                    momory = 0;
                    keynum = 4;     //强制从第一组记忆开始查看
                    goto ad;
                }                   
                else if( keynum == 21 )
                {
                    Screen_Clear();     //后期换成清楚最后一位的函数
                    String_Delete(&input_buf,IBlength,addr);
                    if( addr>0 )
                        addr--;
                }                 
                else if( keynum == 22 )
                {
                    memset(input_buf,'\0',IBlength);
                    memset(output_buf,'\0',16);                    
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
                else if( keynum == '=' )        //計算結果
                {
                    addr = String_length(input_buf);
                    input_buf[addr]='\0';
                    state = 2;
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
                        if( addr>=IBlength )
                        {
                            addr=IBlength;
                            input_buf[IBlength]='\0';
                        }
                    }                    
                } 
                if( state==1 )
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
        case 2://计算结果
            if( alg_calc_expression(input_buf, &re_result, NULL)==1 )
            {
                sprintf(output_buf,"%g",re_result);
                g_ans = re_result;
                Screen_ShowStr(0,6,output_buf);
                Screen_ShowFreshen();
                    
//------------------------------历史参数保存------------------------------
                //这里其实可以用循环队列
                if( SaveBuff[1]>=10 )       //数据大于10组
                {
                    for(i=0;i<(26*9);i++)        //
                        SaveBuff[4+i] = SaveBuff[4+26+i];
                    SaveBuff[1] = 9;
                }
                if( SaveBuff[1]<10 )
                {
                    for(i=0;i<26;i++)        //一个公式26个字符
                    {
                        SaveBuff[4+26*SaveBuff[1]+i] = input_buf[i];
                    }
                    SaveBuff[1]++;
                    IapErase(0x0200);
                    for(i=0;i<512;i++)
                        IapProgram(0x0200+i, SaveBuff[i]);                    
                } 
                        
            }
            else
            {
                Screen_ShowStr(0,6,"INPUT ERROR");
                Screen_ShowFreshen();
            }
            state = 4;
            break;
        case 3://显示历史数据
            break;
        
        case 4://等待下一次输入
            if( keynum>0 )
            {
                state= 1;  
                memset(input_buf,'\0',IBlength);
                memset(output_buf,'\0',16);
                Screen_Clear(); 
                addr = 0;                
                goto a;
            }  
            break;
        case 5://特殊功能輸入
            if( keynum>0 )
            {
                if(( keynum=='1' )||( keynum=='2' )||( keynum=='3' ))
                {
                    addr +=String_Insert(&input_buf,IBlength,addr,120+keynum-'1'); 
                    if( addr>=IBlength )
                    {
                        addr=IBlength;
                        input_buf[IBlength]='\0';
                    }                    
                    state=1;
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                    Screen_ShowFreshen();
                }     
            }                
            break;
        case 10://記憶顯示
            if( keynum>0 )
            {
                if(( keynum!=4 )&&( keynum!=9 ))
                {
                    Screen_Clear();
                    state = 1;
                    goto a;
                }
                    
ad:             if( SaveBuff[1]==0 )
                {
                    Screen_ShowStr(0,(u8)(0),"没有记忆");
                    Screen_ShowFreshen();                
                }
                else
                {
                    if(( keynum==4 ))  
                    {
                        if( ( momory>0 ) )
                            momory--;
                    }
                    else if(( keynum==9  )) 
                    {
                        if( momory<(SaveBuff[1]-1))
                            momory++;
                    }
                    for( i=0;i<26;i++ )
                        input_buf[i] = SaveBuff[momory*26+4+i];
                    
                    alg_calc_expression(input_buf, &re_result, NULL);
                    sprintf(output_buf,"%g",re_result);  
                    
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                    Screen_ShowStr(0,6,output_buf); 
                    Screen_ShowFreshen();
                }
            }                
            break;            
        
    }
}


void complex_calculate(u8 keynum)
{
    static u8 addr;
    u8 result;
    static u8 shift_flag=0;
    double re_result,im_result;       //计算结果
	char input_buf[IBlength];     //输入缓存
	char output_buf[16];    //输出缓存   
    P21=!P21;
    switch( state )
    {
        case 0://初始化
            memset(input_buf,'\0',IBlength);
            memset(output_buf,'\0',16);
            Screen_Clear();
            addr = 0;
            state++;
            break;
     
        case 1://等待输入
a:          Cursor_Blink((u8)(addr*8),0,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )
                {
                    //Screen_ShowFreshen();
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )
                {
                    //Screen_ShowFreshen();
                    if( input_buf[addr]!=0 )
                        addr+=1;
                } 
                else if( keynum == 21 )
                {
                    //Screen_ShowFreshen();
                    Screen_Clear();     //后期换成清楚最后一位的函数
                    String_Delete(&input_buf,IBlength,addr);
                    if( addr>0 )
                        addr--;
                }                 
                else if( keynum == 22 )
                {
                    memset(input_buf,'\0',IBlength);
                    memset(output_buf,'\0',16);                    
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
                else if( keynum == '=' )        //計算結果
                {
                    addr = String_length(input_buf);
                    input_buf[addr]='\0';
                    state = 2;
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
                        if( addr>=IBlength )
                        {
                            addr=IBlength;
                            input_buf[IBlength]='\0';
                        }                        
                    }                    
                } 
                if( state==1 )
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
        case 2://计算结果
            if( alg_calc_expression(input_buf, &re_result, &im_result)==1 )
            {
				if(im_result <0.0)
					sprintf(output_buf,"%g%gi",re_result,im_result);
				else
					sprintf(output_buf,"%g+%gi",re_result,im_result);
                
                Screen_ShowStr(0,6,output_buf);
                Screen_ShowFreshen();
            }
            else
            {
                Screen_ShowStr(0,6,"INPUT ERROR");
                Screen_ShowFreshen();
            }
            state = 4;
            break;
        case 3://显示历史数据
            break;
        
        case 4://等待下一次输入
            if( keynum>0 )
            {
                state= 1;  
                memset(input_buf,'\0',IBlength);
                memset(output_buf,'\0',16);
                Screen_Clear(); 
                addr = 0;                
                goto a;
            }  
            break;
        case 5://特殊功能輸入
            if( keynum>0 )
            {
                if(( keynum=='1' )||( keynum=='2' )||( keynum=='3' ))
                {
                    addr +=String_Insert(&input_buf,IBlength,addr,120+keynum-'1');  
                    state=1;
                    if( addr>=IBlength )
                    {
                        addr=IBlength;
                        input_buf[IBlength]='\0';
                    }                    
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                    Screen_ShowFreshen();
                }     
            }                
            break;
        
    }
}
