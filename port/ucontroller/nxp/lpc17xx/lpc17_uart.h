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
 * @file gpio.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief Definitions of GPIO driver implementation for LPC17xx
 */

#ifndef LPC17_UART_H_
#define LPC17_UART_H_

void uart_init( uint8_t id );
void uart_set_baud( uint8_t id, uint32_t baud );
void uart_tx_enable( uint8_t id );
void uart_tx_disable( uint8_t id );
size_t uart_send( uint8_t id, char *tx_data, size_t len );
size_t uart_read( uint8_t id, char *rx_data, size_t len );

#endif
