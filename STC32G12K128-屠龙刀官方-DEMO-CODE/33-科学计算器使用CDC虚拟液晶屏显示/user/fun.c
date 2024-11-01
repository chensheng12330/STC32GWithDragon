#include "fun.h"
#include "math.h"
#include <stdio.h>

#define E 2.718282
#define PI 3.141593
#define PI_FONT 'p'                 //PI的12864编码
#define DEGREE_FONT 'd'                //°的12864编码

#define check_if(function,value) if(0 == strcmp(strTemp,#function)) return value
#define CASE break;case
#define STACK_PUSH(pTarget,eElem) if(LArray_FALSE == stack_push(pTarget,eElem)){bResult = LArray_FALSE; break;}

//extern uchar g_chCalcStatus;
//extern double x,y,z;
//extern double g_ans;
uchar g_chCalcStatus;
double x,y,z;
double g_ans;

uchar alg_const_handle(char c);

/***********************************************************************************
	功能：比较操作符的优先级顺序
	形参：operator1:操作符1 operator2:操作符 2
	返回：操作符1优先级大于操作符2	--LEVEL_BIGGER
		  操作符1优先级小于操作符2	--LEVEL_SMALLER
		  操作符1优先级等于操作符2	--LEVEL_SAME
		  操作符1操作符2对比不合法	--LEVEL_INVALID
	详解：本函数的功能是比较两个操作符的优先级，实现例如先乘除后加减的功能
		  在转后序表达式的过程中需要比较当前操作符和栈顶操作符优先级顺序
***********************************************************************************/
LEVEL_TYPE alg_compare_level(char operator1, char operator2)
{
	char operator1_level, operator2_level;

	//判断两个操作符在两个优先级表中的位置
	switch(operator1)
	{
		case '+' :
		case '-' : operator1_level = 1;	break;

		case '*' :
		case '/' : operator1_level = 2;	break;

		case '^' :		
		case 'i' :									//虚数标识
		case '!' :
        case DEGREE_FONT : operator1_level = 3;	break;

        case 'f' : operator1_level = 4; break;        //函数标识

		case '(' :
		case ')' : operator1_level = 5; break;
        default  : return LEVEL_INVALID;
	}
	switch(operator2)
	{
		case '+' :
		case '-' : operator2_level = 1;	break;

		case '*' :
		case '/' : operator2_level = 2;	break;

		case '^' :
		case 'i' :									//虚数标识
		case '!' :
        case DEGREE_FONT : operator2_level = 3;	break;

        case 'f' : operator2_level = 4; break;        //函数标识

		case '(' :
		case ')' : operator2_level = 5; break;
        default  : return LEVEL_INVALID;
	}

	//判断两个操作符的优先级关系
	if((operator1_level - operator2_level) > 0)
	{
		return LEVEL_BIGGER;
	}
	else if((operator1_level - operator2_level) < 0)
	{
		return LEVEL_SMALLER;
	}
	else
	{
		return LEVEL_SAME;
	}
}

/***********************************************************************************
	功能：判断字符的类型
	形参：c:输入字符
	返回：算符	--OPERATOR
		  数字	--NUMBER
		  非法	--INVALID_TYPE
	详解：判断字符串当前的值为数字、符号、还是非法输入
***********************************************************************************/
TYPE_FLAG alg_judge_type(char c)
{	
	//普通计算模式
	if(g_chCalcStatus == CALC_NORMAL)
	{
		if (c == '+' ||
			c == '-' ||
			c == '*' ||
			c == '/' ||
			c == '^' ||
			c == '(' ||
			c == ')' ||
			c == '!' ||
	        c == DEGREE_FONT	       //°符号
	        )
		{
			return OPERATOR;
		}
		else if((c>='0' && c<='9') || c=='.')
		{
			return NUMBER;
		}
		else if(alg_const_handle(c) != NO_CONST)    //若为常数
	    {
	        return CONST_NUM;
	    }
		else if(c == 'l' || c == 's' || c == 'c' || c == 't' || c == 'a')       //首字母是s,c,t,l，则判定为函数
		{
		    return FUNCTION;
		}
		else
		{
			return INVALID_TYPE;
		}
        
	}
	
	//复数计算模式
	else if(g_chCalcStatus == CALC_COMPLEX)
	{
		if (c == '+' ||
			c == '-' ||
			c == '*' ||
			c == '/' ||
			c == '^' ||
			c == '(' ||
			c == ')' ||
			c == 'i'					//虚数标识
	        )
		{
			return OPERATOR;
		}
		else if((c>='0' && c<='9') || c=='.')
		{
			return NUMBER;
		}
		else if(alg_const_handle(c) != NO_CONST)    //若为常数
	    {
	        return CONST_NUM;
	    }
		else
		{
			return INVALID_TYPE;
		}
	}

	return INVALID_TYPE;
}


