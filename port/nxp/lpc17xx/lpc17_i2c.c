#include "FreeRTOS.h"
#include "port.h"
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
 * @brief       I2C0 Interrupt handler
 * @return      None
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

void Board_I2C_Init(I2C_ID_T id)
{
    switch (id) {
    case I2C0:
        Chip_IOCON_PinMux(LPC_IOCON, 0, 27, IOCON_MODE_INACT, IOCON_FUNC1);
        Chip_IOCON_PinMux(LPC_IOCON, 0, 28, IOCON_MODE_INACT, IOCON_FUNC1);
        Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
        break;

    case I2C1:
        Chip_IOCON_PinMux(LPC_IOCON, 0,  0, IOCON_MODE_INACT, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 0,  1, IOCON_MODE_INACT, IOCON_FUNC3);
        Chip_IOCON_EnableOD(LPC_IOCON, 0,  0);
        Chip_IOCON_EnableOD(LPC_IOCON, 0,  1);
/*
        Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC2);
        Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC2);
        Chip_IOCON_EnableOD(LPC_IOCON, 0, 19);
        Chip_IOCON_EnableOD(LPC_IOCON, 0, 20);
        */

        break;

    case I2C2:
        Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC2);
        Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC2);
        Chip_IOCON_EnableOD(LPC_IOCON, 0, 10);
        Chip_IOCON_EnableOD(LPC_IOCON, 0, 11);
        break;
    default:
        break;
    }
}

void vI2CConfig( I2C_ID_T id, uint32_t speed )
{
    IRQn_Type irq;

    switch (id) {
    case I2C0:
	irq = I2C0_IRQn;
	break;
    case I2C1:
	irq = I2C1_IRQn;
	break;
    case I2C2:
	irq = I2C2_IRQn;
	break;
    }

    Board_I2C_Init(id);
    Chip_I2C_Init(id);
    Chip_I2C_SetClockRate(id, speed);
    NVIC_SetPriority( irq, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( irq );
    Chip_I2C_Enable( id );

    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
}

/*! @todo Change the I2C driver functions to expect the number of bytes sent/received instead of an error (follow gpio pattern)
 * The exception of this would be xSlaveReceive, that is implemented differently from LPCOpen, so it would be defined in the .c file as is now. The other functions (master write/read/write&read) could just be aliased to LPCOpen ones */
#ifdef macro_test
i2c_err xI2CMasterWrite( I2C_ID_T id, uint8_t addr, uint8_t * tx_buff, uint8_t tx_len )
{
    uint8_t sent_bytes = 0;

    sent_bytes = 
    if ( sent_bytes == tx_len ) {
        return i2c_err_SUCCESS;
    } else {
        return i2c_err_FAILURE;
    }
}

i2c_err xI2CMasterRead( I2C_ID_T id, uint8_t addr, uint8_t * rx_buff, uint8_t rx_len )
{
    uint8_t recv_bytes = 0;

    recv_bytes = 

    if ( recv_bytes == rx_len ) {
        return i2c_err_SUCCESS;
    } else {
        return i2c_err_FAILURE;
    }
}

i2c_err xI2CMasterWriteRead( I2C_ID_T id, uint8_t addr, uint8_t cmd, uint8_t* rx_buff, uint8_t rx_len )
{
    uint8_t recv_bytes = 0;

    recv_bytes = 

    if ( recv_bytes == rx_len ) {
        return i2c_err_SUCCESS;
    } else {
        return i2c_err_FAILURE;
    }
}
#endif
static TaskHandle_t slave_task_id;
I2C_XFER_T slave_cfg;
uint8_t recv_msg[i2cMAX_MSG_LENGTH];
uint8_t recv_bytes;

uint8_t xI2CSlaveReceive( I2C_ID_T id, uint8_t * rx_buff, uint8_t buff_len, TickType_t timeout )
{
    uint8_t bytes_to_copy = 0;
    slave_task_id = xTaskGetCurrentTaskHandle();

    if ( ulTaskNotifyTake( pdTRUE, timeout ) == pdTRUE )
    {
	if (recv_bytes > buff_len) {
	    bytes_to_copy = buff_len;
	} else {
	    bytes_to_copy = recv_bytes;
	}
        /* Copy the rx buffer to the pointer given */
        memcpy( rx_buff, &recv_msg[0], bytes_to_copy );
        return bytes_to_copy;
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

void vI2CSlaveSetup ( I2C_ID_T id, uint8_t slave_addr )
{
    slave_cfg.slaveAddr = slave_addr;
    slave_cfg.txBuff = NULL; /* Not using Slave transmitter right now */
    slave_cfg.txSz = 0;
    slave_cfg.rxBuff = &recv_msg[0];
    slave_cfg.rxSz = (sizeof(recv_msg)/sizeof(recv_msg[0]));
    Chip_I2C_SlaveSetup( id, I2C_SLAVE_0, &slave_cfg, I2C_Slave_Event, SLAVE_MASK);
}
