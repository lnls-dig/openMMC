#include "FreeRTOS.h"
#include "port.h"
#include "flash_spi.h"
#include "pin_mapping.h"
#include <string.h>

void flash_write_enable( void )
{
    uint8_t tx_buff[1] = {FLASH_WRITE_ENABLE};

    ssp_write( FLASH_SPI, tx_buff, sizeof(tx_buff) );
}

void flash_write_disable( void )
{
    uint8_t tx_buff[1] = {FLASH_WRITE_DISABLE};

    ssp_write( FLASH_SPI, tx_buff, sizeof(tx_buff) );
}

void flash_read_id( uint8_t * id_buffer, uint8_t buff_size )
{
    if ((buff_size < 3)|(id_buffer == NULL)) {
	return;
    }

    uint8_t tx_buff[1] = {FLASH_READ_ID};

    /* Size of rx buffer must be the sum of the length of the data expected to receive and the length of the sent data */
    uint8_t rx_buff[4] = {0};

    ssp_write_read( FLASH_SPI, &tx_buff[0], sizeof(tx_buff), &rx_buff[0], sizeof(rx_buff) );

    memcpy(id_buffer, &tx_buff[1], 3);
}

uint8_t flash_read_status_reg( void )
{
    uint8_t tx_buff[1] = {FLASH_READ_STATUS_REG};

    /* Size of rx buffer must be the sum of the length of the data expected to receive and the length of the sent data */
    uint8_t rx_buff[2] = {0};

    ssp_write_read( FLASH_SPI, &tx_buff[0], sizeof(tx_buff), &rx_buff[0], sizeof(rx_buff) );

    return rx_buff[1];
}

void flash_write_status_reg( uint8_t data )
{
    uint8_t tx_buff[2] = {FLASH_WRITE_STATUS_REG, data};

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

    ssp_write_read( FLASH_SPI, &tx_buff[0], sizeof(tx_buff), &rx_buff[0], sizeof(rx_buff) );

    return rx_buff[4];
}

void flash_program_page( uint32_t address, uint8_t * data, uint16_t size )
{
    flash_write_enable();

    /* Use malloc to reserve 'size' plus 4 bytes (address and cmd) */
    uint8_t *tx_buff = pvPortMalloc(size+4);

    tx_buff[0] = FLASH_PROGRAM_PAGE;
    tx_buff[1] = (address >> 16) & 0xFF;
    tx_buff[2] = (address >> 8) & 0xFF;
    tx_buff[3] = address & 0xFF;

    memcpy(&tx_buff[4], data, size);

    ssp_write( FLASH_SPI, &tx_buff[0], sizeof(tx_buff) );

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
