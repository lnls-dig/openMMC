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
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief Port layer (includes all portable functions headers)
 */

#ifndef PORT_H_
#define PORT_H_

/* List of all LPC17xx specific headers to be included */

#ifdef CHIP_LPC177X_8X
#include "chip_lpc177x_8x.h"
#else
#include "chip_lpc175x_6x.h"
#include "lpc17_spi.h"
#endif

#include "lpc17_gpio.h"
#include "lpc17_i2c.h"
#include "lpc17_ssp.h"
#include "lpc17_watchdog.h"
#include "lpc17_interruptions.h"
#include "lpc17_hpm.h"
#include "lpc17_power.h"
#include "lpc17_pincfg.h"
#include "pin_mapping.h"
#include "arm_cm3_reset.h"

#ifdef UART_RINGBUFFER
#include "lpc17_uartrb.h"
#else
#include "lpc17_uart.h"
#endif

#endif
