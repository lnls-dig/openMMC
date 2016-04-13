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

#ifndef AT24MAC_H_
#define AT24MAC_H_

//#define AT24_MAC402
#define AT24MAC602

#define AT24MAC_ID_ADDR 0x80

#if defined(AT24MAC402)
#define AT24MAC_EUI_ADDR 0x98
#elif defined(AT24MAC602)
#define AT24MAC_EUI_ADDR 0x9A
#endif

uint8_t at24mac_read( uint8_t address, uint8_t *rx_data, uint8_t buf_len );
uint8_t at24mac_read_serial_num( uint8_t *rx_data, uint8_t buf_len );
uint8_t at24mac_read_eui( uint8_t *rx_data, uint8_t buf_len );
uint8_t at24mac_write( uint8_t address, uint8_t *tx_data, uint8_t buf_len );

#endif
