#ifndef _CONFIG_H_
#define _CONFIG_H_

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Project Version
// <s> Version
// <i> Defines the project version
#define Version "1.0"
// </h>

// <h> Modbus Master
// <o> Modbus Master Enable
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

// <h> Modbus Slabe
// <o> Modbus Slave Enable
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
// <o> Use Off-Chip Flash
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

// <o> AT Device Enable
// <i> Enable AT Device functionality
#define AT_Device_ENABLE 1

// <o> AT Device Device
//     <0=> USART1
//     <1=> USART2
//     <2=> USART3
// <i> Select which UART to use for AT
#define AT_UART_DEVICE 0  // Default: USART1(uart_devices[0])

// </h>

#endif
