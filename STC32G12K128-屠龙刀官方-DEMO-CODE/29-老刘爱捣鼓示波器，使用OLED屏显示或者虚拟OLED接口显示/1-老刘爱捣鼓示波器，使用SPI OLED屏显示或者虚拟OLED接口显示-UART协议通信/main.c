/*
    Mini DSO with STC MCU

    GitHub: https://github.com/CreativeLau/Mini-DSO
    
    Copyright (c) 2020 Creative Lau (creativelaulab@gmail.com)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

     
    Version: V0.3
    MCU Type: STC8A8K64S4A12 @27MHz
    YouTube Tutorial(First Version): https://youtu.be/c6gBv6Jcz7w
    YouTube Tutorial(V0.3): https://youtu.be/-8PadlS7c4c
    Instructable(First Version): https://www.instructables.com/id/Make-Your-Own-OscilloscopeMini-DSO-With-STC-MCU-Ea/
    Instructable(V0.3): 
    Any question please contact CreativeLauLab@gmail.com
    YouTube Channel: https://www.youtube.com/c/CreativeLau

    Note:
    How to Operation:
        Main Interface - Parameter Mode:
            Single Click Encoder: Run/Stop sampling
            Double Click Encoder: Enter Wave Scroll Mode
            Long Press Encoder: Enter Settings Interface
            Rotate Encoder: Adjust parameters
            Rotate Encoder While Pressing: Switch between options
            
        Main Interface - Wave Scroll Mode: 
            Single Click Encoder: Run/Stop sampling
            Double Click Encoder: Enter Parameter Mode
            Long Press Encoder: Enter Settings Interface
            Rotate Encoder: Scroll waveform horizontally (only available when sampling stopped)
            Rotate Encoder While Pressing: Scroll waveform vertically (only available when sampling stopped)
            
        Settings Interface:
            Single Click Encoder: N/A
            Double Click Encoder: N/A
            Long Press Encoder: Return to Main Interface
            Rotate Encoder: Adjust parameters
            Rotate Encoder While Pressing: Switch between options

    Parameters in Main Interface:
        Seconds Per Division: "500ms", "200ms", "100ms", "50ms", "20ms", "10ms","5ms", "2ms", "1ms", "500us", "200us", "100us"(100us only available in Auto Trigger Mode)
        Voltage Range: Maximum voltage is 30V. Rotate Encoder clockwise continuous to enter auto range. Rotate Encoder anticlockwise to enter manual range. 
        Trigger Level: Set trigger voltage level. The trigger position will be put at the center of chart. 
                       You should set a correct Trigger Level in Single and Normal Trigger Mode in order to capture the waveform.
        Trigger Slope: Trigger on Rising or Falling Edge. 
                       You should set a correct Trigger Slop in Single and Normal Trigger Mode in order to capture the waveform.
        Trigger Mode:
            Auto:   Sweep continuous, indicator always on. Single click the encoder to stop or run sampling. 
                    If triggered, the waveform will be shown on the display and the trigger position will be put at the center of chart. 
                    Otherwise, the waveform will scroll irregular, and 'Fail' will be shown on the display.
            Normal: When complete pre-sampling, you can input signal. If triggered, waveform shown on the display and waiting for new trigger. 
                    If no new trigger, the waveform will be kept.
            Single: When complete pre-sampling, you can input signal. If triggered, waveform shown on display and stop sampling. 
                    User need to single click Encoder to start next sampling.
            For Normal Mode and Single Mode, be sure the trigger level has been adjusted correctly, otherwise no waveform will be shown on the display.

    Status in Main Interface:
        'Run': Sampling Running.
        'Stop': Sampling Stopped.
        'Fail': The Trigger Level beyond the waveform in Auto Trigger Mode.
        'Auto': Auto Voltage Range.
                    
    Parameters in Settings Interface:
        PMode(Plot Mode): Show waveform in Vector or Dots.
        LSB(Sampling Coefficient): 100 times of voltage dividing coefficient. e.g. the resistor for voltage dividing is 10k and 2k, calculate the voltage dividing coefficient (10+2)/2=6. Get the LSB=6x100=600.
        BRT(Brightness): Adjust OLED Brightness

    Saving Settings: When exit settings interface, all parameters in settings and main interface will be saved in EEPROM.

    Indicator:
    Generally, the indicator on means the sampling is running. 
    The more important use is in Single and Normal Trigger Mode, before get into the trigger stage, pre-sampling is required. 
    The indicator will not on during pre-sampling stage. We should not input signal until the indicator comes on. 
    The longer time scale selected, the longer waiting time of pre-sampling.

    2020-04-30 Update
    1. Add Single Sweep and Normal Sweep function. Set trigger value to catch the waveform mutation.
    2. According to datasheet, set ADC port to high-impedance.

    2020-05-3 Update
    1. Show frequency of waveform.
    2. Scroll waveform horizontally/vertically with EC11 Encoder.
    3. Zoom waveform vertically with EC11 Encoder.
    4. The trigger level could be customized.

    简易示波器
    版本：V0.3
    作者：老刘爱捣鼓
    单片机型：STC8A8K64D4 @27MHz
    任何问题请联系CreativeLauLab@gmail.com
    老刘爱捣鼓（全网同名）期待您的关注！
    B站教程（初版）：https://www.bilibili.com/video/BV1ai4y1t79R
    B站教程（V0.3）：https://www.bilibili.com/video/BV1ai4y1t79R
    油管频道: https://www.youtube.com/c/CreativeLau

    操作说明：
        主界面-选项设置模式：
            单击编码器：切换开始/暂停采样
            双击编码器：切换波形水平滚动模式/选项设置模式
            长按编码器：进入设置界面
            旋转编码器：修改当前参数
            按下同时旋转编码器：在参数间切换
            
        主界面-波形水平滚动模式：
            单击编码器：切换开始/暂停采样
            双击编码器：切换波形水平滚动模式/选项设置模式
            长按编码器：进入设置界面
            旋转编码器：水平滚动波形（在暂停采样状态下有效）
            按下同时旋转编码器：垂直滚动波形（仅在暂停采样状态下有效）
            
        设置界面：
            单击编码器：无效
            双击编码器：无效
            长按编码器：返回主界面
            旋转编码器：修改当前选项
            按下同时旋转编码器：切换选项

    主界面参数：
        横轴时间区间："500ms", "200ms", "100ms", "50ms", "20ms", "10ms","5ms", "2ms", "1ms", "500us", "200us", "100us"(100us仅在自动触发模式下可用)
        纵轴电压区间：设定电压量程上限，最大30V，顺时针旋转编码器将电压上限设置为0则进入自动量程，逆时针旋转进入手动量程
        触发值：设定触发电压值，触发位置被放在屏幕中心
        触发方向：上下箭头，分别代表上升沿触发和下降沿触发
        触发方式：
            Auto(自动触发)  ：连续采样，指示灯始终点亮，通过单击编码器控制启停；触发成功时，触发点位于屏幕中心，触发失败则波形无规律滚动，屏幕左侧显示Fail标志
            Normal(普通触发)：等待预采样，待指示灯亮起表示预采样结束，这时可以输入信号；
                              触发成功后，显示波形，并继续等待下次触发；
                              如果有新的触发成功，则自动显示新的波形，如没有新的触发成功则屏幕波形保持并一直等待下次触发；
                              需注意设置正确的触发值，否则一直处于等待触发状态，屏幕不会显示波形
            Single(单次触发)：等待预采样，待指示灯亮起表示预采样结束，这时可以输入信号；
                              触发成功后，显示波形，并停止采样，需用户单击编码器开始下次采样	
                              需注意设置正确的触发值，否则一直处于等待触发状态，屏幕不会显示波形
            
    主界面状态显示：	
        Run：正在采样
        Stop：停止采样
        Fail：自动模式下，触发值超出波形范围引起触发失败
        Auto：自动量程
            
    设置界面选项：
        PMode(绘图模式)：
            Vector：波形以矢量显示
            Dots  ：波形以点阵显示
        LSB(采样系数)：分压系数的100倍，比如用10k加2k的电阻进行分压，分压系数为(10+2/2)=6，LSB理论取值为6x100=600，根据实际电阻精度进行微调	
        BRT：OLED亮度

    保存设置：长按编码器退出设置界面时，设置界面和主界面的所有参数被保存到EEPROM

    2020-04-30更新
    1. 增加单次触发和普通触发功能，可自定义触发点用来捕捉波形变化
    2. 按照规格书要求将ADC采样端口设置为高阻输入
    2020-5-3更新
    1. 增加波形频率显示
    2. 使用编码器水平/垂直滚动波形
    3. 使用编码器垂直缩放波形
    4. 用户可设定触发电平
    2022-7-13更新
    1. 单片机型号修改为 "STC32G12K128"，默认主频 35MHz
    2. 修改显示方式为STC-ISP调试接口里的虚拟OLED屏显示，或者硬件OLED屏显示
    3. 修改EEPROM访问程序
    4. 通过P3.2,P3.3,P3.4,P3.5口按键控制选项设置
*/

