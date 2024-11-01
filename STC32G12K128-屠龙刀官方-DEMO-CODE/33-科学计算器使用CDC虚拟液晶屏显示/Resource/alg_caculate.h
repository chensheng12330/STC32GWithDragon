#ifndef _ALG_CACULATE_H_
#define _ALG_CACULATE_H_



typedef enum
{
	LEVEL_BIGGER,		//优先级高
	LEVEL_SMALLER,		//优先级低
	LEVEL_SAME,			//优先级相等
	LEVEL_INVALID		//算符不合法
}LEVEL_TYPE;

typedef enum
{
	E_FLAG,             //自然对数标志
	PI_FLAG,            //π标志
	X_FLAG,
	Y_FLAG,
	Z_FLAG,
	ANS_FLAG,
	NO_CONST            //非常数
}CONST_FLAG;

typedef enum
{
	LN_FLAG,
	LOG_FLAG,
	SIN_FLAG,
	COS_FLAG,
	TAN_FLAG,
	ABS_FLAG,
	SINH_FLAG,
	COSH_FLAG,
	TANH_FLAG,
	ASIN_FLAG,
	ACOS_FLAG,
	ATAN_FLAG,
	FUNC_INVALID		//函数不合法
}FUNC_FLAG;


typedef enum
{
	CALC_NORMAL,		//普通运算
	CALC_COMPLEX,		//复数运算（支持加减乘除，不支持函数）
}CALC_STATUS;

uchar alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult);

#endif
