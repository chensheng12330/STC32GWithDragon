#include "spi.h"
#include "system.h"

#if (SPI_DMA_CFG == 1)
bit	SpiDmaFlag;
uint8_t xdata SpiDmaTxBuffer[256]; //_at_ DMA_TX_ADDR;
uint16_t data_index = 0;
#endif

//======== PLL ��ض��� =========
#define HSCK_MCLK       0
#define HSCK_PLL        1
#define HSCK_SEL        HSCK_PLL

#define PLL_96M         0       //PLLʱ�� = PLL����ʱ��*8
#define PLL_144M        1       //PLLʱ�� = PLL����ʱ��*12
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

void delay()
{
    int i;
    
    for (i=0; i<100; i++);
}

void PLL_Init(void)
{
    //ѡ��PLL���ʱ��
#if (PLL_SEL == PLL_96M)
    CLKSEL &= ~CKMS;            //ѡ��PLL��96M��ΪPLL�����ʱ��
#elif (PLL_SEL == PLL_144M)
    CLKSEL |= CKMS;             //ѡ��PLL��144M��ΪPLL�����ʱ��
#else
    CLKSEL &= ~CKMS;            //Ĭ��ѡ��PLL��96M��ΪPLL�����ʱ��
#endif
    
    //ѡ��PLL����ʱ�ӷ�Ƶ,��֤����ʱ��Ϊ12M(����Χ��8M~16M, 12M +/- 4M)
    USBCLK &= ~PCKI_MSK;
//#if (MAIN_Fosc == 12000000UL)
//    USBCLK |= PCKI_D1;          //PLL����ʱ��1��Ƶ
//#elif (MAIN_Fosc == 24000000UL)
//    USBCLK |= PCKI_D2;          //PLL����ʱ��2��Ƶ
//#elif (MAIN_Fosc == 48000000UL)
    USBCLK |= PCKI_D4;          //PLL����ʱ��4��Ƶ
//#elif (MAIN_Fosc == 96000000UL)
//    USBCLK |= PCKI_D8;          //PLL����ʱ��8��Ƶ
//#else
//    USBCLK |= PCKI_D1;          //Ĭ��PLL����ʱ��1��Ƶ
//#endif

    //����PLL
    USBCLK |= ENCKM;            //ʹ��PLL��Ƶ
    
    delay();                    //�ȴ�PLL��Ƶ

    //ѡ��HSPWM/HSSPIʱ��
#if (HSCK_SEL == HSCK_MCLK)
    CLKSEL &= ~HSIOCK;          //HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#elif (HSCK_SEL == HSCK_PLL)
    CLKSEL |= HSIOCK;           //HSPWM/HSSPIѡ��PLL���ʱ��Ϊʱ��Դ
#else
    CLKSEL &= ~HSIOCK;          //Ĭ��HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#endif

    //��Ƶ35M��PLL=35M/4*12/2=103.2MHz/2=52.5MHz
    HSCLKDIV = 2;               //HSPWM/HSSPIʱ��Դ2��Ƶ
    
    SPCTL = 0xd3;               //����SPIΪ����ģʽ,�ٶ�ΪSPIʱ��/2(52.5M/2=26.25M)
    HSSPI_CFG2 |= 0x20;         //ʹ��SPI����ģʽ

    P2SR = 0x00;                //��ƽת���ٶȿ죨����IO�ڸ��ٷ�ת�źţ�
    P2DR = 0xff;                //�˿�����������ǿ
}

void HAL_SPI_Init(void)
{
//    HSCLKDIV = 0;  //���ø���IOʱ�ӷ�ƵΪ1��Ƶ��Ĭ��2��Ƶ��ʹSPIʱ��/2

//    SSIG = 1; //���� SS ���Ź��ܣ�ʹ�� MSTR ȷ���������������Ǵӻ�
//    SPEN = 1; //ʹ�� SPI ����
//    DORD = 0; //�ȷ���/�������ݵĸ�λ�� MSB��
//    MSTR = 1; //��������ģʽ
//    CPOL = 0; //SCLK ����ʱΪ�ߵ�ƽ��SCLK ��ǰʱ����Ϊ�½��أ���ʱ����Ϊ������
//    CPHA = 0; //������ SCLK ǰʱ������������ʱ���ز���
//    SPCTL = (SPCTL & ~3) | 3; //SPCTL & 0Xfc;  //SPI ʱ��Ƶ��ѡ��, 0: 4T, 1: 8T,  2: 16T,  3: 2T

    PLL_Init();     //SPI ʹ��PLL����ʱ���ź�
    
    P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO���л�. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2
    SPI_SCK = 0;    // set clock to low initial state
    SPI_MOSI = 1;
    SPIF = 1;   //��SPIF��־
    WCOL = 1;   //��WCOL��־

#if (SPI_DMA_CFG == 1)
    /*DMA Mode*/
	DMA_SPI_STA = 0x00;
	DMA_SPI_CFG = 0xE0;		//bit7 1:Enable Interrupt
	DMA_SPI_AMT = 0x02;		//���ô������ֽ�����n+1

	DMA_SPI_TXAH = (u8)((u16)&SpiDmaTxBuffer >> 8);	//SPI�������ݴ洢��ַ
	DMA_SPI_TXAL = (u8)((u16)&SpiDmaTxBuffer);

	DMA_SPI_CFG2 = 0x00;	//01:P2.2
	DMA_SPI_CR = 0x81;		//bit7 1:ʹ�� SPI_DMA, bit6 1:��ʼ SPI_DMA ����ģʽ, bit0 1:��� SPI_DMA FIFO
#endif    
}

void HAL_SPI_Transmit(uint8_t *Mem, uint16_t MemSize, uint32_t Timeout)
{		
    uint16_t i = 0; 
#if (SPI_DMA_CFG == 1)
    if(MemSize > 0)
    {
        while(SpiDmaFlag); 
        for(i = 0; i< MemSize; i++)
        {
            SpiDmaTxBuffer[i] = Mem[i];
        }  
        SpiDmaFlag = 1;
        DMA_SPI_AMT = MemSize - 1;
        DMA_SPI_CR |= 0x40;
    }
#else
    for(i = 0; i< MemSize; i++)
    {
        SPDAT = *(Mem + i);
        while(SPIF == 0) ;
        SPIF = 1;   //��SPIF��־
        WCOL = 1;   //��WCOL��־
    }
#endif

}

#if (SPI_DMA_CFG == 1)
void SPI_DMA_Interrupt(void) interrupt 13		
{
	DMA_SPI_STA = 0;
	SpiDmaFlag = 0;
	
}
#endif