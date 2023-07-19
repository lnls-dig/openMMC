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
 * @file ina3221.c
 *
 * @brief INA3221 interface implementation
 *
 * @ingroup INA3221
 */

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"

/* Project Includes */
#include "port.h"
#include "sdr.h"
#include "task_priorities.h"
#include "i2c.h"
#include "i2c_mapping.h"
#include "fpga_spi.h"
#include "fru.h"
#include "ina3221.h"

TaskHandle_t vTaskINA3221_Handle;

static ina3221_data_t ina3221_data[MAX_INA3221_COUNT];

void vTaskINA3221( void *Parameters )
{
    uint8_t chip_num;
    uint8_t channel;
    TickType_t xLastWakeTime;
    /* Task will run every 100ms */
    const TickType_t xFrequency = INA3221_UPDATE_RATE / portTICK_PERIOD_MS;

    sensor_t * ina3221_sensor;
    ina3221_data_t * data_ptr;
    SDR_type_01h_t * sdr;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        /* Read all registers from the INA3221s */
        for ( chip_num = 0; chip_num < MAX_INA3221_COUNT; chip_num++) {

            if (!(ina3221_data[chip_num].chipid)){
                break;
            }

            if (!(ina3221_read_voltages( &ina3221_data[chip_num] ))) {
            	break;
            }

            for ( uint8_t i = 0; i < 6; i++ ) {

                ina3221_sensor = ina3221_data[chip_num].sensors[i];
                data_ptr = &ina3221_data[chip_num];

                if (ina3221_sensor == NULL) {
                    continue;
                }

                sdr = (SDR_type_01h_t*)ina3221_sensor->sdr;
                channel = sdr->OEM;

                switch ((GET_SENSOR_TYPE(ina3221_sensor))) {
                case SENSOR_TYPE_VOLTAGE:
                    ina3221_sensor->readout_value = data_ptr->regs[2 * channel + 1] >> 6;
                    break;
                case SENSOR_TYPE_CURRENT:
                    ina3221_sensor->readout_value = (((data_ptr->regs[2 * channel] >> 3) * 40) / ina3221_data[chip_num].config->shunt_resistor[channel]) >> 5;
                    break;
                default:
                    break;
                }

                /* Check for threshold events */
                sensor_state_check(ina3221_sensor);
                check_sensor_event(ina3221_sensor);
            }

        }
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

uint8_t ina3221_read_reg( ina3221_data_t * data, uint8_t reg, uint16_t *read )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t val[2] = {0};
    uint8_t rx_len = 0;

    if( i2c_take_by_chipid( data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        rx_len = xI2CMasterWriteRead( i2c_interf, i2c_addr, reg, &val[0], sizeof(val)/sizeof(val[0]) );

        i2c_give( i2c_interf );

        *read = (val[0] << 8) | (val[1]);
    }

    return rx_len;
}

uint8_t ina3221_read_voltages( ina3221_data_t * data )
{
	uint8_t ret = 0xFF;
    /* Read INA3221 Bus Voltage and Shunt Voltage Registers */
    for ( uint8_t i = 0; i < INA3221_VOLTAGE_REGISTERS; i++ ) {
        ret &= ina3221_read_reg(data, i + 1, &(data->regs[i]) );
    }
    return ret;
}


void ina3221_init( void )
{
    sensor_t *tmp_sensor;
    SDR_type_01h_t* sdr;
    uint8_t ina_channel_num;
    uint8_t chip_num;
    uint8_t sens_num;
    uint8_t signed_flag;

    xTaskCreate( vTaskINA3221, "INA3221", 200, (void *) NULL, tskINA3221SENSOR_PRIORITY, &vTaskINA3221_Handle);

    /* Iterate through the SDR Table to find all the INA3221 entries */
    for ( tmp_sensor = sdr_head; tmp_sensor != NULL; tmp_sensor = tmp_sensor->next) {

        if ( tmp_sensor->task_handle == NULL ) {
            continue;
        }

        /* Check if this task should update the selected SDR */
        if ( *(tmp_sensor->task_handle) != vTaskINA3221_Handle ) {
            continue;
        }

        sdr = (SDR_type_01h_t*)tmp_sensor->sdr;
        ina_channel_num = sdr->OEM;

        for (chip_num = 0; chip_num < MAX_INA3221_COUNT; chip_num++) {

            if (tmp_sensor->chipid == ina3221_data[chip_num].chipid
                    || !(ina3221_data[chip_num].chipid)) {

                if ((GET_SENSOR_TYPE(tmp_sensor)) == SENSOR_TYPE_VOLTAGE ) {
                    signed_flag = 0;
                    sens_num = ina_channel_num;
                } else if ((GET_SENSOR_TYPE(tmp_sensor)) == SENSOR_TYPE_CURRENT ) {
                    signed_flag = 1;
                    sens_num = ina_channel_num + 3;
                } else {
                    break;
                }

                ina3221_data[chip_num].chipid = tmp_sensor->chipid;
                ina3221_data[chip_num].config = (ina3221_config_t *) tmp_sensor->settings;
                ina3221_data[chip_num].sensors[sens_num] = tmp_sensor;
                ina3221_data[chip_num].sensors[sens_num]->signed_flag = signed_flag;

                break;
            }
        }
    }
}
