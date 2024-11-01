#include <STC32G.H>

#define ON 	0		//共阳极LED，低电平点亮
#define OFF 1		//共阳极LED，高电平熄灭

#define MAIN_Fosc 12000000UL		//定义主频12MHz

typedef unsigned char u8;				//unsigned char			类型用u8 表示
typedef unsigned int u16;				//unsigned int			类型用u16表示
typedef unsigned long int u32;	//unsigned long int	类型用u32表示

sbit LED1 = P2^0;				//LED1使用P2.0

void delay1(u16 ms);		//delay1 2层for循环实现
void delay2(u16 ms);		//delay2 do...while循环实现

void main()
{
	WTST = 0;										//设置读取指令等待时间为0，CPU运行程序速度最快
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式
	while(1)
	{
		LED1 = ON;								//点亮LED1
		delay1(1000);							//延时1000毫秒（低电平延时，由delay1延时）
		LED1 = OFF;								//熄灭LED1
		delay2(1000);							//延时1000毫秒（高电平延时，由delay2延时）
	}
}

void delay1(u16 ms)						//简单延时函数，自适应主频，1毫秒*ms
{
	volatile u16 i;
	volatile u32 j;							//volatile关键字防止变量i,j被编译器优化
	for(i = 0; i < ms;i++)
		for(j = 0;j < MAIN_Fosc / 9000;j++);
}

void  delay2(u16 ms)					//简单延时函数，自适应主频，1毫秒*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}