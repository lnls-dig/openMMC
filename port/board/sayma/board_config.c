/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Wojciech Ruclo <wojciech.ruclo@creotech.pl>
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

#include "board_config.h"
#include "port.h"

void board_init(void)
{
    // make sure address is valid
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR1), PIN_NUMBER(GPIO_I2C_MUX_ADDR1), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR2), PIN_NUMBER(GPIO_I2C_MUX_ADDR2), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_SW_RESETn), PIN_NUMBER(GPIO_SW_RESETn), GPIO_LEVEL_HIGH);

    // I2C FIX for 1776
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));

    // Enable RTM P3V3_MP
    if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS))) {
        gpio_set_pin_state(PIN_PORT(GPIO_EN_RTM_MP), PIN_NUMBER(GPIO_EN_RTM_MP), true);
    }
}

void board_config()
{
}
