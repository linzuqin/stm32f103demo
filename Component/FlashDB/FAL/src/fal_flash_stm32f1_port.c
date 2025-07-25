/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <fal.h>
#include "stm32f10x.h"                  // Device header

#if defined(STM32F103xE)
#define PAGE_SIZE     2048
#else
#define PAGE_SIZE     1024
#endif

/*
STM32F1会因容量不同而不同
    小容量和中容量产品主存储块128KB以下，  每页1KB。
    大容量和互联型产品主存储块256KB以上，  每页2KB。

GD32   会因容量不同而不同
    1. Low-density Products     Flash容量从 16KB到  32KB的产品
    2. Medium-density Products  Flash容量从 64KB到 128KB的产品
          全是1K
    3. High-density Products    Flash容量从256KB到 512KB的产品
          全是2K
    4. XL-density Products      Flash容量从768KB到3072KB的产品
          <512K 是2K
          >512K 是4K

雅特力
    全是2K

STM32F4
    STM32F4的flash页尺寸不一样，低地址16KB，高地址32KB或128KB.
*/

extern const struct fal_flash_dev stm32_onchip_flash;
static int init(void)
{
    /* do nothing now */
    return 1;
}


static int ef_err_port_cnt = 0;
int on_ic_read_cnt  = 0;
int on_ic_write_cnt = 0;

void feed_dog(void)
{

}

static int read(long offset, uint8_t *buf, size_t size)
{

    size_t i;
    uint32_t addr = stm32_onchip_flash.addr + offset;

    if( addr%4 != 0)
        ef_err_port_cnt++;

    for (i = 0; i < size; i++, addr++, buf++)
    {
        *buf = *(uint8_t *) addr;
    }
    on_ic_read_cnt++;
    return size;
}


static int write(long offset, const uint8_t *buf, size_t size)
{
    size_t   i;
    uint32_t addr = stm32_onchip_flash.addr + offset;

     uint32_t write_data ;
     uint32_t read_data  ;  

    if(addr % 4 != 0)
        ef_err_port_cnt++;

    FLASH_Unlock();
    for (i = 0; i < size; i += 4, buf += 4, addr += 4) {
        memcpy(&write_data, buf, 4); // Ensure alignment for FLASH_ProgramWord
        FLASH_ProgramWord(addr, write_data);
        read_data = *(uint32_t *)addr;
        /* You can add your code under here. */
        if (read_data != write_data) {
            FLASH_Lock(); 
            return -1;
        }
        else {
            // Flash operations can be time-consuming; feed the watchdog if necessary
            feed_dog();
        }
    }
    FLASH_Lock();

    on_ic_write_cnt++;
    return size;
}


static int erase(long offset, size_t size)
{
    uint32_t addr = stm32_onchip_flash.addr + offset;

    FLASH_Status flash_status;
    size_t erase_pages, i;

    erase_pages = size / PAGE_SIZE;
    if (size % PAGE_SIZE != 0) {
        erase_pages++;
    }

    FLASH_Unlock();

    for (i = 0; i < erase_pages; i++) {
        flash_status = FLASH_ErasePage(addr + (PAGE_SIZE * i));
        if (flash_status != FLASH_COMPLETE) {
            FLASH_Lock();
            return -1;
        } else {
            // Flash operations can be time-consuming; feed the watchdog if necessary
            feed_dog();
        }
    }

    FLASH_Lock();

    return size;
}


/*
  "stm32_onchip" : Flash 设备的名字。
  0x08000000: 对 Flash 操作的起始地址。
  1024*1024：Flash 的总大小（1MB）。
  128*1024：Flash 块/扇区大小（因为 STM32F2 各块大小不均匀，所以擦除粒度为最大块的大小：128K）。
  {init, read, write, erase} ：Flash 的操作函数。 如果没有 init 初始化过程，第一个操作函数位置可以置空。
  8 : 设置写粒度，单位 bit， 0 表示未生效（默认值为 0 ），该成员是 fal 版本大于 0.4.0 的新增成员。各个 flash 写入粒度不尽相同，可通过该成员进行设置，以下列举几种常见 Flash 写粒度：
  nor flash:  1 bit
  stm32f2/f4: 8 bit
  stm32f1:    32 bit
  stm32l4:    64 bit
 */

//1.定义 flash 设备

const struct fal_flash_dev stm32_onchip_flash =
{
    .name       = "stm32_onchip",
    .addr       = 0x08000000,
    .len        = 512*1024,
    .blk_size   = 2*1024,
    .ops        = {init, read, write, erase},
    .write_gran = 32
};
