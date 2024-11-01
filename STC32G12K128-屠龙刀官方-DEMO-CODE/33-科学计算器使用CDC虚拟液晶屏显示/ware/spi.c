#include "spi.h"


#if (SPI_DMA_CFG == 1)
bit	SpiDmaFlag;
uint8_t xdata SpiDmaTxBuffer[256]; //_at_ DMA_TX_ADDR;
uint16_t xdata data_index = 0;
#endif

//======== PLL 相关定义 =========
#define HSCK_MCLK       0
#define HSCK_PLL        1
#define HSCK_SEL        HSCK_PLL

#define PLL_96M         0       //PLL时钟 = PLL输入时钟*8
#define PLL_144M        1       //PLL时钟 = PLL输入时钟*12
#define PLL_SEL         PLL_144M

#define CKMS            0x80
#define HSIOCK          0x40
#define MCK2SEL_MSK     0x0c
#define MCK2SEL_SEL1    0x00
#define MCK2SEL_PLL     0x04
#define MCK2SEL_PLLD2   0x08
#define MCK2SEL_IRC48   0x0c
#define MCKSEL_MSK      0x03
#define MCKSEL_HIRC     0x00
#define MCKSEL_XOSC     0x01
#define MCKSEL_X32K     0x02
#define MCKSEL_IRC32K   0x03

#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

void delay(void)
{
    int xdata i;
    
    for (i=0; i<100; i++);
}
void delay_ms(u8 ms)
{
     u16 xdata i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);   //6T per loop
     }while(--ms);
}

void PLL_Init(void)
{
    //选择PLL输出时钟
#if (PLL_SEL == PLL_96M)
    CLKSEL &= ~CKMS;            //选择PLL的96M作为PLL的输出时钟
#elif (PLL_SEL == PLL_144M)
    CLKSEL |= CKMS;             //选择PLL的144M作为PLL的输出时钟
#else
    CLKSEL &= ~CKMS;            //默认选择PLL的96M作为PLL的输出时钟
#endif
    
    //选择PLL输入时钟分频,保证输入时钟为12M(允许范围：8M~16M, 12M +/- 4M)
    USBCLK &= ~PCKI_MSK;
//#if (MAIN_Fosc == 12000000UL)
//    USBCLK |= PCKI_D1;          //PLL输入时钟1分频
//#elif (MAIN_Fosc == 24000000UL)
//    USBCLK |= PCKI_D2;          //PLL输入时钟2分频
//#elif (MAIN_Fosc == 48000000UL)
    USBCLK |= PCKI_D4;          //PLL输入时钟4分频
//#elif (MAIN_Fosc == 96000000UL)
//    USBCLK |= PCKI_D8;          //PLL输入时钟8分频
//#else
//    USBCLK |= PCKI_D1;          //默认PLL输入时钟1分频
//#endif

    //启动PLL
    USBCLK |= ENCKM;            //使能PLL倍频
    
    delay();                    //等待PLL锁频

    //选择HSPWM/HSSPI时钟
#if (HSCK_SEL == HSCK_MCLK)
    CLKSEL &= ~HSIOCK;          //HSPWM/HSSPI选择主时钟为时钟源
#elif (HSCK_SEL == HSCK_PLL)
    CLKSEL |= HSIOCK;           //HSPWM/HSSPI选择PLL输出时钟为时钟源
#else
    CLKSEL &= ~HSIOCK;          //默认HSPWM/HSSPI选择主时钟为时钟源
#endif

    //主频35M，PLL=35M/4*12/2=103.2MHz/2=52.5MHz
    HSCLKDIV = 2;               //HSPWM/HSSPI时钟源2分频
    
    SPCTL = 0xd3;               //设置SPI为主机模式,速度为SPI时钟/2(52.5M/2=26.25M)
    HSSPI_CFG2 |= 0x20;         //使能SPI高速模式

    P2SR = 0x00;                //电平转换速度快（改善IO口高速翻转信号）
    P2DR = 0xff;                //端口驱动电流增强
}

