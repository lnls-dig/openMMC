/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Krzysztof Macias <krzysztof.macias@creotech.pl>
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

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "idt_8v54816.h"
#include "i2c.h"
#include "i2c_mapping.h"

uint8_t clock_switch_read_reg(uint8_t *rx_data)
{
    uint8_t i2c_addr, i2c_interface;
    uint8_t rx_len = 0;

    if ( i2c_take_by_chipid( CHIP_ID_8V54816, &i2c_addr, &i2c_interface, portMAX_DELAY ) && ( rx_data != NULL ) ) {

        rx_len = xI2CMasterRead( i2c_interface, i2c_addr, rx_data, 16 );
        i2c_give( i2c_interface );
    }
    return rx_len;
}

uint8_t clock_switch_write_reg(uint8_t *tx_data)
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t tx_len = 0;
    if ( i2c_take_by_chipid( CHIP_ID_8V54816, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {
        tx_len = xI2CMasterWrite( i2c_interf, i2c_addr, tx_data, 16);
        i2c_give( i2c_interf );
    }
    return tx_len;
}

uint8_t clock_switch_set_single_channel(uint8_t channel, uint8_t value)
{
    uint8_t data[16] = {0};
    if (clock_switch_read_reg(data)){
        data[channel] = value;
        return clock_switch_write_reg(data);
    }
    return 0;
}

uint8_t clock_switch_read_single_channel(uint8_t channel, uint8_t *data)
{
    uint8_t all_regs[16] = {0};
    if (clock_switch_read_reg(all_regs)){
        *data = all_regs[channel];
        return 1;
    }
    return 0;
}
