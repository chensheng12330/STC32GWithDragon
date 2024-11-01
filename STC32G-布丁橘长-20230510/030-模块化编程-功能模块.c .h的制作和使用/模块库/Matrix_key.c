#include <Matrix_key.h>
#include <delay.h>

u8 Matrix_key()
{
	u8 temp,keyvalue;		//temp：P2值缓存，keyvalue:按键号
	static u8 keystate;				//按键状态
	KEY_PORT = 0xF0;					//列扫描
	if(KEY_PORT != 0xF0 && keystate == 0)			//判断是否有按键按下，如果按键已经是按下状态则不进入if
	{
		delayms(10);				//按键去抖
		if(KEY_PORT != 0xF0 && keystate == 0)		//再次判断是否有按键按下，如果按键已经是按下状态则不进入if
		{
			keystate = 1;		//按键状态为1，按键按下
			temp = KEY_PORT;			//读取P2值
			switch(temp)
			{
				case 0xE0:keyvalue = 1;break;			//第1列有按键按下，键值等于1
				case 0xD0:keyvalue = 2;break;			//第2列有按键按下，键值等于2
				case 0xB0:keyvalue = 3;break;			//第3列有按键按下，键值等于3
				case 0x70:keyvalue = 4;break;			//第4列有按键按下，键值等予4
			}
			KEY_PORT = 0x0F;				//翻转电平，行扫描
			if(KEY_PORT != 0x0F)		//判断是否有按键按下
			{
				temp = KEY_PORT;			//读取P2值
				switch(temp)
				{
					case 0x07:keyvalue += 0;break;		//第1行按下，键值keyvalue+0
					case 0x0B:keyvalue += 4;break;		//第2行按下，键值keyvalue+4
					case 0x0D:keyvalue += 8;break;		//第3行按下，键值keyvalue+8
					case 0x0E:keyvalue += 12;break;		//第4行按下，键值keyvalue+12
				}
			}
			
		}
	}
	if(KEY_PORT == 0xF0)
	{
		keystate = 0;		//松开按键后，清零按键状态
		keyvalue = 0;		//松开按键后，清零按键号
	}
	return keyvalue;								//返回按键号
}