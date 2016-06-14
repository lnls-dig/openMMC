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

/*!
 * @file uart_debug.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date May 2016
 * @brief UART interface implementation
 */

#include "port.h"

void uart_debug_init( uint32_t baud );
void uart_printf( uint8_t id, const char *format, ... );

#ifdef MODULE_UART_DEBUG

#define DEBUG_MSG(msg, ...) uart_printf(UART_DEBUG, msg, ##__VA_ARGS__)
#define DEBUG_CH(ch) uart_send_char(UART_DEBUG, ch)

#else

#undef DEBUG_MSG
#undef DEBUG_CH

#define DEBUG_MSG(...) (void)0
#define DEBUG_CH(...) (void)0

#endif
