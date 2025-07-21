#include "sys.h"
#include "user_flash.h"

/**
 * @brief 在二进制安全缓冲区中搜索子串
 * @param haystack 主缓冲区
 * @param h_len 主缓冲区长度
 * @param needle 子串
 * @param n_len 子串长度
 * @return 首次出现的位置指针，未找到返回NULL
 */
const void* binary_strstr(const void* haystack, size_t h_len, const void* needle, size_t n_len) 
{
    if (!haystack || !needle || n_len == 0 || h_len < n_len) 
    {
      return NULL;
    }

    const unsigned char* h = (const unsigned char*)haystack;
    const unsigned char* n = (const unsigned char*)needle;

    /* ---------- 短子串优化（n_len <= 4） ---------- */
    if (n_len <= 4) {
        for (size_t i = 0; i <= h_len - n_len; i++) 
        {
            if (memcmp(&h[i], n, n_len) == 0) 
            {
              return &h[i];
            }
        }
        return NULL;
    }

    /* ---------- Boyer-Moore-Horspool预处理 ---------- */
    size_t skip[256];
    for (size_t i = 0; i < 256; i++) {
        skip[i] = n_len;
    }
    for (size_t i = 0; i < n_len - 1; i++) {
        skip[n[i]] = n_len - 1 - i;
    }

    /* ---------- 主搜索逻辑 ---------- */
    const unsigned char* end = h + h_len - n_len;
    while (h <= end) 
    {
        if (memcmp(h, n, n_len) == 0) 
        {
            return h;
        }
        h += skip[h[n_len - 1]];
    }

    return NULL;
}

sys_params_t sys_params = 
{
    .version = "1.0.1"  
};


void dwt_init(void) 
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t dwt_read(void) {
    return DWT->CYCCNT;
}

void rt_hw_us_delay(uint32_t us) 
{
    rt_uint32_t ticks;
    rt_uint32_t told, tnow, tcnt = 0;
    rt_uint32_t reload = SysTick->LOAD;

    ticks = us * reload / (1000000 / RT_TICK_PER_SECOND);
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

int board_init(void)
{
	user_flash_task_init();
	my_uart_init();
	AT_Thread_Init();
	OLED_TASK_INIT();
	io_task_init();
	dwt_init();
	TIM3_1s_Init();
	//	W25Q64_Init();
	return 1;
}

INIT_APP_EXPORT(board_init);
