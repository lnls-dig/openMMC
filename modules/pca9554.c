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

/**
 * @file   pca9554.c
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  PCA9554 module interface functions implementations
 *
 * @ingroup PCA9554
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "pca9554.h"
#include "i2c.h"
#include "i2c_mapping.h"

/**
 * @brief  PCA9554 General register read
 *
 * @param[in]  chip_id Chip ID to communicate
 * @param[in]  reg     Selected register
 * @param[out] readout Register value read
 *
 * @return Number of bytes read (1 if successful, 0 if failure)
 */
static uint8_t pca9554_read_reg ( uint8_t chip_id, uint8_t reg, uint8_t *readout )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;

    if (readout == NULL) {
        return 0;
    }

    if( i2c_take_by_chipid( chip_id, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        rx_len = xI2CMasterWriteRead(i2c_id, i2c_addr, &reg, 1, readout, 1);
        i2c_give(i2c_id);
    }
    return rx_len;
}

/**
 * @brief PCA9554 General register write
 *
 * @param[in]  chip_id Chip ID to communicate
 * @param[in]  reg     Selected register
 * @param[in]  data    Value to write to register
 *
 * @return Number of bytes written
 */
static uint8_t pca9554_write_reg ( uint8_t chip_id, uint8_t reg, uint8_t data )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t cmd_data[2] = {reg, data};
    uint8_t tx_len = 0;

    if( i2c_take_by_chipid( chip_id, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        tx_len = xI2CMasterWrite(i2c_id, i2c_addr, cmd_data, sizeof(cmd_data));
        i2c_give(i2c_id);
    }

    return tx_len;
}

/* Pins Read/Write */
uint8_t pca9554_read_port( uint8_t chip_id, uint8_t *readout )
{
    return pca9554_read_reg( chip_id, PCA9554_INPUT_REG, readout );
}

uint8_t pca9554_read_pin( uint8_t chip_id, uint8_t pin, uint8_t *status )
{
    uint8_t rx_len, pin_read = 0;

    rx_len = pca9554_read_port( chip_id, &pin_read );

    if (status) {
        *status = ((pin_read >> pin) & 0x1);
    }

    return rx_len;
}

uint8_t pca9554_write_port( uint8_t chip_id, uint8_t data )
{
    return pca9554_write_reg( chip_id, PCA9554_OUTPUT_REG, data );
}

uint8_t pca9554_write_pin( uint8_t chip_id, uint8_t pin, bool data )
{
    uint8_t output = 0;

    pca9554_read_port( chip_id, &output );
    output &= ~( 1 << pin );
    output |= ( data << pin );

    return pca9554_write_port( chip_id, output );
}

/* Polarity Control */
uint8_t pca9554_set_port_pol( uint8_t chip_id, uint8_t pol )
{
    return pca9554_write_reg( chip_id, PCA9554_POLARITY_REG, pol );
}

uint8_t pca9554_set_pin_pol( uint8_t chip_id, uint8_t pin, bool pol )
{
    uint8_t pol_reg = 0;

    pca9554_read_port( chip_id, &pol_reg );
    pol_reg &= ~( 1 << pin );
    pol_reg |= ( pol << pin );

    return pca9554_set_port_pol( chip_id, pol_reg );
}

uint8_t pca9554_get_port_pol( uint8_t chip_id, uint8_t *pol )
{
    return pca9554_read_reg( chip_id, PCA9554_POLARITY_REG, pol );
}

uint8_t pca9554_get_pin_pol( uint8_t chip_id, uint8_t pin, uint8_t *pol )
{
    uint8_t rx_len;

    rx_len = pca9554_get_port_pol( chip_id, pol );

    /* Mask all bits, except the one requested */
    *pol = ((*pol >> pin) & 0x1);

    return rx_len;
}

/* Pins direction (output/input) */
uint8_t pca9554_set_port_dir( uint8_t chip_id, uint8_t dir )
{
    return pca9554_write_reg( chip_id, PCA9554_CFG_REG, dir );
}

uint8_t pca9554_set_pin_dir( uint8_t chip_id, uint8_t pin, bool dir )
{
    uint8_t dir_reg = 0;

    pca9554_read_port( chip_id, &dir_reg );
    dir_reg &= ~( 1 << pin );
    dir_reg |= ( dir << pin );

    return pca9554_set_port_dir( chip_id, dir_reg );
}

uint8_t pca9554_get_port_dir( uint8_t chip_id, uint8_t *dir )
{
    return pca9554_read_reg( chip_id, PCA9554_CFG_REG, dir );
}

uint8_t pca9554_get_pin_dir( uint8_t chip_id, uint8_t pin, uint8_t *dir )
{
    uint8_t rx_len;

    rx_len = pca9554_get_port_dir( chip_id, dir );

    /* Mask all bits, except the one requested */
    *dir = ((*dir >> pin) & 0x1);

    return rx_len;
}
