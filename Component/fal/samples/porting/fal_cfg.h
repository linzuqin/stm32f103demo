/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

//#include <rtconfig.h>
//#include <board.h>

#define NOR_FLASH_DEV_NAME             "norflash0"
#define FAL_PART_HAS_TABLE_CFG
/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32f4_onchip_flash;
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          	\
{              																												\
		&stm32f4_onchip_flash,																								\
    &nor_flash0,                                           						\
}																																			\

/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD,       "bl",           "stm32_onchip",         0*1024,     128*1024, 0}, \
    {FAL_PART_MAGIC_WORD,       "app",          "stm32_onchip",         128*1024,   384*1024, 0}, \
    {FAL_PART_MAGIC_WORD,       "download",      NOR_FLASH_DEV_NAME,    0*1024,     384*1024, 0}, \
    {FAL_PART_MAGIC_WORD,       "factory",       NOR_FLASH_DEV_NAME,    384*1024,   640*1024, 0}, \
		{FAL_PART_MAGIC_WORD, 			"fdb_kvdb1", 			NOR_FLASH_DEV_NAME,   1024*1024, 2*1024*1024, 0}, \
    {FAL_PART_MAGIC_WORD, 			"fdb_tsdb1", 			NOR_FLASH_DEV_NAME, 	3*1024*1024, 3*1024*1024, 0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
