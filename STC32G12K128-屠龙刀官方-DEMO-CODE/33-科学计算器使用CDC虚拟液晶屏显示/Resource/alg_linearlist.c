#include <stdio.h>
#include <stdlib.h>

#include "alg_linearlist.h"

/***********************************************************************************
	功能：判断顺序表是否为空
	形参：*pArray:顺序表指针
	返回：1-为空
		  0-不为空
	详解：用于判断栈是否出完
***********************************************************************************/
LArray_BOOL ll_arry_is_empty(LArray *pArray)
{
	return pArray->nLength == 0 ? LArray_TRUE : LArray_FALSE;
}

/***********************************************************************************
	功能：判断顺序表是否为满
	形参：*pArray:顺序表指针
	返回：1-为满
		  0-不为满
	详解：用于判断栈是否被填满
***********************************************************************************/
LArray_BOOL ll_arry_is_full(LArray *pArray)
{
	return pArray->nLength >= pArray->nTotalSize ? LArray_TRUE : LArray_FALSE;
}

/***********************************************************************************
	功能：判断顺序表当前长度
	形参：*pArray:顺序表指针
	返回：长度:pArray->nLength
	详解：用于判断压入的位置
***********************************************************************************/
uchar ll_arry_get_len(LArray *pArray)
{
	return pArray->nLength;
}


/*******************************栈操作函数*****************************************/

/***********************************************************************************
	功能：获得当前栈顶元素的值
	形参：*pArray:输入字符
		  *pElem :返回元素
	返回：1-成功
		  0-失败
	详解：下标最大的为栈顶元素	
***********************************************************************************/
LArray_BOOL stack_get_top(LArray *pArray, LArrayElem *pElem)
{
	//判断栈是否为空
	if (0 == pArray->nLength)
	{
		return LArray_FALSE;
	}

	//获得当前栈顶的值
	*pElem =  pArray->pElem[pArray->nLength - 1];

	return LArray_TRUE;
}

/***********************************************************************************
	功能：当前栈入栈一个元素
	形参：*pArray:输入字符
		  *pElem :压入元素
	返回：1-成功
		  0-失败
	详解：下标最大的为栈顶元素	
***********************************************************************************/
LArray_BOOL stack_push(LArray *pArray, const LArrayElem elem)
{
	//如果栈满则中中断计算
	if (LArray_TRUE == ll_arry_is_full(pArray))
	{
		return LArray_FALSE;
	}

	//末尾位置插入值
	pArray->pElem[pArray->nLength++] = elem;//对于字符串和结构体此处的值传递操作可能要改变一下形式

	return LArray_TRUE;
}

/***********************************************************************************
	功能：当前栈出栈一个元素	
	形参：*pArray:输入字符
		  *pElem :返回元素
	返回：1-成功
		  0-失败
	详解：下标最大的为栈顶元素	
***********************************************************************************/
LArray_BOOL stack_pop(LArray *pArray, LArrayElem *pElem)
{
	//判断栈是否为空
	if (0 == pArray->nLength)
	{
		return LArray_FALSE;
	}

	//栈顶出栈
	if (NULL != pElem)
	{
		*pElem = pArray->pElem[pArray->nLength-1];
	}
	pArray->nLength--;

	return LArray_TRUE;
}
