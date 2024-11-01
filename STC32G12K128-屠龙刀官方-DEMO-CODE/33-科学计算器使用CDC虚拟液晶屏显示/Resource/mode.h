#ifndef _MODE_H_
#define _MODE_H_

#define uchar unsigned char
#define uint unsigned int

typedef enum
{
	NORMAL,				//普通运算
	COMPLEX,			//复数运算（支持加减乘除，不支持函数）
	MATRIX,				//矩阵运算
	DRAW_PIC,			//函数图像
	SOLVE_EQUATION,		//解方程
//	TAKE_DIFFER,		//函数求导
	STASTISTICS,		//统计
	DIFFER_EQUATION,    //微分方程
	MODE,				//菜单
	SET,				//设置
//	CHANGE_UNITS,	//单位转化
}PROGRAM_STATUS;

uchar mode_choose(void);

void mode_select(void);

void mode_sleep(void);

void reset(void);		//软复位


#endif