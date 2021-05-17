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
 * @defgroup ADT7420 - Temperature Sensor
 * @ingroup SENSORS
 *
 * The ADT7420 is a temperature sensor, Delta-Sigma analog-to-digital converter, and digital over-temperature detector with I2C interface.
 * The host can query the ADT7420 at any time to read temperature. <br>
 * The open-drain Overtemperature Shutdown (O.S.) output becomes active when the temperature exceeds a programmable limit.
 * This pin can operate in either “Comparator” or “Interrupt” mode.
 */

/**
 * @file adt7420.h
 *
 * @brief Definitions for ADT7420 I2C Temperature Sensor
 *
 * @ingroup ADT7420
 */

#ifndef ADT7420_H_
#define ADT7420_H_

/**
 * @brief Rate at which the ADT7420 sensors are read (in ms)
 */
#define ADT7420_UPDATE_RATE        500

extern TaskHandle_t vTaskADT7420_Handle;

/**
 * @brief Initializes ADT7420 monitoring task
 *
 * @return None
 */
void ADT7420_init( void );

/**
 * @brief Monitoring task for ADT7420 sensor
 *
 * This task unblocks after every #ADT7420_UPDATE_RATE ms and updates the read from all the ADT7420 sensors listed in this module's SDR table
 *
 * @param Parameters Pointer to parameter list passed to task upon initialization (not used here)
 */
void vTaskADT7420( void* Parameters );

#endif
