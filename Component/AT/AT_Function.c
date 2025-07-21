#include "AT_Function.h"

static uint8_t at_msg_buf[AT_MSG_SIZE];
AT_STATUS_t at_status[AT_STATUS_NUM] =
{
    [0] = {.status = AT_HW_INIT, .callback = AT_HW_INIT_CallBack},                  // AT硬件初始化
    [1] = {.status = AT_REGISTER, .callback = AT_REGISTER_CallBack},                // AT设备注册
    [2] = {.status = AT_INIT, .callback = AT_INIT_CallBack},                        // AT指令初始化
    [3] = {.status = AT_IDLE, .callback = AT_IDLE_CallBack},                        // AT空闲状态 解析平台下发的指令
    [4] = {.status = AT_UPDATA, .callback = AT_UPDATA_CallBack},                    // AT更新状态 上报状态至平台
    [5] = {.status = AT_GET_NTP, .callback = AT_GET_NTP_CallBack}                   // AT获取NTP时间
};

AT_Device_t AT_Device = {
    .CMD_TABLE = NULL,
    .URC_TABLE = NULL,
    .msg_buf = at_msg_buf,
    .init_step = 0,
    .rx_flag = &(AT_DEFAULT_UART_DEVICE.rx_flag),
    .status = AT_HW_INIT,
    .mqtt_params = 
    {
        .IP_Address = IP_ADDRESS,
        .Port = PORT_NUMBER,
        .Product_ID = PRODUCT_ID,
        .Device_Name = DEVICE_NAME,
        .SECRET_KEY = MY_SECRET_KEY,
    },
    .wifi_params = 
    {
        .WiFi_SSID = DEFAULT_WIFI_SSID,
        .WiFi_Password = DEFAULT_WIFI_PWD
    }
};

/***** 以下为测试时定义的变量 *****/
uint8_t j_1_value = 0;
char j_2_value[LOT_STRING_SIZE];
uint8_t j_3_value[LOT_ARRAY_SIZE];

uint8_t object_j_1_value = 0;
char object_j_2_value[LOT_STRING_SIZE];
uint8_t object_j_3_value[LOT_ARRAY_SIZE];

property_msg_t lot_msg_son[5] = {
//    [0] = {.key = "j_1", .type = bool_type, .value.property_value = &object_j_1_value},
//    [1] = {.key = "j_2", .type = string_type, .value.property_value = object_j_2_value},
//    [2] = {.key = "j_3", .type = array_type, .value.property_value = object_j_3_value},
};

property_msg_t lot_msg[LOT_MSG_SIZE] =
{
//    [0] = {
//        .key = "j_1",
//        .type = bool_type,
//        .value.property_value = &j_1_value,
//    },
//    [1] = {
//        .key = "j_2",
//        .type = string_type,
//        .value.property_value = j_2_value,
//    },
//    [2] = {
//        .key = "j_3",
//        .type = array_type,
//        .value.property_value = j_3_value,
//    },
//    [3] = {
//        .key = "object",
//        .type = object_type,
//        .value.child = (struct property_msg_t *)lot_msg_son,
//        .child_num = 3,
//    }
};
/********************/

/*at部分延时*/
static void at_delay_ms(uint32_t delay_time)
{
    rt_thread_mdelay(delay_time);
}

/*at数据发送*/
static void net_send(AT_Device_t *at_device, uint8_t *buf, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        while (USART_GetFlagStatus(at_device->uart_port->port, USART_FLAG_TXE) == RESET);
        USART_SendData(at_device->uart_port->port, buf[i]);
    }
    while (USART_GetFlagStatus(at_device->uart_port->port, USART_FLAG_TC) == RESET);
}

/*将数据从串口缓冲区复制到at缓冲区*/
void at_get_msg(AT_Device_t *at_device)
{
    ringbuf_get(at_device->uart_port->ring_buf, at_device->msg_buf, AT_MSG_SIZE);
}

/*获取滴答计时器时间*/
static int at_get_tick(void)
{
    return rt_tick_get();
}

/*清空相关的数组*/
static void at_clear(AT_Device_t *at_device)
{
    *(at_device->rx_flag) = 0;
    ring_reset(at_device->uart_port->ring_buf);
    memset(at_device->msg_buf, 0, AT_MSG_SIZE);
    uartbuf_clear(at_device->uart_port);
}

