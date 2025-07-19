#ifndef _USER_FLASH_H_
#define _USER_FLASH_H_
#include <easyflash.h>
#include <sys.h>

extern struct rt_mailbox mb;

void user_flash_task_init(void);



void flash_set(const char *key, const void *value_buf, size_t buf_len);
size_t flash_get(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len);








#endif
