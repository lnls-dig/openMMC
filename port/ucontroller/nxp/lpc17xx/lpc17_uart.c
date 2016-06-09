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

const lpc_uart_cfg_t usart_cfg[4] = {
    {LPC_UART0, UART0_IRQn},
    {LPC_UART1, UART1_IRQn},
    {LPC_UART2, UART2_IRQn},
    {LPC_UART3, UART3_IRQn}
};