at_err_t AT_SendCmd(AT_Device_t *at_device, const char *cmd, const char *response, uint16_t timeout, uint8_t *data_buf)
{
    // 清空接收缓冲区
    at_clear(at_device);

    // 发送命令到串口（确保以\r\n结尾）
    char full_cmd[256];
    memset(full_cmd, 0, 256);
    snprintf(full_cmd, sizeof(full_cmd), "%s", cmd);
    net_send(at_device, (uint8_t *)full_cmd, strlen(full_cmd));

    // 等待应答
    uint32_t start_time = at_get_tick();
    while (at_get_tick() - start_time < timeout)
    {
        if (*(at_device->rx_flag) == 1)
        {
            at_get_msg(at_device);
            if (binary_strstr(at_device->msg_buf, AT_MSG_SIZE, response, strlen(response)))
            {
                if (data_buf != NULL)
                {
                    memcpy(data_buf, at_device->msg_buf, AT_MSG_SIZE);
                }
                at_clear(at_device);
                return 0; // 命令执行成功
            }
        }

        at_delay_ms(10);
    }
    return 1; // 命令执行失败或超时
}

at_err_t mqtt_pub(AT_Device_t *at_device, const char *topic, const char *data, uint16_t len)
{
    char cmd[128];
    uint8_t retry = 0;

    // 构造MQTT发布命令
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUBRAW=0,\"%s\",%d,0,0\r\n", topic, len);

    if (AT_SendCmd(at_device, cmd, ">", 500, NULL) == AT_CMD_OK)
    {
        if (AT_SendCmd(at_device, data, "OK", 500, NULL) == AT_CMD_OK)
        {
            return AT_MQTT_SEND_SUCCESS;
        }
    }
    return AT_MQTT_SEND_FAIL;
}

uint8_t set_ack(AT_Device_t *at_device, char *id)
{
    char ack_buf[64];
    memset(ack_buf, 0, 64);
    sprintf(ack_buf, "{\"id\": \"%s\",\"code\": 200,\"msg\": \"success\"}", id);
    return mqtt_pub(at_device, SET_ACK_TOPIC, ack_buf, strlen(ack_buf));
}

uint8_t lot_msg_parase(cJSON *root, property_msg_t *msg, uint16_t size)
{
    if (root == NULL || msg == NULL || size == 0)
    {
        return 1;
    }

    uint16_t i = 0;
    for (i = 0; i < size && msg[i].key != NULL; i++)
    {
        cJSON *value_js = cJSON_GetObjectItem(root, msg[i].key);
        if (value_js != NULL)
        {
            switch (msg[i].type)
            {
                case object_type:
                {
                    // 添加全面的指针和大小检查
                    if (msg[i].value.child == NULL || msg[i].child_num == 0)
                    {
                        return 1; // 无效的object配置
                    }

                    // 检查JSON节点类型
                    if (!cJSON_IsObject(value_js))
                    {
                        return 1; // 不是合法的JSON对象
                    }

                    // 递归解析时传递正确的子项数量
                    if (lot_msg_parase(value_js, (property_msg_t *)msg[i].value.child, msg[i].child_num) != 0)
                    {
                        return 1;
                    }
                    break;
                }
                case int_type:
                case bool_type:
                    if (msg[i].value.property_value != NULL)
                    {
                        *(msg[i].value.property_value) = value_js->valueint;
                    }
                    break;

                case float_type:
                    if (msg[i].value.property_value != NULL)
                    {
                        *(msg[i].value.property_value) = value_js->valuedouble;
                    }
                    break;

                case string_type:
                    if (msg[i].value.property_value != NULL && cJSON_IsString(value_js))
                    {
                        size_t max_len = property_value_max_size - 1;
                        size_t src_len = strlen(value_js->valuestring);
                        size_t copy_len = src_len < max_len ? src_len : max_len;
                        memcpy((char *)msg[i].value.property_value, value_js->valuestring, copy_len);
                        msg[i].value.property_value[copy_len] = '\0';
                    }
                    break;

                case array_type: 
                {
                    if (!cJSON_IsArray(value_js)) return 1;
                    uint16_t array_size = cJSON_GetArraySize(value_js);
                    if (array_size > LOT_ARRAY_SIZE) return 1;
                    
                    for (uint16_t j = 0; j < array_size; j++) {
                        cJSON *item = cJSON_GetArrayItem(value_js, j);
                        if (!item) return 1;
                        // 安全写入
                        if (j < LOT_ARRAY_SIZE) {
                            msg[i].value.property_value[j] = cJSON_IsString(item) ? 
                                atoi(item->valuestring) : item->valueint;
                        }
                    }
                    break;
                }

                default:
                    return 1; // 未知类型
            }
        }
        else
        {
            continue;
        }
    }
    return 0;
}

