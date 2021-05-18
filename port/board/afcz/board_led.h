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

#ifndef BOARD_LED_H_
#define BOARD_LED_H_

#define LED_CNT 3

#include "port.h"

enum LED_ID {
    LED_BLUE = 0,
    LED1, /* Red */
    LED2, /* Green */
    LED_MAX_CNT
};

extern const uint32_t led_pincfg[LED_MAX_CNT];
extern const LEDConfig_t leds_config[LED_MAX_CNT];
#ifdef MODULE_RTM
extern const LEDConfig_t rtm_leds_config[LED_MAX_CNT];
#endif

#endif /* BOARD_LED_H_ */
