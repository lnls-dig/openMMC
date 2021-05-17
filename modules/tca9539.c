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

/**
 * @file   tca9539.c
 *
 * @brief  TCA9539 module interface functions implementations
 *
 * @ingroup TCA9539
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "tca9539.h"
#include "i2c.h"
#include "i2c_mapping.h"

/**
 * @brief  TCA9539 General register read
 *
 * @param chip_id   Chip ID
 * @param reg       Selected register
 * @param readout   Register value read
 *
 * @return Number of bytes read (0 if failure)
 */
static uint8_t tca9539_read_reg(uint8_t chip_id, uint8_t reg, uint8_t *readout)
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;

    if (readout == NULL) {
        return 0;
    }

    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, (TickType_t) 10)) {
        rx_len = xI2CMasterWriteRead(i2c_id, i2c_addr, reg, readout, 1);
        i2c_give(i2c_id);
    }

    return rx_len;
}

/**
 * @brief TCA9539 General register write
 *
 * @param chip_id   Chip ID
 * @param reg       Selected register
 * @param data      Value to write to register
 *
 * @return Number of bytes written
 */

static uint8_t tca9539_write_reg(uint8_t chip_id, uint8_t reg, uint8_t data)
{

    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t tx_len = 0;
    uint8_t cmd_data[2];

    cmd_data[0] = reg;
    cmd_data[1] = data;

    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, (TickType_t) 10)) {
        tx_len = xI2CMasterWrite(i2c_id, i2c_addr, cmd_data, sizeof(cmd_data));
        i2c_give(i2c_id);
    }

    return tx_len;
}

/* Pins Read/Write */
uint8_t tca9539_input_port_get(uint8_t chip_id, uint8_t port_num, uint8_t *readout)
{
    return tca9539_read_reg(chip_id, TCA9539_IN_REG + port_num, readout);
}

uint8_t tca9539_input_pin_get(uint8_t chip_id, uint8_t port_num, uint8_t pin, uint8_t *status)
{
    uint8_t rx_len = 0, pin_read = 0;

    rx_len = tca9539_input_port_get(chip_id, port_num, &pin_read);

    if (status) {
        *status = ((pin_read >> pin) & 0x1);
    }

    return rx_len;
}

uint8_t tca9539_output_port_set(uint8_t chip_id, uint8_t port_num, uint8_t data)
{
    return tca9539_write_reg(chip_id, TCA9539_OUT_REG + port_num, data);
}

uint8_t tca9539_output_port_get(uint8_t chip_id, uint8_t port_num, uint8_t *readout)
{
    return tca9539_read_reg(chip_id, TCA9539_OUT_REG + port_num, readout);
}

uint8_t tca9539_output_pin_set(uint8_t chip_id, uint8_t port_num, uint8_t pin, bool data)
{
    uint8_t output = 0;
    uint8_t rx_len, tx_len = 0;

    rx_len = tca9539_output_port_get(chip_id, port_num, &output);
    output &= ~(1 << pin);
    output |= (data << pin);

    if (rx_len) {
        tx_len = tca9539_output_port_set(chip_id, port_num, output);
    }

    return tx_len;
}

/* Polarity Control */
uint8_t tca9539_port_polarity_set(uint8_t chip_id, uint8_t port_num, uint8_t pol)
{
    return tca9539_write_reg(chip_id, TCA9539_IPOL_REG + port_num, pol);
}

uint8_t tca9539_port_polarity_get(uint8_t chip_id, uint8_t port_num, uint8_t *pol)
{
    return tca9539_read_reg(chip_id, TCA9539_IPOL_REG + port_num, pol);
}

/* Pins direction (output / input) */
uint8_t tca9539_port_dir_set(uint8_t chip_id, uint8_t port_num, uint8_t dir)
{
    return tca9539_write_reg(chip_id, TCA9539_CONFIG_REG + port_num, dir);
}

uint8_t tca9539_port_dir_get(uint8_t chip_id, uint8_t port_num, uint8_t *dir)
{
    return tca9539_read_reg(chip_id, TCA9539_CONFIG_REG + port_num, dir);
}

