/*
 * lm75.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015
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
 */

#include "FreeRTOS.h"
#include "task.h"
#include "port.h"
#include "sdr.h"
#include "task_priorities.h"
#include "board_version.h"
#include "lm75.h"

TaskHandle_t vTaskLM75_Handle;

void vTaskLM75( void* Parameters )
{
    const TickType_t xFrequency = LM75_UPDATE_RATE;

    uint8_t i2c_bus_id;
    uint8_t i;

    sensor_t * lm75_sensor;
    uint8_t temp[2];
    uint16_t converted_temp;

    for ( ;; ) {
        /* Try to gain the I2C bus */
        if (i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t)100) == pdTRUE) {
            /* Update all temperature sensors readings */
            for ( i = 0; i < NUM_SDR; i++ ) {
                /* Check if the handle pointer is not NULL */
                if (sensor_array[i].task_handle == NULL) {
                    continue;
                }
                /* Check if this task should update the selected SDR */
                if ( *(sensor_array[i].task_handle) != xTaskGetCurrentTaskHandle() ) {
                    continue;
                }

		lm75_sensor = &sensor_array[i];

                /* Update the temperature reading */
                if (xI2CMasterRead( i2c_bus_id, sensor_array[i].slave_addr, &temp[0], 2) == 2) {
                    converted_temp = ((temp[0] << 1) | ((temp[1]>>8)));
                    lm75_sensor->readout_value = converted_temp;
                }
            }
            i2c_give(i2c_bus_id);
        }
        vTaskDelay(xFrequency);
    }
}

void LM75_init( void )
{
#ifdef LM75_CFG
    uint8_t i2c_bus_id;
    uint8_t i, j;

    for ( j = 0, i = 0; i < NUM_SDR && j < LM75_MAX_COUNT; i++, j++ ) {
        if (i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t) 50) == pdFALSE) {
            continue;
        }
        uint8_t cmd[3] = {0x00, 0x01, 0x9F};
        xI2CMasterWrite(i2c_id, sensor_array[i].slave_addr, ch, 3);

        i2c_give( i2c_bus_id );
    }
#endif
    xTaskCreate( vTaskLM75, "LM75", 330, (void *) NULL, tskLM75SENSOR_PRIORITY, &vTaskLM75_Handle);
}
