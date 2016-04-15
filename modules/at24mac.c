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
#include "at24mac.h"
#include "port.h"
#include "board_version.h"

size_t at24mac_read_generic( uint8_t address, uint8_t *rx_data, size_t buf_len, bool rtos )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t rx_len = 0;

    bool smphr_taken = false;

    /* Only tries to take the semaphore if the "rtos" flag is true */
    if (rtos) {
        smphr_taken = i2c_take_by_chipid(CHIP_ID_EEPROM, &i2c_addr, &i2c_interface, 0);
    } else {
        smphr_taken = true;
        i2c_interface = I2C1;
        i2c_addr = 0x50;
    }

    if ( smphr_taken && ( rx_data != NULL ) ) {
        rx_len = xI2CMasterWriteRead( i2c_interface, i2c_addr, address, rx_data, buf_len);

        if ( rtos ) {
            i2c_give( i2c_interface );
        }
    }

    return rx_len;
}

size_t at24mac_read_serial_num_generic( uint8_t *rx_data, size_t buf_len, bool rtos )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t rx_len = 0;

    bool smphr_taken = false;

    /* Only tries to take the semaphore if the "rtos" flag is true */
    if (rtos) {
        smphr_taken = i2c_take_by_chipid(CHIP_ID_EEPROM_ID, &i2c_addr, &i2c_interface, 0);
    } else {
        smphr_taken = true;
        i2c_interface = I2C1;
        i2c_addr = 0x58;
    }

    if ( smphr_taken && ( rx_data != NULL ) ) {
        rx_len = xI2CMasterWriteRead( i2c_interface, i2c_addr, AT24MAC_ID_ADDR, rx_data, buf_len);

        if ( rtos ) {
            i2c_give( i2c_interface );
        }
    }

    return rx_len;
}

size_t at24mac_read_eui_generic( uint8_t *rx_data, size_t buf_len, bool rtos )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t rx_len = 0;

    bool smphr_taken = false;

    /* Only tries to take the semaphore if the "rtos" flag is true */
    if (rtos) {
        smphr_taken = i2c_take_by_chipid(CHIP_ID_EEPROM_ID, &i2c_addr, &i2c_interface, 0);
    } else {
        smphr_taken = 1;
        i2c_interface = I2C1;
        i2c_addr = 0x58;
    }

    if ( smphr_taken && ( rx_data != NULL ) ) {
        rx_len = xI2CMasterWriteRead( i2c_interface, i2c_addr, AT24MAC_EUI_ADDR, rx_data, buf_len);

        if ( rtos ) {
            i2c_give( i2c_interface );
        }
    }

    return rx_len;
}

size_t at24mac_write_generic( uint8_t address, uint8_t *tx_data, size_t buf_len, bool rtos )
{
    uint8_t i2c_addr;
    uint8_t i2c_interface;
    uint8_t bytes_to_write;
    uint8_t curr_addr;
    size_t tx_len = 0;

    bool smphr_taken = false;

    /* Only tries to take the semaphore if the "rtos" flag is true */
    if (rtos) {
        smphr_taken = i2c_take_by_chipid(CHIP_ID_EEPROM, &i2c_addr, &i2c_interface, 0);
    } else {
        smphr_taken = 1;
        i2c_interface = I2C1;
        i2c_addr = 0x50;
    }

    if ( smphr_taken && ( tx_data != NULL ) ) {
        curr_addr = address;

        while (tx_len < buf_len) {
            bytes_to_write = 16 - (curr_addr % 16);

            if (bytes_to_write > ( buf_len - tx_len )) {
                bytes_to_write = ( buf_len - tx_len );
            }

            /* Dummy write to set the address pointer */
            xI2CMasterWrite( i2c_interface, i2c_addr, &curr_addr, 1);
            /* Write the data */
            tx_len += xI2CMasterWrite( i2c_interface, i2c_addr, tx_data+tx_len, bytes_to_write );
            curr_addr += bytes_to_write;
        }
        if ( rtos ) {
            i2c_give( i2c_interface );
        }
    }

    return tx_len;
}
