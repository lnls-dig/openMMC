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
 * @file ina219.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @author Wojciech Ruclo <wojciech.ruclo@creotech.pl>
 *
 * @brief Definitions for INA219 Current/Voltage/Power Sensor
 *
 * @ingroup INA219
 */

#ifndef INA219_H_
#define INA219_H_

#include "FreeRTOS.h"
#include "port.h"

#define MAX_INA219_COUNT        12

#define INA219_UPDATE_RATE      100

/**
 * @defgroup INA219_REGS INA219 Registers
 * @ingroup INA219
 * @{
 */

/**
 * @brief INA219 Configuration register (Read/write)
 *
 * All-register reset, settings for bus voltage range, PGA gain, ADC resolution/averaging
 *
 * @note Default value = 0x399F
 */
#define INA219_CONFIGURATION_REG        0x00

/**
 * @brief INA219 Shunt Voltage register (Read only)
 *
 * Shunt voltage measurement data
 *
 */
#define INA219_SHUNT_VOLTAGE_REG        0x01

/**
 * @brief INA219 Bus Voltage register (Read only)
 *
 * Bus Voltage measurement data
 *
 */
#define INA219_BUS_VOLTAGE_REG          0x02

/**
 * @brief INA219 Power register (Read only)
 *
 * Power measurement data
 *
 * @note The Power register default to 0 because the Calibration register defaults to 0, yielding a zero current value until the Calibration register is programmed.
 */
#define INA219_POWER_REG                0x03

/**
 * @brief INA219 Current register (Read only)
 *
 * Contains the value of the current flowing through the shunt resistor.
 *
 * @note The Current register default to 0 because the Calibration register defaults to 0, yielding a zero current value until the Calibration register is programmed.
 */
#define INA219_CURRENT_REG              0x04

/**
 * @brief INA219 Calibration register (Read/write)
 *
 * Sets full-scale range and LSB of current and power measurements. Overall system calibration
 *
 * @note Default value = 0x0000
 */
#define INA219_CALIBRATION_REG          0x05

/**
 * @}
 */

/* Scale range values */
#define INA219_16V_SCALE_RANGE          0x00
#define INA219_32V_SCALE_RANGE          0x01

/* PGA gain */
#define INA219_PGA_GAIN_40MV            0x00
#define INA219_PGA_GAIN_80MV            0x01
#define INA219_PGA_GAIN_160MV           0x02
#define INA219_PGA_GAIN_320MV           0x03

/* ADC Resolution/Averaging */
#define INA219_RES_SAMPLES_9BIT         0x0
#define INA219_RES_SAMPLES_10BIT        0x1
#define INA219_RES_SAMPLES_11BIT        0x2
#define INA219_RES_SAMPLES_12BIT        0x3

#define INA219_RES_SAMPLES_2SMP         0x9
#define INA219_RES_SAMPLES_4SMP         0xA
#define INA219_RES_SAMPLES_8SMP         0xB
#define INA219_RES_SAMPLES_16SMP        0xC
#define INA219_RES_SAMPLES_32SMP        0xD
#define INA219_RES_SAMPLES_64SMP        0xE
#define INA219_RES_SAMPLES_128SMP       0xF

/* Operating modes */
#define INA219_MODE_POWER_DOWN          0x0
#define INA219_MODE_SHUNT_TRIG          0x1
#define INA219_MODE_BUS_TRIG            0x2
#define INA219_MODE_SHUNT_BUS_TRIG      0x3
#define INA219_MODE_ADC_OFF             0x4
#define INA219_MODE_SHUNT_CONT          0x5
#define INA219_MODE_BUS_CONT            0x6
#define INA219_MODE_SHUNT_BUS_CONT      0x7

typedef union
{
    struct
    {
        uint16_t mode :3;
        uint16_t shunt_adc_resolution :4;
        uint16_t bus_adc_resolution :4;
        uint16_t pga_gain :2;
        uint16_t bus_voltage_range :1;
        uint16_t reserved :1;
        uint16_t reset :1;
    } cfg_struct;
    uint16_t cfg_word;
} ina219_config_reg_t;

typedef struct
{
    ina219_config_reg_t config_reg;
    uint32_t shunt_resistor; /* mOhm */
    uint8_t maximum_expected_current; /* A */
} ina219_config_t;

typedef struct
{
    sensor_t * sensor;
    const ina219_config_t * config;
    float current_lsb;
} ina219_data_t;

extern TaskHandle_t vTaskINA219_Handle;

/**
 * @brief Initializes INA219 monitoring task
 *
 * @return None
 */
void ina219_init(void);

/**
 * @brief Monitoring task for INA219 sensor
 *
 * This task unblocks after every #INA219_UPDATE_RATE ms and updates the read from all the INA219 sensors listed in this module's SDR table
 *
 * @param Parameters Pointer to parameter list passed to task upon initialization (not used here)
 */
void vTaskINA219(void* Parameters);

#endif
