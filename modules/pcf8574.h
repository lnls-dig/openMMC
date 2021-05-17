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

/*
 * PCF8574.h
 *
 *  Created on: 28-07-2014
 *      Author: Bartek
 */

#ifndef PCF8574_H_
#define PCF8574_H_

#define PCF8574_READ_ERROR		0xFF

uint8_t pcf8574_read_port();
uint8_t pcf8574_read_pin(uint8_t pin_num);

void pcf8574_set_port_dir_output(uint8_t pin_mask);
void pcf8574_set_port_dir_input(uint8_t pin_mask);
void pcf8574_set_port_high(uint8_t pin_mask);
void pcf8574_set_port_low(uint8_t pin_mask);

#endif /* PCF8574_H_ */
