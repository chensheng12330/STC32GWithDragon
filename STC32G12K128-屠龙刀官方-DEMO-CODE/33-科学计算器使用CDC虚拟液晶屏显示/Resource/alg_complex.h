#ifndef _ALG_COMPLEX_H_
#define _ALG_COMPLEX_H_

#define uchar unsigned char
#define uint unsigned int

#define OPERATOR_COMPLEX(operator,eElemX,eElemY,eResult) complex_##operator(eElemX, eElemY, eResult)

double complex_mod(LArrayElem *eElem1);



void complex_plus(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult); 	//复数加法，传入两个LArrayElem结构类型的指针
void complex_subtract(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult); //复数减法，传入两个LArrayElem结构类型的指针
void complex_multiply(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//复数乘法，传入两个LArrayElem结构类型的指针
void complex_divide(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//复数除法，传入两个LArrayElem结构类型的指针
uchar complex_power(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//复数乘方，传入两个LArrayElem结构类型的指针

#endif