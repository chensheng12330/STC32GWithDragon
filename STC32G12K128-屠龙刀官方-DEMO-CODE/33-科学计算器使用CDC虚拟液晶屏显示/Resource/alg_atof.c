#include "alg_atof.h"

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