#ifndef _ALG_LINEARLIST_H_
#define _ALG_LINEARLIST_H_

#define uchar unsigned char
#define uint unsigned int

/************************************************************************/
/* 线性表结构定义                                                       */
/************************************************************************/
#define LArray_FALSE 0
#define LArray_TRUE  1

//栈的大小定义
#define STACK_SIZE 30

typedef unsigned char LArray_BOOL;

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

#endif
