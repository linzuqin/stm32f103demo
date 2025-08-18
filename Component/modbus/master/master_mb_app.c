#include "master_function.h"
#include "General_modbusDevice.h"

//这个表用来保存所有可能用到的设备类型及相关参数
const ModbusDevice_Table_t ModbusDevice_Table[] = {
    [0] = //00类型的设备 读取地址为0 无写入地址 读取长度为1个寄存器
    {
        .readAddress = 0x0000,
        .readLength = 1
    },
    [1] = //01类型的设备 读取地址为1 无写入地址 读取长度为1个寄存器
    { 
        .readAddress = 0x0000,
        .readLength = 1
    }          

};

UserModbusDevice_Table_t UserModbusDevice_Table[] = {
    {"0101" , NULL , NULL , NULL} ,     //类型为01的设备 地址为01 暂无保存数据的地址
};

#if MODBUS_MASTER_ENABLE

uint16_t UserModbusDevice_Num = sizeof(UserModbusDevice_Table) / sizeof(UserModbusDevice_Table_t); //modbus device num
uint16_t ModbusDevice_Num = sizeof(ModbusDevice_Table) / sizeof(ModbusDevice_Table_t); //modbus device num

#define MASTER_MB_PANNEL_STACK_SIZE 1024                            //modbus master task stack size
static struct rt_thread MASTER_MB_PANNEL;                           //modbus master task
static char MASTER_MB_PANNEL_STACK[MASTER_MB_PANNEL_STACK_SIZE];    //modbus master task stack
static uint16_t test = 0;
void User_master_task(void *params)
{
    static uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];                   //agile modbus send buffer
    static uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];                   //agile modbus read buffer
    agile_modbus_rtu_t ctx_rtu;
    agile_modbus_t *ctx = &ctx_rtu._ctx;
    agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));

    int deviceAddr = 0;
    int deviceType = 0;
    uint16_t regAddress = 0;
    uint16_t regLength = 0;
    uint16_t *Data_p = NULL;
    while (1) 
    {
        for(uint16_t i = 0; i < UserModbusDevice_Num ; i++ )
        {
            if(UserModbusDevice_Table[i].DeviceCode == NULL)
            {
                continue;
            }

            /*获取设备类型 以及 设备通信地址*/
            sscanf(UserModbusDevice_Table[i].DeviceCode , "%02d%02d" , &deviceType , &deviceAddr);
					
            /*检查类型是否合法*/
            if(deviceType >= ModbusDevice_Num)
            {
                continue;
            }

            /*检测设备其他参数是否合法*/
            if(UserModbusDevice_Table[i].readAddress == NULL || UserModbusDevice_Table[i].DataBuf == NULL || UserModbusDevice_Table[i].readLength == NULL)
            {
                UserModbusDevice_Table[i].readAddress = (uint16_t *)(&(ModbusDevice_Table[deviceType].readAddress));
                UserModbusDevice_Table[i].readLength = (uint16_t *)(&(ModbusDevice_Table[deviceType].readLength));
                UserModbusDevice_Table[i].DataBuf = &test;

                continue;
            }
            regAddress = *(UserModbusDevice_Table[i].readAddress);
            regLength = *(UserModbusDevice_Table[i].readLength);
            Data_p = UserModbusDevice_Table[i].DataBuf;
            mater_mb_hold_read(ctx , deviceAddr , regAddress , Data_p , regLength , uart_devices[MB_M_DEVICE].rx_buffer);

            rt_thread_mdelay(50);               //适当延时防止读取速度过快
        }
        rt_thread_mdelay(100);
    }
}

static int User_master_start(void)
{
    rt_err_t result;
    result = rt_thread_init(&MASTER_MB_PANNEL, "MASTER_MB_TASK", User_master_task, RT_NULL, &MASTER_MB_PANNEL_STACK[0], sizeof(MASTER_MB_PANNEL_STACK), 22, 100);
    if (result == RT_EOK)
    {
        rt_thread_startup(&MASTER_MB_PANNEL);
        
    }
    else
    {
       
    }
		
    #if MODBUS_MASTER_ENABLE
        My_UART_Init(&uart_devices[MB_M_DEVICE]);
    #endif

    return 1;
}

#endif

	
#if MODBUS_MASTER_ENABLE
INIT_APP_EXPORT(User_master_start);
#endif