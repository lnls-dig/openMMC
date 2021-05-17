/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2021  Wojciech Ruclo <wojciech.ruclo@creotech.pl>
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
 * @file ina219.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @author Wojciech Ruclo <wojciech.ruclo@creotech.pl>
 *
 * @brief INA219 interface implementation
 *
 * @ingroup INA219
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
#include "ina219.h"
#include "fpga_spi.h"
#include "fru.h"

TaskHandle_t vTaskINA219_Handle;

static ina219_data_t ina219_data[MAX_INA219_COUNT];

void vTaskINA219(void *Parameters)
{
    TickType_t xLastWakeTime;

    /* Task will run every 100ms */
    const TickType_t xFrequency = INA219_UPDATE_RATE / portTICK_PERIOD_MS;

    sensor_t * sensor;
    uint8_t i2c_addr, i2c_interf;

    uint8_t val[2];
    int16_t readout;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    int16_t value;
    uint8_t i = 0;

    for (;;) {

        /* Iterate through the SDR Table to find all the INA219 entries */
        for (sensor = sdr_head; sensor != NULL; sensor = sensor->next) {

            if (sensor->task_handle == NULL) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if (*(sensor->task_handle) != xTaskGetCurrentTaskHandle()) {
                continue;
            }

            switch ((GET_SENSOR_TYPE(sensor))) {
            case SENSOR_TYPE_VOLTAGE:

                /* Try to gain the I2C bus */
                if (i2c_take_by_chipid(sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
                    if (xI2CMasterWriteRead(i2c_interf, i2c_addr, INA219_BUS_VOLTAGE_REG, &val[0], 2) == 2) {
                        readout = (val[0] << 8) | (val[1]);
                    }
                    i2c_give(i2c_interf);
                }

                sensor->readout_value = (readout >> 3) / 16;

                break;
            case SENSOR_TYPE_CURRENT:

                // Find current INA219 settings
                for (i = 0; i < MAX_INA219_COUNT; i++) {
                    if (sensor->chipid == ina219_data[i].sensor->chipid){
                        break;
                    }
                }

                /* Try to gain the I2C bus */
                if (i2c_take_by_chipid(sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
                    if (xI2CMasterWriteRead(i2c_interf, i2c_addr, INA219_CURRENT_REG, &val[0], 2) == 2) {
                        readout = (val[0] << 8) | (val[1]);
                    }
                    i2c_give(i2c_interf);
                }

                value = (int16_t) (readout * ina219_data[i].current_lsb * 12.5);
                sensor->readout_value = value;

                break;
            default:
                /* Shunt voltage and power not implemented */
                break;
            }

            /* Check for threshold events */
            sensor_state_check(sensor);
            check_sensor_event(sensor);

            i++;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

static uint8_t ina219_calibrate(ina219_data_t * data)
{
    uint16_t cal;
    uint8_t i2c_interf, i2c_addr;

    cal = (uint16_t) (0.04096 / (data->current_lsb * (data->config->shunt_resistor / 1.0e3)));

    uint8_t cal_reg[3] = { INA219_CALIBRATION_REG, (cal >> 8), (cal & 0xFF) };

    if (i2c_take_by_chipid(data->sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        xI2CMasterWrite(i2c_interf, i2c_addr, cal_reg, sizeof(cal_reg) / sizeof(cal_reg[0]));
        i2c_give(i2c_interf);
        return 0;
    }
    return -1;
}

static uint8_t ina219_configure(ina219_data_t * data)
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t cfg_reg[3] = { INA219_CONFIGURATION_REG, (data->config->config_reg.cfg_word >> 8), (data->config->config_reg.cfg_word & 0xFF) };

    if (i2c_take_by_chipid(data->sensor->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        xI2CMasterWrite(i2c_interf, i2c_addr, cfg_reg, sizeof(cfg_reg) / sizeof(cfg_reg[0]));
        i2c_give(i2c_interf);
        return 0;
    }
    return -1;
}

void ina219_init(void)
{
    sensor_t *sensor;
    uint8_t i = 0;

    xTaskCreate(vTaskINA219, "INA219", 200, (void *) NULL, tskINA220SENSOR_PRIORITY, &vTaskINA219_Handle);

    /* Iterate through the SDR Table to find all the INA219 entries */
    for (sensor = sdr_head; sensor != NULL; sensor = sensor->next) {

        if (sensor->task_handle == NULL) {
            continue;
        }

        /* Check if this task should update the selected SDR */
        if (*(sensor->task_handle) != vTaskINA219_Handle) {
            continue;
        }

        if (i < MAX_INA219_COUNT) {
            ina219_data[i].sensor = sensor;
            ina219_data[i].config = (ina219_config_t *) sensor->settings;
            ina219_data[i].current_lsb = ((float) ina219_data[i].config->maximum_expected_current) / (2 << 15);

            ina219_calibrate(&ina219_data[i]);
            ina219_configure(&ina219_data[i]);

            ina219_data[i].sensor->signed_flag = 0;
            if ((GET_SENSOR_TYPE(ina219_data[i].sensor)) == SENSOR_TYPE_CURRENT) {
                ina219_data[i].sensor->signed_flag = 1;
            }

            i++;
        }
    }
}
