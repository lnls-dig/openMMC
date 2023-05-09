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
 * @file stm32_pincfg.h
 * @brief Pin Config functions redirection for stm32f30x
 *
 * @author Adam Wujek <adam.wujek@cern.ch>, CERN
 */

#ifndef STM32_PINCFG_H_
#define STM32_PINCFG_H_

#include "port.h"

#define PIN_DEF( port, pin, func, dir ) (port)

#define PIN_PORT( x )      (x)
#define PIN_NUMBER( x )    (11) /* dummy values */
#define PIN_FUNC( x )      (12) /* dummy values */
#define PIN_DIR( x )       (14) /* dummy values */

#define NON_GPIO 0xFF

/* For other mcus like Atmel's it should be PORTA, PORTB, etc */
#define PORTA 1
#define PORTB 2
#define PORTC 3
#define PORTD 4
#define PORTE 5
#define PORTF 6

#endif
