/*
 * utils.h
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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
 */

/*! @file utils.h
  @brief General utilities functions
  @author Henrique Silva
*/

TickType_t getTickDifference( TickType_t current_time, TickType_t start_time );

uint8_t calculate_chksum( uint8_t * buffer, uint8_t range );

uint8_t cmpBuffs( uint32_t *buf_a, uint32_t len_a, uint32_t *bufb, uint32_t len_b );

uint8_t isPowerOfTwo( uint8_t x );
