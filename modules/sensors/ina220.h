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
 * @defgroup INA220 INA220 - High- or Low-Side, Bidirectional Current and Power Monitor
 * @ingroup SENSORS
 *
 * The INA220 is a current shunt and power monitor with an I2C- or SMBUS-compatible interface. <br>
 * The INA220 monitors both shunt drop and supply voltage. A programmable calibration value, combined with an internal multiplier, enables direct readouts in amperes.
 * An additional multiplying register calculates power in watts.
 */

/**
 * @file ina220.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Definitions for INA220 Current/Voltage/Power Sensor
 *
 * @ingroup INA220
 */

#ifndef INA220_H_
#define INA220_H_

#include "FreeRTOS.h"
#include "port.h"

#define MAX_INA220_COUNT        12
#define INA220_UPDATE_RATE      100

/**
 * @defgroup INA220_REGS INA220 Registers
 * @ingroup INA220
 * @{
 */

/**
 * @brief INA220 Configuration register (Read/write)
 *
 * All-register reset, settings for bus voltage range, PGA gain, ADC resolution/averaging
 *
 * @note Default value = 0x399F
 */
#define INA220_CONFIG                   0x00

/**
 * @brief INA220 Shunt Voltage register (Read only)
 *
 * Shunt voltage measurement data
 *
 */
#define INA220_SHUNT_VOLTAGE            0x01

/**
 * @brief INA220 Bus Voltage register (Read only)
 *
 * Bus Voltage measurement data
 *
 */
#define INA220_BUS_VOLTAGE              0x02

/**
 * @brief INA220 Power register (Read only)
 *
 * Power measurement data
 *
 * @note The Power register default to 0 because the Calibration register defaults to 0, yielding a zero current value until the Calibration register is programmed.
 */
#define INA220_POWER                    0x03

/**
 * @brief INA220 Current register (Read only)
 *
 * Contains the value of the current flowing through the shunt resistor.
 *
 * @note The Current register default to 0 because the Calibration register defaults to 0, yielding a zero current value until the Calibration register is programmed.
 */
#define INA220_CURRENT                  0x04

/**
 * @brief INA220 Calibration register (Read/write)
 *
 * Sets full-scale range and LSB of current and power measurements. Overall system calibration
 *
 * @note Default value = 0x0000
 */
#define INA220_CALIBRATION              0x05

/**
 * @}
 */

/**
 * @brief INA220 Register Count
 */
#define INA220_REGISTERS                6

/* Scale range values */
#define INA220_16V_SCALE_RANGE          0x00
#define INA220_32V_SCALE_RANGE          0x01

/* PGA gain */
#define INA220_PGA_GAIN_40MV            0x00
#define INA220_PGA_GAIN_80MV            0x01
#define INA220_PGA_GAIN_160MV           0x02
#define INA220_PGA_GAIN_320MV           0x03

/* ADC Resolution/Averaging */
#define INA220_RES_SAMPLES_9BIT         0x0
#define INA220_RES_SAMPLES_10BIT        0x1
#define INA220_RES_SAMPLES_11BIT        0x2
#define INA220_RES_SAMPLES_12BIT        0x3

#define INA220_RES_SAMPLES_2SMP         0x9
#define INA220_RES_SAMPLES_4SMP         0xA
#define INA220_RES_SAMPLES_8SMP         0xB
#define INA220_RES_SAMPLES_16SMP        0xC
#define INA220_RES_SAMPLES_32SMP        0xD
#define INA220_RES_SAMPLES_64SMP        0xE
#define INA220_RES_SAMPLES_128SMP       0xF

/* Operating modes */
#define INA220_MODE_POWER_DOWN          0x0
#define INA220_MODE_SHUNT_TRIG          0x1
#define INA220_MODE_BUS_TRIG            0x2
#define INA220_MODE_SHUNT_BUS_TRIG      0x3
#define INA220_MODE_ADC_OFF             0x4
#define INA220_MODE_SHUNT_CONT          0x5
#define INA220_MODE_BUS_CONT            0x6
#define INA220_MODE_SHUNT_BUS_CONT      0x7

/* worst case is 68.10 ms (~14.6Hz, ina219) */
#define INA220_CONVERSION_RATE          15
#define INA220_MAX_DELAY                69      /* worst case delay in ms */

#define INA220_RSHUNT_DEFAULT           10000

typedef union {
    struct {
#ifdef BF_MS_FIRST
        uint16_t mode:3;
        uint16_t shunt_adc_resolution:4;
        uint16_t bus_adc_resolution:4;
        uint16_t pga_gain:2;
        uint16_t bus_voltage_range:1;
        uint16_t reserved:1;
        uint16_t reset:1;
#else
        uint16_t reset:1;
        uint16_t reserved:1;
        uint16_t bus_voltage_range:1;
        uint16_t pga_gain:2;
        uint16_t bus_adc_resolution:4;
        uint16_t shunt_adc_resolution:4;
        uint16_t mode:3;
#endif
    } cfg_struct;
    uint16_t cfg_word;
} ina220_config_reg_t;

typedef struct {
    ina220_config_reg_t config_reg_default;
    uint32_t calibration_factor;
    uint16_t calibration_reg;
    uint8_t registers;
    uint8_t shunt_div;
    uint8_t bus_voltage_shift;
    uint16_t bus_voltage_lsb;    /* uV */
    uint16_t power_lsb;          /* uW */
} ina220_config_t;

typedef struct {
    sensor_t * sensor;
    const ina220_config_t * config;
    uint32_t rshunt;
    ina220_config_reg_t curr_reg_config;
    uint16_t regs[INA220_REGISTERS];
} ina220_data_t;

TaskHandle_t vTaskINA220_Handle;

uint8_t ina220_config( ina220_data_t * data );
Bool ina220_calibrate( ina220_data_t * data );
Bool ina220_readvalue( ina220_data_t * data, uint8_t reg, uint16_t *read );
void ina220_readall( ina220_data_t * data );
void ina220_init( void );
void vTaskINA220( void* Parameters );

#endif
