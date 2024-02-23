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
 * @file lm75.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Interface functions for LM75 I2C Temperature Sensor
 *
 * @ingroup LM75
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
#include "lm75.h"
#include "utils.h"
#include "uart_debug.h"

TaskHandle_t vTaskLM75_Handle;

void vTaskLM75( void* Parameters )
{
    const TickType_t xFrequency = pdMS_TO_TICKS(LM75_UPDATE_RATE);

    uint8_t i2c_addr, i2c_interf;
    sensor_t * temp_sensor;
    uint8_t temp[2];
    uint16_t converted_temp;

    for ( ;; ) {
        /* Iterate through the SDR Table to find all the LM75 entries */

        for ( temp_sensor = sdr_head; temp_sensor != NULL; temp_sensor = temp_sensor->next) {

            if ( temp_sensor->task_handle == NULL ) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if ( *(temp_sensor->task_handle) != xTaskGetCurrentTaskHandle() ) {
                continue;
            }

            /* Try to gain the I2C bus */
            if ( i2c_take_by_chipid( temp_sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY ) == pdTRUE ) {

                /* Update the temperature reading */
                if (xI2CMasterRead( i2c_interf, i2c_addr, &temp[0], 2) == 2) {
                    converted_temp = ((temp[0] << 1) | ((temp[1]>>7)));
                    temp_sensor->readout_value = converted_temp;
                }
                /* Check for threshold events */
                i2c_give(i2c_interf);
                sensor_state_check(temp_sensor);
                check_sensor_event(temp_sensor);
            }
        }
        vTaskDelay(xFrequency);
    }
}


void LM75_init( void )
{
    xTaskCreate( vTaskLM75, "LM75", 200, (void *) NULL, tskLM75SENSOR_PRIORITY, &vTaskLM75_Handle);
}
