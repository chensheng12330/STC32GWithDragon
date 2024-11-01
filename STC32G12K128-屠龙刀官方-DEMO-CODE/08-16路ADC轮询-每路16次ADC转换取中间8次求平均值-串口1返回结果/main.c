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

/*************	功能说明	**************

本例程基于STC32G核心转接板（屠龙刀）进行编写测试。

演示15路ADC和bandgap查询采样，通过J2接口串口1(P1.6 P1.7)发送给上位机，波特率115200,N,8,1.

0~7通道对应P1.0~P1.7, 8~14通道对应P0.0~P0.6, 15通道为内部1.19V基准电压做输入的ADC值.

初始化时先把要ADC转换的引脚设置为高阻输入.

程序提供了两种软件防抖的方式供参考选用：
1. 使用冒泡排序，去掉最高值、最低值，求中间平均值（不需要的话屏蔽 BUBBLE_SORT 定义）
2. 采样累加，求平均值（不需要的话可将 SUM_LENGTH 定义值改为 1 ）

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_hid_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件
#include "intrins.h"
#include "stdio.h"

/*************	参数定义声明	**************/

//#define MAIN_Fosc     22118400L  //定义主时钟
#define MAIN_Fosc     24000000L  //定义主时钟
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))

#define Timer0_Reload (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define BUBBLE_SORT     //设置使用冒泡排序，去掉最高值、最低值，求中间平均值（不需要的话屏蔽此行）

#define SUM_LENGTH	16	/* 平均值采样次数 最大值16（不需要的话可将定义值改为 1 ）*/

/*************	本地常量声明	**************/

#define	ADC_SPEED	15		/* 0~15, ADC转换时间(CPU时钟数) = (n+1)*32  ADCCFG */
#define	RES_FMT		(1<<5)	/* ADC结果格式 0: 左对齐, ADC_RES: D11 D10 D9 D8 D7 D6 D5 D4, ADC_RESL: D3 D2 D1 D0 0 0 0 0 */
							/* ADCCFG      1: 右对齐, ADC_RES: 0 0 0 0 D11 D10 D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */

/*************	本地变量声明	**************/

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;

#ifdef BUBBLE_SORT  //使用冒泡排序，去掉最高值、最低值，求中间平均值
u16 ADC_Buffer[16];
#endif

/*************	本地函数声明	**************/

void Timer0_Init(void);
void delay_ms(u8 ms);
void ADC_convert(u8 chn);	//chn=0~7对应P1.0~P1.7, chn=8~14对应P0.0~P0.6, chn=15对应BandGap电压
u16	Get_ADC12bitResult(u8 channel);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
	P_SW1 = (P_SW1 & 0x3f) | 0x80;    //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //定时器时钟1T模式
	S1BRT = 0;          //串口1选择定时器1为波特率发生器
	TL1 = TM;
	TH1 = TM>>8;
	TR1 = 1;			//定时器1开始计时
}

