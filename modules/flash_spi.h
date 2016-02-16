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

#ifndef FLASH_SPI_H_
#define FLASH_SPI_H_

#define FLASH_SPI_BITRATE                1000000
#define FLASH_SPI_FRAME_SIZE             8

/* M25P128 Flash commands */
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_READ_ID 0x9F
#define FLASH_READ_STATUS_REG 0x05
#define FLASH_WRITE_STATUS_REG 0x01
#define FLASH_READ_LOCK_REG 0xE8
#define FLASH_WRITE_LOCK_REG 0xE5
#define FLASH_READ_DATA 0x03
#define FLASH_FAST_READ_DATA 0x0B
#define FLASH_PROGRAM_PAGE 0x02
#define FLASH_SECTOR_ERASE 0xD8
#define FLASH_BULK_ERASE 0xC7

void flash_write_enable( void );
void flash_write_disable( void );
void flash_read_id( uint8_t * id_buffer, uint8_t buff_size );
uint8_t flash_read_status_reg( void );
void flash_write_status_reg( uint8_t data );
uint8_t flash_read_data( uint32_t address );
void flash_fast_read_data( uint32_t start_addr, uint8_t * dst, uint32_t size );
void flash_program_page( uint32_t address, uint8_t * data, uint16_t size );
void flash_sector_erase( uint32_t address );
void flash_bulk_erase( void );
uint8_t flash_read_lock_reg( uint32_t address );
void flash_write_lock_reg( uint32_t address, uint8_t data );
uint8_t is_flash_busy( void );

#endif
