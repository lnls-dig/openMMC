#include "FreeRTOS.h"

#define xI2CMasterWrite(id, addr, tx_buff, tx_len) Chip_I2C_MasterSend(id, addr, tx_buff, tx_len)
#define xI2CMasterRead(id, addr, rx_buff, rx_len) Chip_I2C_MasterRead(id, addr, rx_buff, rx_len)
#define xI2CMasterWriteRead(id, addr, cmd, rx_buff, rx_len) Chip_I2C_MasterCmdRead(id, addr, cmd, rx_buff, rx_len)

uint8_t xI2CSlaveReceive( I2C_ID_T id, uint8_t * rx_buff, uint8_t buff_len, TickType_t timeout );
void vI2CSlaveSetup ( I2C_ID_T id, uint8_t slave_addr );
void vI2CConfig( I2C_ID_T id, uint32_t speed );

