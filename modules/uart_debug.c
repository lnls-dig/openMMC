/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
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
 *
 *   @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

/**
 * @file uart_debug.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief UART interface implementation
 */

/* Project includes */
#include "port.h"
#include "uart_debug.h"
#include "stdarg.h"

char debug_buf[100];

void uart_debug_init( uint32_t baud )
{
    Chip_Clock_SetPCLKDiv( SYSCTL_PCLK_UART3, SYSCTL_CLKDIV_2 );

    uart_init( UART_DEBUG );

    pin_config( UART_DEBUG_PORT, UART_DEBUG_TXD_PIN, ( IOCON_MODE_INACT | IOCON_FUNC3 ) );
    pin_config( UART_DEBUG_PORT, UART_DEBUG_RXD_PIN, ( IOCON_MODE_INACT | IOCON_FUNC3 ) );

    uart_set_baud( UART_DEBUG, baud );

    /* Defaults to 8N1, no parity */
    uart_config_data( UART_DEBUG, ( UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS ) );

    uart_tx_enable ( UART_DEBUG );
}

void uart_printf( uint8_t id, const char *format, ... )
{
    va_list args;

    va_start( args, format );
    ( void )vsprintf( debug_buf, format, args );

    uart_send( id, debug_buf, strlen(debug_buf)+1 );
}
