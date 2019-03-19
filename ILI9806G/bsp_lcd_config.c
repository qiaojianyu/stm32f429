
#include "bsp.h"

#define LCD_SPI_RCC_CLOCKCMD          RCC_AHB1Periph_GPIOB
#define LCD_SPI_GPIO_PORT             GPIOB			/* GPIO端口 */
#define LCD_SPI_CS_PIN                GPIO_Pin_9
#define LCD_SPI_SDI_PIN               GPIO_Pin_5
#define LCD_SPI_CLK_PIN               GPIO_Pin_6

#define LCD_SPI_CS_1()          LCD_SPI_GPIO_PORT->BSRRL = LCD_SPI_CS_PIN
#define LCD_SPI_CS_0()          LCD_SPI_GPIO_PORT->BSRRH = LCD_SPI_CS_PIN

#define LCD_SPI_SDI_1()         LCD_SPI_GPIO_PORT->BSRRL = LCD_SPI_SDI_PIN
#define LCD_SPI_SDI_0()         LCD_SPI_GPIO_PORT->BSRRH = LCD_SPI_SDI_PIN

#define LCD_SPI_CLK_1()         LCD_SPI_GPIO_PORT->BSRRL = LCD_SPI_CLK_PIN
#define LCD_SPI_CLK_0()         LCD_SPI_GPIO_PORT->BSRRH = LCD_SPI_CLK_PIN

#define  LCD_SPI_SDI_READ()   (( LCD_SPI_GPIO_PORT->IDR & LCD_SPI_SDI_PIN) != 0)	/* 读SDA口线状态 */

static void LCD_SPI_Delay(void)
{
	uint8_t i;

	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	for (i = 0; i < 100; i++);
}



static void LCD_SPI_GPIO_Init(void)
{	 
GPIO_InitTypeDef GPIO_InitStructure;
RCC_AHB1PeriphClockCmd(LCD_SPI_RCC_CLOCKCMD,ENABLE);
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		               /* 设为输出口 */
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		               /* 设为推挽 */
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	                   /* 上下拉电阻不使能 */
GPIO_InitStructure.GPIO_Pin = LCD_SPI_CLK_PIN|LCD_SPI_SDI_PIN;     //SPI_SCK and SPI_OUT  
GPIO_Init(LCD_SPI_GPIO_PORT,&GPIO_InitStructure);

GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;		    /* 设为输出口 */
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽 */
GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	      /* 上下拉电阻不使能 */
GPIO_InitStructure.GPIO_Pin   = LCD_SPI_CS_PIN;           //SPI_CS  PB9
GPIO_Init(LCD_SPI_GPIO_PORT,&GPIO_InitStructure);

LCD_SPI_CS_0();
LCD_SPI_SDI_0();
LCD_SPI_CLK_0();
}   

static void LCD_SPI_WriteByte(u8 TxData)
{
	int i=0;
	
	LCD_SPI_CLK_0();
	for(i=7;i>=0;i--)
	{
	
		if(TxData&(1<<i))
		{
			LCD_SPI_SDI_1();
		}else
		{
			LCD_SPI_SDI_0();
		}
		LCD_SPI_Delay();
		LCD_SPI_CLK_1();
		LCD_SPI_Delay();
    LCD_SPI_CLK_0();		
		LCD_SPI_Delay();
	}			    
}



static void LCD_SPI_WriteComm(u16 CMD)
{			
	LCD_SPI_CS_0();
	LCD_SPI_SDI_0();
	
	LCD_SPI_Delay();
	LCD_SPI_CLK_1();
	LCD_SPI_Delay();
	LCD_SPI_CLK_0();
	
	LCD_SPI_WriteByte(CMD);
	LCD_SPI_CS_1();
}


static void LCD_SPI_WriteData(u16 DAT)
{			
	LCD_SPI_CS_0();
	LCD_SPI_SDI_1();
	
	LCD_SPI_Delay();
	LCD_SPI_CLK_1();
	LCD_SPI_Delay();
	LCD_SPI_CLK_0();
	
	LCD_SPI_WriteByte(DAT);
	LCD_SPI_CS_1();
}







