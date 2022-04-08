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

#include "FreeRTOS.h"
#include "task.h"
#include "port.h"

#include "i2c_mapping.h"
#include "fpga_spi.h"
#include "task_priorities.h"
#include "at24mac.h"
#include "sdr.h"

#define FPGA_SPI_BITRATE                10000000
#define FPGA_SPI_FRAME_SIZE             8

/* Write one byte on the specified address on the FPGA RAM */
static void write_fpga_dword( uint16_t address, uint32_t data )
{
    uint8_t tx_buff[7];

    tx_buff[0] = WR_COMMAND;
    tx_buff[1] = (address >> 8) & 0xFF;
    tx_buff[2] = address & 0xFF;
    tx_buff[3] = ( ( data >> 24) & 0xFF );
    tx_buff[4] = ( ( data >> 16) & 0xFF );
    tx_buff[5] = ( ( data >> 8)  & 0xFF );
    tx_buff[6] = ( data & 0xFF );

    ssp_write( FPGA_SPI, tx_buff, sizeof(tx_buff) );
}

static void write_fpga_buffer( board_diagnostic_t *diag )
{
    uint16_t i;
    uint32_t *buffer = (uint32_t *)diag;

    /* Send all bytes sequentially, except the last record (FMC slot status), whose address is 0xFF */
    for( i = 0; i < (sizeof(board_diagnostic_t) / sizeof(uint32_t))- 1; i++) {
        write_fpga_dword( i, buffer[i] );
    }
    write_fpga_dword( 0xFF, buffer[i] );
}

/* Send board data to the FPGA RAM via SPI periodically */
void vTaskFPGA_COMM( void * Parameters )
{
    board_diagnostic_t * diag = pvPortMalloc(sizeof(board_diagnostic_t));
    uint8_t i;
    sensor_t * temp_sensor;

    /* Zero fill the diag struct */
    memset( &diag[0], 0, sizeof(board_diagnostic_t));

    /* Check if the FPGA has finished programming itself from the FLASH */
    while (!gpio_read_pin( PIN_PORT(GPIO_FPGA_DONE_B), PIN_NUMBER(GPIO_FPGA_DONE_B))) {
        vTaskDelay(FPGA_UPDATE_RATE);
    }

    ssp_init( FPGA_SPI, FPGA_SPI_BITRATE, FPGA_SPI_FRAME_SIZE, SSP_MASTER, SSP_POLLING );

    /* Initialize diagnostic struct with static data */

    /* Read Card ID from EEPROM (4 bytes) */
    at24mac_read_eui(CHIP_ID_EEPROM, (uint8_t *)&diag->cardID[0], 4,  10);

    /* AMC IPMI address */
    diag->ipmi_addr = ipmb_addr;

    /* AMC Slot Number */
    diag->slot_id = (ipmb_addr-0x70)/2;

    /* Data Valid byte - indicates that LPC is transfering data */
    /* Since every time this buffer is written the bus is held by the LPC, keep this field always as 0x55555555 */
    diag->data_valid = 0x55555555;

    for ( ;; ) {
        /* Update diagnostic struct information */

        /* Data Valid byte - indicates that LPC is transfering data */
        write_fpga_dword( 0x05, 0x55555555 );

        /* Update Sensors Readings */
        for ( i = 0, temp_sensor = sdr_head; (temp_sensor != NULL) && (i <= NUM_SENSOR); temp_sensor = temp_sensor->next) {
            if (temp_sensor->diag_devID != NO_DIAG) {
                diag->sensor[i].dev_id = temp_sensor->diag_devID;
                diag->sensor[i].measure = temp_sensor->readout_value;
                i++;
            }
        }

        diag->fmc_slot.fmc1_pg_c2m = gpio_read_pin( PIN_PORT(GPIO_FMC1_PG_C2M), PIN_NUMBER(GPIO_FMC1_PG_C2M) );
        diag->fmc_slot.fmc2_pg_c2m = gpio_read_pin( PIN_PORT(GPIO_FMC2_PG_C2M), PIN_NUMBER(GPIO_FMC2_PG_C2M) );
        diag->fmc_slot.fmc1_pg_m2c = gpio_read_pin( PIN_PORT(GPIO_FMC1_PG_M2C), PIN_NUMBER(GPIO_FMC1_PG_M2C) );
        diag->fmc_slot.fmc2_pg_m2c = gpio_read_pin( PIN_PORT(GPIO_FMC2_PG_M2C), PIN_NUMBER(GPIO_FMC2_PG_M2C) );
        diag->fmc_slot.fmc1_prsnt_m2c_n = gpio_read_pin( PIN_PORT(GPIO_FMC1_PRSNT_M2C), PIN_NUMBER(GPIO_FMC1_PRSNT_M2C) );
        diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( PIN_PORT(GPIO_FMC2_PRSNT_M2C), PIN_NUMBER(GPIO_FMC2_PRSNT_M2C) );

        write_fpga_buffer( diag );

        /* Data Valid byte - indicates that the bus is idle */
        write_fpga_dword( 0x05, 0xAAAAAAAA );

        vTaskDelay(FPGA_UPDATE_RATE);
    }
}

void fpga_spi_init( void )
{
    xTaskCreate(vTaskFPGA_COMM, "FPGA_COMM", 150, NULL, tskFPGA_COMM_PRIORITY, (TaskHandle_t *) NULL);
}
