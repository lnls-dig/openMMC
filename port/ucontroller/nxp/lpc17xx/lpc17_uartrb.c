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
    {LPC_UART0, UART0_IRQn},
    {LPC_UART1, UART1_IRQn},
    {LPC_UART2, UART2_IRQn},
    {LPC_UART3, UART3_IRQn}
};

/* Transmit and receive ring buffer sizes - MUST be power of 2 */
#define UART_SRB_SIZE 256       /* Send */
#define UART_RRB_SIZE 32        /* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

void UART0_IRQHandler(void)
{
    Chip_UART_IRQRBHandler(LPC_UART0, &rxring, &txring);
}

void UART1_IRQHandler(void)
{
    Chip_UART_IRQRBHandler(LPC_UART1, &rxring, &txring);
}

void UART2_IRQHandler(void)
{
    Chip_UART_IRQRBHandler(LPC_UART2, &rxring, &txring);
}

void UART3_IRQHandler(void)
{
    Chip_UART_IRQRBHandler(LPC_UART3, &rxring, &txring);
}

void uart_init( uint8_t id )
{
    Chip_UART_Init( usart_cfg[id].ptr );

    /* THR Interrupt enable */
    uart_int_enable( id, UART_IER_THREINT );

    RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
    RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

    /* Enable UARTn interruptions */
    NVIC_SetPriority( usart_cfg[id].irq, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( usart_cfg[id].irq );
}
