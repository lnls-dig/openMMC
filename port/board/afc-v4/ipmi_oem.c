/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

#include "ipmi_oem.h"

#include "port.h"
#include "i2c.h"

/** @brief Handler for IPMI_OEM_CMD_I2C_TRANSFER IPMI command
 *
 * Performs a raw I2C master read on the selected bus and return the data
 * Req data:
 * [0] - Bus ID @see i2c_mapping.h
 * [1] - #Chip/Address identification - (0) = ChipID identification on byte 2
 *                                      (1) = I2C Address identification on byte 2
 * [2] - ChipID/I2C_Address - 8 bit address
 * [3] - Data Write len (n)
 * [4] - Data to Write
 * [4+n] - Data Read len (m)
 *
 * @param req[in]
 * @param rsp[out]
 *
 * @return
 */
IPMI_HANDLER(ipmi_oem_cmd_i2c_transfer, NETFN_CUSTOM_OEM, IPMI_OEM_CMD_I2C_TRANSFER, ipmi_msg *req, ipmi_msg* rsp)
{
    uint8_t bus_id = req->data[0];
    uint8_t chipid_sel = req->data[1];
    uint8_t chipid_i2caddr = req->data[2];
    uint8_t write_len = req->data[3];
    uint8_t read_len = req->data[4+write_len];
    uint8_t *read_data;

    uint8_t semph_err;

    uint8_t i2c_interf;
    uint8_t i2c_addr;

    if ( chipid_sel == 0 ) {
        /* Use chip id to take the bus */
        semph_err = i2c_take_by_chipid( chipid_i2caddr, &i2c_addr, &i2c_interf, (TickType_t)10);
    } else {
        semph_err = i2c_take_by_busid( bus_id, &i2c_interf, (TickType_t)10 );
        i2c_addr = chipid_i2caddr;
    }

    if ( semph_err == 0 ) {
        rsp->completion_code = IPMI_CC_UNSPECIFIED_ERROR;
        return;
    }

    if ( write_len > 0 ) {
        if (xI2CMasterWrite( i2c_interf, i2c_addr, &req->data[4], write_len ) == write_len) {
            rsp->completion_code = IPMI_CC_OK;
        } else {
            rsp->completion_code = IPMI_CC_UNSPECIFIED_ERROR;
            return;
        }
    }

    if ( read_len > 0 ) {
        read_data = pvPortMalloc( read_len );
        memset( read_data, read_len, 0 );

        if ( xI2CMasterRead( i2c_interf, i2c_addr, read_data, read_len ) == read_len ) {
            rsp->data[0] = read_len;
            memcpy( &rsp->data[1], read_data, read_len );
            rsp->data_len = read_len+1;
            rsp->completion_code = IPMI_CC_OK;
        } else {
            rsp->data_len = 0;
            rsp->completion_code = IPMI_CC_UNSPECIFIED_ERROR;
        }

        vPortFree( read_data );
    }

    i2c_give( i2c_interf );
}

/* GPIO Access IPMI commands */
/** @brief Handler for IPMI_OEM_CMD_GPIO IPMI command
 *
 * Access and configure the controller's GPIO
 *
 * Req data:
 * [0] - Mode - (0) = Read port status (direction and value)
 *              (1) = Set pin as input
 *              (2) = Set pin as output (pin value is on byte 3)
 * [1] - GPIO Port number
 * [2] - GPIO Pin number
 * [3] - Output pin value (optional)
 *
 * @param req[in]
 * @param rsp[out]
 *
 * @return
 */
IPMI_HANDLER(ipmi_oem_cmd_gpio_pin, NETFN_CUSTOM_OEM, IPMI_OEM_CMD_GPIO_PIN, ipmi_msg *req, ipmi_msg* rsp)
{
    uint8_t mode = req->data[0];
    uint8_t port = req->data[1];
    uint8_t pin = req->data[2];
    uint8_t pin_state;

    uint8_t len = 0;

    rsp->completion_code = IPMI_CC_OK;

    switch (mode) {
    case 0:
        /* Port read, returns port direction and read value*/
        rsp->data[len++] = ( gpio_get_port_dir( port ) >> 24 ) & 0xFF;
        rsp->data[len++] = ( gpio_get_port_dir( port ) >> 16 ) & 0xFF;
        rsp->data[len++] = ( gpio_get_port_dir( port ) >>  8 ) & 0xFF;
        rsp->data[len++] = ( gpio_get_port_dir( port ) >>  0 ) & 0xFF;
        rsp->data[len++] = ( gpio_read_port( port ) >> 24 ) & 0xFF;
        rsp->data[len++] = ( gpio_read_port( port ) >> 16 ) & 0xFF;
        rsp->data[len++] = ( gpio_read_port( port ) >>  8 ) & 0xFF;
        rsp->data[len++] = ( gpio_read_port( port ) >>  0 ) & 0xFF;
        break;

    case 1:
        /* Set pin as input */
        gpio_set_pin_dir( port, pin, GPIO_DIR_INPUT );
        break;

    case 2:
        /* Set pin as output */
        gpio_set_pin_dir( port, pin, GPIO_DIR_OUTPUT );

        /* If given, set the pin output value */
        if (req->data_len > 3) {
            pin_state = req->data[3];
            gpio_set_pin_state( port, pin, pin_state );
        }
        break;

    default:
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
        break;
    }

    rsp->data_len = len;
}
