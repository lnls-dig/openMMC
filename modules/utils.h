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
 * @file utils.h
 * @brief General utilities functions
 * @author Henrique Silva
 */

#include "FreeRTOS.h"

/**
 * @brief Stringify a macro
 *
 * @param x Value to be stringifyed
 */
#define STR(x) _STR(x)
#define _STR(x) #x

/**
 * @brief Gets length of a macro string
 *
 * @note This macro behaviour is equivalent to strlen()
 *
 * @param x Macro string
 */
#define STR_SIZE(x) (sizeof(x) -1)

/**
 * @brief Calculate the difference between 2 tick values
 *
 * Since the tick counter can overflow, we need to check if the current value is higher than the start time before performing any calculations.
 * The Tick counter is expected to overflow at the portMAX_DELAY value
 *
 * @param current_time Current tick count
 * @param start_time Start tick count
 *
 * @return Tick difference between arguments
 */
TickType_t getTickDifference( TickType_t current_time, TickType_t start_time );

/**
 * @brief Calculate a n-byte message 2's complement checksum.
 *
 * The checksum byte is calculated by perfoming a simple 8bit 2's complement of the sum of all previous bytes.
 * Since we're using a unsigned int to hold the checksum value, we only need to subtract all bytes from it.
 *
 * @param buffer Pointer to the message bytes.
 * @param range How many bytes will be used in the calculation.
 *
 * @return Checksum of the specified bytes of the buffer.
 */
uint8_t calculate_chksum( uint8_t * buffer, uint8_t range );

/**
 * @brief Compare 2 buffers
 *
 * @param bufa First buffer to compare
 * @param len_a Length of bufa
 * @param bufb Second buffer to compare
 * @param len_b Length of bufb
 *
 * @retval 0x00 Buffers are equal
 * @retval 0xFF Buffers are different
 */
uint8_t cmpBuffs( uint8_t *bufa, size_t len_a, uint8_t *bufb, size_t len_b );

/**
 * @brief Check is a number is a power of 2
 *
 * @param x Number to be tested
 *
 * @retval 1 Number is a power of two
 * @retval 0 Number is not a power of two
 */
uint8_t isPowerOfTwo( uint8_t x );
