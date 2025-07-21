#ifndef _SYS_H_
#define _SYS_H_
#include "stm32f10x.h"                  // Device header

#include "uart.h"
#include "MyRTC.h"
#include "GPIO.h"
#include "PWM.h"
#include "iwdog.h"
#include "ADC.h"
#include "TIM.h"

#include "log.h"
#include "rtthread.h"

#include "mb.h"
#include "slave_mb_app.h"
#include "AT_Function.h"
#include "MPU6050.h"
#include "cJSON.h"
#include "base64.h"
#include "hmac_sha1.h"
#include "onenet.h"
#include "lot_function.h"
#include "W25Q64.h"
#include "MyIIC.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include <stdarg.h>
#include "time.h"

/*project version*/
#define Version											"1.4.3"

/*modbus master*/
#define MODBUS_MASTER_ENABLE        0
#if MODBUS_MASTER_ENABLE
#define MB_M_DEVICE				            uart3_device  //serial port
#endif

#define MODBUS_MASTER_SENSOR_ENABLE        0
#if MODBUS_MASTER_SENSOR_ENABLE
#define MODBUS_MASTER_SENSOR_DEVICE        uart3_device
#endif

#define MODBUS_SLAVE_ENABLE        0
#if MODBUS_SLAVE_ENABLE
#define MB_S_DEVICE_1							uart1_device
#define MODBUS_SLAVE_ADDR						1  //device addr
#define MODBUS_SLAVE_BOUND					115200 //baudrate
#endif

#define USE_PID										 0


/*是否使用片外flash*/
#define OFF_CHIP						1
#define OFF_CHIP_SPI				SPI1
typedef struct 
{
    /* data */
    char *version;          //系统版本
}sys_params_t;

extern sys_params_t sys_params;

void AT_Thread_Init(void);
void my_uart_init(void);
void OLED_TASK_INIT(void);
void io_tasl_init(void);

void rt_hw_us_delay(uint32_t us) ;
const void* binary_strstr(const void* haystack, size_t h_len, const void* needle, size_t n_len) ;


#endif
