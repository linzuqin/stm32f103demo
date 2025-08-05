#include "user_flash.h"

uint16_t boot_times = 0;
uint16_t default_boot_times = 0;
uint16_t default_test = 0;
uint16_t test = 0;
wifi_params_t default_fiwi_params = {
	.WiFi_SSID = DEFAULT_WIFI_SSID,
	.WiFi_Password = DEFAULT_WIFI_PWD
};

const ef_env default_env_set[] = {
	{"boot_times",&default_boot_times , sizeof(default_boot_times)},
	{"wifi",&default_fiwi_params , sizeof(default_fiwi_params)},

};
uint16_t default_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

void flash_set(const char *key, const void *value_buf, size_t buf_len)
{   
    ef_set_env_blob(key,value_buf,buf_len);
}
size_t flash_get(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    return ef_get_env_blob(key, value_buf, buf_len, saved_value_len);
}

void user_flash_init(void)
{
	if(easyflash_init() == EF_NO_ERR)
	{
//		ef_env_set_default();
		flash_get("boot_times", &boot_times, sizeof(boot_times),NULL);
		boot_times ++;
		flash_set("boot_times", &boot_times, sizeof(boot_times));	
		flash_get("wifi_params", &AT_Device.wifi_params, sizeof(AT_Device.wifi_params),NULL);	
	}
}

uint8_t user_flash_stack[1024];
struct rt_thread user_flash_pannel;

struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[32];

void user_flash_task(void *params)
{
	char *str;
	user_flash_init();
	while(1)
	{
		if (rt_mb_recv(&mb, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
		{
			if(strcmp(str , default_env_set[0].key) == 0)										//boot_times
			{
					
			}
			else if(strcmp(str , default_env_set[1].key) == 0)
			{
				flash_set(default_env_set[1].key, &AT_Device.wifi_params, sizeof(AT_Device.wifi_params));	//wifi params
				NVIC_SystemReset();

			}
		}
		rt_thread_mdelay(500);
	}
}

void user_flash_task_init(void)
{
	rt_err_t result = RT_EOK;
	result = rt_thread_init(&user_flash_pannel , "user_flash_task" , user_flash_task , RT_NULL , user_flash_stack , sizeof(user_flash_stack) , 22 , 100);
	if(result == RT_EOK)
	{
		rt_thread_startup(&user_flash_pannel);
	}

	/*邮箱初始化*/
	result = rt_mb_init(&mb,"mbt",&mb_pool[0],sizeof(mb_pool) / 4,RT_IPC_FLAG_FIFO);         
}