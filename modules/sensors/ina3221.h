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
 * @defgroup INA3221 INA3221 - High- or Low-Side, Bidirectional Current and Power Monitor
 * @ingroup SENSORS
 *
 * The INA3221 is a current shunt and power monitor with an I2C- or SMBUS-compatible interface. <br>
 * The INA3221 monitors both shunt drop and supply voltage.
 */

/**
 * @file ina3221.h
 *
 * @brief Definitions for INA3221 Current/Voltage Sensor
 *
 * @ingroup INA3221
 */

#ifndef INA3221_H_
#define INA3221_H_

#include "FreeRTOS.h"
#include "port.h"

#define MAX_INA3221_COUNT        6
#define INA3221_UPDATE_RATE      100

#define INA3221_CHANNEL_1        0
#define INA3221_CHANNEL_2        1
#define INA3221_CHANNEL_3        2

/**
 * @defgroup INA3221_REGS INA3221 Registers
 * @ingroup INA3221
 * @{
 */

#define INA3221_CONFIG                   0x00
#define INA3221_SHUNT_VOLTAGE            0x01
#define INA3221_BUS_VOLTAGE              0x02
#define INA3221_CRITICAL_ALERT_LIMIT     0x07
#define INA3221_WARNING_ALERT_LIMIT      0x08
#define INA3221_SHUNT_VOLTAGE_SUM        0x0D
#define INA3221_SHUNT_VOLTAGE_SUM_LIMIT  0x0E
#define INA3221_MASK_ENABLE              0x0F
#define INA3221_POWER_UPPER_VALID        0x10
#define INA3221_POWER_VALID_LOWER_LIMIT  0x11
#define INA3221_MANUFACTURER_ID          0xFE
#define INA3221_DIE_ID                   0xFF

/**
 * @}
 */

/* Operating modes */
#define INA3221_MODE_POWER_DOWN          0x0
#define INA3221_MODE_SHUNT_TRIG          0x1
#define INA3221_MODE_BUS_TRIG            0x2
#define INA3221_MODE_SHUNT_BUS_TRIG      0x3
#define INA3221_MODE_SHUNT_CONT          0x5
#define INA3221_MODE_BUS_CONT            0x6
#define INA3221_MODE_SHUNT_BUS_CONT      0x7

/**
 * @brief INA3221 Register Count
 */
#define INA3221_VOLTAGE_REGISTERS        6

#define INA3221_MAX_DELAY                10      /* worst case delay in ms */

#define INA3221_RSHUNT_DEFAULT           2

typedef union {
    struct {
#ifdef BF_MS_FIRST
        uint16_t mode:3;
        uint16_t shunt_voltage_conversion_time:3;
        uint16_t bus_voltage_conversion_time:3;
        uint16_t averaging_mode:3;
        uint16_t channel_enable_mode:3;
        uint16_t reset:1;
#else
        uint16_t reset:1;
        uint16_t channel_enable_mode:3;
        uint16_t averaging_mode:3;
        uint16_t bus_voltage_conversion_time:3;
        uint16_t shunt_voltage_conversion_time:3;
        uint16_t mode:3;
#endif
    } cfg_struct;
    uint16_t cfg_word;
} ina3221_config_reg_t;

typedef struct {
    uint16_t shunt_resistor[3]; /* mOhm */
} ina3221_config_t;

typedef struct {
    uint8_t chipid;
    sensor_t * sensors[6];
    const ina3221_config_t * config;
    uint32_t rshunt;
    ina3221_config_reg_t curr_reg_config;
    uint16_t regs[INA3221_VOLTAGE_REGISTERS];
} ina3221_data_t;

extern TaskHandle_t vTaskINA3221_Handle;

uint8_t ina3221_read_reg( ina3221_data_t * data, uint8_t reg, uint16_t *read );
uint8_t ina3221_read_voltages( ina3221_data_t * data );
void ina3221_init( void );
void vTaskINA3221( void* Parameters );

#endif
