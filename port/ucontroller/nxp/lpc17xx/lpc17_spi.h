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
 * @file lpc17_spi.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief Legacy SPI driver definitions for LPC17xx
 */

#ifndef LPC17xx_SPI_H_
#define LPC17xx_SPI_H_

#include "spi_17xx_40xx.h"

#define spi_init(id)                    Chip_SPI_Init(id)
#define spi_deinit(id)                  Chip_SPI_DeInit(id)
#define spi_flush_rx(id)                Chip_SPI_Int_FlushData(id)
#define spi_set_bitrate(id, bitrate)    Chip_SPI_SetBitRate(id, bitrate)

#define SPI_SLAVE                       0
#define SPI_MASTER                      1
#define SPI_INTERRUPT                   0
#define SPI_POLLING                     1

void spi_config( uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll );
uint32_t spi_write( uint8_t * buffer, uint32_t buffer_len );
void spi_assertSSEL(void);
void spi_deassertSSEL(void);
#endif
