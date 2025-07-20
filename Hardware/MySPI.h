#ifndef __MYSPI_H
#define __MYSPI_H
#include "stm32f10x.h"                  // Device header
#include "sys.h"                  


#define SPI_CS_PORT		GPIOA
#define SPI_CS_PIN		GPIO_Pin_4
#define SPI_CS(x)			GPIO_WriteBit(SPI_CS_PORT , SPI_CS_PIN , (BitAction)x)
#define SPI_CS_GPIO_RCC_INIT			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)


void MySPI_Init(SPI_TypeDef* SPIx);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(SPI_TypeDef* SPIx , uint8_t ByteSend);

#endif
