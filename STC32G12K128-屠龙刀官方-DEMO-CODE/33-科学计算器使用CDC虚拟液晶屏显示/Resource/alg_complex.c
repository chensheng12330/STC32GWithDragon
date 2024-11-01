#include "alg_linearlist.h"
#include "alg_complex.h"
#include <math.h>

#define ELEM_REAL(flag) eElem##flag->elemValue.dbNum
#define ELEM_IMAG(flag) eElem##flag->dbComplex
#define RESULT_REAL eResult->elemValue.dbNum
#define RESULT_IMAG eResult->dbComplex

void complex_plus(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult) //复数加法，传入两个LArrayElem结构类型的指针
{
	//z=（a+bi)+（c+di）=（a+c）+（b+d）i
	RESULT_REAL = ELEM_REAL(1) + ELEM_REAL(2);
	RESULT_IMAG = ELEM_IMAG(1) + ELEM_IMAG(2);
}

void complex_subtract(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult) //复数减法，传入两个LArrayElem结构类型的指针
{
	//z=（a+bi）-（c+di）=（a-c）+（b-d）i
	RESULT_REAL = ELEM_REAL(1) - ELEM_REAL(2);
	RESULT_IMAG = ELEM_IMAG(1) - ELEM_IMAG(2);
}

void complex_multiply(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult)//复数乘法，传入两个LArrayElem结构类型的指针
{
	//z=（a+bi）*（c+di）= ( ac-bd )+( ad+bc ) i
	RESULT_REAL = (ELEM_REAL(1))*(ELEM_REAL(2)) - (ELEM_IMAG(1)) * (ELEM_IMAG(2));
	RESULT_IMAG = (ELEM_REAL(1))*(ELEM_IMAG(2)) + (ELEM_IMAG(1)) * (ELEM_REAL(2));
}

void complex_divide(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult)//复数除法，传入两个LArrayElem结构类型的指针
{
	//z=(a+jb)/(c+jd)=( (ac+bd)+(bc-ad)i )/( c*c+d*d )
	RESULT_REAL= ( (ELEM_REAL(1))*(ELEM_REAL(2)) + (ELEM_IMAG(1))*(ELEM_IMAG(2)) ) 
	/ ( (ELEM_REAL(2))* (ELEM_REAL(2)) + (ELEM_IMAG(2))* (ELEM_IMAG(2)) );
	RESULT_IMAG= ( (ELEM_IMAG(1))*(ELEM_REAL(2)) - (ELEM_REAL(1))*(ELEM_IMAG(2)) ) 
	/ ( (ELEM_REAL(2))* (ELEM_REAL(2)) + (ELEM_IMAG(2))* (ELEM_IMAG(2)) );
}

uchar complex_power(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult)//复数乘方，传入两个LArrayElem结构类型的指针
{
	uint i;
	LArrayElem eBuf;
	
	if(ELEM_IMAG(2) != 0.0 || ELEM_REAL(2) != (uint)ELEM_REAL(2))
	{
		return LArray_FALSE;		//乘方运算只能为整实数
	}
	if(ELEM_REAL(2) > 0)
	{
		*eResult = *eElem1;									//获取eElem的值
		for(i = 1;i < ELEM_REAL(2);i++)					//循环剩下n-1次
		{
			complex_multiply(eElem1, eResult, &eBuf);
			*eResult = eBuf;
		}
	}
	else
	{
		if(0 == complex_mod(eElem1))
			return LArray_FALSE;	//0次幂底数不能为0
		else
		{
			RESULT_REAL = 1;
			RESULT_IMAG = 0;
		}
	}
	
	return LArray_TRUE;
	
}

double complex_mod(LArrayElem *eElem1)//复数求模，传入一个LArrayElem结构类型的指针，返回模的值
{
	double length;
	length = sqrt ( (ELEM_REAL(1))* (ELEM_REAL(1)) + (ELEM_IMAG(1))* (ELEM_IMAG(1)) );
	return length;
}