#include "stc32g.h"
#include "global.h"
#include "delay.h"
#include "adc_stc.h"
#include "config_stc.h"
#include "settings.h"
#include "chart.h"
#include "ssd1306.h"
#include "stc.h"
#include "interface.h"
#include "uart.h"
//#include "stdio.h"

#define Timer1_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 1 中断频率, 1000次/秒


#define VBAT_UPDATE_FREQ 40 //电池电压信息的更新频率 VBAT_UPDATE_FREQ*25ms
#define VBAT_LSB 200        //电池电压采样分压系数*100
#define INT_0 0             //外部中断0序号，编码器旋转
#define INT_1 2             //外部中断1序号，编码器按下
#define TIMER_0 1           //定时器0中断序号

#define KEY_TIMER 30        //按键持续检测时间(ms)

sbit KEY1 = P3 ^ 2;
sbit KEY2 = P3 ^ 3;
sbit KEY3 = P3 ^ 4;
sbit KEY4 = P3 ^ 5;

uint16 Key1_cnt;
uint16 Key2_cnt;
uint16 Key3_cnt;
uint16 Key4_cnt;
bit Key1_Flag;
bit Key2_Flag;
bit Key3_Flag;
bit Key4_Flag;
bit Key3_Short_Flag;
bit Key4_Short_Flag;
//bit Key3_Long_Flag;
//bit Key4_Long_Flag;

