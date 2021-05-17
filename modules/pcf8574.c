/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Henrique Silva <henrique.silva@lnls.br>
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

/*
 * PCF8574.c
 *
 *  Created on: 28-07-2014
 *      Author: Bartek
 */

#include "FreeRTOS.h"

#include "port.h"
#include "pcf8574.h"
#include "i2c.h"
#include "i2c_mapping.h"
#include "pin_mapping.h"

void pcf8574_set_port_dir_output(uint8_t pin_mask)
{
    uint8_t i2c_addr, i2c_id;
    uint8_t state = pcf8574_read_port();

    if (state == PCF8574_READ_ERROR) {
        return;
    }

    uint8_t data = state & (~pin_mask);

    if (i2c_take_by_chipid(CHIP_ID_RTM_PCF8574A, &i2c_addr, &i2c_id, (TickType_t) 200)) {
        xI2CMasterWrite(i2c_id, i2c_addr, &data, sizeof(data));
        i2c_give(i2c_id);
    }
}

void pcf8574_set_port_dir_input(uint8_t pin_mask)
{
    uint8_t i2c_addr, i2c_id;
    uint8_t state = pcf8574_read_port();

    if (state == PCF8574_READ_ERROR) {
        return;
    }

    uint8_t data = state | pin_mask;

    if (i2c_take_by_chipid(CHIP_ID_RTM_PCF8574A, &i2c_addr, &i2c_id, (TickType_t) 200)) {
        xI2CMasterWrite(i2c_id, i2c_addr, &data, sizeof(data));
        i2c_give(i2c_id);
    }
}

void pcf8574_set_port_high(uint8_t pin_mask)
{
    uint8_t i2c_addr, i2c_id;
    uint8_t state = pcf8574_read_port();

    if (state == PCF8574_READ_ERROR) {
        return;
    }

    uint8_t data = state | pin_mask;

    if (i2c_take_by_chipid(CHIP_ID_RTM_PCF8574A, &i2c_addr, &i2c_id, (TickType_t) 200)) {
        xI2CMasterWrite(i2c_id, i2c_addr, &data, sizeof(state));
        i2c_give(i2c_id);
    }
}

void pcf8574_set_port_low(uint8_t pin_mask)
{
    uint8_t i2c_addr, i2c_id;
    uint8_t state = pcf8574_read_port();

    if (state == PCF8574_READ_ERROR) {
        return;
    }

    uint8_t data = state & (~pin_mask);

    if (i2c_take_by_chipid(CHIP_ID_RTM_PCF8574A, &i2c_addr, &i2c_id, (TickType_t) 200)) {
        xI2CMasterWrite(i2c_id, i2c_addr, &data, sizeof(data));
        i2c_give(i2c_id);
    }
}

uint8_t pcf8574_read_port()
{
    uint8_t i2c_addr, i2c_id;
    uint8_t data_rx = PCF8574_READ_ERROR;

    if (i2c_take_by_chipid(CHIP_ID_RTM_PCF8574A, &i2c_addr, &i2c_id, (TickType_t) 200)) {

        /* Check if read was successful */
        if (xI2CMasterRead(i2c_id, i2c_addr, &data_rx, 1) == 0) {
            return PCF8574_READ_ERROR;
        }

        i2c_give(i2c_id);
    }

    return data_rx;
}

uint8_t pcf8574_read_pin(uint8_t pin_num)
{
    pcf8574_set_port_high(1 << pin_num);
    uint8_t state = pcf8574_read_port();

    return (state & (1 << pin_num));
}
