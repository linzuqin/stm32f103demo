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
#include "GPIO.h" 
#include "config.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include <stdarg.h>
#include "time.h"


typedef struct 
{
    /* data */
    char *version;          //ϵͳ�汾
}sys_params_t;

extern sys_params_t sys_params;

void AT_Thread_Init(void);
void my_uart_init(void);
void OLED_TASK_INIT(void);
void io_tasl_init(void);

void rt_hw_us_delay(uint32_t us) ;
const void* binary_strstr(const void* haystack, size_t h_len, const void* needle, size_t n_len) ;


#endif
