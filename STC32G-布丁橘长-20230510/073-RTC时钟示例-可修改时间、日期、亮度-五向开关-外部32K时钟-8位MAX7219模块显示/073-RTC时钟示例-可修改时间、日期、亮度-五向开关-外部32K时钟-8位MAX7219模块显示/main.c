//	@布丁橘长 2023/02/20
//	内置RTC+外部32.768KHz时钟示例程序
//  上电后，显示时钟，五向开关-上键可切换显示时钟/日期
//	通过板载五向开关，可以修改时间、日期、亮度，中键：设置/确定，上下键：增加、减小，左右键：切换位置
//  正常状态：显示时钟，按中间显示S1，左右键可选择S1、S2、S3，S1-时钟设置，S2-日期设置，S3-亮度设置，再次按中键进入对应设置
//	设置完成，按中键确定，返回时钟显示状态
//	初始时间设置为：23年02月20日，23：45:00
//	初始亮度：10（16级亮度调节：0-15）
//	8位MAX7219数码管模块显示，引脚定义：DIN = P6^6; CS = P6^5; CLK = P6^4;
//	五向开关引脚定义：上键：P7.4，下键：P7.3，左键：P7.2，右键：P7.1，中键：P7.0
//	实验开发板：STC32G12K128屠龙刀三.1 主频@12MHz

#include <STC32G.H>
#include "config.h"
#include "MAX7219.h"

#define NORMAL 0					//正常显示界面
#define SETSEL1	1					//SETSEL1:设置选择界面-S1-时钟设置
#define SETSEL2 2					//SETSEL2:设置选择界面-S2-日期设置
#define SETSEL3 3					//SETSEL3:设置选择界面-S3-亮度设置
#define SET1 4						//SET1:	时钟设置界面
#define SET2 5						//SET2：日期设置界面
#define SET3 6						//SET3：亮度设置界面
#define SHOW_CLOCK 0			//显示标志-显示时钟
#define SHOW_DATE 1				//显示标志-显示日期
#define DISP_BLACK 0x0F		//数码管BCODE码-熄灭
#define SET_HOUR 0				//小时设置标志
#define SET_MIN 1					//分钟设置标志
#define SET_SEC 2					//秒钟设置标志
#define SET_YEAR 10				//年设置标志
#define SET_MONTH 11			//月设置标志
#define SET_DAY 12				//日设置标志
#define SET_LIGHT 20			//亮度设置标志

sbit KEY_Up 		= P7^4;				//上键：P7.4
sbit KEY_Down 	= P7^3;				//下键：P7.3
sbit KEY_Left 	= P7^2;				//左键：P7.2
sbit KEY_Right 	= P7^1;				//右键：P7.1
sbit KEY_Middle = P7^0;				//中键：P7.0

void RTC_config(void);				//RTC配置
void DISP_Clock(void);				//显示时钟界面
void DISP_Date(void);					//显示日期界面
void DISP_Clock_SET(void);		//显示时钟设置界面
void DISP_Date_SET(void);			//显示日期设置界面
void DISP_LIGHT_SET(void);		//显示亮度设置界面
void DISP_SETSEL();						//显示设置选择界面
void Timer0_Init(void);				//定时器0初始化
void keyscan();								//按键扫描
void RTC_read();							//读取RTC数据

u8 clockdat;									//时钟数据位置
bit B_1ms;										//1毫秒计时标志
bit B_300ms;									//300毫秒计时标志
u8 year,month,day,hour,min,sec,ssec;																		//年月日时分秒等变量
u8 keyupflag,keydownflag,keyleftflag,keyrightflag,keymiddleflag;				//上下左右中键按下标志
u8 keyupdelay,keydowndelay,keyleftdelay,keyrightdelay,keymiddledelay;		//上下左右中键延时标志
u8 setstate;									//设置状态
u16 timer0;										//定时器conter
u8 blinkflag;									//闪烁标志
u8 showstate;									//显示状态
u8 light;											//亮度

