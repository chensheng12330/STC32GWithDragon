// 	@布丁橘长 2023/02/07
// 	#include 路径示例
//											1：delay.h			在代码工程文件夹根目录
//											2：Matrix_key.h	在工程文件夹内的Matrix_key文件夹中
//											3:Seg595.h			在工程文件夹外的Seg595文件夹中
//	数码管左边第1位，循环显示0-9
//	595驱动数码管模块-8位共阳极，引脚定义：DS(DIO):P6.6 SH(SCK):P6.5 ST(RCK):P6.4
//	基于STC32G12K128-LQFP64 屠龙刀开发板 主频35MHz

#include "STC32G.H"											//STC32G头文件
#include <stdio.h>											//标准库-标准输入输出函数（本代码未使用，仅演示尖括号和双引号用）
#include "..\config\config.h"						//config.h			在工程文件夹外的config文件夹中
#include "Delay.h"											//delay.h				在工程文件夹根目录
#include ".\Matrix_key\Matrix_key.h"		//Matrix_key.h	在工程文件夹内的Matrix_key文件夹中
#include ".\Seg595\Seg595.h"						//Seg595.h			在工程文件夹内的Seg595文件夹中

void main()
{
	u8 number;				//数码管显示的数字
	
	EAXFR = 1;				//使能XFR访问
	CKCON = 0x00;			//设置外部数据总线速度最快
	WTST = 0x00;			//设置程序读取等待时间为0个时钟，CPU执行程序速度最快
	
	P0M1 = 0x00;P0M0 = 0x00;		//设置P0口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P1M1 = 0x00;P1M0 = 0x00;		//设置P1口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P3M1 = 0x00;P3M0 = 0x00;		//设置P3口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P4M1 = 0x00;P4M0 = 0x00;		//设置P4口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P5M1 = 0x00;P5M0 = 0x00;		//设置P5口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	P6M1 = 0x00;P6M0 = 0x00;		//设置P6口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		SEG_Disp(number,1);						//数码管左边第1位显示数字number值
		delayms(300);									//延时300毫秒
		number++;											//number加1
		if(number > 9) number = 0;		//number>9清零number
	}
}