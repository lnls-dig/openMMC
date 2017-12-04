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
#include "scansta1101.h"

void scansta1101_init( void )
{
    gpio_set_pin_state( PIN_PORT(GPIO_SCANSTA1101_ADDR0), PIN_NUMBER(GPIO_SCANSTA1101_ADDR0), GPIO_LEVEL_LOW );
    gpio_set_pin_state( PIN_PORT(GPIO_SCANSTA1101_ADDR1), PIN_NUMBER(GPIO_SCANSTA1101_ADDR1), GPIO_LEVEL_LOW );
    gpio_set_pin_state( PIN_PORT(GPIO_SCANSTA1101_ADDR2), PIN_NUMBER(GPIO_SCANSTA1101_ADDR2), GPIO_LEVEL_LOW );
    gpio_set_pin_state( PIN_PORT(GPIO_SCANSTA1101_ADDR3), PIN_NUMBER(GPIO_SCANSTA1101_ADDR3), GPIO_LEVEL_LOW );
    gpio_set_pin_state( PIN_PORT(GPIO_SCANSTA1101_ADDR4), PIN_NUMBER(GPIO_SCANSTA1101_ADDR4), GPIO_LEVEL_LOW );
    gpio_set_pin_state( PIN_PORT(GPIO_SCANSTA1101_ADDR5), PIN_NUMBER(GPIO_SCANSTA1101_ADDR5), GPIO_LEVEL_LOW );
    gpio_set_pin_state( PIN_PORT(GPIO_SCANSTA1101_ADDR6), PIN_NUMBER(GPIO_SCANSTA1101_ADDR6), GPIO_LEVEL_LOW );
}
