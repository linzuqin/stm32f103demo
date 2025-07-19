使用前需要定义的函数
at_delay_ms     毫秒级的延时函数
net_send	   串口发送函数
at_get_msg      将数据从串口缓冲区复制到at设备的缓冲区 uart_rx_buf --->at_device->msg_buf
at_get_tick	   获取滴答定时时间
at_clear		   配置清空数组等操作
AT_HW_INIT_TASK		串口初始化
user_cmd_register		修改自定义的AT指令


在#include "AT_Function.h"中可以定义连接相关参数


使用方式：
1.定义一个at设备接收数组，例如uint8_t at_msg_buf[AT_MSG_SIZE];
2.定义at设备句柄
AT_Device_t AT_Device = {
    .status = AT_HW_INIT,					---初始化状态									
    .CMD_TABLE = NULL,													
    .URC_TABLE = NULL,													
    .msg_buf = at_msg_buf,					---定义接收数组						
    .init_step = 0,							---定义初始化步骤
    .rx_flag = &(uart_devices[2].rx_flag),		---定义接收标志位
};
3.线程中调用 AT_poll(&AT_Device);
