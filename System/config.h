#ifndef _CONFIG_H_
#define _CONFIG_H_

// <<< Use Configuration Wizard in Context Menu >>>

// <s> Author
// <i> Project author: ÁÖ×æÇÕ create 2025/8/17
#define AUTHOR "ÁÖ×æÇÕ"

// <h> Project Information

// <s> Version
// <i> Defines the project version
#define Version "1.0"
// </h>

// <h> Modbus Master
// <q> Modbus Master Enable
// <i> Enable Modbus Master functionality
#define MODBUS_MASTER_ENABLE 0

#if MODBUS_MASTER_ENABLE
// <o> Modbus Master Device
//     <0=> USART1
//     <1=> USART2
//     <2=> USART3
// <i> Select serial port for Modbus Master
#define MB_M_DEVICE 2  // Default: UART3 (2)
#endif
// </h>

// <h> Modbus Slave
// <q> Modbus Slave Enable
// <i> Enable Modbus Slave functionality
#define MODBUS_SLAVE_ENABLE 0

#if MODBUS_SLAVE_ENABLE
// <o> Modbus Slave Device
//     <0=> USART1
//     <1=> USART2
//     <2=> USART3
// <i> Select serial port for Modbus Slave
#define MB_S_DEVICE_1 1  // Default: uart_devices[1]

// <o> Modbus Slave Address <1-247>
// <i> Set device address for Modbus Slave (1-247)
#define MODBUS_SLAVE_ADDR 1

// <o> Modbus Slave Baudrate
//     <9600=> 9600
//     <19200=> 19200
//     <38400=> 38400
//     <57600=> 57600
//     <115200=> 115200 (Default)
// <i> Set baudrate for Modbus Slave
#define MODBUS_SLAVE_BOUND 115200
#endif
// </h>

// <h> Flash Storage Configuration
// <q> Use Off-Chip Flash
// <i> Enable off-chip flash storage
#define OFF_CHIP 1

#if OFF_CHIP
// <o> Off-Chip SPI Selection
//     <0=> SPI1 (Default)
//     <1=> SPI2
// <i> Select which SPI to use for off-chip flash
#define OFF_CHIP_SPI 0  // Default: SPI1
#endif
// </h>

// <h> AT Device Configuration

// <q> AT Device Enable
// <i> Enable AT Device functionality
#define AT_Device_ENABLE 0

// <o> AT Device Device
//     <0=> USART1
//     <1=> USART2
//     <2=> USART3
// <i> Select which UART to use for AT
#define AT_UART_DEVICE 0  // Default: USART1(uart_devices[0])

// <s> WiFi SSID
// <i> Set the WiFi network name (SSID)
#define WIFI_SSID "main"

// <s> WiFi Password
// <i> Set the WiFi password
#define WIFI_PASSWORD "12345678"

// <h> MQTT Client Configuration
// <q> Enable MQTT Client
// <i> Enable MQTT protocol for communication
#define MQTT_CLIENT_ENABLE 1

#if MQTT_CLIENT_ENABLE
// <s> MQTT Broker Address
// <i> Set MQTT broker domain or IP
#define MQTT_BROKER_ADDRESS "mqtts.heclouds.com"

// <o> MQTT Broker Port
// <i> Set MQTT broker port (default: 1883)
#define MQTT_BROKER_PORT 1883

// <s> MQTT Client ID
// <i> Set unique MQTT client identifier
#define MQTT_CLIENT_ID "lot_device"

// <s> MQTT Username
// <i> Set username for MQTT authentication
#define MQTT_USERNAME "2Its5wq8a3"

// <s> MQTT Password
// <i> Set password for MQTT authentication
#define MQTT_PASSWORD "VDZEcm1HRFRaOG9mZ2NGeEtwenRkd25hQ0dIWTZDVGo="

// <o> MQTT Keep Alive (seconds)
// <i> Set keep alive interval (default: 60)
#define MQTT_KEEP_ALIVE 60

// <o> Enable MQTT TLS
// <i> Enable secure MQTT connection (MQTTS)
#define MQTT_USE_TLS 0
#endif
// </h>

// </h>

#endif
