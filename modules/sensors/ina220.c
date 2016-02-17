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

/*!
 * @file ina220.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief DCDCs Voltage reading using INA220 sensor
 */

#include "FreeRTOS.h"
#include "task.h"
#include "port.h"
#include "sdr.h"
#include "task_priorities.h"
#include "board_version.h"
#include "payload.h"
#include "ina220.h"

const t_ina220_config ina220_cfg = {
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

static t_ina220_data ina220_data[MAX_INA220_COUNT];

void vTaskINA220( void *Parameters )
{
    uint8_t i;

    TickType_t xLastWakeTime;
    /* Task will run every 100ms */
    const TickType_t xFrequency = INA220_UPDATE_RATE / portTICK_PERIOD_MS;
    sensor_t * ina220_sensor;
    t_ina220_data * data_ptr;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        /* Read all registers from the INA220s */
        for ( i = 0; i < MAX_INA220_COUNT; i++) {
            ina220_readall( &ina220_data[i] );

            ina220_sensor = ina220_data[i].sensor;
            data_ptr = &ina220_data[i];

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

            switch (GET_SENSOR_NUMBER(ina220_sensor)) {
            case NUM_SDR_FMC2_12V:
                /* Check if the Payload power is in an acceptable zone */
                if ((ina220_sensor->state == SENSOR_STATE_NORMAL) || (ina220_sensor->state == SENSOR_STATE_HIGH) || (ina220_sensor->state == SENSOR_STATE_LOW ) ) {
                    payload_send_message(PAYLOAD_MESSAGE_P12GOOD);
                } else {
                    payload_send_message(PAYLOAD_MESSAGE_P12GOODn);
                }
                break;
            default:
                break;
            }
            vTaskDelayUntil( &xLastWakeTime, xFrequency );
        }
    }
}

uint8_t ina220_config(uint8_t i2c_id, t_ina220_data * data)
{
    uint8_t i2c_bus = I2C1;

    data->config = &ina220_cfg;
    data->i2c_id = i2c_id;
    if ((data->rshunt == 0) || (data->rshunt > data->config->calibration_factor)) {
        data->rshunt = INA220_RSHUNT_DEFAULT;
    } else {
        return -1;
    }
    data->curr_reg_config = data->config->config_reg_default;

    /* @todo Change to 'i2c_take_by_chipid', which is more generic */
    if( i2c_take_by_busid( I2C_BUS_CPU_ID, &(i2c_bus), (TickType_t) 100) == pdFALSE ) {
        return -1;
    }

    uint8_t cfg_buff[3] = { INA220_CONFIG, ( data->curr_reg_config.cfg_word >> 8) , ( data->curr_reg_config.cfg_word & 0xFFFF) };

    portENABLE_INTERRUPTS();
    xI2CMasterWrite( i2c_bus, data->i2c_id, cfg_buff, sizeof(cfg_buff)/sizeof(cfg_buff[0]) );
    portDISABLE_INTERRUPTS();

    i2c_give(i2c_bus);

    return 0;
}

uint16_t ina220_readvalue( t_ina220_data * data, uint8_t reg )
{
    uint8_t i2c_bus;
    uint8_t val[2];

    /*! @todo: Change to 'i2c_take_by_chipid', which is more generic */
    if( i2c_take_by_busid( I2C_BUS_CPU_ID, &(i2c_bus), (TickType_t) 10) == pdFALSE ) {
        return -1;
    }

    xI2CMasterWriteRead( i2c_bus, data->i2c_id, reg, &val[0], sizeof(val)/sizeof(val[0]) );

    i2c_give( i2c_bus );

    return ( (val[0] << 8) | (val[1]) );
}

void ina220_readall( t_ina220_data * data )
{
    /* Read all INA220 Registers */
    for ( uint8_t i = 0; i < INA220_REGISTERS; i++ ) {
        data->regs[i] = ina220_readvalue( data, i );
    }
}

Bool ina220_calibrate( t_ina220_data * data )
{
    uint8_t i2c_bus;
    uint16_t cal = data->config->calibration_reg;
    uint8_t cal_reg[3] = { INA220_CALIBRATION, (cal >> 8), (cal & 0xFFFF) };

    /*! @todo: Change to 'i2c_take_by_chipid', which is more generic */
    if( i2c_take_by_busid( I2C_BUS_CPU_ID, &(i2c_bus), (TickType_t) 10) == pdFALSE ) {
        return false;
    }

    portENABLE_INTERRUPTS();
    xI2CMasterWrite( i2c_bus, data->i2c_id, &cal_reg[0], sizeof(cal_reg)/sizeof(cal_reg[0]) );
    portDISABLE_INTERRUPTS();

    i2c_give( i2c_bus );

    return true;
}

void ina220_init( void )
{
    uint8_t i, j;
    i = 0;

    xTaskCreate( vTaskINA220, "INA220", 400, (void *) NULL, tskINA220SENSOR_PRIORITY, &vTaskINA220_Handle);

    for ( j = 0; j < NUM_SDR; j++ ) {
        /* Update their SDR */
        /* Check if the handle pointer is not NULL */
        if (sensor_array[j].task_handle == NULL) {
            continue;
        }

        /* Check if this task should update the selected SDR */
        if ( *(sensor_array[j].task_handle) != vTaskINA220_Handle ) {
            continue;
        }

        if (i < MAX_INA220_COUNT ) {
            ina220_data[i].sensor = &sensor_array[j];
            ina220_data[i].i2c_id = sensor_array[j].slave_addr;
            ina220_config( ina220_data[i].i2c_id, &ina220_data[i] );
            ina220_calibrate( &ina220_data[i] );

            if ((GET_SENSOR_TYPE(ina220_data[i].sensor)) == SENSOR_TYPE_CURRENT ) {
                ina220_data[i].sensor->signed_flag = 1;
            }
            i++;
        }
    }
}
