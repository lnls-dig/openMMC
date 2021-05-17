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
 * @defgroup MAX6642 MAX6642  Remote/Local Temperature Sensor with Overtemperature Alarm
 * @ingroup SENSORS
 */

/**
 * @file max6642.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Definitions for MAX6642 I2C Temperature Sensor
 * @ingroup MAX6642
 */

#ifndef MAX6642_H_
#define MAX6642_H_

#include "port.h"

#define MAX6642_UPDATE_RATE             500

#define MAX6642_CMD_READ_LOCAL          0x00
#define MAX6642_CMD_READ_REMOTE         0x01
#define MAX6642_CMD_READ_STATUS         0x02
#define MAX6642_CMD_READ_CONFIG         0x03
#define MAX6642_CMD_READ_LOCAL_LIMIT    0x05
#define MAX6642_CMD_READ_REMOTE_LIMIT   0x07
#define MAX6642_CMD_WRITE_CONFIG        0x09
#define MAX6642_CMD_WRITE_LOCAL_LIMIT   0x0B
#define MAX6642_CMD_WRITE_REMOTE_LIMIT  0x0D
#define MAX6642_CMD_SINGLESHOT          0x0D
#define MAX6642_CMD_READ_REMOTE_EXTD    0x10
#define MAX6642_CMD_READ_LOCAL_EXTD     0x11
#define MAX6642_CMD_READ_MANUF_ID       0xFE

/*  MAX6642 Configuration bits */
#define MAX6642_CFG_ALERT_MASK          (1 << 7)
#define MAX6642_CFG_STOP_RUN_MASK       (1 << 6)
#define MAX6642_CFG_EXT_ONLY_MASK       (1 << 5)
#define MAX6642_CFG_FAULT_QUEUE_MASK    (1 << 4)

/*  MAX6642 Status bits */
#define MAX6642_STATUS_BUSY_MASK        (1 << 7)
#define MAX6642_STATUS_LOCAL_HIGH_MASK  (1 << 6)
#define MAX6642_STATUS_REMOTE_HIGH_MASK (1 << 5)
#define MAX6642_STATUS_OPEN_MASK        (1 << 4)

/**
 * @brief MAX6642 monitor Task Handle
 */
extern TaskHandle_t vTaskMAX6642_Handle;

extern const SDR_type_01h_t SDR_MAX6642_FPGA;

/**
 * @brief Initializes MAX6642 sensor and creates #vTaskMAX6642
 *
 * @return None
 */
void MAX6642_init( void );

/**
 * @brief MAX6642 Monitor Task
 *
 * @param Parameters Pointer to parameter list passed to task upon initialization (not used here)
 */
void vTaskMAX6642( void *Parameters );

/**
 * @brief Reads MAX6642's local temperature value
 *
 * @param sensor Pointer to sensor struct
 *
 * @return Temperature value
 */
Bool max6642_read_local( sensor_t *sensor, uint8_t *temp );

/**
 * @brief Reads MAX6642's remote temperature value
 *
 * @param sensor Pointer to sensor struct
 *
 * @return Temperature value
 */
Bool max6642_read_remote( sensor_t *sensor, uint8_t *temp );

/**
 * @brief Reads MAX6642's local extended temperature value
 *
 * The extended value is the fractional part of the reading, with 0.25 oC steps. The value is the 2 MSBits returned.
 *
 * @param sensor Pointer to sensor struct
 *
 * @return Extended temperature value (2 MSB)
 */
Bool max6642_read_local_extd( sensor_t *sensor, uint8_t *temp );

/**
 * @brief Reads MAX6642's remote extended temperature value
 *
 * The extended value is the fractional part of the reading, with 0.25 oC steps. The value is the 2 MSBits returned.
 *
 * @param sensor Pointer to sensor info struct
 *
 * @return Extended temperature value (2 MSB)
 */
Bool max6642_read_remote_extd( sensor_t *sensor, uint8_t *temp );

/**
 * @brief Reads MAX6642 status byte
 *
 * Status byte: <br>
 * [7] - BUSY -  Indicates the MAX6642 is busy converting data <br>
 * [6] - LHIGH - Internal high temperature fault <br>
 * [5] - Reserved <br>
 * [4] - RHIGH - External high temperature fault <br>
 * [3] - Reserved <br>
 * [2] - OPEN -  Diode open condition <br>
 * [1:0] - Reserved <br>
 *
 * @param sensor Pointer to sensor info struct
 *
 * @return Status byte
 */
uint8_t max6642_read_status( sensor_t *sensor );

/**
 * @brief Reads MAX6642 configuration byte
 *
 * Cfg byte: <br>
 * [7] - MASK1 - Disables 'ALERT interrupts <br>
 * [6] - STOP/RUN - Puts MAX6642 in standby mode <br>
 * [5] - External Only - disables local temperature measurements so that only remote temperature is measured. The measurement rate becomes 8Hz. <br>
 * [4] - Fault queue - (0):'ALERT is set by single fault; (1):Two consecutive faults are required to assert 'ALERT <br>
 * [3:0] - Reserved <br>
 *
 * @param sensor Pointer to sensor info struct
 *
 * @return Configuration byte
 */
uint8_t max6642_read_cfg( sensor_t *sensor );

/**
 * @brief Writes MAX6642 configuration byte
 *
 * Cfg byte: <br>
 * [7] - MASK1 - Disables 'ALERT interrupts <br>
 * [6] - STOP/RUN - Puts MAX6642 in standby mode <br>
 * [5] - External Only - disables local temperature measurements so that only remote temperature is measured. The measurement rate becomes 8Hz. <br>
 * [4] - Fault queue - (0):'ALERT is set by single fault; (1):Two consecutive faults are required to assert 'ALERT <br>
 * [3:0] - Reserved <br>
 *
 * @param sensor Pointer to sensor info struct
 * @param cfg Configuration byte
 *
 * @return None
 */
void max6642_write_cfg( sensor_t *sensor, uint8_t cfg );

/**
 * @brief Sets internal high temperature limit that asserts 'ALERT
 *
 * @param sensor Pointer to sensor info struct
 * @param limit High temperature limit value
 */
void max6642_write_local_limit( sensor_t *sensor, uint8_t limit );

/**
 * @brief Sets external high temperature limit that asserts 'ALERT
 *
 * @param sensor Pointer to sensor info struct
 * @param limit High temperature limit value
 */
void max6642_write_remote_limit( sensor_t *sensor, uint8_t limit );

#endif
