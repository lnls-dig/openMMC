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
 * @file   eeprom_24xx64.c
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  24xx64 EEPROM module interface implementation
 *
 * @ingroup 24xx64
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "string.h"

/* Project Includes */
#include "eeprom_24xx64.h"
#include "port.h"
#include "i2c.h"

size_t eeprom_24xx64_read( uint8_t id, uint16_t address, uint8_t *rx_data, size_t buf_len, TickType_t timeout )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t rx_len = 0;
    uint8_t addr8[2];

    addr8[0] = (address >> 8) & 0xFF;
    addr8[1] = (address) & 0xFF;

    if ( rx_data == NULL ) {
        return 0;
    }

    if (i2c_take_by_chipid( id, &i2c_addr, &i2c_interface, timeout ) ) {
        /* Sets address register */
        xI2CMasterWrite( i2c_interface, i2c_addr, addr8 , sizeof(addr8) );

        /* Read data */
        rx_len = xI2CMasterRead( i2c_interface, i2c_addr, rx_data, buf_len );
        i2c_give( i2c_interface );
    }

    return rx_len;
}

size_t eeprom_24xx64_write( uint8_t id, uint16_t address, uint8_t *tx_data, size_t buf_len, TickType_t timeout )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t bytes_to_write;
    uint8_t page_buf[34];
    uint16_t curr_addr;

    size_t tx_len = 0;

    if ( tx_data == NULL ) {
        return 0;
    }

    if (i2c_take_by_chipid( id, &i2c_addr, &i2c_interface, timeout)) {
        curr_addr = address;

        while (tx_len < buf_len) {
            bytes_to_write = 32 - (curr_addr % 32);

            if (bytes_to_write > ( buf_len - tx_len )) {
                bytes_to_write = ( buf_len - tx_len );
            }
            page_buf[0] = (curr_addr >> 8) & 0xFF;
            page_buf[1] = (curr_addr) & 0xFF;

            memcpy(&page_buf[2], tx_data+tx_len, bytes_to_write);

            /* Write the data */
            tx_len += xI2CMasterWrite( i2c_interface, i2c_addr, &page_buf[0] , bytes_to_write+2 );
            vTaskDelay(10);
            tx_len -= 2; /* Remove the 2 page bytes from the count */
            curr_addr += bytes_to_write;
        }
        i2c_give( i2c_interface );
    }

    return tx_len;
}
