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
 * @file uart_debug.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief UART interface implementation
 */

#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include "port.h"

#undef putchar

#ifdef MODULE_UART_DEBUG

/**
 * @brief Macro to call uart debug printf function
 *
 * @warning When MODULE_UART_DEBUG is not defined, this macro simply throws out its args and do nothing
 *
 * @param msg Formatted string to print
 *
 */
#define putchar(c) uart_send(UART_DEBUG, &c, 1)

#else

#define putchar(c) (void)0

#endif
#endif
