#ifndef _ONENET_H_
#define _ONENET_H_
// #include "uart.h"
#include "cJSON.h"
#include "base64.h"
#include "hmac_sha1.h"
#include "stm32f10x.h"                  // Device header

/*add c lib*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include <stdarg.h>
#include "time.h"

unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag);

#endif
