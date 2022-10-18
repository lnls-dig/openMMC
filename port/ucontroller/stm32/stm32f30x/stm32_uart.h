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
 * @file smt32_uart.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief STM32F30x UART interface definitions
 */

#ifndef STM32_UART_H_
#define STM32_UART_H_

#include "string.h"
#include "stdio.h"

#define STM32_UART_ENABLE_RX 0x1
#define STM32_UART_ENABLE_TX 0x2
#define STM32_UART_ENABLE_RX_IRQ 0x4
#define STM32_UART_ENABLE_TX_IRQ 0x8
#define STM32_UART_FIX_CRLF 0x10

struct stm32_uart_device_priv;

typedef struct stm32_uart_cfg {
    USART_TypeDef *periph_base;
    int baudrate;
    uint32_t periph;
    void (*periph_func)(uint32_t RCC_APBXPeriph, FunctionalState NewState);

    uint32_t flags;

    int irq_id;
    USART_InitTypeDef USART_InitStructure;
    struct stm32_uart_device_priv *priv;
} stm32_uart_cfg_t;

/* Add +1 since they're counted from 1 */
#define UART_MAX_CNT (5+1)

extern stm32_uart_cfg_t usart_cfg[UART_MAX_CNT];


#if 0

#define uart_config_data( id, cfg ) Chip_UART_ConfigData( usart_cfg[id].ptr, cfg )
#define uart_tx_enable( id ) Chip_UART_TXEnable( usart_cfg[id].ptr )
#define uart_tx_disable( id ) Chip_UART_TXDisable( usart_cfg[id].ptr )
#define uart_int_enable( id, mask ) Chip_UART_IntEnable( usart_cfg[id].ptr, mask )
#define uart_int_disable( id, mask ) Chip_UART_IntDisable( usart_cfg[id].ptr, mask )
#define uart_send_char( id, ch ) usart_blocking_write(id, ch)
#define uart_read_char( id ) usart_blocking_read( id )
#define uart_send( id, msg, len ) usart_write_buf(id, msg, len )
#define uart_read( id, buf, len ) Chip_UART_ReadBlocking( usart_cfg[id].ptr, buf, len )

#endif

void uart_init ( uint8_t id );
void uart_send(int id, uint8_t *buff, int len);
int uart_recv(int id, uint8_t *buff, int len);

#endif
