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
 * @file utils.c
 * @brief General utilities functions
 * @author Henrique Silva
*/

/* Kernel includes. */
#include "FreeRTOS.h"
#include "utils.h"

TickType_t getTickDifference(TickType_t current_time, TickType_t start_time)
{
    TickType_t result = 0;
    if (current_time < start_time) {
        result = start_time - current_time;
        result = portMAX_DELAY - result;
    } else {
        result = current_time - start_time;
    }
    return result;
}

uint8_t calculate_chksum ( uint8_t * buffer, uint8_t range )
{
    configASSERT( buffer != NULL );
    uint8_t chksum = 0;
    uint8_t i;
    for ( i = 0; i < range; i++ ) {
        chksum -= buffer[i];
    }
    return chksum;
}

uint8_t cmpBuffs( uint8_t *bufa, size_t len_a, uint8_t *bufb, size_t len_b )
{
    uint16_t i;
    if (len_a != len_b) {
        return 0xFF;
    }

    for( i = 0; i<len_a; i++ ) {
        if( *bufa != *bufb ) {
            return (0xFF);
        }
        bufa++;
        bufb++;
    }
    return (0);
}

uint8_t isPowerOfTwo( uint8_t x )
{
    return ((x != 0) && !(x & (x - 1)));
}
