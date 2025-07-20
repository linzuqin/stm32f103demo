#include "MySPI.h"

void MySPI_Init(SPI_TypeDef* SPIx)
{
	uint16_t mosi_pin = 0;
	uint16_t miso_pin = 0;
	uint16_t sck_pin = 0;
	
	GPIO_TypeDef *mosi_port;
	GPIO_TypeDef *miso_port;
	GPIO_TypeDef *sck_port;

	if(SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		
		mosi_pin = GPIO_Pin_7;
		miso_pin = GPIO_Pin_6;
		sck_pin = GPIO_Pin_5;
		
		mosi_port = GPIOA;
		miso_port = GPIOA;
		sck_port = GPIOA;
	}
	else if(SPIx == SPI2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		
		mosi_pin = GPIO_Pin_15;
		miso_pin = GPIO_Pin_14;
		sck_pin = GPIO_Pin_13;
		
		mosi_port = GPIOB;
		miso_port = GPIOB;
		sck_port = GPIOB;		
	}
	
	/*CS init*/
	MyGPIO_Init(SPI_CS_PORT , SPI_CS_PIN , GPIO_Mode_Out_PP);
//	SPI_CS_GPIO_RCC_INIT;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure);
	
	/*sck init*/
	MyGPIO_Init(sck_port , sck_pin , GPIO_Mode_AF_PP);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Pin = sck_pin;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(sck_port, &GPIO_InitStructure);
	
	/*mosi init*/
	MyGPIO_Init(mosi_port , mosi_pin , GPIO_Mode_AF_PP);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Pin = mosi_pin;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(mosi_port, &GPIO_InitStructure);
	
	/*miso init*/
	MyGPIO_Init(miso_port , miso_pin , GPIO_Mode_IPU);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Pin = miso_pin;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(miso_port, &GPIO_InitStructure);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStructure);
	
	SPI_Cmd(SPIx, ENABLE);
	
	SPI_CS(1);
}

void MySPI_Start(void)
{
	SPI_CS(0);
}

void MySPI_Stop(void)
{
	SPI_CS(1);
}

uint8_t MySPI_SwapByte(SPI_TypeDef* SPIx , uint8_t ByteSend)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) != SET);
	
	SPI_I2S_SendData(SPIx, ByteSend);
	
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) != SET);
	
	return SPI_I2S_ReceiveData(SPIx);
}
