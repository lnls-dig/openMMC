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
 * @file port.h
 * @author Adam Wujek <adam.wujek@cern.ch>, CERN
 * @date March 2019
 *
 * @brief Port layer (includes all portable functions headers)
 */

#ifndef PORT_H_
#define PORT_H_

#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

/* List of all STM32F30x specific headers to be included */
#include "stm32f30x_adc.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_usart.h"

#include "stm32_pincfg.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_timer.h"

#include "pin_mapping.h"
#include "i2c_mapping.h"
#include "stm32_i2c.h"

#define IPMB_TXTASK_STACK_SIZE 512
#define IPMB_RXTASK_STACK_SIZE 512
#define IPMITASK_STACK_SIZE 2048

#endif
