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

#ifndef FPGA_SPI_H_
#define FPGA_SPI_H_

#include <assert.h>

#include "sdr.h"
#include "utils.h"

#define FPGA_UPDATE_RATE        5000    // in ms
#define FPGA_MEM_ADDR_MAX       0xFF

#define WR_COMMAND              0x80
#define RD_COMMAND              0x00

#define NO_DIAG                 0x00
#define FPGA_TEMP_DEVID         0x01
#define FMC1_TEMP_DEVID         0x02
#define FMC2_TEMP_DEVID         0x03
#define DCDC_TEMP_DEVID         0x04
#define RAM_TEMP_DEVID          0x05
#define FMC1_12V_CURR_DEVID     0x06
#define FMC1_P3V3_CURR_DEVID    0x07
#define FMC1_VADJ_CURR_DEVID    0x08
#define FMC2_12V_CURR_DEVID     0x09
#define FMC2_P3V3_CURR_DEVID    0x0A
#define FMC2_VADJ_CURR_DEVID    0x0B
#define FMC1_12V_DEVID          0x0C
#define FMC1_P3V3_DEVID         0x0D
#define FMC1_VADJ_DEVID         0x0E
#define FMC2_12V_DEVID          0x0F
#define FMC2_P3V3_DEVID         0x10
#define FMC2_VADJ_DEVID         0x11


typedef struct __attribute__ ((__packed__)) {
#ifdef BF_MS_FIRST
    uint8_t dev_id;
    uint32_t measure:24;
#else
    uint32_t measure:24;
    uint8_t dev_id;
#endif
} sensor_diag_t;

typedef struct __attribute__ ((__packed__)) {
#ifdef BF_MS_FIRST
    uint32_t reserved:26,
        fmc2_pg_c2m:1,
        fmc1_pg_c2m:1,
        fmc2_pg_m2c:1,
        fmc1_pg_m2c:1,
        fmc2_prsnt_m2c_n:1,
        fmc1_prsnt_m2c_n:1;
#else
    uint32_t fmc1_prsnt_m2c_n:1,
        fmc2_prsnt_m2c_n:1,
        fmc1_pg_m2c:1,
        fmc2_pg_m2c:1,
        fmc1_pg_c2m:1,
        fmc2_pg_c2m:1,
        reserved:26;
#endif
} fmc_diag_t;

/**
 * @brief AFC diagnostic struct sent to FPGA via SPI
 */
typedef struct __attribute__ ((__packed__,aligned(4))) {
    uint32_t cardID[4];
    uint32_t slot_id:16,
        ipmi_addr:16;
    uint32_t data_valid;
    sensor_diag_t sensor[NUM_SENSOR];
    fmc_diag_t fmc_slot;
} board_diagnostic_t;

/* Guarantee buffer can be read as an uint32_t array
 * FIXME: use static_assert when moving build to C11 */
_Static_assert(sizeof(board_diagnostic_t) % sizeof(uint32_t) == 0);

/**
 * @brief FPGA Diagnostics Task
 *
 * This task formats all sensors information and sends to the FPGA via SPI.
 * All the information is accessed by the FPGA using the Wishbone stream.
 *
 * @param Parameters Pointer to parameters passed to the task upon initialization.
 */
void vTaskFPGA_COMM( void * Parameters );

/**
 * @brief Initializes the FPGA Diagnostics Task
 *
 */
void fpga_spi_init( void );

#endif
