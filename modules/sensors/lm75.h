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
 * @defgroup LM75 LM75 - Temperature Sensor
 * @ingroup SENSORS
 *
 * The LM75 is a temperature sensor, Delta-Sigma analog-to-digital converter, and digital over-temperature detector with I2C interface.
 * The host can query the LM75 at any time to read temperature. <br>
 * The open-drain Overtemperature Shutdown (O.S.) output becomes active when the temperature exceeds a programmable limit.
 * This pin can operate in either “Comparator” or “Interrupt” mode.
 */

/**
 * @file lm75.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Definitions for LM75 I2C Temperature Sensor
 *
 * @ingroup LM75
 */

#ifndef LM75_H_
#define LM75_H_

/**
 * @brief Rate at which the LM75 sensors are read (in ms)
 */
#define LM75_UPDATE_RATE        500

TaskHandle_t vTaskLM75_Handle;

extern const SDR_type_01h_t SDR_LM75_uC;
extern const SDR_type_01h_t SDR_LM75_ADN4604;
extern const SDR_type_01h_t SDR_LM75_DCDC;
extern const SDR_type_01h_t SDR_LM75_RAM;

/**
 * @brief Initializes LM75 monitoring task
 *
 * @return None
 */
void LM75_init( void );

/**
 * @brief Monitoring task for LM75 sensor
 *
 * This task unblocks after every #LM75_UPDATE_RATE ms and updates the read from all the LM75 sensors listed in this module's SDR table
 *
 * @param Parameters Pointer to parameter list passed to task upon initialization (not used here)
 */
void vTaskLM75( void* Parameters );

#endif
