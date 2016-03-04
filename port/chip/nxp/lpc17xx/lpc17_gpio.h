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
 * @file gpio.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief Definitions of GPIO driver implementation for LPC17xx
 */

#ifdef LPC17_GPIO_H_
#undef LPC17_GPIO_H_
#endif
#define LPC17_GPIO_H_

#include "gpio_17xx_40xx.h"

#define INPUT 0
#define OUTPUT 1
#define LOW 0
#define HIGH 1

#define gpio_init()                            Chip_GPIO_Init( LPC_GPIO )
#define gpio_read_pin( port, pin )             Chip_GPIO_GetPinState( LPC_GPIO, port, pin )
#define gpio_read_port( port )                 Chip_GPIO_GetPortValue( LPC_GPIO, port )
#define gpio_set_pin( port, pin )              Chip_GPIO_SetPinOutHigh( LPC_GPIO, port, pin )
#define gpio_set_port( port, mask )            Chip_GPIO_SetPortOutHigh( LPC_GPIO, port, mask )
#define gpio_clr_pin( port, pin )              Chip_GPIO_SetPinOutLow( LPC_GPIO, port, pin )
#define gpio_clr_port( port, mask )            Chip_GPIO_SetPortOutLow( LPC_GPIO, port, mask )
#define gpio_pin_toggle( port, pin )           Chip_GPIO_SetPinToggle( LPC_GPIO, port, pin )
#define gpio_set_pin_state( port, pin, state ) Chip_GPIO_SetPinState( LPC_GPIO, port, pin, state )
#define gpio_set_pin_dir( port, pin, dir )     Chip_GPIO_SetPinDIR( LPC_GPIO, port, pin, dir )
