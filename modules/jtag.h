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

#ifndef JTAG_H_
#define JTAG_H_

#define SCANSTA_PORT		2

#define SCANSTA_ADDR0_PIN	0
#define SCANSTA_ADDR1_PIN	1
#define SCANSTA_ADDR2_PIN	2
#define SCANSTA_ADDR3_PIN	3
#define SCANSTA_ADDR4_PIN	4
#define SCANSTA_ADDR5_PIN	5
#define SCANSTA_ADDR6_PIN	6
#define SCANSTA_RST_PIN		7

void scansta_init( void );

#endif
