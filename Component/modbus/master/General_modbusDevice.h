#ifndef _GENERAL_MODBUSDEVICE_H_
#define _GENERAL_MODBUSDEVICE_H_
#include "stdint.h"
#include "string.h"


typedef struct
{
    uint16_t readAddress;  //读寄存器地址
    uint16_t readLength;   //读寄存器长度
}ModbusDevice_Table_t;

typedef struct
{
    char *DeviceCode;   //设备编号 格式为设备类型(2位) + 设备地址(2位)
    uint16_t *DataBuf; //指向所要保存寄存器数据的地址
    uint16_t *readAddress;  //读寄存器地址
    uint16_t *readLength;   //读寄存器长度
}UserModbusDevice_Table_t;

extern const ModbusDevice_Table_t ModbusDevice_Table[];
extern UserModbusDevice_Table_t UserModbusDevice_Table[];

#endif
