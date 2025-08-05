#ifndef _AT_FUNCTION_H_
#define _AT_FUNCTION_H_

/*add user lib*/
#include "uart.h"
#include "cJSON.h"
#include "lot_function.h"
#include "onenet.h"
#include "sys.h"                  // Device header

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

#define AT_RST_PORT     GPIOC
#define AT_RST_PIN      GPIO_Pin_5


#define AT_COMMAND_ARRAY_SIZE 16
#define AT_MSG_SIZE 256                       //AT设备消息缓冲区大小

#define MAX_JSON_SIZE							1024

/*平台连接参数宏定义*/
#define NTP_SERVER "time.windows.com"

#define DEFAULT_WIFI_SSID 					"main" // 默认WiFi SSID
#define DEFAULT_WIFI_PWD  					"12345678" // 默认WiFi密码

#define IP_ADDRESS 									"mqtts.heclouds.com"
#define PORT_NUMBER 								1883
#define PRODUCT_ID 									"2Its5wq8a3"
#define DEVICE_NAME 								"lot_device"
#define MY_SECRET_KEY               "VDZEcm1HRFRaOG9mZ2NGeEtwenRkd25hQ0dIWTZDVGo="
#define TOKEN    										"version=2018-10-31&res=products%2F2Its5wq8a3%2Fdevices%2Flot_device&et=1924876800&method=md5&sign=yEio49aeh%2BTf3XPlD8OyPQ%3D%3D"

#define POST_TOPIC		    					"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/post"
#define SET_TOPIC_ALL     					"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/#"
#define OTA_TOPIC_GET     					"$sys/"PRODUCT_ID"/"DEVICE_NAME"/ota/inform"
#define OTA_TOPIC_ACK     					"$sys/"PRODUCT_ID"/"DEVICE_NAME"/ota/inform_reply"


#define SET_TOPIC          				 	"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set"
#define SET_ACK_TOPIC       				"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set_reply"

#define AT_STATUS_NUM 8
typedef enum
{
    AT_HW_INIT = 0,
    AT_REGISTER,
    AT_INIT,
    AT_IDLE,
    AT_UPDATA,
    AT_GET_NTP,
} AT_STATUS_TYPE_t;

typedef struct
{
	AT_STATUS_TYPE_t status;
	void (*callback)(void *params);
}AT_STATUS_t;

typedef struct
{
    const char *cmd;                            // AT command string
    const char *response;                       // Expected response string
    uint16_t timeout;                     // Timeout in milliseconds
    void (*callback_response)(void);     // Expected response string
} AT_CMD_t;

typedef struct
{
    const char *urc_msg;              // AT command string
    uint8_t (*callback)(void *device ,void *a);     // Expected response string
} AT_URC_t;

typedef struct
{
  char *IP_Address;               // IP 地址
  uint16_t Port;                  // 端口号
  char *Product_ID;              // 产品 ID
  char *Device_Name;              // 设备名称
  char *SECRET_KEY;              // 设备密钥
  char Token[256];                    // 令牌
}mqtt_connect_params_t;

typedef struct
{
  /* data */
  char WiFi_SSID[16];                // WiFi SSID
  char WiFi_Password[16];            // WiFi 密码
}wifi_params_t;

// 定义 AT 设备结构体
typedef struct {
    AT_STATUS_TYPE_t status;                 // 设备状态
    AT_CMD_t *CMD_TABLE;                // AT 命令数组指针
    AT_URC_t *URC_TABLE;                // AT 命令数组指针
    uart_device_t *uart_port;         // UART 设备指针
    uint8_t *msg_buf;                   // 消息缓冲区
		uint8_t init_step;									//初始化步骤
    uint8_t *rx_flag;                   //接收数据标志位
    mqtt_connect_params_t mqtt_params; // MQTT 连接参数
    wifi_params_t wifi_params;   // WiFi 连接参数
} AT_Device_t;

typedef enum
{
	/*AT指令相关错误类型*/
	AT_CMD_OK = 0,                          //AT指令发送成功
	AT_ERR_INVALID_PARAM,                   //AT指令参数错误
	AT_ERR_TIMEOUT,                         //AT指令应答超时
	AT_ERR_ACK,                             //AT指令应答错误
	
	/*AT指令列表相关错误类型*/
  AT_CMD_ADD_SUCCESS,                     //AT指令添加成功
	AT_ERR_TABLE_NOT_INIT,                  //AT设备指令集未初始化
	AT_ERR_TABLE_FULL,                      //AT指令集已满
	AT_ERR_MEMORY,                          //内存不足
	
	/*NTP相关错误类型*/
  AT_NTP_GET_SUCCESS,                     //NTP指令获取成功
	AT_NTP_SEND_ERROR,                      //NTP获取指令发送失败
  AT_NTP_ACK_ERROR,                       //NTP指令应答失败
	AT_NTP_PARSE_ERROR,
	
	/*MQTT相关错误类型*/
	AT_MQTT_SEND_SUCCESS,                   //MQTT指令上报成功
	AT_MQTT_SEND_FAIL,                      //MQTT指令上报失败

}at_err_t;


extern property_msg_t lot_msg[LOT_MSG_SIZE];
extern AT_Device_t AT_Device;

void AT_poll(AT_Device_t *at_device);
uint8_t AT_Cmd_Register(AT_Device_t *at_device, const char *response,
                        uint16_t timeout, void (*callback_response)(void),
                        int insert_count, const char *cmd, ...);
at_err_t AT_SendCmd(AT_Device_t *at_device, const char *cmd, const char *response, uint16_t timeout , uint8_t *data_buf);
const void* binary_strstr(const void* haystack, size_t h_len, const void* needle, size_t n_len);
void AT_HW_INIT_CallBack(void *device);
void AT_INIT_CallBack(void *device);
void AT_REGISTER_CallBack(void *device);
void AT_IDLE_CallBack(void *device);
void AT_UPDATA_CallBack(void *device);
void AT_GET_NTP_CallBack(void *device);

#endif