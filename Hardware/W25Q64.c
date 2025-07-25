#include "stm32f10x.h"                  // Device header
#include "MySPI.h"
#include "W25Q64_Ins.h"
#include "W25Q64.h"

void W25Q64_Init(void)
{
	MySPI_Init(W25Q64_SPI);
}

void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SPI ,W25Q64_JEDEC_ID);
	*MID = MySPI_SwapByte(W25Q64_SPI ,W25Q64_DUMMY_BYTE);
	*DID = MySPI_SwapByte(W25Q64_SPI ,W25Q64_DUMMY_BYTE);
	*DID <<= 8;
	*DID |= MySPI_SwapByte(W25Q64_SPI ,W25Q64_DUMMY_BYTE);
	MySPI_Stop();
}

void W25Q64_WriteEnable(void)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SPI ,W25Q64_WRITE_ENABLE);
	MySPI_Stop();
}

void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SPI ,W25Q64_READ_STATUS_REGISTER_1);
	Timeout = 100000;
	while ((MySPI_SwapByte(W25Q64_SPI ,W25Q64_DUMMY_BYTE) & 0x01) == 0x01)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
	MySPI_Stop();
}

void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	uint16_t i;
	
	W25Q64_WriteEnable();
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SPI ,W25Q64_PAGE_PROGRAM);
	MySPI_SwapByte(W25Q64_SPI ,Address >> 16);
	MySPI_SwapByte(W25Q64_SPI ,Address >> 8);
	MySPI_SwapByte(W25Q64_SPI ,Address);
	for (i = 0; i < Count; i ++)
	{
		MySPI_SwapByte(W25Q64_SPI ,DataArray[i]);
	}
	MySPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SPI ,W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(W25Q64_SPI ,Address >> 16);
	MySPI_SwapByte(W25Q64_SPI ,Address >> 8);
	MySPI_SwapByte(W25Q64_SPI ,Address);
	MySPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SPI ,W25Q64_READ_DATA);
	MySPI_SwapByte(W25Q64_SPI ,Address >> 16);
	MySPI_SwapByte(W25Q64_SPI ,Address >> 8);
	MySPI_SwapByte(W25Q64_SPI ,Address);
	for (i = 0; i < Count; i ++)
	{
		DataArray[i] = MySPI_SwapByte(W25Q64_SPI ,W25Q64_DUMMY_BYTE);
	}
	MySPI_Stop();
}
