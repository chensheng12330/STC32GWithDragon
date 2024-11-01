#ifndef __FUN_H
#define __FUN_H

#include "middle.h"
/************************************************************************/
/* 线性表结构定义                                                       */
/************************************************************************/
#define LArray_FALSE 0
#define LArray_TRUE  1

extern double x,y,z;

//栈的大小定义
#define STACK_SIZE 30
extern double g_ans;
typedef unsigned char LArray_BOOL;

extern uchar g_chCalcStatus;

/*元素类型*/
typedef enum
{
	NUMBER,						//数据元素为操作数
	OPERATOR,					//数据元素为算符
	CONST_NUM,                  //数据为常数符号
	FUNCTION,                   //为函数
	INVALID_TYPE				//数据元素无效
}TYPE_FLAG;

/*算符或者数字元素*/
typedef union
{
	double	dbNum;				//保存操作数类型
	char	cOperator;			//保存算符类型
}TYPE;

/*线性表元素*/
typedef struct
{
	TYPE_FLAG	elemType;		//数据元素类型
	TYPE		elemValue;		//数据元素实部值或算符
	double		dbComplex;		//存储元素虚部值
}LArrayElem;

/*线性表*/
typedef struct
{
	LArrayElem	*pElem;			//数据元素存储区域
	uchar		nLength;		//当前数据元素长度
	uchar		nTotalSize;		//当前线性表长度
}LArray;



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

#define OPERATOR_COMPLEX(operator,eElemX,eElemY,eResult) complex_##operator(eElemX, eElemY, eResult)

double complex_mod(LArrayElem *eElem1);

/************************************************************************/
/* 基础线性表函数声明                                                           */
/************************************************************************/
LArray* ll_arry_create(const uchar nSelect);
void ll_arry_clr(LArray *pArray );

LArray_BOOL ll_arry_is_empty(LArray *pArray);
LArray_BOOL ll_arry_is_full(LArray *pArray);
uchar ll_arry_get_len(LArray *pArray);


/************************************************************************/
/* 栈操作函数声明															*/
/************************************************************************/
LArray_BOOL stack_get_top(LArray *pArray, LArrayElem *pElem);
LArray_BOOL stack_push(LArray *pArray, const LArrayElem elem);
LArray_BOOL stack_pop(LArray *pArray, LArrayElem *pElem);

void complex_plus(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult); 	//复数加法，传入两个LArrayElem结构类型的指针
void complex_subtract(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult); //复数减法，传入两个LArrayElem结构类型的指针
void complex_multiply(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//复数乘法，传入两个LArrayElem结构类型的指针
void complex_divide(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//复数除法，传入两个LArrayElem结构类型的指针
uchar complex_power(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//复数乘方，传入两个LArrayElem结构类型的指针

double alg_atof(char *s);
#endif


