

#ifndef __BSP_LCD_CONFIG_H
#define __BSP_LCD_CONFIG_H
/*
本驱动文件是用来配置ILI9806G LCD驱动芯片，如果使用其RGB8888模式。
则需要将该芯片在使用前使用SPI接口对内部寄存器进行，配置成功之后，
方可使用。
*/
//本驱动使用模拟SPI总线	 
void   LCD_SPI_Init(void);

#endif

