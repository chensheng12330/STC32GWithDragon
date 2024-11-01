#include "keyboard.h"
#include "string.h"

//code uchar sinh[] ="sinh(";
//code uchar cosh[] ="cosh(";
//code uchar tanh[] ="tanh(";

u8 String_length(char* str)
{
    u8 i;
    for(i=0;i<30;i++)
    {
        if( str[i]=='\0' )
            return i;
    }
    return 0xff;
}
//×Ö·û´®²åÈëº¯”µ
u8 String_Insert(char* str,u8 leng,u8 addr,u8 keynuml)
{
    u8 i=0;
    u8 ins_lth ;        //²åÈëµÄ×Ö·û³¤¶È
    char str1[7];
    memset(str1,'\0',7);
    if(( keynuml>='*' )&&( keynuml<='9' ) )
    {
        str1[0] = (char)keynuml;
    }
    else 
    {
        switch( keynuml )
        {
            case 0x83:str1[0] ='e';break;
            case 0x06:str1[0] ='(';break;
            case 0x07:str1[0] =')';break;
            case 0x8B:str1[0] ='i';break;
            case 0x8e:str1[0] ='!';break;
            case (0x80+'1'):str1[0] ='x';break;
            case (0x80+'2'):str1[0] ='y';break;
            case (0x80+'3'):str1[0] ='z';break;
            case 0X0D:str1[0] ='^';break;
            case 18:strcpy(str1,"sin(");break;
            //case 18:str1[0] ='s';str1[1] ='i';break;//str1[1] ='i';str1[2] ='n';str1[3] ='(';
            case 19:strcpy(str1,"cos(");break;
            case 20:strcpy(str1,"tan(");break; 
            case 12:strcpy(str1,"^2");break; 
            case 15:strcpy(str1,"e^(");break; 
            case 0x8f:strcpy(str1,"ln(");break;
            case 23:strcpy(str1,"Ans");break;
            case 11:strcpy(str1,"abs(");break;
            case 16:strcpy(str1,"10^(");break;
            case 0x90:strcpy(str1,"log(");break;
            case (0x80+18):strcpy(str1,"asin(");break;
            case (0x80+19):strcpy(str1,"acos(");break;
            case (0x80+20):strcpy(str1,"atan(");break; 
            case (0x80+12):strcpy(str1,"^(1/2)");break;
            case (0x80+13):strcpy(str1,"^(1/");break;
            case (14):strcpy(str1,"^(-1)");break; 
            
            case (120):strcpy(str1,"sinh(");break;  
            case (121):strcpy(str1,"cosh(");break;  
            case (122):strcpy(str1,"tanh(");break;            
            
            default:break;
        }            
    }
    ins_lth = String_length(str1);
    
    if(( ins_lth<7 )&&( ins_lth>0 ))
    {
        for( i=leng;i>(addr);i-- )
        {
            str[i] = str[i-ins_lth];
        }
        
        for( i=0;i<ins_lth;i++ )
        {
            str[addr+i] = str1[i];
        }
        return ins_lth;
    }
    else
    {
       ins_lth = 0; 
       return 0xff;  
    }  
}


//×Ö·û´®„h³ýº¯”µ
void String_Delete(char *str,u8 length,u8 addr)
{
    u8 i;
    if( addr>0 )
    {
        for( i=addr;i<(length-1);i++ )
        {
            str[i-1] = str[i];
        }  
        str[length] = '\0'; 
    }    
}
    
//×Ö·û´®Çå¿Õ
void String_Clear(char *str,u8 length)
{
    memset(str,0,length);
}