void LCD_SPI_Init(void)
{
    LCD_SPI_GPIO_Init();
    LCD_SPI_WriteComm(0xFF); // EXTC Command Set enable register 
    LCD_SPI_WriteData(0xFF); 
    LCD_SPI_WriteData(0x98); 
    LCD_SPI_WriteData(0x06); 

    LCD_SPI_WriteComm(0xBA); // SPI Interface Setting 
    LCD_SPI_WriteData(0xE0); //pin selected spd and sda

    LCD_SPI_WriteComm(0xBC); // GIP 1 
    LCD_SPI_WriteData(0x03); 
    LCD_SPI_WriteData(0x0F); 
    LCD_SPI_WriteData(0x63); 
    LCD_SPI_WriteData(0x69); 
    LCD_SPI_WriteData(0x01); 
    LCD_SPI_WriteData(0x01); 
    LCD_SPI_WriteData(0x1B); 
    LCD_SPI_WriteData(0x11); 
    LCD_SPI_WriteData(0x70); 
    LCD_SPI_WriteData(0x73); 
    LCD_SPI_WriteData(0xFF); 
    LCD_SPI_WriteData(0xFF); 
    LCD_SPI_WriteData(0x08); 
    LCD_SPI_WriteData(0x09); 
    LCD_SPI_WriteData(0x05); 
    LCD_SPI_WriteData(0x00);
    LCD_SPI_WriteData(0xEE); 
    LCD_SPI_WriteData(0xE2); 
    LCD_SPI_WriteData(0x01); 
    LCD_SPI_WriteData(0x00);
    LCD_SPI_WriteData(0xC1); 

    LCD_SPI_WriteComm(0xBD); // GIP 2 
    LCD_SPI_WriteData(0x01); 
    LCD_SPI_WriteData(0x23); 
    LCD_SPI_WriteData(0x45); 
    LCD_SPI_WriteData(0x67); 
    LCD_SPI_WriteData(0x01); 
    LCD_SPI_WriteData(0x23); 
    LCD_SPI_WriteData(0x45); 
    LCD_SPI_WriteData(0x67); 

    LCD_SPI_WriteComm(0xBE); // GIP 3 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x22); 
    LCD_SPI_WriteData(0x27); 
    LCD_SPI_WriteData(0x6A); 
    LCD_SPI_WriteData(0xBC); 
    LCD_SPI_WriteData(0xD8); 
    LCD_SPI_WriteData(0x92); 
    LCD_SPI_WriteData(0x22); 
    LCD_SPI_WriteData(0x22); 

    LCD_SPI_WriteComm(0xC7); // Vcom 
    LCD_SPI_WriteData(0x1E);
     
    LCD_SPI_WriteComm(0xED); // EN_volt_reg 
    LCD_SPI_WriteData(0x7F); 
    LCD_SPI_WriteData(0x0F); 
    LCD_SPI_WriteData(0x00); 

    LCD_SPI_WriteComm(0xC0); // Power Control 1
    LCD_SPI_WriteData(0xE3); 
    LCD_SPI_WriteData(0x0B); 
    LCD_SPI_WriteData(0x00);
     
    LCD_SPI_WriteComm(0xFC);
    LCD_SPI_WriteData(0x08); 

    LCD_SPI_WriteComm(0xDF); // Engineering Setting ,set mipi div
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x02); 

    LCD_SPI_WriteComm(0xF3); // DVDD Voltage Setting 
    LCD_SPI_WriteData(0x74); 

    LCD_SPI_WriteComm(0xB4); // Display Inversion Control 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x00); 

    LCD_SPI_WriteComm(0xF7); // 480x854
    LCD_SPI_WriteData(0x81); 

    LCD_SPI_WriteComm(0xB1); // Frame Rate 
    LCD_SPI_WriteData(0x00); 
    LCD_SPI_WriteData(0x10); 
    LCD_SPI_WriteData(0x14); 

    LCD_SPI_WriteComm(0xF1); // Panel Timing Control 
    LCD_SPI_WriteData(0x29); 
    LCD_SPI_WriteData(0x8A); 
    LCD_SPI_WriteData(0x07); 

    LCD_SPI_WriteComm(0xF2); //Panel Timing Control 
    LCD_SPI_WriteData(0x40); 
    LCD_SPI_WriteData(0xD2); 
    LCD_SPI_WriteData(0x50); 
    LCD_SPI_WriteData(0x28); 

    LCD_SPI_WriteComm(0xC1); // Power Control 2 
    LCD_SPI_WriteData(0x17);
    LCD_SPI_WriteData(0X85); 
    LCD_SPI_WriteData(0x85); 
    LCD_SPI_WriteData(0x20); 

    LCD_SPI_WriteComm(0xE0); 
    LCD_SPI_WriteData(0x00); //P1 
    LCD_SPI_WriteData(0x0C); //P2 
    LCD_SPI_WriteData(0x15); //P3 
    LCD_SPI_WriteData(0x0D); //P4 
    LCD_SPI_WriteData(0x0F); //P5 
    LCD_SPI_WriteData(0x0C); //P6 
    LCD_SPI_WriteData(0x07); //P7 
    LCD_SPI_WriteData(0x05); //P8 
    LCD_SPI_WriteData(0x07); //P9 
    LCD_SPI_WriteData(0x0B); //P10 
    LCD_SPI_WriteData(0x10); //P11 
    LCD_SPI_WriteData(0x10); //P12 
    LCD_SPI_WriteData(0x0D); //P13 
    LCD_SPI_WriteData(0x17); //P14 
    LCD_SPI_WriteData(0x0F); //P15 
    LCD_SPI_WriteData(0x00); //P16 

    LCD_SPI_WriteComm(0xE1); 
    LCD_SPI_WriteData(0x00); //P1 
    LCD_SPI_WriteData(0x0D); //P2 
    LCD_SPI_WriteData(0x15); //P3 
    LCD_SPI_WriteData(0x0E); //P4 
    LCD_SPI_WriteData(0x10); //P5 
    LCD_SPI_WriteData(0x0D); //P6 
    LCD_SPI_WriteData(0x08); //P7 
    LCD_SPI_WriteData(0x06); //P8 
    LCD_SPI_WriteData(0x07); //P9 
    LCD_SPI_WriteData(0x0C); //P10 
    LCD_SPI_WriteData(0x11); //P11 
    LCD_SPI_WriteData(0x11); //P12 
    LCD_SPI_WriteData(0x0E); //P13 
    LCD_SPI_WriteData(0x17); //P14 
    LCD_SPI_WriteData(0x0F); //P15 
    LCD_SPI_WriteData(0x00); //P16

    LCD_SPI_WriteComm(0x35); //Tearing Effect ON 
    LCD_SPI_WriteData(0x00); 

    LCD_SPI_WriteComm(0x36); //????
    LCD_SPI_WriteData(0x00); 

    LCD_SPI_WriteComm(0x3A); //????
    LCD_SPI_WriteData(0x77); 

    LCD_SPI_WriteComm(0xB6); //select RGB interface
    LCD_SPI_WriteData(0xA2);

    LCD_SPI_WriteComm(0x11); //Exit Sleep 
    LCD_SPI_Delay();
	LCD_SPI_Delay();
    LCD_SPI_WriteComm(0x29); // Display On 
}


























