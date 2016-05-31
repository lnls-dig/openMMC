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

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "pin_mapping.h"
#include "pca9554.h"
#include "i2c.h"
#include "i2c_mapping.h"

/* General Register write/read */
static uint8_t pca9554_read_reg ( uint8_t reg )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx = 0;

    if( i2c_take_by_chipid( CHIP_ID_RTM_PCA9554, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        xI2CMasterWriteRead(i2c_id, i2c_addr, reg, &rx, 1);
        i2c_give(i2c_id);
    }
    return rx;
}

static void pca9554_write_reg ( uint8_t reg, uint8_t data )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t cmd_data[2] = {reg, data};

    if( i2c_take_by_chipid( CHIP_ID_RTM_PCA9554, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        xI2CMasterWrite(i2c_id, i2c_addr, cmd_data, sizeof(cmd_data));
        i2c_give(i2c_id);
    }
}

/* Pins Read/Write */
uint8_t pca9554_read_port( void )
{
    return pca9554_read_reg( PCA9554_INPUT_REG );
}

uint8_t pca9554_read_pin( uint8_t pin )
{
    uint8_t pin_read;

    pin_read = pca9554_read_port();

    return ((pin_read >> pin) & 0x1);
}

void pca9554_write_port( uint8_t data )
{
    pca9554_write_reg( PCA9554_OUTPUT_REG, data );
}

void pca9554_write_pin( uint8_t pin, uint8_t data )
{
    uint8_t output;

    output = pca9554_read_port();
    output &= ~( 1 << pin );
    output |= ( data << pin );

    pca9554_write_port( output );
}

/* Polarity Control */
void pca9554_set_port_pol( uint8_t pol )
{
    pca9554_write_reg( PCA9554_POLARITY_REG, pol );
}

void pca9554_set_pin_pol( uint8_t pin, uint8_t pol )
{
    uint8_t pol_reg;

    pol_reg = pca9554_read_port();
    pol_reg &= ~( 1 << pin );
    pol_reg |= ( pol << pin );

    pca9554_set_port_pol( pol_reg );
}

uint8_t pca9554_get_port_pol( void )
{
    return pca9554_read_reg( PCA9554_POLARITY_REG );
}

uint8_t pca9554_get_pin_pol( uint8_t pin )
{
    uint8_t port_pol;

    port_pol = pca9554_get_port_pol();

    return ((port_pol >> pin) & 0x1);
}

/* Pins direction (output/input) */
void pca9554_set_port_dir( uint8_t dir )
{
    pca9554_write_reg( PCA9554_CFG_REG, dir );
}

void pca9554_set_pin_dir( uint8_t pin, uint8_t dir )
{
    uint8_t dir_reg;

    dir_reg = pca9554_read_port();
    dir_reg &= ~( 1 << pin );
    dir_reg |= ( dir << pin );

    pca9554_set_port_dir( dir_reg );
}

uint8_t pca9554_get_port_dir( void )
{
    return pca9554_read_reg( PCA9554_CFG_REG );
}

uint8_t pca9554_get_pin_dir( uint8_t pin )
{
    uint8_t port_dir;

    port_dir = pca9554_get_port_dir();

    return ((port_dir >> pin) & 0x1);
}
