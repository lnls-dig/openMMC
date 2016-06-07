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

#include "FreeRTOS.h"
#include "port.h"
#include "string.h"

volatile lpc_uart_cfg_t usart_cfg[4] = {
    {LPC_UART0, UART0_IRQn, NULL, NULL, 0},
    {LPC_UART1, UART1_IRQn, NULL, NULL, 0},
    {LPC_UART2, UART2_IRQn, NULL, NULL, 0},
    {LPC_UART3, UART3_IRQn, NULL, NULL, 0}
};

void uart_handler( uint8_t id )
{
    uint32_t int_id = Chip_UART_ReadIntIDReg( usart_cfg[id].ptr );
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if ( int_id & UART_IIR_INTID_THRE ) {
        if ( usart_cfg[id].tx_str[usart_cfg[id].sent_bytes] != '\0' ) {
            Chip_UART_SendByte( usart_cfg[id].ptr, usart_cfg[id].tx_str[usart_cfg[id].sent_bytes] );
            usart_cfg[id].sent_bytes++;
        } else {
            xSemaphoreGiveFromISR( usart_cfg[id].smphr, &xHigherPriorityTaskWoken );
            usart_cfg[id].sent_bytes = 0;
            vPortFreeFromISR( usart_cfg[id].tx_str );
        }
    }

    /* If xHigherPriorityTaskWoken was set to true we should yield. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void UART0_IRQHandler(void)
{
    uart_handler( 0 );
}

void UART1_IRQHandler(void)
{
    uart_handler( 1 );
}

void UART2_IRQHandler(void)
{
    uart_handler( 2 );
}

void UART3_IRQHandler(void)
{
    uart_handler( 3 );
}

void uart_init( uint8_t id )
{
    Chip_UART_Init( usart_cfg[id].ptr );

    usart_cfg[id].smphr = xSemaphoreCreateBinary();
    xSemaphoreGive( usart_cfg[id].smphr );

    /* THR Interrupt enable */
    uart_int_enable( id, UART_IER_THREINT );

    /* Enable UARTn interruptions */
    NVIC_SetPriority( usart_cfg[id].irq, configMAX_SYSCALL_INTERRUPT_PRIORITY+5 );
    NVIC_EnableIRQ( usart_cfg[id].irq );
}

size_t uart_send( uint8_t id, char *tx_data, size_t len )
{
    uint8_t i;

    if ( xSemaphoreTake( usart_cfg[id].smphr, USART_SMPHR_TIMEOUT ) ) {
        usart_cfg[id].sent_bytes = 0;

        /* TX FIFO Reset */
        Chip_UART_SetupFIFOS( usart_cfg[id].ptr, UART_FCR_TX_RS );

        /* Fill TX FIFO */
        for ( i = 0; ( ( i < UART_TX_FIFO_SIZE ) && ( i < len ) ); i++, usart_cfg[id].sent_bytes++ ) {
            Chip_UART_SendByte( usart_cfg[id].ptr, *tx_data++ );
        }

        /* FIFO is full, save the rest of the data in a buffer */
        usart_cfg[id].tx_str = pvPortMalloc( len - usart_cfg[id].sent_bytes );
        if ( usart_cfg[id].tx_str ) {
            memcpy( usart_cfg[id].tx_str, tx_data, len - usart_cfg[id].sent_bytes );
        }
        usart_cfg[id].sent_bytes = 0;

        return len;
    } else {
        return 0;
    }
}

#if 0
size_t uart_read( uint8_t id, char *rx_data, size_t len )
{
    LPC_USART_T *pUART = get_lpc_usart( id );
    return Chip_UART_Read( pUART, rx_data, len );
}
#endif
