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
 * @brief GPIO functions redirection for LPC17xx
 *
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 */

#ifdef LPC17_GPIO_H_
#undef LPC17_GPIO_H_
#endif
#define LPC17_GPIO_H_

#include "gpio_17xx_40xx.h"

#define GPIO_LEVEL_LOW  0
#define GPIO_LEVEL_HIGH 1

#define GPIO_DIR_INPUT  0
#define GPIO_DIR_OUTPUT 1

/**
 * @brief       Initialize GPIO block
 * @return      Nothing
 */
#define gpio_init()                            Chip_GPIO_Init( LPC_GPIO )

/**
 * @brief       Reads a GPIO pin state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : GPIO pin to get state for
 * @return      true (1) if the GPIO is high, false (0) if low
 */
#define gpio_read_pin( port, pin )             Chip_GPIO_GetPinState( LPC_GPIO, port, pin )

/**
 * @brief       Reads a GPIO port state
 * @param       port    : GPIO Port number
 * @return      Current state of all pins in the specified port
 */
#define gpio_read_port( port )                 Chip_GPIO_GetPortValue( LPC_GPIO, port )

/**
 * @brief       Set an individual GPIO output pin to the high state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
#define gpio_set_pin_high( port, pin )         Chip_GPIO_SetPinOutHigh( LPC_GPIO, port, pin )

/**
 * @brief       Set selected GPIO output pins to the high state
 * @param       port    : GPIO Port number where pin is located
 * @param       mask    : Selected pins to set high
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
#define gpio_set_port_high( port, mask )       Chip_GPIO_SetPortOutHigh( LPC_GPIO, port, mask )

/**
 * @brief       Set an individual GPIO output pin to the high state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
#define gpio_set_pin_low( port, pin )          Chip_GPIO_SetPinOutLow( LPC_GPIO, port, pin )

/**
 * @brief       Set selected GPIO output pins to the high state
 * @param       port    : GPIO Port number where pin is located
 * @param       mask    : Selected pins to set high
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
#define gpio_set_port_low( port, mask )        Chip_GPIO_SetPortOutLow( LPC_GPIO, port, mask )

/**
 * @brief       Toggle an individual GPIO output pin to the opposite state
 * @param       port    : GPIO Port number where the pin is located
 * @param       pin     : pin number
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
#define gpio_pin_toggle( port, pin )           Chip_GPIO_SetPinToggle( LPC_GPIO, port, pin )

/**
 * @brief       Set a GPIO pin to the specified state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @param       state   : true (1) for high, false (0) for low
 */
#define gpio_set_pin_state( port, pin, state ) Chip_GPIO_SetPinState( LPC_GPIO, port, pin, state )

/**
 * @brief       Set a GPIO pin direction
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @param       dir     : true (1) for OUTPUT, false (0) for INPUT
 */
#define gpio_set_pin_dir( port, pin, dir )     Chip_GPIO_SetPinDIR( LPC_GPIO, port, pin, dir )

/**
 * @brief       Get a GPIO pin direction
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @return      Current pin direction ( 0 for INPUT, 1 for OUTPUT )
 */
#define gpio_get_pin_dir( port, pin )     Chip_GPIO_GetPinDIR( LPC_GPIO, port, pin )

/**
 * @brief       Set a GPIO port direction
 * @param       port    : GPIO Port number
 * @param       dir     : Bitfield indicating all pins direction (true (1) for OUTPUT, false (0) for INPUT )
 */
#define gpio_set_port_dir( port, dir )     Chip_GPIO_SetPortDIR( LPC_GPIO, port, dir )

/**
 * @brief       Get a GPIO port direction
 * @param       port    : GPIO Port number
 * @return      Bitfield indicating all pins current direction (true (1) for OUTPUT, false (0) for INPUT )
 */
#define gpio_get_port_dir( port )     Chip_GPIO_GetPortDIR( LPC_GPIO, port )
