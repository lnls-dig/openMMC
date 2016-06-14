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

#include "port.h"
#include "jtag.h"

void scansta_init( void )
{
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR0_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR1_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR2_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR3_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR4_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR5_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR6_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_RST_PIN, OUTPUT);

    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR0_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR1_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR2_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR3_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR4_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR5_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR6_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_RST_PIN, HIGH);
}
