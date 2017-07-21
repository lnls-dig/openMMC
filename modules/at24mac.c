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
 * @file   at24mac.c
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  AT24MACX02 EEPROM module interface implementation
 *
 * @ingroup AT24MAC
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "string.h"

/* Project Includes */
#include "at24mac.h"
#include "port.h"
#include "i2c.h"

size_t at24mac_read( uint8_t id, uint16_t address, uint8_t *rx_data, size_t buf_len, uint32_t timeout )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t rx_len = 0;

    if (i2c_take_by_chipid( id, &i2c_addr, &i2c_interface, timeout ) && ( rx_data != NULL ) ) {
        rx_len = xI2CMasterWriteRead( i2c_interface, i2c_addr, address, rx_data, buf_len );
        i2c_give( i2c_interface );
    }

    return rx_len;
}

size_t at24mac_read_serial_num( uint8_t id, uint8_t *rx_data, size_t buf_len, uint32_t timeout )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t rx_len = 0;

    if (i2c_take_by_chipid( id, &i2c_addr, &i2c_interface, timeout ) && ( rx_data != NULL ) ) {
        rx_len = xI2CMasterWriteRead( i2c_interface, i2c_addr+8, AT24MAC_ID_ADDR, rx_data, buf_len);

        i2c_give( i2c_interface );
    }

    return rx_len;
}

size_t at24mac_read_eui( uint8_t id, uint8_t *rx_data, size_t buf_len, uint32_t timeout )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t rx_len = 0;

    if (i2c_take_by_chipid( id, &i2c_addr, &i2c_interface, timeout ) && ( rx_data != NULL ) ) {

        rx_len = xI2CMasterWriteRead( i2c_interface, i2c_addr+8, AT24MAC_EUI_ADDR, rx_data, buf_len);

        i2c_give( i2c_interface );
    }

    return rx_len;
}

size_t at24mac_write( uint8_t id, uint16_t address, uint8_t *tx_data, size_t buf_len, uint32_t timeout )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t bytes_to_write;
    uint8_t curr_addr;
    uint8_t page_buf[17];

    size_t tx_len = 0;

    if (i2c_take_by_chipid( id, &i2c_addr, &i2c_interface, timeout ) && ( tx_data != NULL ) ) {
        curr_addr = address;

        while (tx_len < buf_len) {
            bytes_to_write = 16 - (curr_addr % 16);

            if (bytes_to_write > ( buf_len - tx_len )) {
                bytes_to_write = ( buf_len - tx_len );
            }
            page_buf[0] = curr_addr;
            memcpy(&page_buf[1], tx_data+tx_len, bytes_to_write);

            /* Write the data */
            tx_len += xI2CMasterWrite( i2c_interface, i2c_addr, &page_buf[0] , bytes_to_write+1 );
            curr_addr += bytes_to_write;
        }
        i2c_give( i2c_interface );
    }

    return tx_len;
}
