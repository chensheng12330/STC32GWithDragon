//		@布丁橘长 2022/11/15
//		8位流水灯实验-花式流水灯,10种变化
//		1：	LED从两端向中间逐个点亮，直至全亮
//		2:	再从中间向两端逐个熄灭，直至全灭
//		3:	1、3、5、7逐个点亮
//		4:	2、4、6、8逐个点亮，直至8灯全亮
//		5:	奇数灯和偶数灯交替闪烁3次,8灯全灭
//		6:	LED1正向开始往复流水，LED8反向开始往复流水，往复一次（1-8 8-1）后全灭
//		7:	简易爆闪灯：LED1-LED4为一组，LED5-LED8为一组，交替爆闪
//		8:	简易电子沙漏效果：LED1开始正向流水，到LED8后保持常亮，继续从LED1开始，到LED7后，LED7、LED8常亮...直至全亮
//		9:	LED1开始正向流水，第一次一个LED流水，第二次2个LED流水...直至6个LED流水
//		10:	简易打砖块效果：LED3-LED8开始常亮，LED1开始流水，到LED3后，从LED3开始反向流水，再从LED1开始正向流水，到LED4后，开始反向...直至LED8反向到LED1
//		实验采用 数组法
//		LED电路由8个LED共阳极连接，每个LED配一个限流电阻。公共端接VCC，低电平点亮。（屠龙刀三.1板载LED电路）
//		LED引脚定义：P20-LED1 P21-LED2 P22-LED3 P23-LED4 P24-LED5 P25-LED6 P26-LED7 P27-LED8
//		本实验采用准双向口模式，低电平点亮、高电平熄灭
//		实验开发板：STC32G12K128-LQFP64 屠龙刀三.1  35MHz
// 
#include <STC32G.H>

#define u8 		unsigned char
#define u16 	unsigned int
#define u32		unsigned long

#define MAIN_Fosc 35000000UL	//定义主频35MHz，请根据实际使用频率修改（用于delay函数自适应主频）

void delayms(u16 ms);	//延时函数声明

void LED_Show1();			//LED花式1函数声明
void LED_Show2();			//LED花式2函数声明
void LED_Show3();			//LED花式3函数声明
void LED_Show4();			//LED花式4函数声明
void LED_Show5();			//LED花式5函数声明
void LED_Show6();			//LED花式6函数声明
void LED_Show7();			//LED花式7函数声明
void LED_Show8();			//LED花式8函数声明
void LED_Show9();			//LED花式9函数声明
void LED_Show10();		//LED花式10函数声明

u8 LED_Code[8] ={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};	//LED1-LED8点亮时对应的P2值
u8 sum;

