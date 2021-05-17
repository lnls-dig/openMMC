/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Krzysztof Macias <krzysztof.macias@creotech.pl>
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

#ifndef IDT_8V54816_H_
#define IDT_8V54816_H_

#include "port.h"

#define IDT_MASK_SRC     0xF
#define IDT_MASK_POL     1<<5
#define IDT_MASK_TERM    1<<6
#define IDT_MASK_DIR     1<<7

#define IDT_POL_N        0
#define IDT_POL_P        1<<5

#define IDT_TERM_OFF     0
#define IDT_TERM_ON      1<<6

#define IDT_DIR_IN       0
#define IDT_DIR_OUT      1<<7


enum IDT_CLK_SRC{
    IDT_SRC_CH0,
    IDT_SRC_CH1,
    IDT_SRC_CH2,
    IDT_SRC_CH3,
    IDT_SRC_CH4,
    IDT_SRC_CH5,
    IDT_SRC_CH6,
    IDT_SRC_CH7,
    IDT_SRC_CH8,
    IDT_SRC_CH9,
    IDT_SRC_CH10,
    IDT_SRC_CH11,
    IDT_SRC_CH12,
    IDT_SRC_CH13,
    IDT_SRC_CH14,
    IDT_SRC_CH15
};

uint8_t clock_switch_read_reg(uint8_t *rx_data);
uint8_t clock_switch_write_reg(uint8_t *tx_data);
uint8_t clock_switch_set_single_channel(uint8_t channel, uint8_t value);
uint8_t clock_switch_read_single_channel(uint8_t channel, uint8_t *data);
uint8_t clock_switch_default_config();

#endif
