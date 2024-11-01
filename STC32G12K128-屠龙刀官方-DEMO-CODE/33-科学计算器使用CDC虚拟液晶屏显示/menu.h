#ifndef __MENU_H
#define __MENU_H

#include "middle.h"


#define Null ((void *)0)
	
struct MenuItem     
{  
    char MenuCount;                 //结构体数组的元素个数 
    unsigned char DisplayString[20];//当前LCD显示的信息 
    void (*Subs)();                 //执行的函数的指针. 
    struct MenuItem *Childrenms;    //指向子节点的指针 
    struct MenuItem *Parentms;      //指向父节点的指针 
}; 

struct MenuIndex
{
	char Menu_Depth;					//当前菜单深度
	char Menu_Start[10];				//当前第一个显示的是编号几
    char Menu_Pilot[10];				//菜单导航，当前选中的为第几项
	char Menu_KeyFunc;					//0:菜单切换    1:菜单里的参数设置
};

extern struct MenuItem *P_menu;     //当前菜单节点的指针
extern struct MenuIndex P_Index;	 //当前菜单的导航信息
extern void  (*function)(u8 num);			//系统的指针函数

void Menu_Init(void);
void Menu_Display(void);
void Menu_Deal(char* key);

void Menu_MainFunc1(void);
void Menu_MainFunc2(void);
void Menu_MainFunc31(void);
void Menu_MainFunc32(void);
void Menu_MainFunc33(void);
void Menu_MainFunc4(void);
void Menu_MainFunc5(void);
void Menu_MainFunc6(void);
void Menu_MainFunc61(void);
void Menu_MainFunc7(void);
void Menu_MainFunc8(void);

#endif