static uint8_t at_parase_callback(void *device, void *buf)
{
    uint8_t *data = buf;
    AT_Device_t *at_device = device;
    if (data != NULL)
    {
        // 使用strrchr查找最后一个}，确保找到完整的JSON结束标记
        char *json_start = strchr((char *)data, '{');
        char *json_end = strrchr((char *)data, '}');

        // 增加更严格的验证
        if (!json_start || !json_end || json_end < json_start)
        {
            at_device->status = AT_IDLE;
            return 1;
        }

        // 计算JSON长度时确保不越界
        size_t json_len = json_end - json_start + 1;
        if (json_len > MAX_JSON_SIZE)
        {
            at_device->status = AT_IDLE;
            return 2;
        }

        // 复制JSON字符串到临时缓冲区
        char json_buf[MAX_JSON_SIZE + 1];
        strncpy(json_buf, json_start, json_len);
        json_buf[json_len] = '\0';

        cJSON *IPD_js = cJSON_Parse(json_buf);
        if (IPD_js == NULL)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            // 记录错误日志
            return 3;
        }
        cJSON *id_js = cJSON_GetObjectItemCaseSensitive(IPD_js, "id");
        if (id_js != NULL)
        {
            uint8_t timeout = 5;
            while (set_ack(at_device, id_js->valuestring) != AT_MQTT_SEND_SUCCESS)
            {
                if (timeout-- == 0)
                {
                    // LOG_I("lot cmd ack fail");
                    break;
                }
                at_delay_ms(100);
            }
        }
        else
        {
            cJSON_Delete(IPD_js);
            return 4;
        }
        cJSON *params_js = cJSON_GetObjectItem(IPD_js, "params");
        if (params_js != NULL)
        {
            lot_msg_parase(params_js, lot_msg, LOT_MSG_SIZE);
        }
        cJSON_Delete(IPD_js);
    }
    return 0;
}

/*发生mqtt断链的回调指令的回调函数*/
static uint8_t MQTTDISCONNECTED_CallBack(void *device, void *data)
{
	AT_Device_t *at_device = device;
	at_device->init_step = 0;
	at_device->status = AT_INIT;
	
	return 0;
}

/*指令集*/
AT_CMD_t AT_Cmd_table[AT_COMMAND_ARRAY_SIZE] = {
    {"AT\r\n", "OK", 1000, NULL},
    {"AT+CWMODE=1\r\n", "OK", 1000, NULL},
    {"AT+CWDHCP=1,1\r\n", "OK", 1000, NULL},
    {"AT+MQTTCLEAN=0\r\n", "ERROR", 1000, NULL}, // 返回error说明当前无mqtt链接 可以进行新的连接
};

AT_URC_t AT_URC_table[AT_COMMAND_ARRAY_SIZE] = {
    {"+MQTTDISCONNECTED", MQTTDISCONNECTED_CallBack},
    {"+MQTTSUBRECV:", at_parase_callback},
    {"ERROR", MQTTDISCONNECTED_CallBack},

};

static void at_device_register(AT_Device_t *at_device, AT_CMD_t *cmd_table, AT_URC_t *urc_table)
{
    if (!at_device)
        return;

    at_device->uart_port = &AT_DEFAULT_UART_DEVICE;
    at_device->CMD_TABLE = cmd_table ? cmd_table : AT_Cmd_table;
    at_device->URC_TABLE = urc_table ? urc_table : AT_URC_table;
    at_device->init_step = 0;
}

void user_cmd_register(AT_Device_t *at_device)
{

    OneNET_Authorization("2018-10-31", at_device->mqtt_params.Product_ID, 1956499200, at_device->mqtt_params.SECRET_KEY, at_device->mqtt_params.Device_Name, at_device->mqtt_params.Token, sizeof(at_device->mqtt_params.Token), 1);

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+CWJAP=\"%s\",\"%s\"\r\n", at_device->wifi_params.WiFi_SSID, at_device->wifi_params.WiFi_Password);

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n");

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",
                    at_device->mqtt_params.Device_Name, at_device->mqtt_params.Product_ID, at_device->mqtt_params.Token);

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+MQTTCONNCFG=0,10,0,\"\",\"\",0,0\r\n");

    //    AT_Cmd_Register(at_device, "OK", 100, NULL, -1,
    //                    "AT+MQTTCLIENTID=0,\"%s\"\r\n",
    //                    clientId);

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+MQTTCONN=0,\"%s\",%d,1\r\n", IP_ADDRESS, PORT_NUMBER);

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+MQTTSUB=0,\"%s\",1\r\n", SET_TOPIC_ALL);
}

