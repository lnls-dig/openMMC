/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

#ifndef AFC_BOARD_VERSION_H_
#define AFC_BOARD_VERSION_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "port.h"

#define CARRIER_TYPE_UNKNOWN    0xFF
#define CARRIER_TYPE_AFC        0x01
#define CARRIER_TYPE_AFCK       0x02

#define BOARD_VERSION_AFC_V1_0  0x00
#define BOARD_VERSION_AFC_V2_0  0x01
#define BOARD_VERSION_AFC_V3_0  0x02
#define BOARD_VERSION_AFC_V3_1  0x03
#define BOARD_VERSION_UNKNOWN   0xFF

#define I2CMODE_POOLING         1
#define I2CMODE_INTERRUPT       0
#define SPEED_100KHZ            100000

// BUS_ID
// 0 - FMC1
// 1 - FMC2
// 3 - CPU_ID
//
///////////////////////

enum {
    I2C_BUS_FMC1_ID,
    I2C_BUS_FMC2_ID,
    I2C_BUS_CPU_ID,
    I2C_BUS_RTM_ID,
    I2C_BUS_CLOCK_ID,
    I2C_BUS_FPGA_ID,
    I2C_BUS_UNKNOWN_ID = 0xFF
};

enum {
    CHIP_ID_MUX = 0,
    CHIP_ID_LM75AIM_0,
    CHIP_ID_LM75AIM_1,
    CHIP_ID_LM75AIM_2,
    CHIP_ID_LM75AIM_3,
    CHIP_ID_MAX6642,
    CHIP_ID_RTC,
    CHIP_ID_RTC_EEPROM,
    CHIP_ID_EEPROM,
    CHIP_ID_EEPROM_ID,
    CHIP_ID_INA_0,
    CHIP_ID_INA_1,
    CHIP_ID_INA_2,
    CHIP_ID_INA_3,
    CHIP_ID_INA_4,
    CHIP_ID_INA_5,
    CHIP_ID_ADN,
    CHIP_ID_SI57x,
    CHIP_ID_FMC1_EEPROM,
    CHIP_ID_FMC1_LM75_1,
    CHIP_ID_FMC1_LM75_0,
    CHIP_ID_FMC2_EEPROM,
    CHIP_ID_FMC2_LM75_1,
    CHIP_ID_FMC2_LM75_0
};

typedef struct {
    uint8_t carrier_type;
    uint8_t board_version;
    uint8_t manufacturer[3];
    uint8_t manufacturing_day[2];
    uint8_t crc;
} manufacturing_info_raw;

typedef struct {
    uint8_t carrier_version;
    uint8_t board_version;
    uint32_t manufacturer;
    uint16_t manufacturing_day;
} manufacturing_info;

typedef struct  {
    SemaphoreHandle_t semaphore;
    TickType_t start_time;
    I2C_ID_T i2c_bus;
} I2C_Mutex;

extern manufacturing_info_raw board_info;

void board_i2c_init( void );
void board_discover( void );
void get_manufacturing_info( manufacturing_info_raw *p_board_info );
void get_board_type( uint8_t *carrier_type, uint8_t *board_version);
Bool i2c_take_by_busid( uint8_t bus_id, I2C_ID_T * i2c_interface, TickType_t max_wait_time );
Bool i2c_take_by_chipid( uint8_t chip_id, uint8_t * i2c_address, I2C_ID_T * i2c_interface,  TickType_t max_wait_time );
void i2c_give( I2C_ID_T i2c_interface );

#endif /* AFC_BOARD_VERSION_H_ */
