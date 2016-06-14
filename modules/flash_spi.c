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

/* FreeRTOS Includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "flash_spi.h"
#include "pin_mapping.h"
#include <string.h>

void flash_write_enable( void )
{
    do {
        uint8_t tx_buff[1] = {FLASH_WRITE_ENABLE};
        ssp_write( FLASH_SPI, tx_buff, sizeof(tx_buff) );
    } while (!(flash_read_status_reg() & 0x2));
}

void flash_write_disable( void )
{
    do {
        uint8_t tx_buff[1] = {FLASH_WRITE_DISABLE};
        ssp_write( FLASH_SPI, tx_buff, sizeof(tx_buff) );
    } while (flash_read_status_reg() & 0x2);
}

void flash_read_id( uint8_t * id_buffer, uint8_t buff_size )
{
    if ((buff_size < 3)|(id_buffer == NULL)) {
        return;
    }

    uint8_t tx_buff[1] = {FLASH_READ_ID};

    /* Size of rx buffer must be the sum of the length of the data expected to receive and the length of the sent data */
    uint8_t rx_buff[4] = {0};

    ssp_write_read( FLASH_SPI, &tx_buff[0], 1, &rx_buff[0], 3, portMAX_DELAY );

    memcpy(id_buffer, &rx_buff[1], 3);
}

uint8_t flash_read_status_reg( void )
{
    uint8_t tx_buff[1] = {FLASH_READ_STATUS_REG};

    /* Size of rx buffer must be the sum of the length of the data expected to receive and the length of the sent data */
    uint8_t rx_buff[2] = {0};

    ssp_write_read( FLASH_SPI, &tx_buff[0], 1, &rx_buff[0], 1, portMAX_DELAY );

    return rx_buff[1];
}

void flash_write_status_reg( uint8_t data )
{
    uint8_t tx_buff[2] = {FLASH_WRITE_STATUS_REG, data};

    flash_write_enable();

    ssp_write( FLASH_SPI, &tx_buff[0], sizeof(tx_buff) );
}

uint8_t flash_read_lock_reg( uint32_t address )
{
    uint8_t tx_buff[4];

    tx_buff[0] = FLASH_READ_LOCK_REG;
    tx_buff[1] = (address >> 16) & 0xFF;
    tx_buff[2] = (address >> 8) & 0xFF;
    tx_buff[3] = address & 0xFF;

    /* Size of rx buffer must be the sum of the length of the data expected to receive and the length of the sent data */
    uint8_t rx_buff[5] = {0};

    ssp_write_read( FLASH_SPI, &tx_buff[0], sizeof(tx_buff), &rx_buff[0], sizeof(rx_buff), portMAX_DELAY );

    return rx_buff[4];
}

void flash_write_lock_reg( uint32_t address, uint8_t data )
{
    uint8_t tx_buff[5];

    tx_buff[0] = FLASH_WRITE_LOCK_REG;
    tx_buff[1] = (address >> 16) & 0xFF;
    tx_buff[2] = (address >> 8) & 0xFF;
    tx_buff[3] = address & 0xFF;
    tx_buff[4] = data;

    flash_write_enable();

    ssp_write( FLASH_SPI, &tx_buff[0], sizeof(tx_buff) );
}


uint8_t flash_read_data( uint32_t address )
{
    uint8_t tx_buff[4];

    tx_buff[0] = FLASH_READ_DATA;
    tx_buff[1] = (address >> 16) & 0xFF;
    tx_buff[2] = (address >> 8) & 0xFF;
    tx_buff[3] = address & 0xFF;

    /* Size of rx buffer must be the sum of the length of the data expected to receive and the length of the sent data */
    uint8_t rx_buff[5] = {0};

    ssp_write_read( FLASH_SPI, &tx_buff[0], 4, &rx_buff[0], 1, portMAX_DELAY );

    return rx_buff[4];
}

void flash_fast_read_data( uint32_t start_addr, uint8_t * dst, uint32_t size )
{
    /* Size of rx buffer must be the sum of the length of the data expected to receive and the length of the sent data */
    /* Use an additional Dumb byte */
    uint8_t *rx_buff = pvPortMalloc(size+5);

    uint8_t tx_buff[5];

    tx_buff[0] = FLASH_FAST_READ_DATA;
    tx_buff[1] = (start_addr >> 16) & 0xFF;
    tx_buff[2] = (start_addr >> 8) & 0xFF;
    tx_buff[3] = start_addr & 0xFF;
    tx_buff[4] = 0xFF; /* Dumb Byte */

    ssp_write_read( FLASH_SPI, &tx_buff[0], 5, &rx_buff[0], size, portMAX_DELAY );

    memcpy(dst, &rx_buff[5], size);

    vPortFree(rx_buff);
}

void flash_program_page( uint32_t address, uint8_t * data, uint16_t size )
{
    /* The sector MUST be erased before trying to program new data into it */
    flash_write_enable();

    /* Use malloc to reserve 'size' plus 4 bytes (address and cmd) */
    uint8_t *tx_buff = pvPortMalloc(size+4);

    tx_buff[0] = FLASH_PROGRAM_PAGE;
    tx_buff[1] = (address >> 16) & 0xFF;
    tx_buff[2] = (address >> 8) & 0xFF;
    tx_buff[3] = address & 0xFF;

    memcpy(&tx_buff[4], data, size);

    ssp_write( FLASH_SPI, &tx_buff[0], size+4 );

    vPortFree(tx_buff);
}

void flash_sector_erase( uint32_t address )
{
    uint8_t tx_buff[4];

    tx_buff[0] = FLASH_SECTOR_ERASE;
    tx_buff[1] = (address >> 16) & 0xFF;
    tx_buff[2] = (address >> 8) & 0xFF;
    tx_buff[3] = address & 0xFF;

    flash_write_enable();

    ssp_write( FLASH_SPI, tx_buff, sizeof(tx_buff) );
}

void flash_bulk_erase( void )
{
    uint8_t tx_buff[1] = {FLASH_BULK_ERASE};

    flash_write_enable();

    ssp_write( FLASH_SPI, tx_buff, sizeof(tx_buff) );
}

uint8_t is_flash_busy( void )
{
    uint8_t status = flash_read_status_reg();
    return (status & 0x01);
}
