/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Henrique Silva <henrique.silva@lnls.br>
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
 * @file   pca9554.h
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  PCA9554 module interface functions declarations
 *
 * @ingroup PCA9554
 */

#ifndef PCA9554_H_
#define PCA9554_H_

/**
 * @defgroup PCA9554 PCA9554 8-bit I2C/SMBus I/O Expander Module
 * @ingroup PERIPH_IC
 */

/**
 * @defgroup PCA9554_REGS PCA9554 Registers
 * @ingroup PCA9554
 * @{
 */

/**
 * @brief PCA9554 Input port status register (Read only)
 * @note Default register value is not defined
 */
#define PCA9554_INPUT_REG       0

/**
 * @brief PCA9554 Output port register (Read/write)
 * @note Default register value = 0xFF
 */
#define PCA9554_OUTPUT_REG      1

/**
 * @brief PCA9554 Polarity Inversion register (Read/write)
 * @note Default register value = 0x00
 */
#define PCA9554_POLARITY_REG    2

/**
 * @brief PCA9554 Configuration register (Read/write)
 * @note Default register value = 0xFF
 */
#define PCA9554_CFG_REG         3

/**
 * @}
 */


/* Pins Read/Write */
/**
 * @brief Read all pins status
 *
 * @return 8-bit value with the pins status
 */
uint8_t pca9554_read_port( uint8_t *readout );

/**
 * @brief Read a pin status
 *
 * @param pin Selected pin to read
 *
 * @return Pin logical status
 */
uint8_t pca9554_read_pin( uint8_t pin, uint8_t *status );

/**
 * @brief Write all output pins
 *
 * @param data 8-bit value with the pins status
 *
 * @return None
 */
uint8_t pca9554_write_port( uint8_t data );

/**
 * @brief Write a output pin
 *
 * @param pin Selected pin to write
 * @param data Logical status to be set
 *
 * @return None
 */
uint8_t pca9554_write_pin( uint8_t pin, bool data );

/* Polarity Control */
/**
 * @brief Set all pins polarity
 *
 * @param pol Polarity (active-high/low)
 */
uint8_t pca9554_set_port_pol( uint8_t pol );

/**
 * @brief Set pin polarity
 *
 * @param pin Selected pin
 * @param pol Pin Polarity (active-high/low)
 */
uint8_t pca9554_set_pin_pol( uint8_t pin, bool pol );

/**
 * @brief Read all pins polarity
 *
 * @return Pins polarity (active-high/low)
 */
uint8_t pca9554_get_port_pol( uint8_t *pol );

/**
 * @brief Read pin polarity
 *
 * @param pin Selected pin
 *
 * @return Pin polarity value
 */
uint8_t pca9554_get_pin_pol( uint8_t pin, uint8_t *pol );

/* Pins direction (output/input) */
/**
 * @brief Set all pins direction
 *
 * @param dir Pin direction ( 0-Output 1-Input )
 *
 * @return None
 */
uint8_t pca9554_set_port_dir( uint8_t dir );

/**
 * @brief Set pin direction
 *
 * @param pin Selected pin
 * @param dir Pin direction ( 0-Output 1-Input )
 *
 * @return None
 */
uint8_t pca9554_set_pin_dir( uint8_t pin, bool dir );

/**
 * @brief Read all pins direction
 *
 * @return 8-bit value with the pins direction setup
 */
uint8_t pca9554_get_port_dir( uint8_t *dir );

/**
 * @brief Read pin direction
 *
 * @param pin Selected pin
 *
 * @return Pin direction
 */
uint8_t pca9554_get_pin_dir( uint8_t pin, uint8_t *dir );

#endif
