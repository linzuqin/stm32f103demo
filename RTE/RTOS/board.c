/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "stm32f10x.h"                  // Device header
#include "SEGGER_RTT.h"
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (15*1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

uint8_t sys_clock_set(uint32_t plln)
{
    uint32_t retry = 0;
    uint8_t retval = 0;
    RCC->CR |= 0x00010000;          

    while (retry < 0XFFF0)
    {
        __nop();

        if (RCC->CR & (1 << 17) && retry > 0X8000)
        {
            break;
        }

        retry++;        
    }

    if (retry >= 0XFFF0)
    {
        retval = 1;     
    }
    else
    {
        RCC->CFGR = 0X00000400;     
        plln -= 2;                  
        RCC->CFGR |= plln << 18;   
        RCC->CFGR |= 1 << 16;       

        FLASH->ACR = 1 << 4;        /* PRFTBE = 1 开启预取缓冲区 */
        FLASH->ACR |= 2 << 0;       /* LATENCY[2:0] = 2 FLASH两个等待周期 */

        RCC->CR |= 1 << 24;         /* PLLON = 1, 使能PLL */

        while (!(RCC->CR >> 25));   /* 等待PLL锁定 */

        RCC->CFGR |= 2 << 0;        /* SW[1:0] = 2, 选择PLL输出作为系统时钟 */

        while (((RCC->CFGR >> 2) & 0X03) != 2); /* 等待PLL作为系统时钟设置成功 */
    }

    return retval;
}

void sys_stm32_clock_init(uint32_t plln)
{
    RCC->APB1RSTR = 0x00000000;     /* 复位结束 */
    RCC->APB2RSTR = 0x00000000;
    
    RCC->AHBENR = 0x00000014;       /* 睡眠模式闪存和SRAM时钟使能.其他关闭 */
    RCC->APB2ENR = 0x00000000;      /* 外设时钟关闭 */
    RCC->APB1ENR = 0x00000000;
    
    RCC->CR |= 0x00000001;          /* 使能内部高速时钟HSION */
    RCC->CFGR &= 0xF8FF0000;        /* 复位SW[1:0], SWS[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], ADCPRE[1:0], MCO[2:0] */
    RCC->CR &= 0xFEF6FFFF;          /* 复位HSEON, CSSON, PLLON */
    RCC->CR &= 0xFFFBFFFF;          /* 复位HSEBYP */
    RCC->CFGR &= 0xFF80FFFF;        /* 复位PLLSRC, PLLXTPRE, PLLMUL[3:0] 和 USBPRE/OTGFSPRE */
    RCC->CIR = 0x009F0000;          /* 关闭所有RCC中断并清除中断标志 */

    sys_clock_set(plln);            /* 设置时钟 */
}


void SysTick_Handler(void)
{
    rt_interrupt_enter();
    
    rt_tick_increase();

    rt_interrupt_leave();
}
/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
//#error "TODO 1: OS Tick Configuration."
    /* 
     * TODO 1: OS Tick Configuration
     * Enable the hardware timer and call the rt_os_tick_callback function
     * periodically with the frequency RT_TICK_PER_SECOND. 
     */
		sys_stm32_clock_init(9);

//    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

#ifdef RT_USING_CONSOLE

//static int uart_init(void)
//{
//#error "TODO 2: Enable the hardware uart and config baudrate."
//    return 0;
//}
//INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char *str)
{
//#error "TODO 3: Output the string 'str' through the uart."
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
			SEGGER_RTT_Write(0,&a,1);
        }
		SEGGER_RTT_Write(0,(uint8_t *)(str + i),1);
    }
}

#endif

