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
 * @file lpc17_uart.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief
 */

#include "port.h"

static LPC_USART_T * get_lpc_usart( uint8_t id )
{
    switch( id ) {
    case 0:
	return LPC_UART0;
    case 1:
	return LPC_UART0;
    case 2:
	return LPC_UART0;
    case 3:
	return LPC_UART0;
    }

    return NULL;
}

void uart_init( uint8_t id )
{
    LPC_USART_T *pUART = get_lpc_usart( id );
    Chip_UART_Init( pUART );
}

void uart_set_baud( uint8_t id, uint32_t baud )
{
    LPC_USART_T *pUART = get_lpc_usart( id );
    Chip_UART_SetBaud( pUART, baud );
}

void uart_tx_enable( uint8_t id )
{
    LPC_USART_T *pUART = get_lpc_usart( id );
    Chip_UART_TXEnable( pUART );
}

void uart_tx_disable( uint8_t id )
{
    LPC_USART_T *pUART = get_lpc_usart( id );
    Chip_UART_TXDisable( pUART );
}

size_t uart_send( uint8_t id, char *tx_data, size_t len )
{
    LPC_USART_T *pUART = get_lpc_usart( id );
    return Chip_UART_Send( pUART, tx_data, len );
}

size_t uart_read( uint8_t id, char *rx_data, size_t len )
{
    LPC_USART_T *pUART = get_lpc_usart( id );
    return Chip_UART_Read( pUART, rx_data, len );
}