bit oled_init_flag;

//bit Key1_Function;
//bit Key2_Function;
//bit Key3_Function;
//bit Key4_Short_Function;
//bit Key4_Long_Function;

bit PWM1_Flag;
uint16 PWM1_Duty;

void KeyScan(void);

void init()
{
    InSettings = 0;         //初始化开机界面
    OptionInSettings = 0;   //初始化设置界面的选项编号
    OptionInChart = 0;      //初始化主界面的选项标号
    WaveScroll = 0;         //清零波形滚动标志
    OptionChanged = 0;      //清零设置修改标志
    ADCRunning = 1;         //置位采样标志
    ADCInterrupt = 0;       //清零ADC中断标志
    ADCComplete = 0;        //清零ADC完成标志
    WaveUpdate = 0;         //清零波形更新标志
    ClearWave = 0;          //清零清除波形标志
    ClearDisplay = 1;       //置位清空屏幕标志
    EC11PressAndRotate = 0; //清零编码器旋转标志
    UpdateVbat = 0;         //清零采样电池电压标志
    ADCComplete = 0;        //清零ADC完成采样标志
    WaveFreq = 0;           //清零波形频率
    TriPos = 50;            //初始化触发点位置
    TriPosOffset = 0;       //初始化触发点偏移量
    TriFail = 0;            //清零触发失败标志
    VMax = 0;               //清零波形电压最大值
    VMin = 0;               //清零波形电压最小值
    WaveLengthSumNum = 0;   //清零波形长度求和次数
    WaveLengthSum = 0;      //清零波形长度求和

    /* 获取内部参考电压值
       Get internal reference voltage */
    BGV = GetBGV();
}

void runWhenInSettrings()
{
    P_Ready = 0;
    ClearDisplay = 1;
    while (InSettings)
    {
        /* 更新电池电压信息
           Update voltage infor of battery */
        if (UpdateVbat)
        {
            UpdateVbat = 0;
            VBat = GetVoltage(ADC_BAT, VBAT_LSB);
        }

        /* 更新屏幕亮度
           Update OLED Brightness */
        if (OLED_BrightnessChanged)
        {
            OLED_BrightnessChanged = 0;
            
#if(VirtualDevice)
            OLED12864_SetContrast(OLED_Brightness * 10);    //亮度值（范围0～255）
#else
            OLED_Write_Command(0x81);
            OLED_Write_Command(OLED_Brightness * 10);
#endif
        }

        /* 更新参数
           Update parameters */
        if (DisplayUpdate)
        {
            DisplayUpdate = 0;
            PlotSettings();
            OLED_Display();
        }
    }

    /* 保存设置
       Save Settings */
//    EA = 0;
    if (OptionChanged)
    {
        OptionChanged = 0;
        ClearDisplay = 0;
        /* 保存设置并显示保存状态
           Save settings and display saving status*/
        PlotSaveStatus(Save_Options());
        OLED_Display();
        delay_ms(500);
    }

    /* 退出settings后刷新屏幕
       Redraw display after exit settings */
    ClearDisplay = 1;
    PlotChart();
    OLED_Display();
//    EA = 1;
}