void HAL_SPI_Init(void)
{

//    PLL_Init();     //SPI 使用PLL高速时钟信号
//    
//    P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO口切换. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2
//    SPI_SCK = 0;    // set clock to low initial state
//    SPI_MOSI = 1;
//    SPIF = 1;   //清SPIF标志
//    WCOL = 1;   //清WCOL标志

//#if (SPI_DMA_CFG == 1)
//    /*DMA Mode*/
//	DMA_SPI_STA = 0x00;
//	DMA_SPI_CFG = 0xE0;		//bit7 1:Enable Interrupt
//	DMA_SPI_AMT = 0x02;		//设置传输总字节数：n+1

//	DMA_SPI_TXAH = (u8)((u16)&SpiDmaTxBuffer >> 8);	//SPI发送数据存储地址
//	DMA_SPI_TXAL = (u8)((u16)&SpiDmaTxBuffer);

//	DMA_SPI_CFG2 = 0x00;	//01:P2.2
//	DMA_SPI_CR = 0x81;		//bit7 1:使能 SPI_DMA, bit6 1:开始 SPI_DMA 主机模式, bit0 1:清除 SPI_DMA FIFO
//#endif    
}

//void HAL_SPI_Transmit(uint8_t *Mem, uint16_t MemSize, uint32_t Timeout)
//{		
//    uint16_t xdata i = 0; 
//#if (SPI_DMA_CFG == 1)
//    if(MemSize > 0)
//    {
//        while(SpiDmaFlag); 
//        for(i = 0; i< MemSize; i++)
//        {
//            SpiDmaTxBuffer[i] = Mem[i];
//        }  
//        SpiDmaFlag = 1;
//        DMA_SPI_AMT = MemSize - 1;
//        DMA_SPI_CR |= 0x40;
//    }
//#else
//    for(i = 0; i< MemSize; i++)
//    {
//        SPDAT = *(Mem + i);
//        while(SPIF == 0) ;
//        SPIF = 1;   //清SPIF标志
//        WCOL = 1;   //清WCOL标志
//    }
//#endif

//}

//#if (SPI_DMA_CFG == 1)
//void SPI_DMA_Interrupt(void) interrupt 49		
//{
//	DMA_SPI_STA = 0;
//	SpiDmaFlag = 0;
//	SPI_CS = 1;
//    OLED_DC = 1;
//}
//#endif

////发送一个字节
//void OLED_WR_Byte(u8 dat,u8 cmd)
//{
//    u8 udat[1] ;
//    udat[0] = dat;
//    if(cmd)
//    {
//        OLED_DC = 1;
//    }
//    else
//    {
//        OLED_DC = 0;
//    }
//    SPI_CS = 0;
//    delay();delay();delay();delay();delay();
//    HAL_SPI_Transmit(udat, 1, 0xff);   
//}

void SPI_init(void)
{
    P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO口切换. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2
    SSIG = 1; //忽略 SS 引脚功能，使用 MSTR 确定器件是主机还是从机
    SPEN = 1; //使能 SPI 功能
    DORD = 0; //先发送/接收数据的高位（ MSB）
    MSTR = 1; //设置主机模式
    CPOL = 0; //SCLK 空闲时为低电平，SCLK 的前时钟沿为上升沿，后时钟沿为下降沿
    CPHA = 0; //数据 SS 管脚为低电平驱动第一位数据并在 SCLK 的后时钟沿改变数据
    SPCTL = (SPCTL & ~3) | 3;   //SPI 时钟频率选择, 0: 4T, 1: 8T,  2: 16T,  3: 2T
    
   SPI_SCK = 0;    // set clock to low initial state
    SPI_MOSI = 1;
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
}

/************************************************************************/
void SPI_WriteByte(u8 out)
{
    SPDAT = out;
    while(SPIF == 0);
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
}

void OLED_WR_Byte(u8 dat,u8 cmd)
{
    u8 udat[1] ;
    udat[0] = dat;
    if(cmd)
    {
        OLED_DC = 1;
    }
    else
    {
        OLED_DC = 0;
    }
    SPI_CS = 0;
    SPI_WriteByte(dat);
    delay();
    SPI_CS = 1;
    OLED_DC = 1;
}

//OLED的初始化
void OLED_Init_SPI(void)
{
    SPI_init();
    OLED_REST = 1;
    delay_ms(200);
    OLED_REST=0;//复位
    delay_ms(200);
    OLED_REST = 1;

    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x00,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_WR_Byte(0xAF,OLED_CMD);
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte((u8)(0xb0+y),OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
    unsigned int xdata j=0;
    unsigned char xdata x,y;

    for(y=y0;y<(y1+y0);y++)
    {
        OLED_Set_Pos(x0,y);
        for(x=0;x<x1;x++)
        {      
            OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
        }
    }

    
} 