/***********************************************************************************
	功能：常数处理
	形参：c:输入字符
	返回：常数的类型
	详解：常数和未知数的判断，如果判断结果为常数或未知数，则在转后序表达式过程中替换
***********************************************************************************/
uchar alg_const_handle(char c)
{
    switch(c)
    {
        case 'e'     : return E_FLAG;
        case PI_FONT : return PI_FLAG;
		case 'x'	 : return X_FLAG;
		case 'y'	 : return Y_FLAG;
		case 'z'	 : return Z_FLAG;
		case 'A'	 : return ANS_FLAG;
    }

    return NO_CONST;
}


/***********************************************************************************
	功能：函数处理
	形参：c:输入字符
	返回：函数的类型
	详解：识别函数名，例如sin，cos等，并在转后续表达式中，按照规则对函数名进行转化
***********************************************************************************/
uchar alg_func_handle(char *c)
{
    char strTemp[5] = {0,0,0,0,0};      //声明并初始化一个用于存储函数名的临时数组

    strTemp[0] = c[0];
    strTemp[1] = c[1];                  //抽取前两个字符
    check_if(ln,LN_FLAG);                //检查是否为双字符的函数

    strTemp[2] = c[2];                  //抽取前三个字符
    if('h' != c[3])                     //防止双区三角函数干扰
    {
        check_if(log,LOG_FLAG);             //检查是否为三字符的函数
        check_if(sin,SIN_FLAG);
        check_if(cos,COS_FLAG);
        check_if(tan,TAN_FLAG);
		check_if(abs,ABS_FLAG);
    }

    strTemp[3] = c[3];                  //抽取前四个字符
    //检查是否为四个字符的函数
    check_if(sinh,SINH_FLAG);
    check_if(cosh,COSH_FLAG);
    check_if(tanh,TANH_FLAG);
	check_if(asin,ASIN_FLAG);
    check_if(acos,ACOS_FLAG);
    check_if(atan,ATAN_FLAG);

    return FUNC_INVALID;
}


