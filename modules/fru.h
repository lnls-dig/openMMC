/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Julian Mendez  <julian.mendez@cern.ch>
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

#ifndef FRU_H_
#define FRU_H_

#include <stdint.h>
#include <stdbool.h>

enum {
    FRU_AMC,
#ifdef MODULE_RTM
    FRU_RTM,
#endif
    FRU_COUNT
};

typedef size_t (* fru_build_t)(uint8_t **buffer);

typedef struct fru_data {
    uint8_t eeprom_id;
    uint8_t *buffer;
    size_t fru_size;
    bool runtime;
    fru_build_t build_func;
} fru_data_t;

void fru_init( uint8_t id );
size_t fru_read( uint8_t id, uint8_t *rx_buff, uint16_t offset, size_t len );
size_t fru_write( uint8_t id, uint8_t *tx_buff, uint16_t offset, size_t len );

#endif
