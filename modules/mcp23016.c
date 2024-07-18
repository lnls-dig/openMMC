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
 * @return MMC_OK if success, an error code otherwise
 */
static mmc_err mcp23016_read_reg ( uint8_t reg, uint8_t *readout )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;
    uint8_t data[2] = {0};

    if (readout == NULL) {
        return MMC_INVALID_ARG_ERR;
    }

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, pdMS_TO_TICKS(10)) ) {
        rx_len = xI2CMasterWriteRead(i2c_id, i2c_addr, &reg, 1, data, sizeof(data));
        i2c_give(i2c_id);
    } else {
        return MMC_TIMEOUT_ERR;
    }

    if (rx_len != sizeof(data)) {
        return MMC_IO_ERR;
    }

    *readout = data[0];

    return MMC_OK;
}

/**
 * @brief MCP23016 General register write
 *
 * @param[in] reg   Selected register
 * @param[in] data  Value to write to register
 *
 * @return MMC_OK if success, an error code otherwise
 */

static mmc_err mcp23016_write_reg (uint8_t reg, uint8_t data) {

    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t tx_len = 0;
    uint8_t cmd_data[2];

    cmd_data[0] = reg;
    cmd_data[1] = data;

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, pdMS_TO_TICKS(10)) ) {
        tx_len = xI2CMasterWrite(i2c_id, i2c_addr, cmd_data, sizeof(cmd_data));
        i2c_give(i2c_id);
    } else {
        return MMC_TIMEOUT_ERR;
    }

    if (tx_len != sizeof(cmd_data)) {
        return MMC_IO_ERR;
    }

    return MMC_OK;
}


/* Pins Read/Write */
mmc_err mcp23016_read_port( uint8_t port_num, uint8_t *readout )
{
    return mcp23016_read_reg( MCP23016_GP_REG + port_num, readout );
}

mmc_err mcp23016_read_pin( uint8_t port_num, uint8_t pin, uint8_t *status )
{
    uint8_t pin_read = 0;
    mmc_err err;

    err = mcp23016_read_port(port_num, &pin_read );

    if (status) {
        *status = ((pin_read >> pin) & 0x1);
    }

    return err;
}

mmc_err mcp23016_write_port( uint8_t port_num, uint8_t data )
{
    return mcp23016_write_reg( MCP23016_GP_REG + port_num, data );
}

mmc_err mcp23016_write_pin( uint8_t port_num, uint8_t pin, bool data )
{
    uint8_t output = 0;
    mmc_err err;

    err = mcp23016_read_port( port_num, &output );

    if (err != MMC_OK) {
        return err;
    }

    output &= ~( 1 << pin );
    output |= ( data << pin );

    return mcp23016_write_port( port_num, output );
}

/* Polarity Control */
mmc_err mcp23016_set_port_pol( uint8_t port_num, uint8_t pol )
{
    return mcp23016_write_reg( MCP23016_IPOL_REG + port_num, pol );
}

mmc_err mcp23016_get_port_pol( uint8_t port_num, uint8_t *pol )
{
    return mcp23016_read_reg( MCP23016_IPOL_REG + port_num, pol );
}

/* Pins direction (output/input) */
mmc_err mcp23016_set_port_dir( uint8_t port_num, uint8_t dir )
{
    return mcp23016_write_reg( MCP23016_IODIR_REG + port_num, dir );
}

mmc_err mcp23016_get_port_dir( uint8_t port_num, uint8_t *dir )
{
    return mcp23016_read_reg( MCP23016_IODIR_REG + port_num, dir );
}


mmc_err mcp23016_read_reg_pair ( uint8_t reg, uint16_t *readout ) {
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;
    uint8_t data[2] = {0};

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, pdMS_TO_TICKS(10)) ) {
        rx_len = xI2CMasterWriteRead(i2c_id, i2c_addr, &reg, 1, data, sizeof(data));
        i2c_give(i2c_id);
    } else {
        return MMC_TIMEOUT_ERR;
    }

    if (rx_len != sizeof(data)) {
        return MMC_IO_ERR;
    }

    *readout = (data[0] << 8) | data[1];

    return MMC_OK;
}

mmc_err mcp23016_write_reg_pair ( uint8_t reg, uint16_t data )
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t cmd_data[3] = {
    		reg,
			(data >> 8) & 0xFF,
			(data) & 0xFF
    };
    uint8_t tx_len = 0;

    if( i2c_take_by_chipid( CHIP_ID_MCP23016, &i2c_addr, &i2c_id, pdMS_TO_TICKS(10)) ) {
        tx_len = xI2CMasterWrite(i2c_id, i2c_addr, cmd_data, sizeof(cmd_data));
        i2c_give(i2c_id);
    } else {
        return MMC_TIMEOUT_ERR;
    }

    if (tx_len != sizeof(cmd_data)) {
        return MMC_IO_ERR;
    }

    return MMC_OK;
}
