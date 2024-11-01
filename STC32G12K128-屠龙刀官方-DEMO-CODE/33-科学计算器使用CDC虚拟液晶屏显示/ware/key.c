#include "key.h"
#include "vk.h"
#include "middle.h"
#include "spi.h"

unsigned char CDC_KeyRead(unsigned char dat)
{
    unsigned char xdata keyval;
    switch( dat )
    {
        case VK_DIGIT_0 :
        case VK_DIGIT_1 :
        case VK_DIGIT_2 :
        case VK_DIGIT_3 :
        case VK_DIGIT_4 :
        case VK_DIGIT_5 :
        case VK_DIGIT_6 :
        case VK_DIGIT_7 :
        case VK_DIGIT_8 :
        case VK_DIGIT_9 :   keyval = dat; break;
        case VK_ADD     :   keyval = '+'; break;    
        case VK_SUBTRACT:   keyval = '-'; break;
        case VK_MULTIPLY:   keyval = '*'; break;
        case VK_DIVIDE  :   keyval = '/'; break; 
        case VK_DECIMAL :   keyval = '.'; break;
        case VK_EQUALS  :   keyval = '='; break; 
        case VK_ALPHA_A :    
        case VK_ALPHA_B :
        case VK_ALPHA_C :
        case VK_ALPHA_D :
        case VK_ALPHA_E :
        case VK_ALPHA_F :
        case VK_ALPHA_G :
        case VK_ALPHA_H :
        case VK_ALPHA_I :
        case VK_ALPHA_J :
        case VK_ALPHA_K :
        case VK_ALPHA_L :
        case VK_ALPHA_M :
        case VK_ALPHA_N :
        case VK_ALPHA_O :
        case VK_ALPHA_P :
        case VK_ALPHA_Q :
        case VK_ALPHA_R :
        case VK_ALPHA_S :
        case VK_ALPHA_T : 
        case VK_ALPHA_U : 
        case VK_ALPHA_V : 
        case VK_ALPHA_W : 
        case VK_ALPHA_X : keyval = dat+1-'A'; break;
        default:  keyval = 0; break;      
    }
    return keyval;
}

const u8 code  keymap[8][5] = 
{ 
    { 1 , 2 , 3 , 4 ,5} , 
    { 6 , 7 , 8 , 9 ,10 }, 
    {11 ,12 , 13,14 ,15 }, 
    {16 ,17 , 18,19 ,20 },
    {'7','8','9',21 ,22 },
    {'4','5','6','*','/'},
    {'1','2','3','+','-'},
    {'0','.',23 ,24 ,'='},    
};

u8 keyscan(void)
{ 
	static u16 xdata keybefore = 0;
    static u16 xdata key = 0;
    static u16 xdata keycode = 0;
    
	u8 xdata keyvalue = 0;
	u8 xdata keyrow = 0;
    u8 xdata keyline = 0; 
    u8 xdata i = 0;
    u8 xdata     j = 0;
//	
    P0 = 0XFF;      //读取按下的行
    P3 &= 0x07;
    delay();
	keyrow = P0 ^ 0xff ;
    
    P0 = 0X00;      //读取按下的行
    P3 |= 0xF8;
    delay();
    keyline = (P3&(0xf8)); 
    keyline ^= 0xF8;
    
//    Screen_ShowNum(100,0,keyrow);
//    Screen_ShowNum(100,2,keyline);
//    Screen_ShowFreshen();
	//以下为三行代码
	keycode = (((u16)keyline<<8) | keyrow);
	key = keycode & (keycode ^ keybefore);
	keybefore = keycode;
	
	if(key)
	{ 
		for(i = 0; i < 8; i++)
        {
			for(j = 0; j < 5; j++)
            {
				if(key == (0x8000 >> j | 0x01 << i))
				{ 
					keyvalue = keymap[i][j];
					break;
				}
            }
         }
	}
	
	return keyvalue;
}