/***********************************************************************************
	功能：根据输入数字和操作数计算结果
	形参：eElem1,eElem2	两个LArrayElem结构
		  nOperator		算符
		  eResult		保存结果的LArrayElem指针
	返回：计算成功	--1
		  计算失败	--0
	详解：用于计算后序表达式，弹出操作符和数字，计算后压回后续表达式
***********************************************************************************/
LArray_BOOL alg_calc(LArrayElem *eElem1, char nOperator, LArrayElem *eElem2, LArrayElem *eResult)
{
	uchar i;
	if(g_chCalcStatus == CALC_NORMAL)
	{
		switch (nOperator)
		{
	        case '+' :
	            eResult->elemValue.dbNum = eElem1->elemValue.dbNum + eElem2->elemValue.dbNum;
	            break;
	
	        case '-' :
	            eResult->elemValue.dbNum = eElem1->elemValue.dbNum - eElem2->elemValue.dbNum;
	            break;
	
	        case '*' :
	            eResult->elemValue.dbNum = eElem1->elemValue.dbNum * eElem2->elemValue.dbNum;
	            break;
	
	        case '/' :
	            if (0 == eElem2->elemValue.dbNum)
	            {
	                return LArray_FALSE;
	            }
	
	            eResult->elemValue.dbNum = eElem1->elemValue.dbNum / eElem2->elemValue.dbNum;
	            break;
				
	        case '^' :
	            if (0 > eElem1->elemValue.dbNum && eElem2->elemValue.dbNum != (uchar)eElem2->elemValue.dbNum)
	            {
	                return LArray_FALSE;
	            }
	            eResult->elemValue.dbNum = pow(eElem1->elemValue.dbNum,eElem2->elemValue.dbNum);
	            break;
	
	        case DEGREE_FONT :
	            eResult->elemValue.dbNum = eElem1->elemValue.dbNum * PI / 180;
	            break;
			
			case '!' :
				if (((int)eElem1->elemValue.dbNum != eElem1->elemValue.dbNum) 
					|| eElem1->elemValue.dbNum > 60 || eElem1->elemValue.dbNum < 0 )	//必须是正整数
	            {
	                return LArray_FALSE;
	            }
				if(eElem1->elemValue.dbNum == 1 ||eElem1->elemValue.dbNum == 0)
				{
					eResult->elemValue.dbNum = 1;
					break;
				}
				eResult->elemValue.dbNum = 1;
				for(i=1;i<eElem1->elemValue.dbNum+1;i++)
				{
	            	eResult->elemValue.dbNum = eResult->elemValue.dbNum  * i;
				}
	            break;
				
	        case 'f' :
	            switch((uchar)eElem1->elemValue.dbNum)
	            {
	                case LN_FLAG    :   eResult->elemValue.dbNum = log(eElem2->elemValue.dbNum);
	                CASE LOG_FLAG   :   eResult->elemValue.dbNum = log10(eElem2->elemValue.dbNum);
	                CASE SIN_FLAG   :   eResult->elemValue.dbNum = sin(eElem2->elemValue.dbNum);
	                CASE COS_FLAG   :   eResult->elemValue.dbNum = cos(eElem2->elemValue.dbNum);
	                CASE TAN_FLAG   :   eResult->elemValue.dbNum = tan(eElem2->elemValue.dbNum);
	                CASE SINH_FLAG  :   eResult->elemValue.dbNum = sinh(eElem2->elemValue.dbNum);
	                CASE COSH_FLAG  :   eResult->elemValue.dbNum = cosh(eElem2->elemValue.dbNum);
	                CASE TANH_FLAG  :   eResult->elemValue.dbNum = tanh(eElem2->elemValue.dbNum);
					CASE ASIN_FLAG  :	eResult->elemValue.dbNum = asin(eElem2->elemValue.dbNum);
            		CASE ACOS_FLAG  :	eResult->elemValue.dbNum = acos(eElem2->elemValue.dbNum);
            		CASE ATAN_FLAG  :	eResult->elemValue.dbNum = atan(eElem2->elemValue.dbNum);
					CASE ABS_FLAG	:	eResult->elemValue.dbNum = fabs(eElem2->elemValue.dbNum);
	
	                break;
	            }
	            break;
		}
	}
	
	else if(g_chCalcStatus == CALC_COMPLEX)
	{
		switch (nOperator)
		{
			case '+' :
	            OPERATOR_COMPLEX(plus,eElem1,eElem2,eResult);
	            break;
	
	        case '-' :
	            OPERATOR_COMPLEX(subtract,eElem1,eElem2,eResult);
	            break;
	
	        case '*' :
	            OPERATOR_COMPLEX(multiply,eElem1,eElem2,eResult);
	            break;
	
	        case '/' :
	            if (0 == complex_mod(eElem2))	//被除数的模不能为0
	            {
	                return LArray_FALSE;
	            }
				OPERATOR_COMPLEX(divide,eElem1,eElem2,eResult);
				break;
			
			case '^' :
				if(LArray_FALSE == complex_power(eElem1,eElem2,eResult))
				{
					return LArray_FALSE;
				}
				break;
			
			case 'i' :					//虚数声明
				eResult->elemValue.dbNum = -(eElem1->dbComplex);
				eResult->dbComplex = eElem1->elemValue.dbNum;		//交换虚实两部，且虚部反号
				break;
		}
	}
	
	return LArray_TRUE;
}


/***********************************************************************************
    功能：移动表达式的当前指针到下一个元素
    形参：无
    返回：无
    详解：本函数用来在转后序表达式中，判断前一个数字的位置，并移动指针到下一个需要判断的位置
***********************************************************************************/
void alg_pmove(char **ppCur)
{
	if(OPERATOR == alg_judge_type((*ppCur)[0]))
	{
		*ppCur +=1;
	}
	else if(CONST_NUM == alg_judge_type((*ppCur)[0]))
	{
		if(ANS_FLAG == alg_const_handle((*ppCur)[0]))
			*ppCur +=3;			//Ans是三字
		else 
			*ppCur +=1;
	}
	else if(FUNCTION == alg_judge_type((*ppCur)[0]))
    {
        switch(alg_func_handle(*ppCur))
        {
            case LN_FLAG  : *ppCur +=2; break;    //二字函数

            case LOG_FLAG :
            case SIN_FLAG :
            case COS_FLAG :
            case TAN_FLAG :
			case ABS_FLAG : *ppCur +=3; break;    //三字函数

            case SINH_FLAG:
            case COSH_FLAG:
            case TANH_FLAG: 
			case ASIN_FLAG :
            case ACOS_FLAG :
            case ATAN_FLAG : *ppCur +=4; break;    //四字函数
        }
    }
	else
	{
		while(NUMBER == alg_judge_type((*ppCur)[0]) && '\0' != (*ppCur)[0])
		{
			*ppCur +=1;
		}
	}
}

