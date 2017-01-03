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
 * @file lpc17_ssp.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief SSP driver for LPC17xx
 */

#include "port.h"
#include "string.h"
#include "pin_mapping.h"

static ssp_config_t ssp_cfg[MAX_SSP_INTERFACES] = {
    [FPGA_SPI] = {
        .lpc_id = LPC_SSP0,
        .irq = SSP0_IRQn,
        .ssel_pin = SSP0_SSEL,
    },
    [FLASH_SPI] = {
        .lpc_id = LPC_SSP1,
        .irq = SSP1_IRQn,
        .ssel_pin = SSP1_SSEL,
    }
};

static void ssp_irq_handler( LPC_SSP_T * ssp_id )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    Chip_SSP_DATA_SETUP_T * xf_setup;
    uint8_t ssp_cfg_index;

    ssp_cfg_index = (ssp_id == LPC_SSP0) ? 0 : 1;

    xf_setup = &ssp_cfg[ssp_cfg_index].xf_setup;

    /* Disable SSP interrupts */
    Chip_SSP_Int_Disable(ssp_id);

    if (ssp_cfg[ssp_cfg_index].frame_size <= 8) {
        Chip_SSP_Int_RWFrames8Bits(ssp_id, xf_setup);
    }
    else {
        Chip_SSP_Int_RWFrames16Bits(ssp_id, xf_setup);
    }

    if ((xf_setup->rx_cnt != xf_setup->length) || (xf_setup->tx_cnt != xf_setup->length)) {
        /* Enable ssp interrupts, we're going to read/write more data */
        Chip_SSP_Int_Enable(ssp_id);
    }
    else {
        /* Transfer is completed, notify the caller task */
        vTaskNotifyGiveFromISR(ssp_cfg[ssp_cfg_index].caller_task, &xHigherPriorityTaskWoken);
        /* Deassert SSEL pin */
        ssp_ssel_control(ssp_cfg_index, DEASSERT);
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void SSP0_IRQHandler( void )
{
    ssp_irq_handler(LPC_SSP0);
}

void SSP1_IRQHandler( void )
{
    ssp_irq_handler(LPC_SSP1);
}

/*! @brief Function that controls the Slave Select (SSEL) signal
 * This pin is controlled manually because the internal SSP driver resets the SSEL pin every 8 bits that are transfered
 */
void ssp_ssel_control( uint8_t id, uint8_t state )
{
    gpio_set_pin_state( PIN_PORT(ssp_cfg[id].ssel_pin), PIN_NUMBER(ssp_cfg[id].ssel_pin), state );
}

void ssp_init( uint8_t id, uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll )
{
    ssp_cfg[id].polling = poll;
    ssp_cfg[id].frame_size = frame_sz;

    /* Set up clock and muxing for SSP0/1 interface */
    Chip_IOCON_Init(LPC_IOCON);

    Chip_SSP_Init(ssp_cfg[id].lpc_id);
    Chip_SSP_SetBitRate(ssp_cfg[id].lpc_id, bitrate);
    Chip_SSP_SetMaster(ssp_cfg[id].lpc_id, master_mode);
    Chip_SSP_SetFormat(ssp_cfg[id].lpc_id, (frame_sz-1), SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA0_CPOL0);
    Chip_SSP_Enable(ssp_cfg[id].lpc_id);

    if (!poll) {
        /* Configure interruption priority and enable it */
        NVIC_SetPriority( ssp_cfg[id].irq, configMAX_SYSCALL_INTERRUPT_PRIORITY );
        NVIC_EnableIRQ( ssp_cfg[id].irq );
    }

}

uint8_t *tx_ssp;
uint8_t *rx_ssp;

void ssp_write_read( uint8_t id, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len, uint32_t timeout )
{
    Chip_SSP_DATA_SETUP_T * data_st = &ssp_cfg[id].xf_setup;

    tx_ssp = pvPortMalloc(tx_len);
    rx_ssp = pvPortMalloc(rx_len+tx_len);

    memcpy(tx_ssp, tx_buf, tx_len);

    ssp_cfg[id].caller_task = xTaskGetCurrentTaskHandle();
    data_st->tx_cnt = 0;
    data_st->rx_cnt = 0;
    data_st->tx_data = tx_ssp;
    data_st->rx_data = rx_ssp;
    data_st->length = rx_len+tx_len;

    /* Assert Slave Select pin to enable the transfer */
    ssp_ssel_control(id, ASSERT);

    if (ssp_cfg[id].polling) {
        Chip_SSP_RWFrames_Blocking(ssp_cfg[id].lpc_id, data_st);
        ssp_ssel_control(id, DEASSERT);
    } else {
        Chip_SSP_Int_FlushData(ssp_cfg[id].lpc_id);

        /* Enable interrupt-based data transmission */
        Chip_SSP_Int_Enable( ssp_cfg[id].lpc_id );

        /* User defined timeout ? */
        /* Wait until the transfer is finished */
        ulTaskNotifyTake(pdTRUE, timeout);
    }
    if (rx_buf && rx_len > 0) {
        memcpy(rx_buf, rx_ssp, rx_len+tx_len);
    }
    vPortFree(rx_ssp);
    vPortFree(tx_ssp);
}
