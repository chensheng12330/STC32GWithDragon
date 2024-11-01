/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于STC32G核心转接板（屠龙刀）进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

采集单总线传感器DHT11的温湿度数据

串口(115200,N,8,1)打印采集结果

下载时, 默认时钟 12MHz (用户可自行修改频率).

******************************************/

#include "STC32G.h"
#include "intrins.h"
#include "stdio.h"

#define MAIN_Fosc        12000000UL

/*****************************************************************************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf 使用 UART1; 2:printf 使用 UART2

sbit DHT=P1^6;//DHT11模块

unsigned int Temperature[2];
unsigned int Humidity[2];

unsigned char Data_Receive[5];

void delay_ms(unsigned int ms);
void DHT_Collect();

/******************** 串口打印函数 ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //定时器时钟1T模式
	S1BRT = 0;          //串口1选择定时器1为波特率发生器
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;				//定时器1开始计时

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //串口1选择定时器2为波特率发生器
#else
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //定时器2时钟1T模式,开始计时
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
	TI = 0;
#else
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/*****************************************************************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

	UartInit();

	printf("STC32G DHT11 Test.\r\n");

	while(1)
    {
        DHT_Collect();  //DHT11数据采集
        printf("Temperature = %u.%u°C\r\n",Temperature[0],Temperature[1]);
        printf("Humidity = %u.%u%%RH\r\n",Humidity[0],Humidity[1]);
        delay_ms(1000); //delay 1s
    }
}
/*****************************************************************************/

void Delay30us()//@12.000MHz
{
	unsigned long i;

	_nop_();
	_nop_();
	_nop_();
	i = 88UL;
	while (i) i--;
}

void Delay20ms()//@12.000MHz
{
	unsigned long i;

	_nop_();
	_nop_();
	_nop_();
	i = 59998UL;
	while (i) i--;
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);   //6T per loop
    }while(--ms);
}

void DHT_Collect()//温度采集函数
{
    unsigned char i,j;//for循环变量
    unsigned int t;//超时判断
    unsigned char dat8=0;//一次读取的8位数据，需要读5次
    unsigned int CheckValue=0;//校验和
    
    DHT=0;//主机发起始信号
    Delay20ms();//主机拉低总线至少18ms
    DHT=1;//主机拉高总线20~40us
    Delay30us();
    
    t=160;//设置超时等待时间
    while(DHT&&t--);//等待DHT11拉低总线
    if(t==0)//超时
    {
        DHT=1;
    }
    t=600;//设置超时等待时间
    while(!DHT&&t--);//等待DHT11拉高总线
    if(t==0)//超时
    {
        DHT=1;
    }
    t=600;//设置超时等待时间
    while(DHT&&t--);//等待DHT11拉低总线
    if(t==0)//超时
    {
        DHT=1;
    }

    for(j=0;j<5;j++)//5次读取
    {
        for(i=0;i<8;i++)//1次8个位
        {
            //等待50us开始时隙
            t=400;//设置超时等待时间
            while(!DHT&&t--);//等待DHT11拉高总线
            if(t==0)//超时
            {
                DHT=1;
            }
            Delay30us();
            Delay30us();
            dat8<<=1;
            if(DHT) //高电平持续时间较长(70us)
            dat8+=1;//传输值为1
            
            t=300;//设置超时等待时间
            while(DHT&&t--);//等待并记录高电平持续时间
        }
        Data_Receive[j]=dat8;
    }
    
    Delay30us();//等待DHT11拉低50us
    Delay30us();
    DHT=1;

    for(i=0;i<4;i++)
    {
        CheckValue+=Data_Receive[i];//计算校验值
    }
    if((unsigned char)CheckValue==Data_Receive[4])//比对
    {
        Humidity[0]=Data_Receive[0];    //湿度整数
        Humidity[1]=Data_Receive[1];    //湿度小数
        Temperature[0]=Data_Receive[2]; //温度整数
        Temperature[1]=Data_Receive[3]%10;  //温度小数，最高位为正负标志
    }
}
