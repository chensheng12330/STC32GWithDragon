// 	@布丁橘长 2023/03/27
// 	ADC示例：16键ADC按键，MAX7219数码管显示键号，采样通道9：P0.0
//  ！！！设置为ADC通道的引脚，必须设置为高阻输入模式！！！
//  本例Vref参考电压为2.5V（屠龙刀R114电阻已经将Vref连接2.5V参考电压，不能再将Vref引脚用线接到VCC，除非断开R114）
//  AMX7219数码管模块引脚定义： CS = P6^5; DIN = P6^6; CLK = P6^4; （可在MAX7219.h中修改）
//  实验开发板：屠龙刀三.1 @主频12MHz

#include "stc32g.h"
#include "config.h"
#include "MAX7219.h"

void SYS_Ini();								// STC32初始化设置
void SEG_Disp(void);					// 数码管显示
void Timer0_Init(void);				// 定时器0初始化
void ADC_Config();						// ADC初始化设置
u16 Get_ADC12bitResult();			// 获取ADC	
void CalculateAdcKey(u16 adc);// 计算键值

u8  ADC_KeyState,ADC_KeyState1,ADC_KeyState2,ADC_KeyState3; //键状态
u8  ADC_KeyHoldCnt; //键按下计时
u8  KeyCode;    //给用户使用的键码, 1~16有效

bit B_1ms;										// 1ms计时标志
u16 adc_result;								// ADC转换结果
u8 segdelay;									// 数码管延时计数				
u8 adcdelay;									// ADC采样延时计数