void main()
{
	WTST = 0;	//设置等待时间为0个时钟，CPU运行速度最快
	
	P2M1 = 0x00;P2M0 = 0x00;		//设置P2口为准双向口模式 00：准双向口 01：推挽输出 10：高阻输入 11：开漏输出
	
	while(1)
	{
		sum = 0;
		LED_Show1();			//LED花式1- 两端向中间点亮
		LED_Show2();			//LED花式2- 中间向两端熄灭
		LED_Show3();			//LED花式3- 中间向两端熄灭
		LED_Show4();			//LED花式4- 1、3、5、7逐个点亮并保持点亮状态
		LED_Show5();			//LED花式5- 2、4、6、8逐个点亮并保持点亮状态,直至8灯全亮
		LED_Show6();			//LED花式6- 奇数灯和偶数灯交替闪烁3次，然后全灭
		LED_Show7();			//LED花式7- LED1和LED8双向往复流水一次
		LED_Show8();			//LED花式8- LED1-LED4为一组，LED5-LED8为一组，交替爆闪
		LED_Show9();			//LED花式9- 第一次1个LED流水，第二次2个LED流水....第6次6个LED流水
		LED_Show10();			//LED花式10-简易打砖块效果
	}
}
void LED_Show1()
{
	u8 i = 0;
	while(i < 4)		//1-两端向中间点亮
	{
		sum += ~LED_Code[i] + ~LED_Code[7-i];//取反后相加，就可以点亮2个灯，累加就可以保持已点亮的灯
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show2()
{
	u8 i = 4;
	sum = 0xFF;
	while(i < 8)		//2-中间向两端熄灭
	{
		sum -= ~LED_Code[i]+~LED_Code[7-i];//取反后相减，就可以熄灭2个灯，累减就可以保持已熄灭的灯
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show3()
{
	u8 i = 0;
	while(i < 4)		//3-1、3、5、7逐个点亮并保持点亮状态
	{
		sum += ~LED_Code[2*i];
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show4()
{
	u8 i = 0;
	while(i < 4)		//4-2、4、6、8逐个点亮并保持点亮状态,直至8灯全亮
	{
		sum += ~LED_Code[2*i+1];
		P2 = ~sum;
		i++;
		delayms(500);
	}
}
void LED_Show5()		//5-奇数灯和偶数灯交替闪烁3次，然后全灭
{
	u8 i = 0;
	u8 sum1 = 0;
	u8 sum2 = 0;
	for(i = 0;i < 4;i++)			//计算奇数灯全亮的数值，和偶数灯全亮的数值
	{
		sum1 += ~LED_Code[2*i];
		sum2 += ~LED_Code[2*i+1];
	}
	i = 0;
	while(i < 3)			//交替闪烁3次
	{
		P2 = ~sum1;
		delayms(500);
		P2 = ~sum2;
		delayms(500);
		i++;
	}
	P2 = 0xFF;			//闪烁完成后，熄灭
}
void LED_Show6()		//LED1和LED8双向往复流水一次
{
	u8 i;
	for(i = 0;i < 8;i++)
	{
		sum = ~LED_Code[i] + ~LED_Code[7-i];
		P2 = ~sum;
		if(i != 4)delayms(500);
	}
	P2 = 0xFF;
}
void LED_Show7()		//LED1-LED4为一组，LED5-LED8为一组，交替爆闪
{
	u8 i = 0;
	u8 j;
	u8 sum1 = 0;
	u8 sum2 = 0;
	for(i = 0;i < 4;i++)			//计算第1组全亮数值，和第2组全亮的数值
	{
		sum1 += ~LED_Code[i];
		sum2 += ~LED_Code[4+i];
	}
	i = 0;
	while(i < 3)							//重复3次
	{
		for(j = 0;j < 12;j++)		//第一组LED爆闪12次
		{
			P2 = ~sum1;
			delayms(15);
			P2 = 0xFF;
			delayms(45);
		}
		for(j = 0;j < 12;j++)		//第二组LED爆闪12次
		{
			P2 = ~sum2;
			delayms(15);
			P2 = 0xFF;
			delayms(45);
		}
		i++;
	}
	P2 = 0xFF;
}
void LED_Show8()//8-简易电子沙漏效果
{
	u8 i,j;
	sum = 0;
	for(i = 0;i < 8;i++)
	{
		for(j = 0;j < 8-i;j++)
		{
			P2 = ~(sum + ~LED_Code[j]);		//移动的LED和底部LED累加值相加后，再一起点亮
			delayms(500);
		}
		sum += ~LED_Code[7-i];		//将底部已经点亮的LED值累加
	}
}
void LED_Show9()		//第一次1个LED流水，第二次2个LED流水....第6次6个LED流水
{
	u8 i,j;
	for(i = 0;i < 6;i++)		//流水次数
	{
		for(j = i;j < 8;j++)	//流水步数
		{
			if(i == 0) sum = 0;		//第一次
			if(i == 1) sum = ~LED_Code[j-1];		//第二次
			if(i == 2) sum = ~LED_Code[j-2] + ~LED_Code[j-1];		//第三次
			if(i == 3) sum = ~LED_Code[j-3] + ~LED_Code[j-2]+~LED_Code[j-1];		//第四次
			if(i == 4) sum = ~LED_Code[j-4] + ~LED_Code[j-3] + ~LED_Code[j-2]+~LED_Code[j-1];		//第五次
			if(i == 5) sum = ~LED_Code[j-5] + ~LED_Code[j-4] + ~LED_Code[j-3] + ~LED_Code[j-2]+~LED_Code[j-1];		//第六次	
			P2 = ~(sum + ~LED_Code[j]);		//相加后，一起点亮
			delayms(500);
		}
	}
}
void LED_Show10()//10-简易打砖块效果
{
	u8 i,j;
	sum = 0;
	for(i = 0;i < 6;i++)//球发射的次数
	{
		for(j = 0;j < 2+i;j++)	//正向流水（发射->打砖块）
		{
			if(i == 5) sum = ~LED_Code[7];		//第一次
			else if(i == 4) sum = ~LED_Code[7] + ~LED_Code[6];		//第二次
			else if(i == 3) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5];		//第三次
			else if(i == 2) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4];		//第四次
			else if(i == 1) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4] + ~LED_Code[3];		//第五次
			else if(i == 0) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4] + ~LED_Code[3] + ~LED_Code[2];		//第六次	
			P2 = ~(sum + ~LED_Code[j]);		//相加后，一起点亮
			if(j != i+1) delayms(500);
		}
		for(j = 2+i;j > 0;j--)	//反向流水（打砖块后->返回）
		{
			if(i == 5) sum = 0;		//第一次
			else if(i == 4) sum = ~LED_Code[7];		//第二次
			else if(i == 3) sum = ~LED_Code[7] + ~LED_Code[6];		//第三次
			else if(i == 2) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5];		//第四次
			else if(i == 1) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4];		//第五次
			else if(i == 0) sum = ~LED_Code[7] + ~LED_Code[6] + ~LED_Code[5] + ~LED_Code[4] + ~LED_Code[3];		//第六次	
			P2 = ~(sum + ~LED_Code[j]);		//相加后，一起点亮
			if(j != i+2) delayms(500);
		}
	}
}
void  delayms(u16 ms)		//简单延时函数，自适应主频，1毫秒*ms
{
	u16 i;
	do{
			i = MAIN_Fosc / 6000;
			while(--i);
		}while(--ms);
}