/*
    ADCRuning=1
    |	WaveUpdate=1
    |	|	ClearWave=1
    |	|	ClearWave=0
    |	WaveUpdate=0
    ADCRuning=0
        DisplayUpdate=1
            WaveUpdate=1
            |	ClearWave=1
            |	ClearWave=0
            WaveUpdate=0
*/
void runWhenADCInterrupt()
{
    ADCInterrupt = 0;
    P_Ready = 0;

    /* ADC采样运行
       ADC Sampling Running */
    if (ADCRunning)
    {
        if (WaveUpdate)
        {
            WaveUpdate = 0;
            ClearDisplay = 1;

            /* 清空波形缓存
               Clear waveform in buffer*/
            if (ClearWave)
            {
                ClearWave = 0;
                memset(ADCbuf, 0x00, SAMPLE_NUM * 2);
                WaveFreq = 0;
                TriFail = 0;
                VMax = 0;
                VMin = 0;
                PlotChart();
            }
            /* 显示缓存中的波形
               Display waveform in buffer*/
            else
            {
                AnalyseData();
                PlotChart();
                PlotWave();
            }
        }
        else
        {
            ClearDisplay = 0;
            PlotChart();
        }
        OLED_Display();
    }

    /* ADC采样停止
       ADC Sampling Stopped */
    else
    {
        while (!ADCRunning && !InSettings)
        {
            if (DisplayUpdate)
            {
                DisplayUpdate = 0;
                if (WaveUpdate)
                {
                    WaveUpdate = 0;
                    /* 分析缓存中完成采样的采样数据
                       Analyse completed sampling data in buffer */
                    AnalyseData();
                    ClearDisplay = 1;

                    /* 清空屏幕，绘制波形
                       Clear display and draw waveform */
                    if (ScaleH == ScaleH_tmp)
                    {
                        /* 采样点数有限，不支持横向缩放波形，只有时间区间与完成采样的时间区间相同才绘制波形
                           Since the sampling points limitation, scaling waveform along horizontal is not support.
                           Show waveform only when time scale is same as that when sampling completed */
                        PlotChart();
                        PlotWave();
                    }

                    /* 清空屏幕，不绘制波形
                       Clear display and no waveform */
                    else
                    {
                        WaveFreq = 0; //清零WaveFreq，绘制界面时，频率位置会显示****Hz
                        PlotChart();
                    }
                }
                /* 只更新屏幕上的参数，波形不变
                   Update parameters on display only, keep waveform */
                else
                {
                    ClearDisplay = 0;
                    PlotChart();
                }

                OLED_Display();
            }
        }
    }
}

void runWhenADCComplete()
{
    /*  ADC采样完成 - 单次触发
        ADC Sampling Complete - Single Trigger Mode */
    if (TriMode == 2)
    {
        ADCInterrupt = 1;
        DisplayUpdate = 0;
        WaveUpdate = 0;
        ADCRunning = 0; //清零ADC运行标志，停止采样
        AnalyseData();
        ClearDisplay = 1;
        PlotChart();
        PlotWave();
        OLED_Display();
    }

    /*  ADC采样完成 - 自动触发或普通触发
        ADC Sampling Complete - Auto or Normal Trigger Mode */
    else
    {
        ClearDisplay = 1; //置位清空屏幕标志
        AnalyseData();    //分析采样数据
        PlotChart();      //绘制主界面
        PlotWave();       //绘制波形
        OLED_Display();
    }
}

void Timer0Init(void)		//25毫秒@1MHz
{
    TM0PS = 34;     //35分频
	AUXR |= 0x80;	//定时器时钟1T模式
	TMOD &= 0xF0;	//设置定时器模式
	TL0 = 0x58;		//设置定时初始值
	TH0 = 0x9E;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
//	TR0 = 1;		//定时器0开始计时
    ET0 = 1;        //允许定时器0中断
}

void Timer1Init(void)		//1毫秒
{
	AUXR |= 0x40;	//定时器时钟1T模式
	TMOD &= 0x0F;	//设置定时器模式
	TH1 = (uint8)(Timer1_Reload / 256);	//设置定时初始值
	TL1 = (uint8)(Timer1_Reload % 256);	//设置定时初始值
	TF1 = 0;		//清除TF1标志
    ET1 = 1;		//允许中断
	TR1 = 1;		//定时器1开始计时
}