void main(void)
{
	SYS_Ini();									// STC32初始化设置
	ADC_Config();								// ADC初始化设置
	Timer0_Init();							// 定时器0初始化
	EA = 1;											// 使能EA总中断
	MAX7219_Ini();							// MAX7219初始化
	SEG_Disp();									// 数码管显示
	KeyCode = 0;				
	while (1)
	{
		if(B_1ms == 1)
		{
			B_1ms = 0;
			segdelay++;
			adcdelay++;
		}
		if(adcdelay == 10)
		{
			adc_result = Get_ADC12bitResult(); 
      if(adc_result < 4096)    CalculateAdcKey(adc_result); //计算按键
			adcdelay = 0;
		}
		if(segdelay == 100)				// 每50毫秒刷新一次数码管
		{	
			SEG_Disp();							// 刷新数码管
			segdelay = 0;						// 清零数码管计时
		}
	}
}
void SYS_Ini()								// STC32初始化设置
{
	EAXFR = 1; 									// 使能访问 XFR
	CKCON = 0x00; 							// 设置外部数据总线速度为最快
	WTST = 0x00; 								// 设置程序代码等待参数，赋值为 0 可将 CPU 执行程序的速度设置为最快
	
	P0M1 = 0x00;P0M0 = 0x00;		// 设置P0口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0xFF;P1M0 = 0x00;		// 设置P1口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		// 设置P4口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		// 设置P5口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		// 设置P6口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P7M1 = 0x00;P7M0 = 0x00;		// 设置P7口为准双向口模式 //00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
}
void ADC_Config()
{
	P0M1 = 0x01;P0M0 = 0x00;		// P0.0设置为高阻输入模式
	ADC_CONTR &= 0xF0;					
	ADC_CONTR |= 0x08;					// 设置ADC通道为P0.0
	ADCTIM = 0x3F;							// ADC时序设置：通道选择时间：1个时钟，通道保持时间：4个时钟，采样时间：32个时钟
	ADCCFG = 0x2F;							// ADC频率设置：系统时钟/2/16，数据右对齐
	ADC_POWER = 1;							// 使能ADC
}
//========================================================================
// 函数: u16 Get_ADC12bitResult(u8 channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 12位ADC结果.
// 版本: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult()  
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_START = 1;							//启动ADC转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   		//wait for ADC finish
    ADC_FLAG = 0;    						//清除ADC结束标志
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}
/***************** ADC键盘计算键码 *****************************
电路和软件算法设计: Coody
本ADC键盘方案在很多实际产品设计中, 验证了其稳定可靠, 即使按键使用导电膜,都很可靠.
16个键,理论上各个键对应的ADC值为 (4096 / 16) * k = 256 * k, k = 1 ~ 16, 特别的, k=16时,对应的ADC值是4095.
但是实际会有偏差,则判断时限制这个偏差, ADC_OFFSET为+-偏差, 则ADC值在 (256*k-ADC_OFFSET) 与 (256*k+ADC_OFFSET)之间为键有效.
间隔一定的时间,就采样一次ADC,比如10ms.
为了避免偶然的ADC值误判, 或者避免ADC在上升或下降时误判, 使用连续3次ADC值均在偏差范围内时, ADC值才认为有效.
以上算法, 能保证读键非常可靠.
**********************************************/
#define ADC_OFFSET  120												//偏差值设置为：120,
void CalculateAdcKey(u16 adc)
{
    u8  i;
    u16 j;
    
    if(adc < (256-ADC_OFFSET))							// 排除ADC在0-256范围时的情况
    {
      ADC_KeyState = 0;  										// 键状态归0
      ADC_KeyHoldCnt = 0;										// 长按计时清零
			KeyCode = 0;													// 按键号0
    }
    j = 256;
    for(i=1; i<=16; i++)										// 从按键1-按键16区间，逐个匹配
    {
        if((adc >= (j - ADC_OFFSET)) && (adc <= (j + ADC_OFFSET)))  break;  //判断是否在偏差范围内
        j += 256;
    }
    ADC_KeyState3 = ADC_KeyState2;					// state1、2、3互换数据，用于连续记录连续3次的按键号是否相同
    ADC_KeyState2 = ADC_KeyState1;
    if(i > 16)  ADC_KeyState1 = 0;  				//键无效
    else                        						//键有效
    {
        ADC_KeyState1 = i;
        if((ADC_KeyState3 == ADC_KeyState2) && (ADC_KeyState2 == ADC_KeyState1) &&
           (ADC_KeyState3 > 0) && (ADC_KeyState2 > 0) && (ADC_KeyState1 > 0))
        {
            if(ADC_KeyState == 0)   				//第一次检测到
            {
                KeyCode  = i;  							//保存键码
                ADC_KeyState = i;  					//保存键状态
                ADC_KeyHoldCnt = 0;
            }
						
            if(ADC_KeyState == i)   				//连续检测到同一键按着
            {
                if(++ADC_KeyHoldCnt >= 100) //按下1秒后,以10次每秒的速度Repeat Key
                {
                    ADC_KeyHoldCnt = 90;
                    KeyCode  = i;   				//保存键码
                }
            }
            else ADC_KeyHoldCnt = 0; 				//按下时间计数归0
        }
    }
}
void Timer0_Isr(void) interrupt 1
{
	B_1ms = 1;									// 1毫秒到
}
void Timer0_Init(void)				//1毫秒@12.000MHz
{
	AUXR |= 0x80;								//定时器时钟1T模式
	TMOD &= 0xF0;								//设置定时器模式
	TL0 = 0x20;									//设置定时初始值
	TH0 = 0xD1;									//设置定时初始值
	TF0 = 0;										//清除TF0标志
	TR0 = 1;										//定时器0开始计数
	ET0 = 1;										//使能定时器0中断
}
void SEG_Disp(void)						// MAX7219数码管显示函数								
{							
	Write7219(8,15); 						// 左起第1位显示 熄灭
	Write7219(7,15); 						// 左起第2位显示 熄灭
	Write7219(6,15); 						// 左起第3位显示 熄灭
	Write7219(5,15); 						// 左起第4位显示 熄灭
	Write7219(4,15); 						// 左起第5位显示 熄灭
	Write7219(3,15); 						// 左起第6位显示 熄灭
	Write7219(2,(u16)(KeyCode/10)); 	//左起第7位显示 键值十位
	Write7219(1,(u16)(KeyCode%10)); 	//左起第8位显示 键值个位
}