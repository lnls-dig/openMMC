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

/*! @file utils.h
  @brief General utilities functions
  @author Henrique Silva
*/

#define STR(x) _STR(x)
#define _STR(x) #x
#define STR_SIZE(x) (sizeof(x) -1)

uint32_t getTickDifference( uint32_t current_time, uint32_t start_time );

uint8_t calculate_chksum( uint8_t * buffer, uint8_t range );

uint8_t cmpBuffs( uint8_t *bufa, size_t len_a, uint8_t *bufb, size_t len_b );

uint8_t isPowerOfTwo( uint8_t x );