void PWMInit(void)
{
    PWMA_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCMR1 = 0x68; //通道模式配置，开启CCR预装载功能
    PWMA_CCER1 = 0x05; //配置通道输出使能和极性

    PWMA_ARRH = 0x00;  //设置周期时间
    PWMA_ARRL = 0x0f;
    PWMA_CCR1H = 0;
    PWMA_CCR1L = 0;    //设置占空比时间

    PWMA_ENO = 0x01;   //PWM1P使能输出
    PWMA_PS = 0x00;    //高级 PWM 通道输出脚选择位

    PWMA_BKR = 0x80;   //使能主输出
    PWMA_CR1 |= 0x81;  //ARR由预装载缓冲器缓冲，开始计时
}

void UpdatePwm(void)
{
    PWMA_CCR1H = (u8)(PWM1_Duty >> 8); //设置占空比时间
    PWMA_CCR1L = (u8)(PWM1_Duty);
}

/* 程序入口
   Program Entry */
void main()
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度
    
    /* 将ADC端口P0.4，P0.6设置为高阻
       Set ADC port P0.4, P0.6 as high-impedance */
//    P0M1 |= 0x50;
//    P0M0 &= ~0x50;

    P0M1 = 0x00;
    P0M0 = 0x03;

    /* 设置端口P1为准双向
       Set port P1 as quasi bidirectional */
//    P1M1 &= ~0x02;
//    P1M0 |= 0x02;
    P1M1 = 0x10;
    P1M0 = 0x00;

    /* 设置OLED端口P2.3，P2.4，P2.5，P2.6，P2.7为准双向
       Set OLED port P2.3, P2.4, P2.5, P2.6 as quasi bidirectional */
//    P2M1 &= ~0xF8;
//    P2M0 &= ~0xF8;
    P2M1 = 0x00;
    P2M0 = 0x00;

    /* 设置编码器端口P3.2，P3.3，P3.4为准双向
       Set Encoder port P3.2, P3.3, P3.4 as quasi bidirectional */
//    P3M1 &= ~0x1C;
//    P3M0 &= ~0x1C;
    P3M1 = 0x00;
    P3M0 = 0x00;

    /* 设置指示灯端口P4为准双向
       Set indicator port P4 as quasi bidirectional */
    P4M1 = 0x00;
    P4M0 = 0x00;

    P7M1 = 0x00;
    P7M0 = 0x00;

    PWM1_Flag = 0;
    PWM1_Duty = 0;
    
    ADC_POWER = 1;       //使能 ADC 模块
    Timer0Init();
    Timer1Init();
    PWMInit();
#if(VirtualDevice)
    uart_init();
#endif

    IAP_TPS = 35; //设置EEPROM操作时间 35MHz/1000000
    Read_Options();

    /* 开总中断
       Enable global interrupt */
    EA = 1;

    init();
//    OLED_Init();
    OLED_SetFontWidth(6);
    PlotChart();
//    OLED_Display();

    while (1)
    {
        if(oled_init_flag == 0)
            continue;

#if(VirtualDevice)
        if (bUartRecvReady)
        {
            uart_OUT_done();
        }
#endif
        
        P_Ready = 0;

        /* 采样波形
           Sampling*/
        GetWaveData();
        
        /* 进入设置界面
           Enter Settings Interface */
        if (InSettings)
        {
            runWhenInSettrings();
        }

        /* ADC采样中断
           ADC Sampling Interrupt */
        else if (ADCInterrupt)
        {
            runWhenADCInterrupt();
        }

        /* ADC采样完成
           ADC Sampling Complete */
        else
        {
            runWhenADCComplete();
        }
    }
}

/* 为更新Vbat计时
   Timer for updating Vbat */
void TIMER0_interrupt(void) interrupt TIMER_0
{
    static uint8 n;

    if (++n >= VBAT_UPDATE_FREQ)
    {
        n = 0;
        UpdateVbat = 1;
        DisplayUpdate = 1;
    }
}

void TIMER1_interrupt(void) interrupt 3
{
    KeyScan();
    	
    if(!PWM1_Flag)
    {
        PWM1_Duty++;
        if(PWM1_Duty >= 8) PWM1_Flag = 1;
    }
    else
    {
        PWM1_Duty--;
        if(PWM1_Duty <= 0) PWM1_Flag = 0;
    }
    UpdatePwm();
}

