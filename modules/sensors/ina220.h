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

/* Scale range values */
#define INA220_16V_SCALE_RANGE          0x00
#define INA220_32V_SCALE_RANGE          0x01

/* PGA gain */
#define INA220_PGA_GAIN_40MV           0x00
#define INA220_PGA_GAIN_80MV           0x01
#define INA220_PGA_GAIN_160MV          0x02
#define INA220_PGA_GAIN_320MV          0x03

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
#define INA220_MAX_DELAY                69 /* worst case delay in ms */

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
} t_ina220_config_reg;

typedef struct {
    t_ina220_config_reg config_reg_default;
    uint32_t calibration_factor;
    uint16_t calibration_reg;
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
    t_ina220_config_reg curr_reg_config;
    uint16_t regs[INA220_REGISTERS];
} t_ina220_data;

TaskHandle_t vTaskINA220_Handle;

uint8_t ina220_config(uint8_t i2c_id, t_ina220_data * data);
Bool ina220_calibrate( t_ina220_data * data );
uint16_t ina220_readvalue( t_ina220_data * data, uint8_t reg );
uint16_t ina220_readvalue( t_ina220_data * data, uint8_t reg );
void ina220_readall( t_ina220_data * data );
void ina220_sdr_init ( TaskHandle_t handle );
void ina220_init( void );

void vTaskINA220( void* Parameters );

#endif
