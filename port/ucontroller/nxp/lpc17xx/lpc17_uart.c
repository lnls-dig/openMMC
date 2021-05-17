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
 * @date June 2016
 *
 * @brief
 */

#include "port.h"

#ifdef CHIP_LPC177X_8X
const lpc_uart_cfg_t usart_cfg[4] = {
    {LPC_UART0, UART0_IRQn},
    {LPC_UART1, UART1_IRQn},
    {LPC_UART2, UART2_IRQn},
    {LPC_UART3, UART3_IRQn}
};

void uart_init( uint8_t id )
{
    Chip_UART_Init( usart_cfg[id].ptr );
    
    Chip_IOCON_PinMuxSet(LPC_IOCON, UART_DEBUG_PORT, UART_DEBUG_TXD_PIN, ( IOCON_MODE_INACT | IOCON_FUNC2 ) );
    Chip_IOCON_PinMuxSet(LPC_IOCON, UART_DEBUG_PORT, UART_DEBUG_RXD_PIN, ( IOCON_MODE_INACT | IOCON_FUNC2 ) );

    uart_set_baud( UART_DEBUG, 115200 );

    /* Defaults to 8N1, no parity */
    uart_config_data( UART_DEBUG, ( UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS ) );

    uart_tx_enable ( UART_DEBUG );
}
#else
const lpc_uart_cfg_t usart_cfg[4] = {
    { LPC_UART0, UART0_IRQn, SYSCTL_PCLK_UART0 },
    { LPC_UART1, UART1_IRQn, SYSCTL_PCLK_UART1 },
    { LPC_UART2, UART2_IRQn, SYSCTL_PCLK_UART2 },
    { LPC_UART3, UART3_IRQn, SYSCTL_PCLK_UART3 }
};

void uart_init ( uint8_t id )
{
    Chip_Clock_SetPCLKDiv( usart_cfg[id].sysclk, SYSCTL_CLKDIV_2 );

    Chip_UART_Init( usart_cfg[id].ptr );

    /* Standard 19200 baud rate */
    uart_set_baud( UART_DEBUG, 19200 );

    /* Defaults to 8N1, no parity */
    uart_config_data( UART_DEBUG, ( UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS ) );

    uart_tx_enable ( UART_DEBUG );
}
#endif