//========================================================================
// 函数: void KeyScan(void)
// 描述: 按键扫描函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void KeyScan(void)
{
	//短按按键
	if(!KEY1)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt >= KEY_TIMER)	//30ms防抖
			{
				Key1_Flag = 1;			//设置按键状态，防止重复触发
                Change_Val(1);      /* 正转  Clockwise */
//				Key1_Function = 1;
			}
		}
	}
	else
	{
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	//短按按键
	if(!KEY2)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;
			if(Key2_cnt >= KEY_TIMER)	//30ms防抖
			{
				Key2_Flag = 1;			//设置按键状态，防止重复触发
                Change_Val(0);      /* 反转  Anticlockwise */
//				Key2_Function = 1;
			}
		}
	}
	else
	{
		Key2_cnt = 0;
		Key2_Flag = 0;	//按键释放
	}

	//单纯短按按键
	if(!KEY3)
	{
		if(!Key3_Flag)
		{
			Key3_cnt++;
			if(Key3_cnt >= 1000)		//长按1s
			{
				Key3_Short_Flag = 0;	//清除短按标志
//				Key3_Long_Flag = 1;		//设置长按标志
				Key3_Flag = 1;				//设置按键状态，防止重复触发
//				Key3_Long_Function = 1;

                /* 主界面
                   Main Interface*/
                if (!InSettings)
                {
                    WaveScroll = ~WaveScroll;
                    OptionChanged = 1;
                    ADCInterrupt = 1;
                    DisplayUpdate = 1;
                    ClearWave = 0;
                }
            }
			else if(Key3_cnt >= KEY_TIMER)	//30ms防抖
			{
				Key3_Short_Flag = 1;		//设置短按标志
			}
		}
	}
	else
	{
		if(Key3_Short_Flag)			//判断是否短按
		{
			Key3_Short_Flag = 0;	//清除短按标志
//			Key3_Short_Function = 1;

            /* 切换主界面和设置界面 - Switch main interface and settings interface */
            InSettings = ~InSettings;

            /*  进入设置界面
                Enter Settings  */
            if (InSettings)
            {
                DisplayUpdate = 1;
                UpdateVbat = 1;
                TF0 = 0; //清零定时器0溢出标志
                TR0 = 1; //定时器0开始计时，开始电池电压信息更新计时
            }

            /*  回到主界面
                Retrurn to main interface   */
            else
            {
                TR0 = 0; //清零定时器0溢出标志
                TF0 = 0; //定时器0停止计时，停止电池电压信息更新计时
                WaveFreq = 0;
                TriFail = 0;
                VMax = 0;
                VMin = 0;
                DisplayUpdate = 1;
                WaveUpdate = 1;
                ClearWave = 0;
            }
            ADCInterrupt = 1;
		}
		Key3_cnt = 0;
		Key3_Flag = 0;
	}

	//长按短按按键
	if(!KEY4)
	{
		if(!Key4_Flag)
		{
			Key4_cnt++;
			if(Key4_cnt >= 1000)		//长按1s
			{
				Key4_Short_Flag = 0;	//清除短按标志
//				Key4_Long_Flag = 1;		//设置长按标志
				Key4_Flag = 1;				//设置按键状态，防止重复触发
//				Key4_Long_Function = 1;

                /* 在主界面，滚动波形和选项模式之间切换 - Switch between Waveform Scroll Mode and Parameter Mode in Main Interface */
                if(oled_init_flag == 0)
                {
                    oled_init_flag = 1;
                    OLED_Init();
                    OLED_Display();
                }
			}
			else if(Key4_cnt >= KEY_TIMER)	//30ms防抖
			{
				Key4_Short_Flag = 1;		//设置短按标志
			}
		}
	}
	else
	{
		if(Key4_Short_Flag)			//判断是否短按
		{
			Key4_Short_Flag = 0;	//清除短按标志
//			Key4_Short_Function = 1;

            /* 在主界面，切换Stop/Run状态 - Switch Run/Stop in main interface    */
            if (!InSettings)
            {
                ADCRunning = ~ADCRunning;
                if (ADCRunning)
                {
                    WaveUpdate = 1;
                    ClearWave = 1;
                }
                else
                {
                    DisplayUpdate = 1;
                    WaveUpdate = 1;
                }
                ADCInterrupt = 1;
            }
		}
		Key4_cnt = 0;
		Key4_Flag = 0;	//按键释放
	}
}
