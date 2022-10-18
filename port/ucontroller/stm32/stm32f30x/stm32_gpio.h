/*
 *   openMMC  --
 *
 *   Copyright (C) 2019  CERN
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
 * @brief GPIO functions redirection for STM32
 *
 * @author Adam Wujek  <adam.wujek@cern.ch>, CERN
 */

#ifndef STM32_GPIO_H_
#define STM32_GPIO_H_

#define GPIO_LEVEL_HIGH Bit_SET
#define GPIO_LEVEL_LOW  Bit_RESET

#define PORTS_MAX 7
#define PINS_MAX 16


struct pin_def {
    uint16_t port;
    uint16_t pin;
    GPIOMode_TypeDef mode;
    GPIOPuPd_TypeDef pupd;
    uint32_t af;
    GPIOOType_TypeDef type;
};

/**
 * @brief       Initialize GPIO block
 * @return      Nothing
 */
void gpio_init(void);

/**
 * @brief       Reads a GPIO pin state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : GPIO pin to get state for
 * @return      true (1) if the GPIO is high, false (0) if low
 */
int gpio_read_pin( int port, int pin );

/**
 * @brief       Set an individual GPIO output pin to the high state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
void gpio_set_pin_high( int port, int pin );

/**
 * @brief       Set an individual GPIO output pin to the high state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
void gpio_set_pin_low( int port, int pin );

/**
 * @brief       Toggle an individual GPIO output pin to the opposite state
 * @param       port    : GPIO Port number where the pin is located
 * @param       pin     : pin number
 * @note        This commands only applies for pins selected as outputs. Writing '0' shouldn't affect the pin state
 */
void gpio_pin_toggle(uint8_t portx, uint8_t pinx);

/**
 * @brief       Set a GPIO pin to the specified state
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @param       state   : true (1) for high, false (0) for low
 */
void gpio_set_pin_state( int port, int pin, int state );

/**
 * @brief       Set a GPIO pin direction
 * @param       port    : GPIO Port number where pin is located
 * @param       pin     : pin number
 * @param       dir     : true (1) for OUTPUT, false (0) for INPUT
 */
void gpio_set_pin_dir(uint8_t portx, uint8_t pinx, uint8_t dir);

void gpio_configure_pin(uint8_t port);

#endif
