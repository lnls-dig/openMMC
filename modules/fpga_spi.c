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
#include "fpga_spi.h"
#include "pin_mapping.h"
#include "task_priorities.h"
#include "string.h"
#include "led.h"
#include "board_version.h"
#include "sdr.h"

#define FPGA_SPI_BITRATE                10000000
#define FPGA_SPI_FRAME_SIZE             8

/* Write one byte on the specified address on the FPGA RAM */
static void write_fpga_byte( uint16_t address, uint32_t data )
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

static void write_fpga_buffer( t_board_diagnostic diag )
{
    uint16_t i;
    /* Send all bytes sequentially, except the last, whose address is different (0xFF) */
    for( i = 0; i < ((sizeof(diag.buffer)/sizeof(diag.buffer[0]))- 1); i++) {
        write_fpga_byte( i, diag.buffer[i] );
    }
    write_fpga_byte( diag.buffer[i] , 0xFF );
}

/* Read one byte from the specified address on the FPGA RAM */
static uint32_t read_fpga_byte( uint16_t address )
{
    uint8_t tx_buff[3];
    uint8_t rx_buff[7];

    tx_buff[0] = RD_COMMAND;
    tx_buff[1] = (address >> 8) & 0xFF;
    tx_buff[2] = address & 0xFF;

    ssp_write_read( FPGA_SPI, &tx_buff[0], sizeof(tx_buff), &rx_buff[0], sizeof(rx_buff), portMAX_DELAY );

    return ( (rx_buff[3] << 24) | (rx_buff[4] << 16) | (rx_buff[5] << 8) | rx_buff[6]);
}

static void read_fpga_buffer( uint32_t * buffer, uint32_t buffer_len )
{
    for (uint8_t i = 0; i < buffer_len; i++) {
        buffer[i] = read_fpga_byte(i);
    }
}

static void init_diag_struct( board_diagnostic * diag )
{
    uint8_t i;
    sensor_t * temp_sensor;

    /* Card ID */
    diag->cardID[0] = 0;
    diag->cardID[1] = 0;
    diag->cardID[2] = 0;
    diag->cardID[3] = 0;

    /* AMC IPMI address */
    diag->ipmi_addr = ipmb_addr;

    /* AMC Slot Number */
    diag->slot_id = (ipmb_addr-0x70)/2;

    /* Data Valid */
    /* Indicates that LPC is transfering data */
    diag->data_valid = 0x55555555;

    /* Sensors Readings */
    for ( i = 0, temp_sensor = sdr_head; (temp_sensor != NULL) && (i <= NUM_SENSOR); temp_sensor = temp_sensor->next) {
        if (temp_sensor->diag_devID != NO_DIAG) {
            diag->sensor[i].dev_id = temp_sensor->diag_devID;
            diag->sensor[i].measure = temp_sensor->readout_value;
            i++;
        }
    }

    diag->fmc_slot.fmc2_pg_c2m = gpio_read_pin( 1, 19 );
    diag->fmc_slot.fmc1_pg_c2m = gpio_read_pin( 1, 18 );
    diag->fmc_slot.fmc2_pg_m2c = gpio_read_pin( 1, 17 );
    diag->fmc_slot.fmc1_pg_m2c = gpio_read_pin( 1, 16 );
    diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 15 );
    diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 14 );
}

/* Send board data to the FPGA RAM via SPI periodically */
void vTaskFPGA_COMM( void * Parameters )
{
    t_board_diagnostic diag_struct;
    board_diagnostic * diag = &(diag_struct.info);
    uint8_t i;
    sensor_t * temp_sensor;

    /* Zero fill the diag struct */
    memset( &(diag_struct.buffer[0]), 0, sizeof(diag_struct.buffer));

    /* Initialize diagnostic struct with known data */
    init_diag_struct( diag );

    /* Check if the FPGA has finished programming itself from the FLASH */
    while (!gpio_read_pin( GPIO_DONE_B_PORT, GPIO_DONE_B_PIN)) {
        vTaskDelay(FPGA_UPDATE_RATE);
    }

    if (board_info.board_version == BOARD_VERSION_AFC_V3_1) {
        gpio_set_pin_state(0, 19, HIGH);
    }

    ssp_init( FPGA_SPI, FPGA_SPI_BITRATE, FPGA_SPI_FRAME_SIZE, SSP_MASTER, SSP_POLLING );

    for ( ;; ) {
        /* Update diagnostic struct information */

        /* Data Valid byte - indicates that LPC is transferring data */
        diag->data_valid = 0x55555555;

        /* Update Sensors Readings */

        for ( i = 0, temp_sensor = sdr_head; (temp_sensor != NULL) && (i <= NUM_SENSOR); temp_sensor = temp_sensor->next) {
        	if (temp_sensor->diag_devID != NO_DIAG) {
                diag->sensor[i].dev_id = temp_sensor->diag_devID;
                diag->sensor[i].measure = temp_sensor->readout_value;
                i++;
            }
        }

        diag->fmc_slot.fmc2_pg_c2m = gpio_read_pin( 1, 19 );
        diag->fmc_slot.fmc1_pg_c2m = gpio_read_pin( 1, 18 );
        diag->fmc_slot.fmc2_pg_m2c = gpio_read_pin( 1, 17 );
        diag->fmc_slot.fmc1_pg_m2c = gpio_read_pin( 1, 16 );
        diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 15 );
        diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 14 );

        write_fpga_buffer( diag_struct );

        /* BUG: The SSP interface keeps returning 0 when trying to read the
         * data from the FPGA, despite the fact that the waveform shows it's
         * being returned correctly */

#if SSP_TESTS
	uint32_t rx_trace[FPGA_MEM_ADDR_MAX] = {0};
        read_fpga_buffer( &rx_trace[0], sizeof(rx_trace)/sizeof(rx_trace[0]) );

	uint32_t data;

	if( cmpBuffs( &(diag_struct.buffer[0]), sizeof(diag_struct.buffer)/sizeof(diag_struct.buffer[0]), &rx_trace[0], sizeof(rx_trace)/sizeof(rx_trace[0]) != 0 ) ) {
            data = 0xAAAAAAAA;
            LED_update(LED_RED, &LED_2Hz_Blink_Activity);
        } else {
            data = 0x55555555;
            LED_update(LED_RED, &LED_Off_Activity);
        }

        write_fpga_byte( 0x05, data );
#endif
        write_fpga_byte( 0x05, 0x55555555 );
        vTaskDelay(FPGA_UPDATE_RATE);
    }
}

void init_fpga_spi( void )
{
    xTaskCreate(vTaskFPGA_COMM, "FPGA_COMM", 150, NULL, tskFPGA_COMM_PRIORITY, (TaskHandle_t *) NULL);
}