/***********************************************************************************
    功能：将中序表达式转换为后序表达式
    形参：pExpression:字符串型表达式 pPostArray:保存后序表达式的线性表
    返回：1:转换成功 2:转换失败
    详解：转后续表达式的函数，转换说明如下：
	（1）分配一个栈和一个线性表，stack用于临时存储运算符，此算符在栈内用于临时存储运算符，此算符在栈内遵循先入后出的原则。线性表ll用于存储转换完成后的后序表达式。
	（2）从中序表达式算术表达式字符串左端开始逐个读取字符c：
			A.若c是数字，则读完整个数字，转换成浮点数后插入ll末端
			B.若c是符号：
				①若c是'('，则直接压入stack；
				②若c是')'，则将距离stack顶最近的一个'('之前的运算符，逐个出栈，插入线性表ll末端，此时'()'被抛弃；
				③若c是运算符:x
					a.若x是'-'：
					判断是否是字符串第一个值，是，则将后面的数字反号，同时丢弃这个负号
					判断栈顶是不是'('，是，则将后面数字反号，同时丢弃这个负号
				④若x是常量标识符(e,π,...)转换成数字插入ll末端
				⑤若是函数，则抽出整个函数名后匹配。匹配结果以数字形式插入线性表，再插入一个'f'做函数标识
				⑥若是'°'度数算符，则压入0后压入'd'
				⑦.若栈顶是'('则直接压栈
				⑧.若栈顶不是'('则比较优先级，x优先级高就压入，否则将stack顶操作符弹出插入线性表末端，直到x优先级更高，再压入
	运算符优先级表：
	    括号>函数>乘方>乘除>加减；注意函数和'°'是单目运算的。负号在转后序表达式时就处理了，度数、函数需要在求值过程再处理。所以在
	后序表达式求值的时候需要考虑。
***********************************************************************************/
LArray_BOOL alg_mid_to_post(char *pExpression, LArray *pPostArray)
{
    uchar minus_flag = 1;                 //第一个字符标志
    uchar function_temp;                   //当前函数状态指针
	char *pCur;
	LArrayElem eTop;
	LArray_BOOL bResult;
	LArray laStackOperator;
	
	//创建算符栈
	LArray						*pStackOperator = &laStackOperator;
	LArrayElem					pElem_StackOperator[STACK_SIZE/2];
	//初始化算符栈
	pStackOperator->pElem 		= pElem_StackOperator;
	pStackOperator->nLength		= 0;
	pStackOperator->nTotalSize	= STACK_SIZE/2;
	
	pCur = pExpression;
	eTop.elemValue.dbNum = 0;
	eTop.dbComplex 		= 0;
	
	bResult	= LArray_TRUE;
	
	//遍历算术表达式字符串
	while(pCur[0] != '\0')
	{
        if (INVALID_TYPE == alg_judge_type(pCur[0]))//输入不合法
		{
			bResult	= LArray_FALSE;
			break;
		}

        else if(CONST_NUM == alg_judge_type(pCur[0]))//输入为常数
        {
            eTop.elemType = NUMBER;
            switch(alg_const_handle(pCur[0]))
            {
                case E_FLAG  :  eTop.elemValue.dbNum = E;   break;
                case PI_FLAG :  eTop.elemValue.dbNum = PI;  break;
				case X_FLAG  :  eTop.elemValue.dbNum = x;  break;
				case Y_FLAG  :  eTop.elemValue.dbNum = y;  break;
				case Z_FLAG  :  eTop.elemValue.dbNum = z;  break;
				case ANS_FLAG:	eTop.elemValue.dbNum = g_ans;  break;
            }
			STACK_PUSH(pPostArray, eTop);
        }

		else if(NUMBER == alg_judge_type(pCur[0]))//操作数直接输入到线性表
		{
			eTop.elemType			= NUMBER;
			eTop.elemValue.dbNum	= alg_atof(pCur);//strtod(pCur,(char **)NULL);
			STACK_PUSH(pPostArray, eTop);
		}

		else//输入算符或函数
		{
		    if(minus_flag == 1)  //对于栈为空且或前一个算符为‘(’,说明是正负号
            {
                if(pCur[0] == '-' || pCur[0] == '+')
                {
                    eTop.elemType			= NUMBER;       //压入0
                    eTop.elemValue.dbNum	= 0;
                    STACK_PUSH(pPostArray, eTop);
                }
            }
            if(pCur[0] == DEGREE_FONT || pCur[0] == '!') //如果算符是°或!，则需要在处理前加0
            {
                eTop.elemType			= NUMBER;       //压入0
                eTop.elemValue.dbNum	= 0;
                STACK_PUSH(pPostArray, eTop);
            }			
			if(pCur[0] == 'i')
			{
				if(minus_flag == 1 || OPERATOR == alg_judge_type(pCur[-1]))			//如果是满足负号条件，则一定需要补1，否则检查上一个是不是符号
				{																	//此处用了奇技淫巧，要是是字符串开始，则会满足负号条件，数组越界访问不会执行。如果不是字符串开始，则[-1]不会导致越界
					eTop.elemType			= NUMBER;       //压入1
					eTop.elemValue.dbNum	= 1;			//用于解决 a+i的问题
					STACK_PUSH(pPostArray, eTop);
				}
				
                eTop.elemType			= NUMBER;       //压入0
                eTop.elemValue.dbNum	= 0;
                STACK_PUSH(pPostArray, eTop);
			}
			
            else if(FUNCTION == alg_judge_type(pCur[0]))         //函数处理
            {
                eTop.elemType			= NUMBER;
                eTop.elemValue.dbNum    = alg_func_handle(pCur);       //压入函数类型
                if(FUNC_INVALID == alg_func_handle(pCur))               //函数错误则表达式错误
                {
                    bResult	= LArray_FALSE;
                    break;
                }

                STACK_PUSH(pPostArray, eTop);
                function_temp = pCur[0];
                pCur[0] = 'f';              //添加函数标识

            }

			if(pCur[0] == '(')//输入'('
			{
				eTop.elemType				= OPERATOR;
				eTop.elemValue.cOperator	= pCur[0];
				STACK_PUSH(pStackOperator, eTop);
				minus_flag = 1;                 //允许正负号
			}
			else if (pCur[0] == ')')//输入')'
			{
				//一直出栈压入线性表直到'('
				while (LArray_TRUE == stack_get_top(pStackOperator, &eTop) &&
					   eTop.elemValue.cOperator != '(')
				{
					stack_pop(pStackOperator, &eTop);
                    STACK_PUSH(pPostArray, eTop);
				}

				if (LArray_TRUE == stack_get_top(pStackOperator, &eTop) &&
					eTop.elemValue.cOperator == '(')
				{
					stack_pop(pStackOperator, NULL);//直接弹出'('
				}
				else
				{
					bResult	= LArray_FALSE;
					break;
				}
			}
			else if (LArray_TRUE == stack_get_top(pStackOperator, &eTop) &&
					 eTop.elemValue.cOperator == '(')//栈顶元素为'('则当前的算符压入算符栈
			{
				eTop.elemType				= OPERATOR;
				eTop.elemValue.cOperator	= pCur[0];
				STACK_PUSH(pStackOperator,eTop);
			}
			else if (LArray_FALSE == stack_get_top(pStackOperator, &eTop) ||
					 LEVEL_SMALLER == alg_compare_level(eTop.elemValue.cOperator, pCur[0]))
			{
				//栈为空或栈顶算符的优先级小于当前输入的算符(此时当前算符不等于())，则将当前的算符压入算符栈
				eTop.elemType				= OPERATOR;
				eTop.elemValue.cOperator	= pCur[0];
				STACK_PUSH(pStackOperator, eTop);
			}
			else
			{

				//对于栈不为空且栈顶算符的优先级大于等于当前输入的算符(此时当前算符不等于()),
				//一直出栈压入线性表直到不满足这个条件

				while(LArray_TRUE  == stack_get_top(pStackOperator, &eTop) &&
					  eTop.elemValue.cOperator != '(' &&
					  LEVEL_SMALLER != alg_compare_level(eTop.elemValue.cOperator, pCur[0]))
				{
					stack_pop(pStackOperator, &eTop);
                    STACK_PUSH(pPostArray, eTop);
				}

				//当前算符入栈
				eTop.elemType				= OPERATOR;
				eTop.elemValue.cOperator	= pCur[0];
				STACK_PUSH(pStackOperator, eTop);
			}
		}
        if(pCur[0] != '(')
            minus_flag = 0;                     //清空第一个字符标志
        if(pCur[0] == 'f')
            pCur[0] = function_temp;            //清除函数标识
		alg_pmove(&pCur);
	}

	//将剩下的算符栈中全部出栈加入到线性表中
	if (bResult	== LArray_TRUE)
	{
		while (LArray_FALSE == ll_arry_is_empty(pStackOperator))
		{
			stack_pop(pStackOperator, &eTop);
            stack_push(pPostArray, eTop);
		}
	}

	return bResult;
}


