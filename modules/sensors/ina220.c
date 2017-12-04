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
 * @file ina220.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief INA220 interface implementation
 *
 * @ingroup INA220
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
#include "payload.h"
#include "ina220.h"
#include "fpga_spi.h"
#include "fru.h"

const ina220_config_t ina220_cfg = {
    .config_reg_default.cfg_struct = { .bus_voltage_range = INA220_16V_SCALE_RANGE,
                                       .pga_gain = INA220_PGA_GAIN_40MV,
                                       .bus_adc_resolution = INA220_RES_SAMPLES_12BIT,
                                       .shunt_adc_resolution = INA220_RES_SAMPLES_12BIT,
                                       .mode = INA220_MODE_SHUNT_BUS_CONT },
    .calibration_factor = 40960000,
    .calibration_reg = 0xA000, /* Calculated using INA220EVM software from Texas Instruments */
    .registers = INA220_REGISTERS,
    .shunt_div = 100,
    .bus_voltage_shift = 3,
    .bus_voltage_lsb = 4, /* in mV */
    .power_lsb = 20
};

static ina220_data_t ina220_data[MAX_INA220_COUNT];

void vTaskINA220( void *Parameters )
{
    uint8_t i;
    TickType_t xLastWakeTime;
    /* Task will run every 100ms */
    const TickType_t xFrequency = INA220_UPDATE_RATE / portTICK_PERIOD_MS;

    extern const SDR_type_01h_t SDR_FMC1_12V;

    sensor_t * ina220_sensor;
    ina220_data_t * data_ptr;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        /* Read all registers from the INA220s */
        for ( i = 0; i < MAX_INA220_COUNT; i++) {
            ina220_readall( &ina220_data[i] );

            ina220_sensor = ina220_data[i].sensor;
            data_ptr = &ina220_data[i];

            if (ina220_sensor == NULL) {
                continue;
            }

            switch ((GET_SENSOR_TYPE(ina220_sensor))) {
            case SENSOR_TYPE_VOLTAGE:
                ina220_sensor->readout_value = (data_ptr->regs[INA220_BUS_VOLTAGE] >> data_ptr->config->bus_voltage_shift)/16;
                break;
            case SENSOR_TYPE_CURRENT:
                /* Current in mA */
                ina220_sensor->readout_value = data_ptr->regs[INA220_CURRENT]/32;
                break;
            default:
                /* Shunt voltage and power not implemented */
                break;
            }

            /* Check for threshold events */
            check_sensor_event(ina220_sensor);

#ifdef MODULE_PAYLOAD
            if( ina220_sensor->sdr == &SDR_FMC1_12V ) {
                /* Check if the Payload power is in an acceptable zone */
                SDR_type_01h_t * ina220_sdr = ( SDR_type_01h_t * ) ina220_sensor->sdr;
                if ( ( ina220_sensor->readout_value >= (ina220_sdr->lower_critical_thr ) ) &&
                     ( ina220_sensor->readout_value <= (ina220_sdr->upper_critical_thr ) ) ) {
                    payload_send_message( FRU_AMC, PAYLOAD_MESSAGE_PPGOOD );
                } else {
                    payload_send_message( FRU_AMC, PAYLOAD_MESSAGE_PPGOODn );
                }
            }
#endif
            vTaskDelayUntil( &xLastWakeTime, xFrequency );
        }
    }
}

uint8_t ina220_config( ina220_data_t * data )
{
    uint8_t i2c_interf, i2c_addr;

    data->config = &ina220_cfg;

    if ((data->rshunt == 0) || (data->rshunt > data->config->calibration_factor)) {
        data->rshunt = INA220_RSHUNT_DEFAULT;
    } else {
        return -1;
    }
    data->curr_reg_config = data->config->config_reg_default;

    if( i2c_take_by_chipid( data->sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        uint8_t cfg_buff[3] = { INA220_CONFIG, ( data->curr_reg_config.cfg_word >> 8) , ( data->curr_reg_config.cfg_word & 0xFFFF) };

        xI2CMasterWrite( i2c_interf, i2c_addr, cfg_buff, sizeof(cfg_buff)/sizeof(cfg_buff[0]) );

        i2c_give( i2c_interf );
        return 0;
    }
    return -1;
}

Bool ina220_readvalue( ina220_data_t * data, uint8_t reg, uint16_t *read )
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t val[2] = {0};

    if( i2c_take_by_chipid( data->sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        xI2CMasterWriteRead( i2c_interf, i2c_addr, reg, &val[0], sizeof(val)/sizeof(val[0]) );

        i2c_give( i2c_interf );

        *read = (val[0] << 8) | (val[1]);
        return true;
    }

    return false;
}

void ina220_readall( ina220_data_t * data )
{
    /* Read all INA220 Registers */
    for ( uint8_t i = 0; i < INA220_REGISTERS; i++ ) {
        ina220_readvalue( data, i, &(data->regs[i]) );
    }
}

Bool ina220_calibrate( ina220_data_t * data )
{
    uint8_t i2c_interf, i2c_addr;
    uint16_t cal = data->config->calibration_reg;
    uint8_t cal_reg[3] = { INA220_CALIBRATION, (cal >> 8), (cal & 0xFFFF) };

    if( i2c_take_by_chipid( data->sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {

        xI2CMasterWrite( i2c_interf, i2c_addr, &cal_reg[0], sizeof(cal_reg)/sizeof(cal_reg[0]) );

        i2c_give( i2c_interf );
        return true;
    }

    return false;
}

void ina220_init( void )
{
    sensor_t *temp_sensor;
    uint8_t i = 0;

    xTaskCreate( vTaskINA220, "INA220", 100, (void *) NULL, tskINA220SENSOR_PRIORITY, &vTaskINA220_Handle);

    /* Iterate through the SDR Table to find all the INA220 entries */
    for ( temp_sensor = sdr_head; temp_sensor != NULL; temp_sensor = temp_sensor->next) {

        if ( temp_sensor->task_handle == NULL ) {
            continue;
        }

        /* Check if this task should update the selected SDR */
        if ( *(temp_sensor->task_handle) != vTaskINA220_Handle ) {
            continue;
        }

        if (i < MAX_INA220_COUNT ) {
            ina220_data[i].sensor = temp_sensor;
            ina220_config( &ina220_data[i] );
            ina220_calibrate( &ina220_data[i] );
            ina220_data[i].sensor->signed_flag = 0;

            if ((GET_SENSOR_TYPE(ina220_data[i].sensor)) == SENSOR_TYPE_CURRENT ) {
                ina220_data[i].sensor->signed_flag = 1;
            }
            i++;
        }
    }
}
