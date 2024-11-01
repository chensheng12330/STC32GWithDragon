#include "EEPROM.h"
#include <STC32G.h>

void IapIdle()
{
	IAP_CONTR = 0; 						// 关闭 IAP 功能
	IAP_CMD = 0; 							// 清除命令寄存器
	IAP_TRIG = 0; 						// 清除触发寄存器
	IAP_ADDRE = 0x00;					// 清零最高地址寄存器
	IAP_ADDRH = 0x00;					// 清零高地址寄存器
	IAP_ADDRL = 0x00;					// 清零低地址寄存器
}

char IapRead(unsigned long addr)
{
	char dat;
	IAP_CONTR = 0x80; 				// 使能 IAP
	IAP_TPS = 12; 						// 设置等待参数 12MHz
	IAP_CMD = 1; 							// 设置 IAP 读命令
	IAP_ADDRL = addr; 				// 设置 IAP 低地址
	IAP_ADDRH = addr >> 8; 		// 设置 IAP 高地址
	IAP_ADDRE = addr >> 16;		// 设置 IAP 最 高地址
	IAP_TRIG = 0x5a; 					// 写触发命令 (0x5a)
	IAP_TRIG = 0xa5; 					// 写触发命令 (0xa5)
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	dat = IAP_DATA; 					// 读 IAP 数据
	IapIdle(); 								// 关闭 IAP 功能
	return dat;
}
void IapProgram(unsigned long addr, char dat)
{
	IAP_CONTR = 0x80; 				// 使能 IAP
	IAP_TPS = 12; 						// 设置等待参数 12MHz
	IAP_CMD = 2;							// 设置 IAP 写命令
	IAP_ADDRL = addr; 				// 设置 IAP 低地址
	IAP_ADDRH = addr >> 8; 		// 设置 IAP 高地址
	IAP_ADDRE = addr >> 16; 	// 设置 IAP 最 高地址
	IAP_DATA = dat; 					// 写 IAP 数据
	IAP_TRIG = 0x5a; 					// 写触发命令 (0x5a)
	IAP_TRIG = 0xa5; 					// 写触发命令 (0xa5)
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	IapIdle();								// 关闭 IAP 功能
}
void IapErase(unsigned long addr)
{
	IAP_CONTR = 0x80; 				// 使能 IAP
	IAP_TPS = 12; 						// 设置等待参数 12MHz
	IAP_CMD = 3; 							// 设置 IAP 擦除命令
	IAP_ADDRL = addr; 				// 设置 IAP 低地址
	IAP_ADDRH = addr >> 8; 		// 设置 IAP 高地址
	IAP_ADDRE = addr >> 16;		// 设置 IAP 最 高地址
	IAP_TRIG = 0x5a; 					// 写触发命令 (0x5a)
	IAP_TRIG = 0xa5; 					// 写触发命令 (0xa5)
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	IapIdle(); 								// 关闭 IAP 功能
}