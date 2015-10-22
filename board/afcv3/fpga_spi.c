#include "FreeRTOS.h"
#include "task.h"
#include "port.h"
#include "fpga_spi.h"
#include "pin_mapping.h"
#include "task_priorities.h"
#include "i2c.h"
#include "string.h"
#include "led.h"

/* Write one byte on the specified address on the FPGA RAM */
static void write_fpga_byte( uint32_t data, uint32_t address )
{
    uint8_t tx_buff[7];

    tx_buff[0] = WR_COMMAND;
    tx_buff[1] = 0x00;
    tx_buff[2] = address;
    tx_buff[3] = ( ( data >> 24) & 0xff );
    tx_buff[4] = ( ( data >> 16) & 0xff );
    tx_buff[5] = ( ( data >> 8)  & 0xff );
    tx_buff[6] = ( data & 0xff );

    ssp_write( FPGA_SPI, tx_buff, sizeof(tx_buff)/sizeof(tx_buff[0]) );
}

/* Read one byte from the specified address on the FPGA RAM */
static uint32_t read_fpga_byte( uint32_t address )
{
    uint8_t tx_buff[3];
    uint8_t rx_buff[7];

    tx_buff[0] = RD_COMMAND;
    tx_buff[1] = 0x00;
    tx_buff[2] = address;

    ssp_write_read( FPGA_SPI, tx_buff, sizeof(tx_buff), rx_buff, sizeof(rx_buff)-sizeof(tx_buff) );

    return ( (uint32_t) ( (rx_buff[3] << 24) | (rx_buff[4] << 16) | (rx_buff[5] << 8) | rx_buff[6] ) );
}

static void write_fpga_buffer( t_board_diagnostic diag )
{
    uint16_t i;
    /* Send all bytes sequentially, except the last, whose address is different (0xFF) */
    for( i = 0; i < (sizeof(diag.buffer)/sizeof(diag.buffer[0])- 1); i++) {
        write_fpga_byte( diag.buffer[i], i );
    }
    write_fpga_byte( diag.buffer[i+1] , 0xFF );
}

/* Send board data to the FPGA RAM via SPI periodically */
void vTaskFPGA_COMM( void * Parameters )
{
    t_board_diagnostic diag_struct;
    board_diagnostic * diag = &(diag_struct.info);
    uint32_t rx_trace[FPGA_MEM_ADDR_MAX+1] = {0};
    uint32_t data;

    /* Zero fill the diag struct */
    memset( &(diag_struct.buffer[0]), 0, sizeof(diag_struct.buffer));

    /* Initialize diagnostic struct with known data */
    init_diag_struct( diag );

    for ( ;; ) {


        /* Check if the FPGA has finished programming itself from the FLASH */
        while (!gpio_read_pin( GPIO_DONE_B_PORT, GPIO_DONE_B_PIN)) {
            vTaskDelay(FPGA_UPDATE_RATE);
        }

        /* Update diagnostic struct information */

        /* Data Valid byte - indicates that LPC is transferring data */
        diag->data_valid = 0x55555555;

        /* Update Sensors Readings */
        for (uint8_t i = 0, j = 0; i <= NUM_SENSOR; i++) {
            if (sensor_array[i].diag_devID != NO_DIAG) {
                diag->sensor[j].dev_id = sensor_array[i].diag_devID;
                diag->sensor[j].measure = sensor_array[i].data->readout_value;
                j++;
            }
        }

        diag->fmc_slot.fmc2_pg_c2m = gpio_read_pin( 1, 19 );
        diag->fmc_slot.fmc1_pg_c2m = gpio_read_pin( 1, 18 );
        diag->fmc_slot.fmc2_pg_m2c = gpio_read_pin( 1, 17 );
        diag->fmc_slot.fmc1_pg_m2c = gpio_read_pin( 1, 16 );
        diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 15 );
        diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 14 );

        write_fpga_buffer( diag_struct );

        /* Read the buffer from the FPGA to check eventual corruptions */
        for( uint32_t addressFPGAram = 0; addressFPGAram < FPGA_MEM_ADDR_MAX; addressFPGAram++ ) {
            rx_trace[addressFPGAram] = read_fpga_byte(addressFPGAram);
        }

        if( !cmpBuffs( &(diag_struct.buffer[0]), sizeof(diag_struct.buffer)/sizeof(diag_struct.buffer[0]), &rx_trace[0], sizeof(rx_trace)/sizeof(rx_trace[0]) ) ) {
            data = 0xAAAAAAAA;
	    extern const LED_activity_desc_t LED_2Hz_Blink_Activity;
	    LED_update(LED_RED, &LED_2Hz_Blink_Activity);
        } else {
            data = 0x55555555;
	    extern const LED_activity_desc_t LED_Off_Activity;
	    LED_update(LED_RED, &LED_Off_Activity);
        }

        write_fpga_byte( data, 0x05 );

	vTaskDelay(FPGA_UPDATE_RATE);
    }
}

void init_fpga_spi( void )
{
    ssp_config( FPGA_SPI, FPGA_BITRATE, 8, true, false );
    xTaskCreate(vTaskFPGA_COMM, "FPGA_COMM", configMINIMAL_STACK_SIZE*5, NULL, tskFPGA_COMM_PRIORITY, (TaskHandle_t *) NULL);
}

void init_diag_struct( board_diagnostic * diag )
{
    uint8_t i,j;

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
    for (i = 0, j = 0; i <= NUM_SENSOR; i++) {
        if (sensor_array[i].diag_devID != NO_DIAG) {
            diag->sensor[j].dev_id = sensor_array[i].diag_devID;
            diag->sensor[j].measure = sensor_array[i].data->readout_value;
            j++;
        }
    }

    diag->fmc_slot.fmc2_pg_c2m = gpio_read_pin( 1, 19 );
    diag->fmc_slot.fmc1_pg_c2m = gpio_read_pin( 1, 18 );
    diag->fmc_slot.fmc2_pg_m2c = gpio_read_pin( 1, 17 );
    diag->fmc_slot.fmc1_pg_m2c = gpio_read_pin( 1, 16 );
    diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 15 );
    diag->fmc_slot.fmc2_prsnt_m2c_n = gpio_read_pin( 1, 14 );
}

/* Compare two buffers' size and data
 * Returns 0 if equal, 0xFF if different */
uint8_t cmpBuffs( uint32_t *bufa, uint32_t len_a, uint32_t *bufb, uint32_t len_b )
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
