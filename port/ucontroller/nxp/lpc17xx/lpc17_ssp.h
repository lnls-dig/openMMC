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

/**
 * @file lpc17_ssp.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief SSP driver definitions for LPC17xx
 */

#ifndef LPC17xx_SSP_H_
#define LPC17xx_SSP_H_

#ifdef CHIP_LPC177X_8X
#include "chip_lpc177x_8x.h"
#else
#include "chip_lpc175x_6x.h"
#endif

#include "ssp_17xx_40xx.h"
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief SSP Interfaces count in LPC17xx
 */
#define MAX_SSP_INTERFACES      2

/**
 * @brief Macro to retrieve the pointer to LPC_SSP registers based on id number
 */
#define SSP(n)                  LPC_SSP##n

#define SSP_SLAVE        0
#define SSP_MASTER       1
#define SSP_INTERRUPT    0
#define SSP_POLLING      1

/**
 * @brief Slave select states
 */
enum ssel_state {
    ASSERT = 0,
    DEASSERT
};

/**
 * @brief SSP Interface config struct
 */
typedef struct ssp_config {
    LPC_SSP_T * lpc_id;
    IRQn_Type irq;
    uint32_t ssel_pin;
    uint8_t polling;
    uint8_t frame_size;
    Chip_SSP_DATA_SETUP_T xf_setup;
    TaskHandle_t caller_task;
} ssp_config_t;

void ssp_init( uint8_t id, uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll );
void ssp_ssel_control( uint8_t id, uint8_t state );
void ssp_write_read( uint8_t id, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len, uint32_t timeout );

#define ssp_chip_init(id)                             Chip_SSP_Init(SSP(id))
#define ssp_chip_deinit(id)                           Chip_SSP_DeInit(SSP(id))
#define ssp_flush_rx(id)                              Chip_SSP_Int_FlushData(SSP(id))
#define ssp_set_bitrate(id, bitrate)                  Chip_SSP_SetBitRate(SSP(id), bitrate)
#define ssp_write(id, buffer, buffer_len)             ssp_write_read(id, buffer, buffer_len, NULL, 0, 0)
#define ssp_read(id, buffer, buffer_len, timeout)     ssp_write_read(id, NULL, 0, buffer, buffer_len, timeout)

#endif
