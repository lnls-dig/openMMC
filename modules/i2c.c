/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
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

/* Project includes */
#include "FreeRTOS.h"
#include "port.h"
#include "i2c.h"
#include "i2c_mapping.h"

/**
 * @brief Number of I2C peripheral buses that are being controlled
 * @note This value is contabilized at compile time and may change from board to board. Check the board's i2c_mapping.c file for more info
 *
 * @see i2c_mapping.c
 */
#define I2C_MUX_COUNT (sizeof(i2c_mux) / sizeof(i2c_mux_state_t))

/**
 * @brief Number of I2C Chips that are being controlled
 * @note This value is contabilized at compile time and may change from board to board. Check the board's i2c_mapping.c file for more info
 *
 * @see i2c_mapping.c
 */
#define I2C_CHIP_MAP_COUNT (sizeof(i2c_chip_map)/sizeof(i2c_chip_mapping_t))

void i2c_init( void )
{
    for ( uint8_t i = 0; i < sizeof(i2c_mux)/sizeof(i2c_mux_state_t); i++ ) {
        i2c_mux[i].semaphore = xSemaphoreCreateBinary();
        vI2CConfig( i2c_mux[i].i2c_interface, SPEED_100KHZ );
        xSemaphoreGive( i2c_mux[i].semaphore );
    }
}

bool i2c_take_by_busid( uint8_t bus_id, uint8_t *i2c_interface, TickType_t timeout )
{
    i2c_mux_state_t *p_i2c_mux = NULL;
    i2c_bus_mapping_t *p_i2c_bus = &i2c_bus_map[bus_id];

    uint8_t tmp_interface_id = i2c_bus_map[bus_id].i2c_interface;

    for ( p_i2c_mux = i2c_mux; p_i2c_mux != NULL; p_i2c_mux++ ) {
        if ( p_i2c_mux->i2c_interface == tmp_interface_id ) {
            break;
        }
    }

    if ( p_i2c_mux == NULL ) {
        return false;
    }
    if ( p_i2c_bus->enabled == 0 ) {
        return false;
    }
    if ( p_i2c_mux->semaphore == 0 ) {
        return false;
    }

    /* Try to take the semaphore to win the bus */
    if ( xSemaphoreTake( p_i2c_mux->semaphore, timeout ) == pdFALSE ) {
        return false;
    }

    /* This bus is not multiplexed, no action needed */
    if ( p_i2c_bus->mux_bus == -1 ) {
        *i2c_interface = p_i2c_mux->i2c_interface;
        portENABLE_INTERRUPTS();
        return true;
    }


    p_i2c_mux->state = i2c_get_mux_bus( bus_id, p_i2c_mux );

    /* This bus mux is in correct state */
    if ( p_i2c_mux->state != p_i2c_bus->mux_bus ) {
        if ( i2c_set_mux_bus( bus_id, p_i2c_mux, p_i2c_bus->mux_bus ) == false ) {
            return false;
        }
    }
    *i2c_interface = p_i2c_mux->i2c_interface;
    portENABLE_INTERRUPTS();
    return true;
}

bool i2c_take_by_chipid( uint8_t chip_id, uint8_t *i2c_address, uint8_t *i2c_interface,  uint32_t timeout )
{
    if ( chip_id > I2C_CHIP_MAP_COUNT ) {
        return false;
    }

    uint8_t bus_id = i2c_chip_map[chip_id].bus_id;
    if ( i2c_address != NULL ) {
        *i2c_address = i2c_chip_map[chip_id].i2c_address;
    }

    return i2c_take_by_busid( bus_id, i2c_interface, timeout );
}

void i2c_give( uint8_t i2c_interface )
{
    i2c_mux_state_t *mux;
    for ( mux = i2c_mux; mux != NULL; mux++ ) {
        if ( mux->i2c_interface == i2c_interface ) {
            xSemaphoreGive( mux->semaphore );
            break;
        }
    }
}
