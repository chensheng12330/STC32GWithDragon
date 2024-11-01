/*
本例程由热心网友提供，例程源链接如下：
https://github.com/songzhishuo/STC32_u8g2_drv

使用注意事项：
1.在处理的时候需要注意 不要绘制到屏幕以外的区域，否则可能导致设备跑飞
2.本工程仅在STC32G12K128Beat LQFP64 上进行了测试，如有发现bug请联系我们。

本例程在STC32G12K128屠龙刀核心板上飞线测试，脚位定义如下：

OLED-GND -> P2.7
OLED-VCC -> P2.6

IIC模式：
SCL -> P2.5
SDA -> P2.4
RST -> P2.3
DC  -> P2.2
需要将“iic.h”文件中，U8g2_I2C_MODE/IIC_DMA_CFG 定义置1，同时将“spi.h”文件中，U8g2_SPI_MODE/SPI_DMA_CFG 定义置0。

SPI模式：
SCK -> P2.5
MISO -> P2.4
MOSI -> P2.3
RST -> P2.2
DC  -> P2.1
CS  -> P2.0
需要将“iic.h”文件中，U8g2_I2C_MODE/IIC_DMA_CFG 定义置0，同时将“spi.h”文件中，U8g2_SPI_MODE/SPI_DMA_CFG 定义置1。

程序默认使用内部35MHz IRC时钟。

SPI时钟默认使用PLL做时钟源，设置SPI_SCK频率26.25M，用户可根据自身需求进行调整。

注： SPI 和 IIC 模式不可同时开启。

*/