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

#ifndef PCA9554_H_
#define PCA9554_H_

#define PCA9554_INPUT_REG	0
#define PCA9554_OUTPUT_REG	1
#define PCA9554_POLARITY_REG	2
#define PCA9554_CFG_REG		3

/* Pins Read/Write */
uint8_t pca9554_read_port( void );
uint8_t pca9554_read_pin( uint8_t pin );
void pca9554_write_port( uint8_t data );
void pca9554_write_pin( uint8_t pin, uint8_t data );

/* Polarity Control */
void pca9554_set_port_pol( uint8_t pol );
void pca9554_set_pin_pol( uint8_t pin, uint8_t pol );
uint8_t pca9554_get_port_pol( void );
uint8_t pca9554_get_pin_pol( uint8_t pin );

/* Pins direction (output/input) */
void pca9554_set_port_dir( uint8_t dir );
void pca9554_set_pin_dir( uint8_t pin, uint8_t dir );
uint8_t pca9554_get_port_dir( void );
uint8_t pca9554_get_pin_dir( uint8_t pin );

#endif
