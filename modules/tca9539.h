/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
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
 * @file   tca9539.h
 *
 * @brief  TCA9539 module interface functions declarations
 *
 * @ingroup TCA9539
 */

#ifndef TCA9539_H_
#define TCA9539_H_

#include "port.h"

/**
 * @defgroup TCA9539 TCA9539 16-bit I2C/SMBus I/O Expander Module
 * @ingroup PERIPH_IC
 */

/**
 * @defgroup TCA9539_REGS TCA9539 Registers
 * @ingroup TCA9539
 * @{
 */

/**
 * @brief TCA9539 General purpose I/O port register (Read/write)
 */
#define TCA9539_IN_REG           0x0

/**
 * @brief TCA9539 Output latch register (read / write)
 * @note Default register value = 0xFF
 */
#define TCA9539_OUT_REG          0x2

/**
 * @brief TCA9539 Input polarity port register (read / write)
 * @note Default register value = 0x00
 */
#define TCA9539_IPOL_REG         0x4

/**
 * @brief TCA9539 I/O direction register (read / write)
 * @note Default register value = 0xFF
 */
#define TCA9539_CONFIG_REG       0x6

/**
 * @}
 */

/* Pins Read/Write */
/**
 * @brief Read port pins status
 *
 * @return 8-bit value with the status of port pins
 */
uint8_t tca9539_input_port_get(uint8_t chip_id, uint8_t port_num, uint8_t *readout);

/**
 * @brief Read a pin status
 *
 * @param pin Selected pin to read
 *
 * @return Pin logical status
 */
uint8_t tca9539_input_pin_get(uint8_t chip_id, uint8_t port_num, uint8_t pin, uint8_t *status);

/**
 * @brief Write port output pins
 *
 * @param data 8-bit value with the pins status
 *
 * @return None
 */
uint8_t tca9539_output_port_set(uint8_t chip_id, uint8_t port_num, uint8_t data);

/**
 * @brief Read port output pins
 *
 * @return 8-bit value with the status of port pins
 */
uint8_t tca9539_output_port_get(uint8_t chip_id, uint8_t port_num, uint8_t *readout);

/**
 * @brief Write a output pin
 *
 * @param pin Selected pin to write
 * @param data Logical status to be set
 *
 * @return None
 */
uint8_t tca9539_output_pin_set(uint8_t chip_id, uint8_t port_num, uint8_t pin, bool data);

/* Polarity Control */
/**
 * @brief Set port pins polarity
 *
 * @param pol Polarity (active-high/low)
 */
uint8_t tca9539_port_polarity_set(uint8_t chip_id, uint8_t port_num, uint8_t pol);

/**
 * @brief Read port pins polarity
 *
 * @return Pins polarity (active-high/low)
 */
uint8_t tca9539_port_polarity_get(uint8_t chip_id, uint8_t port_num, uint8_t *pol);

/* Pins direction (output/input) */
/**
 * @brief Set port pins direction
 *
 * @param dir Pin direction ( 0 - Output 1 - Input )
 *
 * @return None
 */
uint8_t tca9539_port_dir_set(uint8_t chip_id, uint8_t port_num, uint8_t dir);

/**
 * @brief Read port pins direction
 *
 * @return 8-bit value with the pins direction setup
 */
uint8_t tca9539_port_dir_get(uint8_t chip_id, uint8_t port_num, uint8_t *dir);

#endif
