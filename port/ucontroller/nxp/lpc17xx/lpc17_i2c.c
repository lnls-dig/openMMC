/*
 *   openMMC  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * @file lpc17_i2c.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief I2C driver for LPC17xx
 */

#include "port.h"
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

void vI2CConfig( I2C_ID_T id, uint32_t speed )
{
    IRQn_Type irq;

    switch (id) {
    case I2C0:
        irq = I2C0_IRQn;
        /* Set special I2C pins configuration */
#ifdef CHIP_LPC175X_6X
        Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
#endif
        break;
    case I2C1:
        irq = I2C1_IRQn;
#ifdef CHIP_LPC175X_6X
        /* Set special I2C pins configuration */
        Chip_IOCON_EnableOD(LPC_IOCON, PIN_PORT(I2C1_SDA), PIN_NUMBER(I2C1_SDA));
        Chip_IOCON_EnableOD(LPC_IOCON, PIN_PORT(I2C1_SCL), PIN_NUMBER(I2C1_SCL));
#endif
        break;
    case I2C2:
        irq = I2C2_IRQn;
#ifdef CHIP_LPC175X_6X
        /* Set special I2C pins configuration */
        Chip_IOCON_EnableOD(LPC_IOCON, PIN_PORT(I2C2_SDA), PIN_NUMBER(I2C2_SDA));
        Chip_IOCON_EnableOD(LPC_IOCON, PIN_PORT(I2C2_SCL), PIN_NUMBER(I2C2_SCL));
#endif
        break;
    default:
        return;
    }

    Chip_I2C_Init(id);
    Chip_I2C_SetClockRate(id, speed);
    NVIC_SetPriority( irq, configMAX_SYSCALL_INTERRUPT_PRIORITY -1 );
    NVIC_EnableIRQ( irq );
    Chip_I2C_Enable( id );

    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
}

static TaskHandle_t slave_task_id;
I2C_XFER_T slave_cfg;
uint8_t recv_msg[i2cMAX_MSG_LENGTH];
uint8_t recv_bytes;

uint8_t xI2CSlaveReceive( I2C_ID_T id, uint8_t * rx_buff, uint8_t buff_len, uint32_t timeout )
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
    /* expects i2c addr < 0x80 */
    slave_addr <<= 1;
    slave_cfg.slaveAddr = slave_addr;
    slave_cfg.txBuff = NULL; /* Not using Slave transmitter right now */
    slave_cfg.txSz = 0;
    slave_cfg.rxBuff = &recv_msg[0];
    slave_cfg.rxSz = (sizeof(recv_msg)/sizeof(recv_msg[0]));
    Chip_I2C_SlaveSetup( id, I2C_SLAVE_0, &slave_cfg, I2C_Slave_Event, SLAVE_MASK);
}