/***********************************************************************************
	功能：将表达式计算出结果
	形参：pExpression:字符串型表达式 pRealResult:实数部分 pImaginaryResult：虚数部分
	返回：计算成功	--1
		  计算失败	--0
	详解：可以被其他函数调用，输入一个字符串，例如"1+1"可直接从result中读取结果，结果分为实部和虚部
		  结果，如果只是实数运算，则可以虚部直接传NULL，但是需要调整g_chCalcStatus的值来设置函数工作
		  模式
***********************************************************************************/
LArray_BOOL alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult)
{
	LArrayElem eNum1, eNum2, eResult;
	LArray_BOOL	bResult;
	uchar i;
	LArray laPostArray, laStackNum;

	//创建线性表
	LArray					  *pPostArray = &laPostArray;
	LArrayElem				  pElem_PostArry[STACK_SIZE];
	//创建算符操作数栈
	LArray					  *pStackNum = &laStackNum;
	LArrayElem				  pElem_StackNum[(STACK_SIZE-6)/4+3];		//因为只有5个运算等级，因此最多出现栈上有5个数，


	//线性表初始化
	pPostArray->pElem			= pElem_PostArry;
	pPostArray->nLength			= 0;
	pPostArray->nTotalSize		= STACK_SIZE;	
	//初始化算符操作数栈
	pStackNum->pElem 			= pElem_StackNum;
	pStackNum->nLength			= 0;
	pStackNum->nTotalSize		= (STACK_SIZE-6)/4+3;

	//元素初始化
	eResult.elemValue.dbNum = 0;
	eResult.dbComplex 		= 0;
	
	bResult	= LArray_TRUE;
	
	//得到后序表达式
	if(LArray_TRUE == alg_mid_to_post(pExpression, pPostArray))
	{
		//遍历线性表
		for (i = 0; i < pPostArray->nLength; i++)
		{
			if (pPostArray->pElem[i].elemType == NUMBER)
			{
				stack_push(pStackNum, pPostArray->pElem[i]);
			}
			else
			{
				if (LArray_FALSE == stack_pop(pStackNum, &eNum2) ||
					LArray_FALSE == stack_pop(pStackNum, &eNum1))
				{
					//出栈错误(输入不合法时)
					bResult	= LArray_FALSE;
					break;
				}
				else
				{
					if (LArray_FALSE == alg_calc(&eNum1,
											     pPostArray->pElem[i].elemValue.cOperator,
											     &eNum2,
											     &eResult))
					{
						//计算错误(输入不合法时)
						bResult	= LArray_FALSE;
						break;
					}
					else
					{
						eResult.elemType = NUMBER;
						stack_push(pStackNum, eResult);
					}
				}
			}
		}
	}
	else
	{
		bResult	= LArray_FALSE;
	}

	if (bResult	== LArray_TRUE)
	{
		//如果操作数栈长度不等于1则运算表达式不合法
		if (ll_arry_get_len(pStackNum) != 1)
		{
			bResult	= LArray_FALSE;
		}
		else
		{
			stack_get_top(pStackNum, &eResult);
			*pRealResult = eResult.elemValue.dbNum;
			*pImaginaryResult = eResult.dbComplex;
		}
	}
	
	return bResult;
}


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

char is_digit(char ch)
{
    if(ch>='0'&&ch<='9')
        return 1;
    else
        return 0;
}
uchar is_space(char ch)
{
    if(ch==' ')
        return 1;
    else
        return 0;
}
double alg_atof(char *s)
{
    double power,value;
    int i,sign;
    for(i=0;is_space(s[i]);i++);//除去字符串前的空格
    sign=(s[i]=='-')?-1:1;
    if(s[i]=='-'||s[i]=='+')//要是有符号位就前进一位
        i++;
    for(value=0.0;is_digit(s[i]);i++)//计算小数点钱的数字
        value=value*10.0+(s[i]-'0');
    if(s[i]=='.')
        i++;
    for(power=1.0;is_digit(s[i]);i++)//计算小数点后的数字
    {
        value=value*10.0+(s[i]-'0');
        power*=10.0;
    }
    return sign*value/power;
}

