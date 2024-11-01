#include "menu.h"
#include "nor_cal.h"
#include "fun.h"
#include "matrix.h"
#include "funcpic.h"
#include "Solve.h"
#include "Sensus.h"
#include "calculus.h"
#include "set.h"

void  (*function)(u8 num);			//系统的指针函数
struct MenuItem *P_menu;     //当前菜单节点的指针
struct MenuIndex P_Index=	 //当前菜单的导航信息
{
	0,
	{0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
	0			//默认按键为为菜单切换功能
};

void Nop(u8 num) 
{}

struct MenuItem Menu_Main[9];       //主菜单
struct MenuItem Menu_Other[9];      //其他菜单
struct MenuItem Menu_Matrix[10];     //矩阵菜单选项 
struct MenuItem Menu_Matrix1[4];    //矩阵菜单1
struct MenuItem Menu_Matrix2_4[5];  //矩阵菜单2-4
struct MenuItem Menu_Matrix5_9[4];  //矩阵菜单5-9 
struct MenuItem Menu_FuncImg[7];    //函数图像界面 
struct MenuItem Menu_ODEFunc[6];    //常微分方程界面    
struct MenuItem Menu_Sensus[4];
struct MenuItem Menu_Sensus1[3];
	
	
//结构体实现 
struct MenuItem Menu_Main[9]= 
{ 
    {8,"1.普通计算",Menu_MainFunc1,Null,Null}, 
    {8,"2.复数\xfd计算",Menu_MainFunc2,Null,Null}, 
    {8,"3.矩阵运算",Nop,Menu_Matrix,Null}, 
    {8,"4.函数\xfd图像",Nop,Menu_FuncImg,Null}, 
    {8,"5.解方程",Menu_MainFunc5,Null,Null}, 
    {8,"6.统计",Nop,Menu_Sensus,Null}, 
    {8,"7.一阶常微分方程",Nop,Menu_ODEFunc,Null}, 
    {8,"8.参数\xfd设置",Nop,Menu_Other,Null},  //时间显示
//    {9,"9.预留",Nop,Null,Null},  //功能设置
};

struct MenuItem Menu_Matrix[10]= 
{ 
    {10,"1.计算行列式",Nop,Menu_Matrix1,Menu_Main}, 
    {10,"2.计算矩阵加法",Nop,Menu_Matrix2_4,Menu_Main}, 
    {10,"3.计算矩阵减法",Nop,Menu_Matrix2_4,Menu_Main}, 
    {10,"4.计算矩阵乘法",Nop,Menu_Matrix2_4,Menu_Main}, 
    {10,"5.计算方阵高次幂",Nop,Menu_Matrix5_9,Menu_Main}, 
    {10,"6.求矩阵的逆",Nop,Menu_Matrix5_9,Menu_Main}, 
    {10,"7.求转置矩阵",Nop,Menu_Matrix5_9,Menu_Main}, 
    {10,"8.求矩阵的秩",Nop,Menu_Matrix5_9,Menu_Main},  
    {10,"9.计算矩阵的数\xfd乘",Nop,Menu_Matrix5_9,Menu_Main},  
    {10,"0.返回",Nop,Nop,Menu_Main}, 
};
struct MenuItem Menu_Matrix1[4]= 
{ 
    {4,"1.行列式阶",Menu_MainFunc31,Null,Menu_Matrix}, 
    {4,"2.查看并更改",Menu_MainFunc31,Null,Menu_Matrix}, 
    {4,"3.计算行列式",Menu_MainFunc31,Null,Menu_Matrix}, 
    {4,"0.返回",Nop,Null,Menu_Matrix}, 
};
struct MenuItem Menu_Matrix2_4[5]= 
{ 
    {5,"1.矩阵的阶",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"2.修改第一矩阵",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"3.修改第二矩阵",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"4.计算矩阵",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"0.返回",Nop,Null,Menu_Matrix},     
};
struct MenuItem Menu_Matrix5_9[4]= 
{ 
    {4,"1.设置矩阵行列",Menu_MainFunc33,Null,Menu_Matrix}, 
    {4,"2.查看或修改矩阵",Menu_MainFunc33,Null,Menu_Matrix}, 
    {4,"3.      ",Menu_MainFunc33,Null,Menu_Matrix}, 
    {4,"0.返回",Menu_MainFunc33,Null,Menu_Matrix}, 
};

struct MenuItem Menu_FuncImg[7]= 
{ 
    {7,"1.原点坐标X0",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"2.原点坐标y0",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"3.横坐标单位长度",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"4.纵坐标单位长度",Menu_MainFunc4,Null,Menu_Main},
    {7,"5.图像表达式",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"6.显示图像",Menu_MainFunc4,Null,Menu_Main},   
    {7,"0.返回",Nop,Null,Menu_Main},      
};

struct MenuItem Menu_Sensus[4]= 
{ 
    {4,"1.输入数\xfd据个数\xfd",Menu_MainFunc6,Null,Menu_Main}, 
    {4,"2.输入数\xfd据",Menu_MainFunc6,Null,Menu_Main}, 
    {4,"3.计算",Nop,Menu_Sensus1,Menu_Main},   
    {4,"0.返回",Nop,Null,Menu_Main},      
};
struct MenuItem Menu_Sensus1[3]= 
{ 
    {3,"1.查看数\xfd据结果",Menu_MainFunc61,Null,Menu_Sensus}, 
    {3,"2.查看众数\xfd",Menu_MainFunc61,Null,Menu_Sensus},  
    {3,"0.返回",Nop,Null,Menu_Main},      
};


struct MenuItem Menu_ODEFunc[6]= 
{ 
    {6,"1.右边的函数\xfd是",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"2.左端点a",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"3.右端点b",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"4.步长h",Menu_MainFunc7,Null,Menu_Main},  
    {6,"5.左端点的y 值",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"6.结果",Menu_MainFunc7,Null,Menu_Main},      
};


//结构体实现 
struct MenuItem Menu_Other[9]= 
{ 
    {6,"1.屏幕亮度",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"2.shift 提示",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"3.节电设置",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"4.日期设置",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"5.时钟设置",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"6.关于",Menu_MainFunc8,Null,Menu_Main}, 
};

//------------------------------------------------------------------菜单初始化、菜单切换、函数执行等的的操作函数-------------------------------------------------------------
void Menu_Init(void)
{
    P_menu = &Menu_Main[0];      //指针指向初始节点 
	P_Index.Menu_Depth = 0;	//菜单深度为0
    function = Nop;
    P_Index.Menu_KeyFunc = 0;   //菜單處理
//    Menu_Display();//显示屏上显示菜单
}

void Menu_Display(void)
{
	int i,j;
	
	j = P_Index.Menu_Start[P_Index.Menu_Depth];		//当前页面开始的的选项
	Screen_Clear();

                
    for( i=0;i<4;i++ )
    {
        if( i<P_menu[0].MenuCount )
            Screen_ShowStr(0,(u8)(i*2),P_menu[i+j].DisplayString);
        else 
            break;
        
    }    
    if( P_Index.Menu_Depth==2 )    //强制显示
    {
        switch( P_Index.Menu_Pilot[P_Index.Menu_Depth-1] )
        {
            case 4:Screen_ShowStr(16,4,"输入次方n");
                   Screen_ShowStr(0,6,"4.计算  0.返回");break;
            
            case 5:Screen_ShowStr(16,4,"查看矩阵的逆");break;
            case 6:
                Screen_ShowStr(16,4,"查看转换后矩阵");break;
            case 7:Screen_ShowStr(16,4,"查看矩阵的秩");break;  
            case 8:Screen_ShowStr(16,4,"输入数\xfd乘系数\xfdk");
                    Screen_ShowStr(0,6,"4.计算  0.返回");
                    break; 
        }
        //Screen_ShowFreshen();
    }
    Screen_ShowFreshen();
}

void Run(struct MenuItem *manyou) 
{ 
	(*(manyou->Subs))(); 
}

void Menu_Deal(char* key)
{
    u8 keyval = *key;
    
    if( P_Index.Menu_KeyFunc==0 )		//菜单切换
    {
        if( keyval >0 )
        {
            if( keyval==4 ) //上翻
            {
                if( P_Index.Menu_Start[P_Index.Menu_Depth]>0 )
                    P_Index.Menu_Start[P_Index.Menu_Depth]--;
            }
            else if( keyval==9 ) //下翻
            {
                if( P_Index.Menu_Start[P_Index.Menu_Depth]+4<P_menu[0].MenuCount )
                    P_Index.Menu_Start[P_Index.Menu_Depth]++;		//指针             
            } 
            else if(( keyval==22 )||( keyval=='0' )) //返回
            {
                unsigned char p =  P_Index.Menu_Start[P_Index.Menu_Depth];		//计算当前选中的项的编号
                if( P_menu[p].Parentms != Null )
                {
                    P_Index.Menu_Start[P_Index.Menu_Depth]=0;
                    P_Index.Menu_Pilot[P_Index.Menu_Depth]=0;
                    
                    P_Index.Menu_Depth--;
                    P_menu = P_menu[p].Parentms;
                }  

            } 
            else if(( keyval >='1' )&&( keyval<='9' )) //进入
            {
               // if( (u8)(keyval-'0')<(u8)P_menu[P_Index.Menu_Depth-1].MenuCount )
                {                    
                    unsigned char p = keyval-'1' ;		//计算当前选中的项的编号
                    
                    if( P_menu[p].Childrenms != Null )
                    {
                        P_Index.Menu_Pilot[P_Index.Menu_Depth]=p;
                        P_Index.Menu_Depth++;
                        P_menu = P_menu[p].Childrenms;
                    }
                    else if( P_menu[p].Subs != Null )
                    { 
                        P_Index.Menu_Pilot[P_Index.Menu_Depth]=p;
                        Run( &P_menu[p]);
                    }  
                }                
            }     
            if( P_Index.Menu_KeyFunc==0 )
            {
                Menu_Display();
            }
            *key = 0;            
        }
        if( state>0 )
        {
            Menu_Display();
            state = 0;
        }
    }
    else
    {
        if( keyval==5 )
        {
            Menu_Display();  
            P_Index.Menu_KeyFunc=0;
        }
        else
            function(keyval);
        *key = 0;
    }
}

//------------------------------------------------------------------菜单初始化以及菜单切换的操作函数-------------------------------------------------------------
void Menu_MainFunc1(void)
{
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_NORMAL;
	function = normal_calculate;
}

void Menu_MainFunc2(void)
{
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_COMPLEX;
	function = complex_calculate;
}

void Menu_MainFunc31(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    
    if( address==0 )
    {
        function = Ranks_Set;
    }
    else if( address==1 )
    {
        function = Ranks_Amend;
    }
    else if( address==2 )
    {
        function = Ranks_Caculate;
    }
}

void Menu_MainFunc32(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    
    if( address==0 )
    {
        function = Ranks_2_4Set;
    }
    else if( address==1 )
    {
        function = Ranks_AmendA;
    }
    else if( address==2 )
    {
        function = Ranks_AmendB;
    }
    else if( address==3 )
    {
        function = Ranks_Caculate2;
    }    
}

void Menu_MainFunc33(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    
    if( address==0 )
    {
        if(( P_Index.Menu_Pilot[P_Index.Menu_Depth-1]==6 )
            ||( P_Index.Menu_Pilot[P_Index.Menu_Depth-1]==7 )
            ||( P_Index.Menu_Pilot[P_Index.Menu_Depth-1]==8 )
        )
            function = Ranks_2_4Set;       //輸入行和列兩個數值
        else
            function = Ranks_Set;       //輸入1-5n
    }
    else if( address==1 )
    {
        function = Ranks_AmendA;        //输入矩阵
    }
    else if( address==2 )
    {
        switch( P_Index.Menu_Pilot[P_Index.Menu_Depth-1] )
        {
            case 4:function = Ranks_Setpower;break;     //次方輸入
            case 5:function = Ranks_Caculate3;break;   //查看矩阵的逆
            case 6:function = Ranks_Caculate3;break;//查看转换后矩阵
            case 7:function = Ranks_changeCaculate;break;//查看矩阵的秩
            case 8:function = Ranks_Setpower;break;     //输入数乘系数k
            default:function = Nop;break;
        }
    }
    else if( address==3 )
    {
        switch( P_Index.Menu_Pilot[P_Index.Menu_Depth-1] )
        {
            case 4:function = Ranks_Caculate3;break;
            case 8:function = Ranks_Caculate3;break;
            default:function = Nop;break;
        }
        //function = Ranks_Caculate2;
    }    
}

void Menu_MainFunc4(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_NORMAL;
    
    if( address==0 )
    {
        function = FuncPic_SetX0;
    }
    else if( address==1 )
    {
        function = FuncPic_SetY0;
    }
    else if( address==2 )
    {
        function = FuncPic_SetKX;
    }
    else if( address==3 )
    {
        function = FuncPic_SetKY;
    }    
    else if( address==4 )
    {
        function = FuncPic_Write;
    }   
    else if( address==5 )
    {
        function = FuncPic_ShowPic;
    }        
}


void Menu_MainFunc5(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    function = Solve;    
}

void Menu_MainFunc6(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    if( address==0 )
        function = Sensus_Set;  
    else if( address==1 )
        function =  Sensus_Amend;
}
void Menu_MainFunc61(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    if( address==0 )
        function = Sensus_Check;  
    else if( address=1 )
    function =  Sensus_Check2;
}

void Menu_MainFunc7(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_NORMAL; 
    if( address==0 )
        function = calculus_funcWR;  
    else if( address==1 )
        function =  calculus_funcWRa;
    else if( address==2 )
        function =  calculus_funcWRb;
    else if( address==3 )
        function =  calculus_funcWRh;
    else if( address==4 )
        function =  calculus_funcWRy;    
    else if( address==5 )
        function =  calculus_funcShow;
    
}
void Menu_MainFunc8(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//当前页面开始的的选项
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;

    if( address==0 )
        function = Set_Light;  
    else if( address==1 )
        function =  Set_LightShift;
    else if( address==2 )
        function =  Set_Sleep;
    else if( address==3 )
        function =  Set_Data;
    else if( address==4 )
        function =Set_Time  ;    
//    else if( address==5 )
//        function =  calculus_funcShow;
    
}

