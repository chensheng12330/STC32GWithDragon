/*
�����������������ṩ������Դ�������£�
https://github.com/songzhishuo/STC32_u8g2_drv

ʹ��ע�����
1.�ڴ����ʱ����Ҫע�� ��Ҫ���Ƶ���Ļ��������򣬷�����ܵ����豸�ܷ�
2.�����̽���STC32G12K128Beat LQFP64 �Ͻ����˲��ԣ����з���bug����ϵ���ǡ�

��������STC32G12K128���������İ��Ϸ��߲��ԣ���λ�������£�

OLED-GND -> P2.7
OLED-VCC -> P2.6

IICģʽ��
SCL -> P2.5
SDA -> P2.4
RST -> P2.3
DC  -> P2.2
��Ҫ����iic.h���ļ��У�U8g2_I2C_MODE/IIC_DMA_CFG ������1��ͬʱ����spi.h���ļ��У�U8g2_SPI_MODE/SPI_DMA_CFG ������0��

SPIģʽ��
SCK -> P2.5
MISO -> P2.4
MOSI -> P2.3
RST -> P2.2
DC  -> P2.1
CS  -> P2.0
��Ҫ����iic.h���ļ��У�U8g2_I2C_MODE/IIC_DMA_CFG ������0��ͬʱ����spi.h���ļ��У�U8g2_SPI_MODE/SPI_DMA_CFG ������1��

����Ĭ��ʹ���ڲ�35MHz IRCʱ�ӡ�

SPIʱ��Ĭ��ʹ��PLL��ʱ��Դ������SPI_SCKƵ��26.25M���û��ɸ�������������е�����

ע�� SPI �� IIC ģʽ����ͬʱ������

*/