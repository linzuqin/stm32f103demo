#ifndef __W25Q64FLASH_H__
#define __W25Q64FLASH_H__

void W25Q64Flash_SPI_SendByte(uint8_t Byte);
uint8_t W25Q64Flash_SPI_ReceiveByte(void);
uint8_t W25Q64Flash_WriteReadByte(uint8_t ByteSend);
void W25Q64Flash_Init(void);						//��ʼ��
void W25Q64_ReadID(uint8_t* MID,uint16_t* DID);		//��ȡоƬID
void W25Q64_WriteEnable(void);						//дʹ��
void W25Q64_WriteBusy(void);						//�ȴ�æ״̬
void W25Q64Flash_EraseSector(uint32_t Addr);		//�������� Addr:������ַ
void W25Q64Flash_EraseBlock(uint32_t Addr);			//����� Addr:���ַ
void W25Q64Flash_EraseAll(void);					//ȫƬ����
void W25Q64Flash_WriteData(uint32_t Addr,uint8_t* Data,uint16_t Size);	//д���� Addr:��ʼ��ַ Data:д�����ݵ�ָ��
//Size:д�������ֽ���������һ��д�����ݲ��ܿ�ҳ���ò������ܴ���256
void W25Q64Flash_ReadData(uint32_t Addr,uint8_t* Data,uint32_t Size);	//������ Addr:��ʼ��ַ Data:��ȡ���ݵ�ָ��
//Size:��ȡ�ֽ���
void W25Q64Flash_PageProgram(uint16_t BlockAddr,uint16_t SectorAddr,uint16_t PageAddr,
	uint16_t ByteAddr,uint8_t* Data,uint16_t Size);

#endif