void main(void)
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		//设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	year = 23;									//初始年：2023
	month = 2;									//初始月：02
	day = 21;										//初始日：20
	hour = 21;									//初始时：21
	min = 45;										//初始分：45
	sec = 0;										//初始秒：0
	light = 10;									//初始亮度：10（0-15共16级亮度调节）
  RTC_config();								//RTC配置
	MAX7219_Ini();							//MAX7219初始化
	Timer0_Init();							//定时器0初始化
	EA = 1;											//使能总中断

  while(1)
  {
		if(setstate == NORMAL && showstate == SHOW_CLOCK) 		DISP_Clock();		//设置状态为普通模式，并且显示状态为显示时钟，则显示时钟
		else if(setstate == NORMAL && showstate == SHOW_DATE)	DISP_Date();		//设置状态为普通模式，并且显示状态为显示日期，则显示日期
		else if(setstate == SET1) DISP_Clock_SET();														//设置状态为设置1模式，则显示设置时钟
		else if(setstate == SET2) DISP_Date_SET();														//设置状态为设置2模式，则显示设置日期
		else if(setstate == SET3) DISP_LIGHT_SET();														//设置状态为设置3模式，则显示设置亮度
		else if(setstate == SETSEL1 || setstate == SETSEL2 || setstate == SETSEL3) DISP_SETSEL();//设置状态为设置选择模式，则显示设置选择界面
		keyscan();								//按键扫描
  }
} 
void Timer0_Isr(void) interrupt 1						//定时器0提供1毫秒节拍，用于按键扫描等
{
	B_1ms = 1;									//1毫秒标志
	timer0++;										//毫秒计时++
	if(timer0 >= 300) 					//计时300毫秒
	{
		B_300ms = 1;							//300毫秒标志
		timer0 = 0;								//清零300毫秒计数
	}
}
void Timer0_Init(void)				//1毫秒@12.000MHz
{
	AUXR |= 0x80;								//定时器时钟1T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0x20;									//设置定时初始值
	TH0 = 0xD1;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计时
	ET0 = 1;										//使能定时器0中断
}
void DISP_SETSEL()						//设置选择界面
{
	if(setstate == SETSEL1)			//选择界面-S1
	{
		Write7219(8,5); 					//左边第1位：显示S
		Write7219(7,1); 					//左边第2位：显示1
	}
	if(setstate == SETSEL2)			//选择界面-S2
	{
		Write7219(8,5); 					//左边第1位：显示S
		Write7219(7,2); 					//左边第2位：显示2
	}		
	if(setstate == SETSEL3)			//选择界面-S3
	{
		Write7219(8,5); 					//左边第1位：显示S
		Write7219(7,3); 					//左边第2位：显示3
	}	
		Write7219(6,DISP_BLACK);	//数码管其余6位熄灭
		Write7219(5,DISP_BLACK);
		Write7219(4,DISP_BLACK);
		Write7219(3,DISP_BLACK);
		Write7219(2,DISP_BLACK);
		Write7219(1,DISP_BLACK);
}
void DISP_Clock(void)												//显示时钟函数
{			
	Write7219(8,(u8)(HOUR / 10)); 						//左起第1位显示时钟十位
	Write7219(7,(u8)(HOUR % 10)); 						//左起第2位显示时钟个位
	Write7219(6,10); 													//左起第3位显示时钟横杆
	Write7219(5,(u8)(MIN / 10)); 							//左起第4位显示分钟十位
	Write7219(4,(u8)(MIN % 10));							//左起第5位显示分钟个位							
	Write7219(3,10); 													//左起第6位显示分钟横杆
	Write7219(2,(u8)(SEC / 10)); 							//左起第7位显示秒钟十位
	Write7219(1,(u8)(SEC % 10)); 							//左起第8位显示时钟个位
}
void DISP_Date(void)												//显示日期函数
{							
	Write7219(8,2); 													//左起第1位显示年千位
	Write7219(7,0); 													//左起第2位显示年百位
	Write7219(6,(u8)(YEAR / 10)); 						//左起第3位显示年十位
	Write7219(5,(u8)(YEAR % 10)); 						//左起第4位显示年个位
	Write7219(4,(u8)(MONTH / 10)); 						//左起第5位显示月十位
	Write7219(3,(u8)(MONTH % 10)); 						//左起第6位显示月个位
	Write7219(2,(u8)(DAY / 10)); 							//左起第7位显示日十位
	Write7219(1,(u8)(DAY % 10)); 							//左起第8位显示月个位
}
void DISP_Clock_SET(void)										//显示设置时钟函数
{	
	if(clockdat != SET_HOUR || blinkflag % 2 == 0)
	{
		Write7219(8,(u8)(HOUR / 10)); 					//左起第1位显示时钟十位
		Write7219(7,(u8)(HOUR % 10)); 					//左起第2位显示时钟个位
	}
	Write7219(6,10); 													//左起第3位显示时钟横杆
	if(clockdat != SET_MIN || blinkflag % 2 == 0)
	{
		Write7219(5,(u8)(MIN / 10)); 						//左起第4位显示分钟十位
		Write7219(4,(u8)(MIN % 10)); 						//左起第5位显示分钟个位	
	}
	Write7219(3,10); 													//左起第6位显示分钟横杆
	if(clockdat != SET_SEC || blinkflag % 2 == 0)
	{
		Write7219(2,(u8)(SEC / 10)); 						//左起第7位显示秒钟十位
		Write7219(1,(u8)(SEC % 10)); 						//左起第8位显示时钟个位	
	}
	if(B_300ms == 1)
	{
		blinkflag++;
		B_300ms = 0;
		if(blinkflag > 99) blinkflag = 0;				//控制数码管各位闪烁时的熄灭状态
		if(blinkflag % 2 == 1)
		{
			if(clockdat == SET_MIN)
			{
				Write7219(5,DISP_BLACK);
				Write7219(4,DISP_BLACK);
			}
			if(clockdat == SET_SEC)
			{
				Write7219(2,DISP_BLACK);
				Write7219(1,DISP_BLACK);
			}
			if(clockdat == SET_HOUR)
			{
				Write7219(8,DISP_BLACK);
				Write7219(7,DISP_BLACK);
			}
		}
	}
}
void DISP_Date_SET()
{
	if(clockdat != SET_YEAR || blinkflag % 2 == 0)
	{
		Write7219(8,2); 													//左起第1位显示年千位
		Write7219(7,0); 													//左起第2位显示年百位
		Write7219(6,(u8)(YEAR / 10)); 						//左起第3位显示年十位
		Write7219(5,(u8)(YEAR % 10)); 						//左起第4位显示年个位
	}
	if(clockdat != SET_MONTH || blinkflag % 2 == 0)
	{
		Write7219(4,(u8)(MONTH / 10)); 						//左起第5位显示月十位
		Write7219(3,(u8)(MONTH % 10)); 						//左起第6位显示月个位
	}
	if(clockdat != SET_DAY || blinkflag % 2 == 0)
	{
		Write7219(2,(u8)(DAY / 10)); 							//左起第7位显示日十位
		Write7219(1,(u8)(DAY % 10)); 							//左起第8位显示月个位
	}
	if(B_300ms == 1)
	{
		blinkflag++;
		B_300ms = 0;
		if(blinkflag > 99) blinkflag = 0;					
		if(blinkflag % 2 == 1)
		{
			if(clockdat == SET_YEAR)								//控制数码管各位闪烁时的熄灭状态
			{
				Write7219(8,DISP_BLACK); 													
				Write7219(7,DISP_BLACK); 													
				Write7219(6,DISP_BLACK); 						
				Write7219(5,DISP_BLACK); 					
			}
			if(clockdat == SET_MONTH)
			{
				Write7219(4,DISP_BLACK); 													
				Write7219(3,DISP_BLACK); 	
			}			
			if(clockdat == SET_DAY)
			{
				Write7219(2,DISP_BLACK); 													
				Write7219(1,DISP_BLACK); 
			}
		}
	}
}
void DISP_LIGHT_SET()
{
	if(B_300ms == 1)												//当300毫秒标志为1
	{
		blinkflag++;													//每300毫秒，闪烁标志+1
		B_300ms = 0;													//清零300毫秒标志
		if(blinkflag > 99) blinkflag = 0;			//防溢出，清零
		if(blinkflag % 2 == 1)								//每300毫秒亮一次
		{
			Write7219(8,(u8)(light / 10)); 			//左边第1位，显示亮度十位									
			Write7219(7,(u8)(light % 10)); 			//左边第2位，显示亮度个位														
		}
		else if(blinkflag % 2 == 0)						//每300毫秒灭一次
		{
			Write7219(8,DISP_BLACK); 						//左边第1、2位熄灭						
			Write7219(7,DISP_BLACK); 												
		}
	}	
	Write7219(6,DISP_BLACK); 								//数码管其余6位：熄灭
	Write7219(5,DISP_BLACK); 						
	Write7219(4,DISP_BLACK); 													
	Write7219(3,DISP_BLACK); 			
	Write7219(2,DISP_BLACK); 													
	Write7219(1,DISP_BLACK); 
}
void RTC_config(void)							//RTC配置函数
{
    INIYEAR = year;    						//RTC初始化寄存器，装载年月日时分秒变量值						
    INIMONTH = month;   											
    INIDAY = day;     												
    INIHOUR = hour;    												
    INIMIN = min;     												
    INISEC = sec;     												
    INISSEC = 0;      											
																	//STC32G 芯片使用内部32K时钟，休眠无法唤醒
	//	IRC32KCR = 0x80;   					//启动内部32K晶振.
  //  while (!(IRC32KCR & 1));  	//等待时钟稳定
	//	RTCCFG = 0x03;    					//选择内部32K时钟源，并触发RTC寄存器初始化

		X32KCR = 0x80 + 0x40;   			//启动外部32K晶振, 低增益+0x00, 高增益+0x40.
    while (!(X32KCR & 1));  			//等待时钟稳定
		RTCCFG = 0x01;   							//选择外部32K时钟源，触发RTC寄存器初始化
	
    RTCCR = 0x01;     						//RTC使能
    while(RTCCFG & 0x01);					//等待初始化完成,需要在 "RTC使能" 之后判断. 
																	//设置RTC时间需要32768Hz的1个周期时间,大约30.5us. 由于同步, 所以实际等待时间是0~30.5us.
}
void RTC_read()										//读取RTC数据
{
	year = YEAR;										//读取值分别存入对应的变量
	month = MONTH;
	day = DAY;
	hour = HOUR;
	min = MIN;
	sec = SEC;
}
void keyscan()
{
//------------------------中键----------------------------------------------
	if(KEY_Middle == 0)
	{
		if(keymiddleflag == 0)				//中键按下计时
		{
			if(B_1ms == 1)							//1毫秒到
			{
				keymiddledelay++;					//中键按下延时+1
				B_1ms = 0;								//清零1毫秒标志
			}
			if(keymiddledelay == 10)		//10毫秒到（10毫秒消抖）
			{
				keymiddleflag = 1;				//中键按下标志置1
				if(setstate == NORMAL) 						setstate = SETSEL1;														//如果是正常显示状态按下中键，状态改为：设置选择1-S1
				else if(setstate == SETSEL1) 	{setstate = SET1;clockdat = SET_HOUR;}						//如果状态是设置选择1，状态改为：设置1-时钟设置，光标设置在HOUR
				else if(setstate == SETSEL2)	{setstate = SET2;clockdat = SET_YEAR;}						//如果状态是设置选择2，状态改为：设置2-日期设置，光标设置在YEAR
				else if(setstate == SETSEL3)  {setstate = SET3;clockdat = SET_LIGHT;}						//如果状态是设置选择3，状态改为：设置3-亮度设置，光标设置在LIGHT
				else if(setstate == SET1)			{setstate = NORMAL;clockdat = SET_HOUR;}					//如果状态是设置1-时钟设置，状态改为：普通，光标设置在HOUR
				else if(setstate == SET2)			{setstate = NORMAL;clockdat = SET_HOUR;}					//如果状态是设置2-日期设置，状态改为：普通，光标设置在HOUR
				else if(setstate == SET3)			{setstate = NORMAL;Write7219(INTENSITY,light);}		//如果状态是设置3-亮度设置，状态改为：普通
			}
		}
	}
	if(KEY_Middle == 1) 						//松开中键后
	{
		keymiddledelay = 0;						//清零中键计时
		keymiddleflag = 0;						//清零中键按下标志
	}
//------------------------上键----------------------------------------------
	if(KEY_Up == 0)
	{
		if(keyupflag == 0)						//中键按下计时
		{
			if(B_1ms == 1)							//1毫秒到
			{
				keyupdelay++;							//上键计时+1
				B_1ms = 0;								//清零1毫秒标志
			}
			if(keyupdelay == 10)				//10毫秒到
			{
				keyupflag = 1;						//上键按下标志置1
				if(setstate == NORMAL && showstate == SHOW_CLOCK) showstate = SHOW_DATE;				//如果状态是普通，并且显示状态是时间，状态改为：显示日期
				else if(setstate == NORMAL && showstate == SHOW_DATE) showstate = SHOW_CLOCK;		//如果状态是普通，并且显示状态是日期，状态改为：显示时间
				if(setstate == SET1 || setstate == SET2 || setstate == SET3)										//如果状态是设置1、设置2、设置3
				{
					RTC_read();							//读取RTC数据
					switch(clockdat)				//判断光标位置
					{
						case 0:	if(HOUR < 23) hour++;RTC_config();break;										//光标在HOUR，小时+1，更新RTC寄存器并初始化
						case 1:	if(MIN < 59) 	min++;RTC_config();break;											//光标在MIN，分钟+1，更新RTC寄存器并初始化
						case 2:	if(SEC < 59) 	sec++;RTC_config();break;											//光标在SEC，秒钟+1，更新RTC寄存器并初始化
						case 10:if(YEAR < 99)	year++;RTC_config();break;										//光标在YEAR，年+1，更新RTC寄存器并初始化
						case 11:if(MONTH < 12) month++;RTC_config();break;									//光标在MONTH，月+1，更新RTC寄存器并初始化
						case 12:if(DAY < 31)	day++;RTC_config();break;											//光标在DAY，日+1，更新RTC寄存器并初始化
						case 20:if(light < 15) light++; Write7219(INTENSITY,light); break;	//光标在light，亮度+1，更新RTC寄存器并初始化
						default:break;
					}
				}
			}
		}
	}
	if(KEY_Up == 1) 								//上键松开
	{
		keyupdelay = 0;								//清零上键计时
		keyupflag = 0;								//清零上键按下标志
	}
	//------------------------下键----------------------------------------------
	if(KEY_Down == 0)								//下键按下
	{	
		if(keydownflag == 0)					//下键按下计时
		{
			if(B_1ms == 1)							//1毫秒到
			{
				keydowndelay++;						//下键计时+1
				B_1ms = 0;								//清零1毫秒标志
			}
			if(keydowndelay == 10)			//10毫秒到
			{
				keydownflag = 1;					//下键按下标志置1
				if(setstate == SET1 || setstate == SET2 || setstate == SET3)					//如果是设置1、设置2、设置3状态
				{
					RTC_read();							//读取RTC数据
					switch(clockdat)				//判断光标位置
					{
						case 0:	if(HOUR > 0) hour--;RTC_config();break;										//光标在HOUR，小时-1，更新并初始化RTC
						case 1:	if(MIN > 0) 	min--;RTC_config();break;										//光标在MIN，分钟-1，更新并初始化RTC
						case 2:	if(SEC > 0) 	sec--;RTC_config();break;										//光标在SEC，秒钟-1，更新并初始化RTC
						case 10:if(YEAR > 0)	year--;RTC_config();break;									//光标在YEAR，年-1，更新并初始化RTC
						case 11:if(MONTH > 1) month--;RTC_config();break;									//光标在MONTH，月-1，更新并初始化RTC
						case 12:if(DAY > 1)	day--;RTC_config();break;											//光标在DAY，日-1，更新并初始化RTC
						case 20:if(light > 0) light--;Write7219(INTENSITY,light);break;		//光标在light，亮度-1，更新并初始化RTC
						default:break;
					}
				}
			}
		}
	}
	if(KEY_Down == 1) 							//松开下键
	{
		keydowndelay = 0;							//清零下键计时
		keydownflag = 0;							//清零下键按下标志
	}
//------------------------左键----------------------------------------------
	if(KEY_Left == 0)								//左键按下
	{
		if(keyleftflag == 0)					//左键计时
		{
			if(B_1ms == 1)							//1毫秒到
			{
				keyleftdelay++;						//左键按下计时+1
				B_1ms = 0;								//清零1毫秒标志
			}
			if(keyleftdelay == 10)			//10毫秒到
			{
				keyleftflag = 1;					//左键按下标志置 
				if(setstate == SET1)			//如果状态是设置1-时间设置
				{
					if(clockdat > SET_HOUR)clockdat--;								//如果光标大于HOUR位置，光标位置-1（左移）
				}
				else if(setstate == SET2)														//如果状态是SET2-日期设置
				{
					if(clockdat > SET_YEAR) clockdat--;								//如果光标大于YEAR位置，光标位置-1（左移）
				}
				else if(setstate == SETSEL1) setstate = SETSEL3;		//如果是设置选择界面-S1，切换到S3
				else if(setstate == SETSEL2) setstate = SETSEL1;		//如果是设置选择界面-S2，切换到S1
				else if(setstate == SETSEL3) setstate = SETSEL2;		//如果是设置选择界面-S3，切换到S2
			}
		}
	}
	if(KEY_Left == 1) 							//松开左键
	{
		keyleftdelay = 0;							//清零左键计时
		keyleftflag = 0;							//清零左键按下标志
	}
	//------------------------右键----------------------------------------------
	if(KEY_Right == 0)							//右键按下
	{
		if(keyrightflag == 0)					//右键按下计时
		{
			if(B_1ms == 1)							//1毫秒到
			{
				keyrightdelay++;					//右键按下计时+1
				B_1ms = 0;								//1毫秒到
			}
			if(keyrightdelay == 10)			//10毫秒到
			{
				keyrightflag = 1;					//右键按下标志置1
				if(setstate == SET1 )			//如果是设置1-设置时间状态
				{
					if(clockdat < SET_SEC) clockdat++;		//如果光标位置小于SEC，光标右移
				}
				else if(setstate == SET2)								//如果是设置2-设置日期状态
				{
					if(clockdat < SET_DAY) clockdat++;		//如果光标位置小于DAY，光标右移		
				}
				else if(setstate == SETSEL1) setstate = SETSEL2;		//如果是设置选择-S1，切换到S2
				else if(setstate == SETSEL2) setstate = SETSEL3;		//如果是设置选择-S2，切换到S3
				else if(setstate == SETSEL3) setstate = SETSEL1;		//如果是设置选择-S3，切换到S1
			}
		}
	}
	if(KEY_Right == 1) 							//右键松开
	{
		keyrightdelay = 0;						//清零右键计时
		keyrightflag = 0;							//清零右键按下标志
	}
}