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

/**
 * @file   mcp23016.c
 *
 * @brief  MCP23016 module interface functions implementations
 *
 * @ingroup MCP23016
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "mcp23016.h"
#include "i2c.h"
#include "i2c_mapping.h"

/**
 * @brief  MCP23016 General register read
 *
 * @param[in]  reg     Selected register
 * @param[out] readout Register value read
 *
 * @return Number of bytes read (0 if failure)
 */
static uint8_t mcp23016_read_reg ( uint8_t reg, uint8_t *readout )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;
    uint8_t data[2] = {0};

    if (readout == NULL) {
        return 0;
    }

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        rx_len = xI2CMasterWriteRead(i2c_id, i2c_addr, reg, data, 2);
        i2c_give(i2c_id);
    }

    *readout = data[0];

    return rx_len;
}

/**
 * @brief MCP23016 General register write
 *
 * @param reg   Selected register
 * @param data  Value to write to register
 *
 * @return Number of bytes written
 */

static uint8_t mcp23016_write_reg (uint8_t reg, uint8_t data) {

    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0, tx_len = 0;
    uint8_t read[2] = {0};
    uint8_t cmd_data[3];

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        rx_len = xI2CMasterWriteRead(i2c_id, i2c_addr, reg, read, 2);
        i2c_give(i2c_id);
    }

    if (!rx_len) {
    	return 0;
    }

    cmd_data[0] = reg;
    cmd_data[1] = data;
    cmd_data[2] = read[1];

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        tx_len = xI2CMasterWrite(i2c_id, i2c_addr, cmd_data, sizeof(cmd_data));
        i2c_give(i2c_id);
    }

    return tx_len;
}


/* Pins Read/Write */
uint8_t mcp23016_read_port( uint8_t port_num, uint8_t *readout )
{
    return mcp23016_read_reg( MCP23016_GP_REG + port_num, readout );
}

uint8_t mcp23016_read_pin( uint8_t port_num, uint8_t pin, uint8_t *status )
{
    uint8_t rx_len = 0, pin_read = 0;

    rx_len = mcp23016_read_port(port_num, &pin_read );

    if (status) {
        *status = ((pin_read >> pin) & 0x1);
    }

    return rx_len;
}

uint8_t mcp23016_write_port( uint8_t port_num, uint8_t data )
{
    return mcp23016_write_reg( MCP23016_GP_REG + port_num, data );
}

uint8_t mcp23016_write_pin( uint8_t port_num, uint8_t pin, bool data )
{
    uint8_t output = 0;

    mcp23016_read_port( port_num, &output );
    output &= ~( 1 << pin );
    output |= ( data << pin );

    return mcp23016_write_port( port_num, output );
}

/* Polarity Control */
uint8_t mcp23016_set_port_pol( uint8_t port_num, uint8_t pol )
{
    return mcp23016_write_reg( MCP23016_IPOL_REG + port_num, pol );
}

uint8_t mcp23016_get_port_pol( uint8_t port_num, uint8_t *pol )
{
    return mcp23016_read_reg( MCP23016_IPOL_REG + port_num, pol );
}

/* Pins direction (output/input) */
uint8_t mcp23016_set_port_dir( uint8_t port_num, uint8_t dir )
{
    return mcp23016_write_reg( MCP23016_IODIR_REG + port_num, dir );
}

uint8_t mcp23016_get_port_dir( uint8_t port_num, uint8_t *dir )
{
    return mcp23016_read_reg( MCP23016_IODIR_REG + port_num, dir );
}


uint8_t mcp23016_read_reg_pair ( uint8_t reg, uint16_t *readout ) {
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;
    uint8_t read[2] = {0};

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        rx_len = xI2CMasterWriteRead(i2c_id, i2c_addr, reg, read, 2);
        i2c_give(i2c_id);
    }

    *readout = (read[0] << 8) | read[1];

    return rx_len;
}

uint8_t mcp23016_write_reg_pair ( uint8_t reg, uint16_t data )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t cmd_data[3] = {
    		reg,
			(data >> 8) & 0xFF,
			(data) & 0xFF
    };
    uint8_t tx_len = 0;

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, (TickType_t) 10) ) {
        tx_len = xI2CMasterWrite(i2c_id, i2c_addr, cmd_data, sizeof(cmd_data));
        i2c_give(i2c_id);
    }

    return tx_len;
}