void UartPutc(unsigned char dat)
{
	SBUF = dat; 
	while(TI==0);
	TI = 0;
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/**********************************************/
void main(void)
{
	u8	i;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    RSTFLAG |= 0x04;   //设置硬件复位后需要检测P3.2的状态选择运行区域，否则硬件复位后进入USB下载模式

	P0M1 = 0x7f;   P0M0 = 0x00;   //设置为高阻输入
	P1M1 = 0x3b;   P1M0 = 0x00;   //设置为高阻输入,P1.6 P1.7做串口通信
	P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
	P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
	P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
	P5M1 = 0x10;   P5M0 = 0x00;   //设置P5.4为高阻输入
	P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
	P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
	
    usb_init();
    Timer0_Init();

	ADCTIM = 0x3f;  //设置通道选择时间、保持时间、采样时间
	ADCCFG = RES_FMT + ADC_SPEED;
	//ADC模块电源打开后，需等待1ms，MCU内部ADC电源稳定后再进行AD转换
	ADC_CONTR = 0x80 + 0;	//ADC on + channel

	UartInit();
    EUSB = 1;   //IE2相关的中断位操作使能后，需要重新设置EUSB
	EA = 1;
	printf("STC32G系列ADC测试程序!\r\n");

	while (1)
	{
		for(i=0; i<16; i++)
		{
			delay_ms(200);
			//ADC_convert(3);		//发送固定通道AD值

			ADC_convert(i);		//发送轮询通道AD值
			if((i & 7) == 7)	//分两行打印
			{
				printf("\r\n");
			}
		}
	}
}

#ifdef BUBBLE_SORT  //使用冒泡排序
//========================================================================
// 函数: void DataSwap(u16* data1, u16* data2)
// 描述: 数据交换函数。
// 参数: data1,data2 要交换的数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-9-27
// 备注: 
//========================================================================
void DataSwap(u16* data1, u16* data2)
{
	u16 temp;
	temp = *data1;
	*data1 = *data2;
	*data2 = temp;
}

//========================================================================
// 函数: void BubbleSort(u16* pDataArry, u8 DataNum)
// 描述: 冒泡排序函数。
// 参数: pDataArry需要排序的数组，DataNum需要排序的数据个数.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-9-27
// 备注: 
//========================================================================
void BubbleSort(u16* pDataArry, u8 DataNum)
{
	bit flag;
	u8 i,j;
	for(i=0;i<DataNum-1;i++)
	{
		flag = 0;
		for(j=0;j<DataNum-i-1;j++)
		{
			if(pDataArry[j] > pDataArry[j+1])
			{
				flag = 1;
				DataSwap(&pDataArry[j], &pDataArry[j+1]);
			}
		}
		if(!flag)  //上一轮比较中不存在数据交换，则退出排序
		{
			break;
		}
	}
}
#endif

//========================================================================
// 函数: u16 Get_ADC12bitResult(u8 channel))	//channel = 0~15
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC, 0~15.
// 返回: 12位ADC结果.
// 版本: V1.0, 2016-4-28
//========================================================================
u16	Get_ADC12bitResult(u8 channel)	//channel = 0~15
{
	ADC_RES = 0;
	ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //设置ADC转换通道
    ADC_START = 1;//启动ADC转换
    _nop_();
    _nop_();
    _nop_();
    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //清除ADC结束标志
    return (((u16)ADC_RES << 8) | ADC_RESL);
}

/***********************************
查询方式做一次ADC, chn为通道号, chn=0~7对应P1.0~P1.7, chn=8~14对应P0.0~P0.6, chn=15对应BandGap电压.
***********************************/
void ADC_convert(u8 chn)
{
	u16	j;
	u8	k;		//平均值滤波时使用

	Get_ADC12bitResult(chn);		//参数i=0~15,查询方式做一次ADC, 切换通道后第一次转换结果丢弃. 避免采样电容的残存电压影响.
	Get_ADC12bitResult(chn);		//参数i=0~15,查询方式做一次ADC, 切换通道后第二次转换结果丢弃. 避免采样电容的残存电压影响.

#ifdef BUBBLE_SORT  //使用冒泡排序，去掉最高值、最低值，求中间平均值

	for(k=0; k<16; k++)	ADC_Buffer[k] = Get_ADC12bitResult(chn);
	BubbleSort(ADC_Buffer,16);  //冒泡排序
	for(k=4, j=0; k<12; k++) j += ADC_Buffer[k];  //取中间8个数据
	j = j / 8;		// 求平均

#else   //采样累加，求平均值（不需要的话可将 SUM_LENGTH 定义值改为 1 ）

	for(k=0, j=0; k<SUM_LENGTH; k++)	j += Get_ADC12bitResult(chn);	// 采样累加和 参数0~15,查询方式做一次ADC, 返回值就是结果
	j = j / SUM_LENGTH;		// 求平均

#endif

	if(chn == 15)	printf("Bandgap=%04u  ",j);	//内基准 1.19V
	else		//ADC0~ADC14
	{
		printf("ADC%02d=%04u  ",chn,j);
	}
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
	u16 i;
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i);
	}while(--ms);
}

//========================================================================
// 函数: void Timer0_Init(void)
// 描述: 定时器0初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-2
// 备注: 
//========================================================================
void Timer0_Init(void)
{
    // Timer0初始化
    TMOD &= 0xf0;//16 bits timer auto-reload
    T0x12 = 1;  //Timer0 set as 1T
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// 函数: void KeyResetScan(void)
// 描述: P3.2口按键长按1秒触发软件复位，进入USB下载模式。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//连续1000ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

                USBCON = 0x00;      //清除USB设置
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
                while (1);
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}

//========================================================================
// 函数: void timer0_int(void)
// 描述: Timer0 1ms中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void timer0_int(void) interrupt 1
{
    if (bUsbOutReady) //USB调试及复位所需代码
    {
        //USB_SendData(UsbOutBuffer,64);    //发送数据缓冲区，长度
        
        usb_OUT_done();
    }

    KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
}

