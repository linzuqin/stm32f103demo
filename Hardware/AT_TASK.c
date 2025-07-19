#include "AT_Function.h"
#include "sys.h"
static struct rt_timer timer1;
static struct rt_timer timer2;


struct rt_thread at_thread;
uint8_t at_thread_stack[4096];

void AT_Thread_Entry(void *parameter)
{
    while (1)
    {
        AT_poll(&AT_Device);
        rt_thread_mdelay(10);
    }
}

void timeout1(void *params)
{
	if(AT_Device.status == AT_IDLE)
	{
    AT_Device.status = AT_GET_NTP;
	}
}

void timeout2(void *params)
{
	if(AT_Device.status == AT_IDLE)
	{
    AT_Device.status = AT_UPDATA;
	}
}

void AT_Thread_Init(void)
{
	rt_err_t result = rt_thread_init(&at_thread,
									"at_thread",
									AT_Thread_Entry,
									NULL,
									at_thread_stack,
									sizeof(at_thread_stack),
									10,
									100);
	if (result == RT_EOK)
	{
		rt_thread_startup(&at_thread);
	}
	
	rt_timer_init(&timer1, "timer1",  
							timeout1, 
							RT_NULL,
							60 * 1000, 
							RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer1);

	rt_timer_init(&timer2, "timer2",  
							timeout2, 
							RT_NULL,
							3 * 1000, 
							RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer2);
}

