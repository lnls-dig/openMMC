#include "FreeRTOS.h"
#include "i2c_17xx_40xx.h"
#include "i2c.h"
#include "string.h"

#define SLAVE_MASK 0xFF

/* State machine handler for I2C0 and I2C1 */
static void i2c_state_handling(I2C_ID_T id)
{
    if (Chip_I2C_IsMasterActive(id)) {
        Chip_I2C_MasterStateHandler(id);
    } else {
        Chip_I2C_SlaveStateHandler(id);
    }
}

/**
 * @brief	I2C0 Interrupt handler
 * @return	None
 */
void I2C0_IRQHandler(void)
{
    i2c_state_handling(I2C0);
}

void I2C1_IRQHandler(void)
{
    i2c_state_handling(I2C1);
}

void I2C2_IRQHandler(void)
{
    i2c_state_handling(I2C2);
}

i2c_err port_I2C_Master_Write( I2C_ID_T id, uint8_t addr, uint8_t * tx_buff, uint8_t tx_len )
{
    uint8_t sent_bytes = 0;

    sent_bytes = Chip_I2C_MasterSend(id, addr, tx_buff, tx_len);
    if ( sent_bytes == tx_len ) {
        return i2c_err_SUCCESS;
    } else {
        return i2c_err_FAILURE;
    }
}

i2c_err port_I2C_Master_Read( I2C_ID_T id, uint8_t addr, uint8_t * rx_buff, uint8_t rx_len )
{
    uint8_t recv_bytes = 0;

    recv_bytes = Chip_I2C_MasterRead(id, addr, rx_buff, rx_len);

    if ( recv_bytes == rx_len ) {
        return i2c_err_SUCCESS;
    } else {
        return i2c_err_FAILURE;
    }
}

i2c_err port_I2C_Master_Write_Read( I2C_ID_T id, uint8_t addr, uint8_t cmd, uint8_t* rx_buff, uint8_t rx_len )
{
    uint8_t recv_bytes = 0;

    recv_bytes = Chip_I2C_MasterCmdRead(id, addr, cmd, rx_buff, rx_len);

    if ( recv_bytes == rx_len ) {
        return i2c_err_SUCCESS;
    } else {
        return i2c_err_FAILURE;
    }
}

static TaskHandle_t slave_task_id;
I2C_XFER_T slave_cfg;
uint8_t recv_msg[i2cMAX_MSG_LENGTH];
uint8_t recv_bytes;

uint8_t port_I2C_Slave_Receive( I2C_ID_T id, uint8_t * rx_buff, uint8_t buff_len, TickType_t timeout )
{
    slave_task_id = xTaskGetCurrentTaskHandle();

    if ( ulTaskNotifyTake( pdTRUE, timeout ) == pdTRUE )
    {
        /* Copy the rx buffer to the pointer given */
        memcpy( rx_buff, &recv_msg[0], recv_bytes );
        return recv_bytes;
    } else {
        return 0;
    }
}

static void I2C_Slave_Event(I2C_ID_T id, I2C_EVENT_T event)
{
    static BaseType_t xHigherPriorityTaskWoken;
    switch (event) {
    case I2C_EVENT_DONE:
        recv_bytes = i2cMAX_MSG_LENGTH - slave_cfg.rxSz;
        slave_cfg.rxSz = i2cMAX_MSG_LENGTH;
        slave_cfg.rxBuff = &recv_msg[0];

        vTaskNotifyGiveFromISR( slave_task_id, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

    case I2C_EVENT_SLAVE_RX:
        break;
    default:
        break;
    }
}

void port_I2C_Slave_Setup ( I2C_ID_T id, uint8_t slave_addr)//, uint8_t * rx_buff, uint8_t buff_len )
{
    slave_cfg.slaveAddr = slave_addr;
    slave_cfg.txBuff = NULL; /* Not using Slave transmitter right now */
    slave_cfg.txSz = 0;
    slave_cfg.rxBuff = &recv_msg[0];
    slave_cfg.rxSz = (sizeof(recv_msg)/sizeof(recv_msg[0]));
    Chip_I2C_SlaveSetup( id, I2C_SLAVE_0, &slave_cfg, I2C_Slave_Event, SLAVE_MASK);
}
