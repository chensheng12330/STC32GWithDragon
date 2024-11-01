#include "oled.h"
#include "font.h"
#include "stc.h"
#include "usb.h"
#include "spi.h"
#include "tim.h"
#include "middle.h"





unsigned char xdata ShowBUFF[8][128];

void OLED_Init(void)     //OLED初始化
{
    delay_ms(1000);
    #if Drive_Func==0
        OLED_DisplayOn();             //打开OLED12864的显示
    #else
        OLED_Init_SPI();
        OLED_DisplayOn();
//        OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
//        OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
//        OLED_WR_Byte(0xA1,OLED_CMD);    
    #endif
}

void OLED_DisplayOn(void)     //开显示
{
    #if Drive_Func==0
        OLED12864_DisplayOn();             //打开OLED12864的显示
        OLED12864_DisplayContent();        //显示屏幕内容
    #else
        OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
        OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
        OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕    
    #endif        
}
  
void OLED_DisplayOff(void)     //关显示
{
    #if Drive_Func==0    
        OLED12864_DisplayOff();            //关闭OLED12864的显示
    #else
        OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
        OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
        OLED_WR_Byte(0xAF,OLED_CMD);//关闭屏幕    
    #endif       
}

void OLED_LightSet(unsigned char num)     //亮度设置
{
    #if Drive_Func==0    
        OLED12864_SetContrast(num);       //亮度值（范围0～255）
    #else
        OLED_WR_Byte(0x81,OLED_CMD);//
        OLED_WR_Byte(num,OLED_CMD);//  
        OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
        OLED_WR_Byte(0x20,OLED_CMD);//Set VCOM Deselect Level
        
    #endif       
}


/****************************************直接驱动****************************************/
void OLED_ShowChar(unsigned char x,unsigned char y,char asc)     //OLED显示一个8*16字符
{
    #if Drive_Func==0
        OLED12864_ShowPicture(x, y, 8, 2, (BYTE *)&Ascll_16[(asc-' ')*2]);
    #else
        
    #endif    
}

void OLED_ShowGBK(unsigned char x,unsigned char y,char *gbk)     //OLED显示一个16*16字符
{
    unsigned char i;
    for( i=0;i<100;i++ )
    {
        if(( gbk[0]== GBK16[i].gbn_bum[0] )&&( gbk[1]== GBK16[i].gbn_bum[1] ))
        {
            #if Drive_Func==0
                OLED12864_ShowPicture(x,y, 16, 2, (BYTE *)&GBK16[i].gbk_font[0]);  
            #else
            
            #endif              
        }
    }  
}
void OLED_ShowString(unsigned char x,unsigned char y,char *s)
{
    char s_num[2];
	while(*s != '\0')       //字符串不为空，循环
	{
        if ((unsigned char)*s < 0x80)     //根据输入数据的大小判断是字符还是汉字，
        {
            OLED_ShowChar(x,y,*s);
            x+=8;
            s++;
        }
        else
        {
            s_num[0] = *s ;
            s_num[1] = *(s+1) ;
            OLED_ShowGBK(x,y,s_num);
            x+=16;
            s+=2;
        }
		if(x>128)
        {
            x=0;
            y+=16;
        }
	}  
}

/****************************************缓存驱动****************************************/
void OLED_BuffClear(void)     //清屏(CDC未实现)
{
    u8 xdata i,n;
    memset(ShowBUFF,0,128*8);
    #if Drive_Func==0    
        OLED12864_ShowPicture(0,0, 128,8, (BYTE *)&ShowBUFF);
    #else
        OLED_DrawBMP(0,0, 128,8, (BYTE *)&ShowBUFF);
//        for(i=0;i<8;i++)
//        {
//           OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
//           OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
//           OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
////            
////SPI_CS = 0;
////             OLED_DC = 0;
////        HAL_SPI_Transmit(ShowBUFF[i],128, 0xff);
////       SPI_CS = 1;     
////        delay();
//                     
//           for(n=0;n<128;n++)
//             OLED_WR_Byte(ShowBUFF[i][n],OLED_DATA);
//        }        
    #endif        
}
void OLED_BuffShow(void)     
{
    u8 xdata i,n;
    #if Drive_Func==0    
        OLED12864_ShowPicture(0,0, 128,8, (BYTE *)&ShowBUFF);
    #else
        for(i=0;i<8;i++)
        {
           OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
           OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
           OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
//            
//SPI_CS = 0;
//             OLED_DC = 0;
//        HAL_SPI_Transmit(ShowBUFF[i],128, 0xff);
//       SPI_CS = 1;     
//        delay();
                     
           for(n=0;n<128;n++)
             OLED_WR_Byte(ShowBUFF[i][n],OLED_DATA);
        }
    #endif        
}

void OLED_BuffShowPoint(unsigned char x,unsigned char y)     //OLED显示一个dian
{
    ShowBUFF[y/8][x] |= 1<<(y%8);
}

