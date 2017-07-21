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

/**
 * @defgroup AT24MAC AT24MACx02 2kbit Serial EEPROM
 * @ingroup PERIPH_IC
 */

/**
 * @file   at24mac.h
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  AT24MACX02 EEPROM module interface declarations
 *
 * @ingroup AT24MAC
 */

#ifndef AT24MAC_H_
#define AT24MAC_H_

//#define AT24_MAC402
#define AT24MAC602

#define AT24MAC_ID_ADDR 0x80

#if defined(AT24MAC402)
#define AT24MAC_EUI_ADDR 0x9A
#elif defined(AT24MAC602)
#define AT24MAC_EUI_ADDR 0x98
#endif

/**
 * @brief Read serial data from AT24MAC EEPROM
 *
 * @param id       EEPROM chip id
 * @param address  Starting read address
 * @param rx_data  Buffer to store the data
 * @param buf_len  Buffer max length
 * @param timeout  Read timeout
 *
 * @return Number of bytes actually received
 */
size_t at24mac_read( uint8_t id, uint16_t address, uint8_t *rx_data, size_t buf_len, uint32_t timeout );

/**
 * @brief Read EEPROM serial number
 *
 * @param id       EEPROM chip id
 * @param rx_data  Buffer to store the data
 * @param buf_len  Buffer max length
 * @param timeout  Read timeout
 *
 * @return Number of bytes read
 */
size_t at24mac_read_serial_num( uint8_t id, uint8_t *rx_data, size_t buf_len, uint32_t timeout );

/**
 * @brief Read EEPROM EUI number
 *
 * @param id       EEPROM chip id
 * @param rx_data  Buffer to store the data
 * @param buf_len  Buffer max length
 * @param timeout  Read timeout
 *
 * @return Number of bytes read
 */
size_t at24mac_read_eui( uint8_t id, uint8_t *rx_data, size_t buf_len, uint32_t timeout );

/**
 * @brief Write serial data to EEPROM
 *
 * @param id       EEPROM chip id
 * @param address  Write start address
 * @param tx_data  Buffer holding the data to write
 * @param buf_len  Buffer max len
 * @param timeout  Write timout
 *
 * @return Number of bytes actually written
 */
size_t at24mac_write( uint8_t id, uint16_t address, uint8_t *tx_data, size_t buf_len, uint32_t timeout );

#endif
