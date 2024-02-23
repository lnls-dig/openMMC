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

/**
 * @file max6642.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief MAX6642 I2C Temperature Sensor Interface implementation
 * @ingroup MAX6642
 */

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "port.h"

/* Project Includes */
#include "sdr.h"
#include "task_priorities.h"
#include "i2c.h"
#include "i2c_mapping.h"
#include "max6642.h"
#include "utils.h"
#include "uart_debug.h"

TaskHandle_t vTaskMAX6642_Handle;

void vTaskMAX6642( void* Parameters )
{
    const TickType_t xFrequency = pdMS_TO_TICKS(MAX6642_UPDATE_RATE);

    sensor_t * temp_sensor;

    for ( ;; ) {
        /* Iterate through the SDR Table to find all the LM75 entries */

        for ( temp_sensor = sdr_head; temp_sensor != NULL; temp_sensor = temp_sensor->next ) {

            if ( temp_sensor->task_handle == NULL ) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if ( *(temp_sensor->task_handle) != xTaskGetCurrentTaskHandle() ) {
                continue;
            }

            /* Update the temperature reading */
            max6642_read_remote( temp_sensor, (uint8_t *) &(temp_sensor->readout_value) );

            /* Check for threshold events */
            sensor_state_check(temp_sensor);
            check_sensor_event(temp_sensor);
        }
        vTaskDelay(xFrequency);
    }
}

void MAX6642_init( void )
{
    xTaskCreate( vTaskMAX6642, "MAX6642", 200, (void *) NULL, tskMAX6642SENSOR_PRIORITY, &vTaskMAX6642_Handle );
}

Bool max6642_read_local( sensor_t *sensor, uint8_t *temp )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t read = 0;
    const uint8_t cmd = MAX6642_CMD_READ_LOCAL;
    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY ) == pdTRUE ) {

        xI2CMasterWriteRead( i2c_interf, i2c_addr, &cmd, 1, &read, 1 );
        i2c_give( i2c_interf );

        *temp = read;
        return true;
    }

    return false;
}

Bool max6642_read_remote( sensor_t *sensor, uint8_t *temp )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t read = 0;
    const uint8_t cmd = MAX6642_CMD_READ_REMOTE;

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY ) == pdTRUE ) {

        xI2CMasterWriteRead( i2c_interf, i2c_addr, &cmd, 1, &read, 1 );
        i2c_give( i2c_interf );

        *temp = read;
        return true;
    }

    return false;
}

Bool max6642_read_local_extd( sensor_t *sensor, uint8_t *temp )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t read = 0;
    const uint8_t cmd = MAX6642_CMD_READ_LOCAL_EXTD;

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY ) == pdTRUE ) {

        xI2CMasterWriteRead( i2c_interf, i2c_addr, &cmd, 1, &read, 1 );
        i2c_give( i2c_interf );

        *temp = read;
        return true;
    }

    return false;
}

Bool max6642_read_remote_extd( sensor_t *sensor, uint8_t *temp )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t read = 0;
    const uint8_t cmd = MAX6642_CMD_READ_REMOTE_EXTD;

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY ) == pdTRUE ) {

        xI2CMasterWriteRead( i2c_interf, i2c_addr, &cmd, 1, &read, 1 );
        i2c_give( i2c_interf );

        *temp = read;
        return true;
    }

    return false;
}

uint8_t max6642_read_status( sensor_t *sensor )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t stat = 0;
    const uint8_t cmd = MAX6642_CMD_READ_STATUS;

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY ) == pdTRUE ) {

        xI2CMasterWriteRead( i2c_interf, i2c_addr, &cmd, 1, &stat, 1 );
        i2c_give( i2c_interf );
    }

    return stat;
}

uint8_t max6642_read_cfg( sensor_t *sensor )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t cfg = 0;
    const uint8_t cmd = MAX6642_CMD_READ_CONFIG;

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        xI2CMasterWriteRead( i2c_interf, i2c_addr, &cmd, 1, &cfg, 1 );
        i2c_give( i2c_interf );
    }

    return cfg;
}

void max6642_write_cfg( sensor_t *sensor, uint8_t cfg )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t msg[2] = { MAX6642_CMD_WRITE_CONFIG, cfg };

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        xI2CMasterWrite( i2c_interf, i2c_addr, &msg[0], 2);
        i2c_give( i2c_interf );
    }
}

void max6642_write_local_limit( sensor_t *sensor, uint8_t limit )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t msg[2] = { MAX6642_CMD_WRITE_LOCAL_LIMIT, limit };

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        xI2CMasterWrite( i2c_interf, i2c_addr, &msg[0], 2);
        i2c_give( i2c_interf );
    }
}

void max6642_write_remote_limit( sensor_t *sensor, uint8_t limit )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t msg[2] = { MAX6642_CMD_WRITE_REMOTE_LIMIT, limit };

    if ( i2c_take_by_chipid( sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        xI2CMasterWrite( i2c_interf, i2c_addr, &msg[0], 2);
        i2c_give( i2c_interf );
    }
}