uint8_t AT_Cmd_Register(AT_Device_t *at_device, const char *response,
                        uint16_t timeout, void (*callback_response)(void),
                        int insert_count, const char *cmd, ...)
{
    if (!at_device || !cmd || !response)
    {
        //        LOG_E("Invalid parameters");
        return AT_ERR_INVALID_PARAM;
    }

    if (!at_device->CMD_TABLE)
    {
        //        LOG_E("CMD_TABLE not initialized");
        return AT_ERR_TABLE_NOT_INIT;
    }

    if (insert_count >= 0 && insert_count < AT_COMMAND_ARRAY_SIZE) 
    {
        if (at_device->CMD_TABLE[insert_count].cmd) 
        {
            free((void*)at_device->CMD_TABLE[insert_count].cmd);
        }
    }


    char cmd_buf[256];
    va_list ap;
    va_start(ap, cmd);
    vsnprintf(cmd_buf, sizeof(cmd_buf), cmd, ap);
    va_end(ap);

    // LOG_I("Registering command: %s", cmd_buf);

    int slot = -1;

    if (insert_count >= 0 && insert_count < AT_COMMAND_ARRAY_SIZE)
    {
        slot = insert_count;
    }
    else
    {
        for (int i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
        {
            if (at_device->CMD_TABLE[i].cmd == NULL)
            {
                slot = i;
                break;
            }
        }
    }

    if (slot == -1)
    {
        return AT_ERR_TABLE_FULL;
    }

    size_t cmd_len = strlen(cmd_buf) + 1;
    char *cmd_copy = (char *)malloc(cmd_len);
    if (!cmd_copy)
    {
        return AT_ERR_MEMORY;
    }
    strncpy(cmd_copy, cmd_buf, cmd_len);

    if (at_device->CMD_TABLE[slot].cmd)
    {
        free((char *)at_device->CMD_TABLE[slot].cmd);
    }

    at_device->CMD_TABLE[slot] = (AT_CMD_t){
        .cmd = cmd_copy,
        .response = response,
        .timeout = timeout,
        .callback_response = callback_response};

    return AT_CMD_ADD_SUCCESS;
}

void AT_HW_INIT_CallBack(void *device)
{
    AT_Device_t *at_device = device;
    // 初始化UART

    at_device->status = AT_REGISTER;
}

void AT_INIT_CallBack(void *device)
{
    AT_Device_t *at_device = device;

    if (at_device->init_step >= AT_COMMAND_ARRAY_SIZE)
    {
        at_device->init_step = 0;
        at_device->status = AT_IDLE;
        return;
    }
    else if (at_device->CMD_TABLE[at_device->init_step].cmd == NULL)
    {
        at_device->init_step = 0;
        at_device->status = AT_IDLE;
        return;
    }
    else
    {
        if (AT_SendCmd(at_device, at_device->CMD_TABLE[at_device->init_step].cmd, at_device->CMD_TABLE[at_device->init_step].response, at_device->CMD_TABLE[at_device->init_step].timeout, NULL) == AT_CMD_OK)
        {
            if (at_device->CMD_TABLE[at_device->init_step].callback_response != NULL)
            {
                at_device->CMD_TABLE[at_device->init_step].callback_response();
            }
            at_device->init_step++;
        }
        else
        {
            //					at_device->init_step = 0;
        }
    }
}

void AT_REGISTER_CallBack(void *device)
{
    AT_Device_t *at_device = device;

    // 注册at设备
    at_device_register(at_device, NULL, NULL);

    // 添加用户自定义at指令
    user_cmd_register(at_device);

    // 进入at初始化状态
    at_device->status = AT_INIT;
}

void AT_IDLE_CallBack(void *device)
{
    AT_Device_t *at_device = device;

    if (!at_device || !(*(at_device->rx_flag)))
        return;

    *(at_device->rx_flag) = 0;

    at_get_msg(at_device);

    for (uint16_t i = 0; i < AT_COMMAND_ARRAY_SIZE && at_device->URC_TABLE[i].urc_msg; i++)
    {
        char *urc_msg = strstr((char *)at_device->msg_buf, at_device->URC_TABLE[i].urc_msg);
        if (urc_msg)
        {
            if (at_device->URC_TABLE[i].callback)
            {
                at_device->URC_TABLE[i].callback(at_device, urc_msg);
            }
            at_clear(at_device);
            return;
        }
    }
    at_device->status = AT_IDLE;
}

void AT_UPDATA_CallBack(void *device)
{
    AT_Device_t *at_device = device;

    // Update task logic can be implemented here
//        lot_msg_t msg;
//        lot_create_root(&msg);
//        lot_Add_Number(&msg, "test_time", Unix_Time);
//        lot_generate_str(&msg);
//        mqtt_pub(at_device, POST_TOPIC, msg.str, msg.len);
//        lot_clean(&msg);

    at_device->status = AT_IDLE;
}

void AT_GET_NTP_CallBack(void *device)
{
    AT_Device_t *at_device = device;

    at_device->status = AT_IDLE;
}

void AT_poll(AT_Device_t *at_device) 
{
    if (!at_device) return;
    
    AT_STATUS_TYPE_t index_status = at_device->status;

    if (index_status >= AT_STATUS_NUM || at_status[index_status].callback == NULL) 
    {
        return;
    }
    at_status[index_status].callback(at_device);
}

