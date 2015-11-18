/*
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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

/*!
 * @file lm75.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief Definitions for LM75 I2C Temperature Sensor
 */

#ifndef INA220_H_
#define INA220_H_

#include "FreeRTOS.h"
#include "port.h"

#define MAX_INA220_COUNT        6
#define INA220_UPDATE_RATE      100

/* common register definitions */
#define INA220_CONFIG                   0x00
#define INA220_SHUNT_VOLTAGE            0x01 /* readonly */
#define INA220_BUS_VOLTAGE              0x02 /* readonly */
#define INA220_POWER                    0x03 /* readonly */
#define INA220_CURRENT                  0x04 /* readonly */
#define INA220_CALIBRATION              0x05

/* register count */
#define INA220_REGISTERS                6

/* settings - depend on use case */
#define INA220_CONFIG_DEFAULT           0x399F  /* PGA=8 */

/* worst case is 68.10 ms (~14.6Hz, ina219) */
#define INA220_CONVERSION_RATE          15
#define INA220_MAX_DELAY                69 /* worst case delay in ms */

#define INA220_RSHUNT_DEFAULT           10000

typedef struct {
    uint16_t config_default;
    uint32_t calibration_factor;
    uint8_t registers;
    uint8_t shunt_div;
    uint8_t bus_voltage_shift;
    uint16_t bus_voltage_lsb;    /* uV */
    uint16_t power_lsb;          /* uW */
} t_ina220_config;

typedef struct {
    uint8_t i2c_id;
    sensor_t * sensor;
    SDR_type_01h_t * pSDR;
    const t_ina220_config * config;
    uint32_t rshunt;
    uint16_t curr_config;
    uint16_t regs[INA220_REGISTERS];
} t_ina220_data;

TaskHandle_t vTaskINA220_Handle;

uint8_t ina220_config(uint8_t i2c_id, t_ina220_data * data);
uint16_t ina220_readvalue( t_ina220_data * data, uint8_t reg );
uint16_t ina220_readvalue( t_ina220_data * data, uint8_t reg );
void ina220_readall( t_ina220_data * data );
void ina220_sdr_init ( TaskHandle_t handle );
void ina220_init( void );

void vTaskINA220( void* Parameters );

#endif