void OLED_BuffShowLine( unsigned char x1, unsigned char y1, unsigned char x2,unsigned char y2)
{
    unsigned char x,y;
    if( x1>x2 )
    {
        x=x1;x1=x2;x2=x;
        y=y1;y1=y2;y2=y;
    }
    if(x1!=x2)
    {
        for( x = x1; x <= x2; x++ )
        {
            if( y2>y1 )
                OLED_BuffShowPoint(x, (unsigned char)(y1+(u16)(y2-y1)*(u16)x/(u16)(x2-x1)));
            else
                OLED_BuffShowPoint(x, (unsigned char)(y1-(u16)(y1-y2)*(u16)x/(u16)(x2-x1)));
        }        
    }
    else
    {
        if( y1>y2 )
        {
            for( y = y2; y <= y1; y++ )
               OLED_BuffShowPoint(x1, y); 
        }
        else
        {
            for( y = y1; y <= y2; y++ )
               OLED_BuffShowPoint(x1, y);             
        }
    }
        

}



void OLED_BuffShowChar(unsigned char x,unsigned char y,char asc)     //OLED显示一个8*16字符
{
 
    //OLED12864_ShowPicture(x, y, 8, 2, (BYTE *)&Ascll_16[(asc-' ')*2]);
    unsigned char j,k;
    for( j=0;j<2;j++ )
    {
        for( k=0;k<8;k++ )
        {
            ShowBUFF[j+y][x+k]=Ascll_16[(asc-' ')*2][j*8+k];
        }
    }    
}

void OLED_BuffShowCharOpp(unsigned char x,unsigned char y,char asc)     //OLED显示一个8*16字符
{
 
    //OLED12864_ShowPicture(x, y, 8, 2, (BYTE *)&Ascll_16[(asc-' ')*2]);
    unsigned char j,k;
    for( j=0;j<2;j++ )
    {
        for( k=0;k<8;k++ )
        {
            ShowBUFF[j+y][x+k]=~Ascll_16[(asc-' ')*2][j*8+k];
        }
    }    
}



void OLED_BuffShowGBK(unsigned char x,unsigned char y,char *gbk)     //OLED显示一个16*16字符
{
    unsigned char i;
    unsigned char j,k;
    for( i=0;i<150;i++ )
    {
        if(( gbk[0]== GBK16[i].gbn_bum[0] )&&( gbk[1]== GBK16[i].gbn_bum[1] ))
        {
            for( j=0;j<2;j++ )
            {
                for( k=0;k<16;k++ )
                {
                    ShowBUFF[j+y][x+k]=GBK16[i].gbk_font[j*16+k];
                }
            }
            break;
        }
    }    
}
void OLED_BuffShowString(unsigned char x,unsigned char y,char *s)
{
    char s_num[2];
	while(*s != '\0')       //字符串不为空，循环
	{
        if ((unsigned char)*s < 0x80)     //根据输入数据的大小判断是字符还是汉字，
        {
            OLED_BuffShowChar(x,y,*s);
            x+=8;
            s++;
        }
        else
        {
            s_num[0] = *s ;
            s_num[1] = *(s+1) ;
            OLED_BuffShowGBK(x,y,s_num);
            x+=16;
            s+=2;
        }
		if(x>127)
        {
            x=0;
            y+=2;
        }
	}       
}

void OLED_BuffShowAscll32(unsigned char  x,unsigned char  y,u8 num)     //OLED显示一个8*16字符
{
    //OLED12864_ShowPicture(x, y, 8, 2, (BYTE *)&Ascll_16[(asc-' ')*2]);
    unsigned char j,k;

    for( j=0;j<4;j++ )
    {
        for( k=0;k<16;k++ )
        {
            ShowBUFF[j+y][x+k]=lib_num1632[num][j*16+k];
        }
    }            

}
void Cursor_Blink( u8 x,u8 y,u8 count )
{
    static  u32 xdata count_now=0;       //上一次时间
    static bit BlinkFlag=0;
    BYTE CurBuff[2] = {0xff,0xff};
    if(x>127)
    {
        x-=128;
        y+=2;
    }
    if( Sys_Time_10ms >= ( count_now+count ) )
    {
        count_now = Sys_Time_10ms;
        if( BlinkFlag==0 )
        {
            CurBuff[0] = CurBuff[1] =0xff;
            #if Drive_Func==0   
                OLED12864_ShowPicture(x,y, 1, 2, (BYTE *)&CurBuff); 
            #else    
                OLED_DrawBMP(x,y, 1, 2, (BYTE *)&CurBuff); 

            #endif
        } 
        else
        {
            //OLED12864_ShowPicture(x,y, 1, 2, (BYTE *)&ShowBUFF[x][y]); 
            CurBuff[0] = CurBuff[1] =0x00;
            #if Drive_Func==0      
                OLED12864_ShowPicture(x,y, 1, 2, (BYTE *)&CurBuff);   
            #else    
                OLED_DrawBMP(x,y, 1, 2, (BYTE *)&CurBuff);           
            #endif            
        }     
        BlinkFlag =!BlinkFlag;
        
//    Screen_ShowNum(0,4,BlinkFlag);
//    Screen_ShowFreshen(); 
        
    }
}

