/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

/* Project Includes */
#include "port.h"
#include "payload.h"
#include "ipmi.h"
#include "task_priorities.h"
#include "adn4604.h"
#include "ad84xx.h"
#include "hotswap.h"
#include "utils.h"
#include "fru.h"
#include "led.h"
#include "board_led.h"
#include "clock_config.h"
#include "i2c_mapping.h"
#include "eeprom_24xx02.h"

/* payload states
 *   0 - No power
 *
 *   1 - Power Good wait
 *       Enable DCDC Converters
 *       Hotswap backend power failure and shutdown status clear
 *
 *   2 - FPGA setup
 *       One-time configurations (clock switch - ADN4604)
 *
 *   3 - FPGA on
 *
 *   4 - Power switching off
 *       Disable DCDC Converters
 *       Send "quiesced" event if requested
 *
 *   5 - Power quiesced
 *       Payload was safely turned off
 *       Wait until payload power goes down to restart the cycle
 */

static void fpga_soft_reset( void )
{
    gpio_set_pin_low( PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET) );
    asm("NOP");
    gpio_set_pin_high( PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET) );

    /* Blink RED LED to indicate to the user that the Reset was performed */
    LEDUpdate( FRU_AMC, LED1, LEDMODE_LAMPTEST, LEDINIT_ON, 5, 0 );
}

static void check_fpga_reset( void )
{
    static TickType_t edge_time;
    static uint8_t reset_lock;
    static uint8_t last_state = 1;

    TickType_t diff;
    TickType_t cur_time = xTaskGetTickCount();

    uint8_t cur_state = gpio_read_pin( PIN_PORT(GPIO_FRONT_BUTTON), PIN_NUMBER(GPIO_FRONT_BUTTON));

    if ( (cur_state == 0) && (last_state == 1) ) {
        /* Detects the falling edge of the front panel button */
        edge_time = cur_time;
        reset_lock = 0;
    }

    diff = getTickDifference( cur_time, edge_time );

    if ( (diff > pdMS_TO_TICKS(2000)) && (reset_lock == 0) && (cur_state == 0) ) {
        fpga_soft_reset();
        /* If the user continues to press the button after the 2s, prevent this action to be repeated */
        reset_lock = 1;
    }

    last_state = cur_state;
}

uint8_t payload_check_pgood( uint8_t *pgood_flag )
{
    sensor_t * p_sensor;
    SDR_type_01h_t *sdr;

    extern const SDR_type_01h_t SDR_FMC1_12V;

    /* Iterate through the SDR Table to find all the LM75 entries */
    for ( p_sensor = sdr_head; (p_sensor != NULL) || (p_sensor->task_handle == NULL); p_sensor = p_sensor->next) {
        if (p_sensor->sdr == &SDR_FMC1_12V) {
            sdr = ( SDR_type_01h_t * ) p_sensor->sdr;
            *pgood_flag = ( ( p_sensor->readout_value >= (sdr->lower_critical_thr ) ) &&
                            ( p_sensor->readout_value <= (sdr->upper_critical_thr ) ) );
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Set AFC's DCDC Converters state
 *
 * @param on DCDCs state
 *
 * @warning The FMC1_P12V DCDC is not affected by this function since it has to be always on in order to measure the Payload power status on the AFC board.
 */
void setDC_DC_ConvertersON( bool on )
{
    gpio_set_pin_state( PIN_PORT(GPIO_EN_FMC1_PVADJ), PIN_NUMBER(GPIO_EN_FMC1_PVADJ), on );
    //gpio_set_pin_state( PIN_PORT(GPIO_EN_FMC1_P12V), PIN_NUMBER(GPIO_EN_FMC1_P12V), on );
    gpio_set_pin_state( PIN_PORT(GPIO_EN_FMC1_P3V3), PIN_NUMBER(GPIO_EN_FMC1_P3V3), on );

    gpio_set_pin_state( PIN_PORT(GPIO_EN_FMC2_PVADJ), PIN_NUMBER(GPIO_EN_FMC2_PVADJ), on );
    gpio_set_pin_state( PIN_PORT(GPIO_EN_FMC2_P12V), PIN_NUMBER(GPIO_EN_FMC2_P12V), on );
    gpio_set_pin_state( PIN_PORT(GPIO_EN_FMC2_P3V3), PIN_NUMBER(GPIO_EN_FMC2_P3V3), on );

    gpio_set_pin_state( PIN_PORT(GPIO_EN_P1V0), PIN_NUMBER(GPIO_EN_P1V0), on );
    gpio_set_pin_state( PIN_PORT(GPIO_EN_P1V8), PIN_NUMBER(GPIO_EN_P1V8), on ); // <- this one causes problems if not switched off before power loss
    gpio_set_pin_state( PIN_PORT(GPIO_EN_P1V2), PIN_NUMBER(GPIO_EN_P1V2), on );
    gpio_set_pin_state( PIN_PORT(GPIO_EN_P1V5_VTT), PIN_NUMBER(GPIO_EN_P1V5_VTT), on );
    gpio_set_pin_state( PIN_PORT(GPIO_EN_P3V3), PIN_NUMBER(GPIO_EN_P3V3), on );
}

#ifdef MODULE_DAC_AD84XX
void set_vadj_volt( uint8_t fmc_slot, float v )
{
    uint32_t res_total;
    uint32_t res_dac;

    res_total = (uint32_t) (1162.5/(v-0.775)) - 453;
    res_dac = (1800*res_total)/(1800-res_total);

    /* Use only the lower 8-bits (the dac only has 256 steps) */
    res_dac &= 0xFF;

    dac_ad84xx_set_res( fmc_slot, res_dac );
}
#endif

EventGroupHandle_t amc_payload_evt = NULL;
#ifdef MODULE_RTM
EventGroupHandle_t rtm_payload_evt = NULL;
#endif

void payload_send_message( uint8_t fru_id, EventBits_t msg)
{
    if ( (fru_id == FRU_AMC) && amc_payload_evt ) {
        xEventGroupSetBits( amc_payload_evt, msg );
#ifdef MODULE_RTM
    } else if ( (fru_id == FRU_RTM) && rtm_payload_evt ) {
        xEventGroupSetBits( rtm_payload_evt, msg );
#endif
    }
}

TaskHandle_t vTaskPayload_Handle;

void payload_init( void )
{
    /* Set standalone mode if the module is disconnected from a create*/
    bool standalone_mode = false;

    if (get_ipmb_addr() == IPMB_ADDR_DISCONNECTED) {
        standalone_mode = true;
    }

    if (!standalone_mode) {
        /* Wait until ENABLE# signal is asserted ( ENABLE == 0) */
        while ( gpio_read_pin( PIN_PORT(GPIO_MMC_ENABLE), PIN_NUMBER(GPIO_MMC_ENABLE) ) == 1 ) {};
    }

    /* Recover clock switch configuration saved in EEPROM */
    eeprom_24xx02_read(CHIP_ID_RTC_EEPROM, 0x0, clock_config, 16, 10);

    xTaskCreate( vTaskPayload, "Payload", 256, NULL, tskPAYLOAD_PRIORITY, &vTaskPayload_Handle );

    amc_payload_evt = xEventGroupCreate();
#ifdef MODULE_RTM
    rtm_payload_evt = xEventGroupCreate();
#endif

#ifdef MODULE_DAC_AD84XX
    /* Configure the PVADJ DAC */
    dac_ad84xx_init();
    set_vadj_volt( 0, 2.5 );
    set_vadj_volt( 1, 2.5 );
#endif

    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_HIGH );
}

void vTaskPayload( void *pvParameters )
{
    uint8_t state = PAYLOAD_NO_POWER;
    /* Use arbitrary state value to force the first state update */
    uint8_t new_state = -1;

    /* Payload power good flag */
    uint8_t PP_good = 0;

    /* Payload DCDCs good flag */
    uint8_t DCDC_good = 0;

    uint8_t QUIESCED_req = 0;
    EventBits_t current_evt;

    extern sensor_t * hotswap_amc_sensor;

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_HIGH );

    for ( ;; ) {
        check_fpga_reset();

        /* Initialize one of the FMC's DCDC so we can measure when the Payload Power is present */
        gpio_set_pin_state( PIN_PORT(GPIO_EN_FMC1_P12V), PIN_NUMBER(GPIO_EN_FMC1_P12V), GPIO_LEVEL_HIGH );

        new_state = state;

        current_evt = xEventGroupGetBits( amc_payload_evt );

        /*
         * When receive a PAYLOAD_MESSAGE_CLOCK_CONFIG command, write the new configuration
         * in EEPROM memory, reset the clock configuration and perform the new configuration.
        */
        if( current_evt & PAYLOAD_MESSAGE_CLOCK_CONFIG ){
            eeprom_24xx02_write(CHIP_ID_RTC_EEPROM, 0x0, clock_config, 16, 10);
            if (PAYLOAD_FPGA_ON){
                adn4604_reset();
                clock_configuration(clock_config);
            }
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_CLOCK_CONFIG);
        }
        if ( current_evt & PAYLOAD_MESSAGE_QUIESCE ) {

            /*
             * If you issue a shutdown fru command in the MCH shell, the payload power
             * task will receive a PAYLOAD_MESSAGE_QUIESCE message and set the
             * QUIESCED_req flag to '1' and the MCH will shutdown the 12VP0 power,
             * making the payload power task go to PAYLOAD_NO_POWER state.
             * So, if we are in the PAYLOAD_QUIESCED state and receive a
             * PAYLOAD_MESSAGE_QUIESCE message, the QUIESCED_req flag
             * should be '0'
             */

            if (state == PAYLOAD_QUIESCED) {
                QUIESCED_req = 0;
            } else {
                QUIESCED_req = 1;
            }
            xEventGroupClearBits( amc_payload_evt, PAYLOAD_MESSAGE_QUIESCE );
        }

        if ( current_evt & PAYLOAD_MESSAGE_COLD_RST ) {
            state = PAYLOAD_RESET;
            xEventGroupClearBits( amc_payload_evt, PAYLOAD_MESSAGE_COLD_RST );
        }

        if ( (current_evt & PAYLOAD_MESSAGE_REBOOT) || (current_evt & PAYLOAD_MESSAGE_WARM_RST) ) {
            fpga_soft_reset();
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_REBOOT | PAYLOAD_MESSAGE_WARM_RST);
        }

        payload_check_pgood(&PP_good);
        DCDC_good = gpio_read_pin( PIN_PORT(GPIO_DCDC_PGOOD), PIN_NUMBER(GPIO_DCDC_PGOOD) );

        switch(state) {

        case PAYLOAD_NO_POWER:
            if (PP_good) {
                new_state = PAYLOAD_POWER_GOOD_WAIT;
            }
            break;

        case PAYLOAD_POWER_GOOD_WAIT:
            /* Turn DDC converters on */
            setDC_DC_ConvertersON( true );

            /* Clear hotswap sensor backend power failure bits */
            hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK );
            hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_FAILURE_MASK );

            if ( QUIESCED_req || ( PP_good == 0 ) ) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if ( DCDC_good == 1 ) {
                new_state = PAYLOAD_STATE_FPGA_SETUP;
            }
            break;

        case PAYLOAD_STATE_FPGA_SETUP:
            gpio_set_pin_state(PIN_PORT(GPIO_FMC1_PG_C2M), PIN_NUMBER(GPIO_FMC1_PG_C2M), GPIO_LEVEL_HIGH);
            gpio_set_pin_state(PIN_PORT(GPIO_FMC2_PG_C2M), PIN_NUMBER(GPIO_FMC2_PG_C2M), GPIO_LEVEL_HIGH);
#ifdef MODULE_ADN4604
            /* Configure clock switch */
            if (clock_configuration(clock_config) == MMC_OK) {
                new_state = PAYLOAD_FPGA_ON;
            }
#else
            new_state = PAYLOAD_FPGA_ON;
#endif
            break;

        case PAYLOAD_FPGA_ON:
            if ( QUIESCED_req == 1 || PP_good == 0 || DCDC_good == 0 ) {
                new_state = PAYLOAD_SWITCHING_OFF;
            }
            break;

        case PAYLOAD_SWITCHING_OFF:
            setDC_DC_ConvertersON( false );

            gpio_set_pin_state(PIN_PORT(GPIO_FMC1_PG_C2M), PIN_NUMBER(GPIO_FMC1_PG_C2M), GPIO_LEVEL_LOW);
            gpio_set_pin_state(PIN_PORT(GPIO_FMC2_PG_C2M), PIN_NUMBER(GPIO_FMC2_PG_C2M), GPIO_LEVEL_LOW);

            /* Respond to quiesce event if any */
            if ( QUIESCED_req ) {
                hotswap_set_mask_bit( HOTSWAP_AMC, HOTSWAP_QUIESCED_MASK );
                hotswap_send_event( hotswap_amc_sensor, HOTSWAP_STATE_QUIESCED );
                hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_QUIESCED_MASK );
                QUIESCED_req = 0;
            }
            new_state = PAYLOAD_QUIESCED;
            break;

        case PAYLOAD_QUIESCED:
            /* Wait until power goes down to restart the cycle */
            if (PP_good == 0 && DCDC_good == 0) {
                new_state = PAYLOAD_NO_POWER;
            }
            break;
        case PAYLOAD_RESET:
            /*Reset DCDC converters*/
            setDC_DC_ConvertersON( false );
            new_state = PAYLOAD_NO_POWER;
            break;

        default:
            break;
        }

        state = new_state;
        vTaskDelayUntil( &xLastWakeTime, PAYLOAD_BASE_DELAY );
    }
}


/* HPM Functions */
#ifdef MODULE_HPM

#include "flash_spi.h"
#include "string.h"

uint8_t *hpm_page = NULL;
uint8_t hpm_pg_index;
uint32_t hpm_page_addr;

uint8_t payload_hpm_prepare_comp( void )
{
    /* Initialize variables */
    if (hpm_page != NULL) {
        vPortFree(hpm_page);
    }

    hpm_page = (uint8_t *) pvPortMalloc(PAYLOAD_HPM_PAGE_SIZE);

    if (hpm_page == NULL) {
        /* Malloc failed */
        return IPMI_CC_OUT_OF_SPACE;
    }

    memset(hpm_page, 0xFF, PAYLOAD_HPM_PAGE_SIZE);

    hpm_pg_index = 0;
    hpm_page_addr = 0;

    /* Initialize flash */
    ssp_init( FLASH_SPI, FLASH_SPI_BITRATE, FLASH_SPI_FRAME_SIZE, SSP_MASTER, SSP_INTERRUPT );

    /* Prevent the FPGA from accessing the Flash to configure itself now */
    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_HIGH );
    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_LOW );
    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_HIGH );
    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_LOW );

    /* Erase FLASH */
    flash_bulk_erase();

    return IPMI_CC_COMMAND_IN_PROGRESS;
}

uint8_t payload_hpm_upload_block( uint8_t * block, uint16_t size )
{
    /* TODO: Check DONE pin before accessing the SPI bus, since the FPGA may be reading it in order to boot */
    uint8_t remaining_bytes_start;

    if ( PAYLOAD_HPM_PAGE_SIZE - hpm_pg_index > size ) {
        /* Our page is not full yet, just append the new data */
        memcpy(&hpm_page[hpm_pg_index], block, size);
        hpm_pg_index += size;

        return IPMI_CC_OK;

    } else {
        /* Complete the remaining bytes on the buffer */
        memcpy(&hpm_page[hpm_pg_index], block, (PAYLOAD_HPM_PAGE_SIZE - hpm_pg_index));
        remaining_bytes_start = (PAYLOAD_HPM_PAGE_SIZE - hpm_pg_index);

        /* Program the complete page in the Flash */
        flash_program_page( hpm_page_addr, &hpm_page[0], PAYLOAD_HPM_PAGE_SIZE);

        hpm_page_addr += PAYLOAD_HPM_PAGE_SIZE;

        /* Empty our buffer and reset the index */
        memset(hpm_page, 0xFF, PAYLOAD_HPM_PAGE_SIZE);
        hpm_pg_index = 0;

        /* Save the trailing bytes */
        memcpy(&hpm_page[hpm_pg_index], block+remaining_bytes_start, size-remaining_bytes_start);

        hpm_pg_index = size-remaining_bytes_start;

        return IPMI_CC_COMMAND_IN_PROGRESS;
    }
}

uint8_t payload_hpm_finish_upload( uint32_t image_size )
{
    uint8_t cc = IPMI_CC_OK;

    /* Check if the last page was already programmed */
    if (!hpm_pg_index) {
        /* Program the complete page in the Flash */
        flash_program_page( hpm_page_addr, &hpm_page[0], (PAYLOAD_HPM_PAGE_SIZE-hpm_pg_index));
        hpm_pg_index = 0;
        hpm_page_addr = 0;

        cc = IPMI_CC_COMMAND_IN_PROGRESS;
    }

    /* Free page buffer */
    vPortFree(hpm_page);
    hpm_page = NULL;

    return cc;
}

uint8_t payload_hpm_get_upgrade_status( void )
{
    if (is_flash_busy()) {
        return IPMI_CC_COMMAND_IN_PROGRESS;
    } else {
        return IPMI_CC_OK;
    }
}

uint8_t payload_hpm_activate_firmware( void )
{
    /* Reset FPGA - Pulse PROGRAM_B pin */
    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_LOW);
    gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_HIGH);

    return IPMI_CC_OK;
}

mmc_err clock_configuration(const uint8_t clk_cfg[16])
{
    adn_connect_map_t con;
    mmc_err error;

    /* Translate the configuration to enable or disable the outputs */
    uint16_t out_enable_flag = {
        ((clk_cfg[0] & 0x80) >> 7) << 0 |
        ((clk_cfg[1] & 0x80) >> 7) << 1 |
        ((clk_cfg[2] & 0x80) >> 7) << 2 |
        ((clk_cfg[3] & 0x80) >> 7) << 3 |
        ((clk_cfg[4] & 0x80) >> 7) << 4 |
        ((clk_cfg[5] & 0x80) >> 7) << 5 |
        ((clk_cfg[6] & 0x80) >> 7) << 6 |
        ((clk_cfg[7] & 0x80) >> 7) << 7 |
        ((clk_cfg[8] & 0x80) >> 7) << 8 |
        ((clk_cfg[9] & 0x80) >> 7) << 9 |
        ((clk_cfg[10] & 0x80) >> 7) << 10 |
        ((clk_cfg[11] & 0x80) >> 7) << 11 |
        ((clk_cfg[12] & 0x80) >> 7) << 12 |
        ((clk_cfg[13] & 0x80) >> 7) << 13 |
        ((clk_cfg[14] & 0x80) >> 7) << 14 |
        ((clk_cfg[15] & 0x80) >> 7) << 15
    };

    /* Disable UPDATE' pin by pulling it GPIO_LEVEL_HIGH */
    gpio_set_pin_state( PIN_PORT(GPIO_ADN_UPDATE), PIN_NUMBER(GPIO_ADN_UPDATE), GPIO_LEVEL_HIGH );

    /* There's a delay circuit in the Reset pin of the clock switch, we must wait until it clears out */
    while( gpio_read_pin( PIN_PORT(GPIO_ADN_RESETN), PIN_NUMBER(GPIO_ADN_RESETN) ) == 0 ) {
        vTaskDelay( 50 );
    }

    /* Configure the interconnects*/
    con.out0 = clk_cfg[0] & 0x0F;
    con.out1 = clk_cfg[1] & 0x0F;
    con.out2 = clk_cfg[2] & 0x0F;
    con.out3 = clk_cfg[3] & 0x0F;
    con.out4 = clk_cfg[4] & 0x0F;
    con.out5 = clk_cfg[5] & 0x0F;
    con.out6 = clk_cfg[6] & 0x0F;
    con.out7 = clk_cfg[7] & 0x0F;
    con.out8 = clk_cfg[8] & 0x0F;
    con.out9 = clk_cfg[9] & 0x0F;
    con.out10 = clk_cfg[10] & 0x0F;
    con.out11 = clk_cfg[11] & 0x0F;
    con.out12 = clk_cfg[12] & 0x0F;
    con.out13 = clk_cfg[13] & 0x0F;
    con.out14 = clk_cfg[14] & 0x0F;
    con.out15 = clk_cfg[15] & 0x0F;

    error = adn4604_xpt_config( ADN_XPT_MAP0_CON_REG, con );
    if (error != MMC_OK) {
        return error;
    }

    /* Enable desired outputs */
    for ( uint8_t i = 0; i < 16; i++ ) {
        if ( ( out_enable_flag >> i ) & 0x1 ) {
            adn4604_tx_control( i, TX_ENABLED );
        } else {
            adn4604_tx_control( i, TX_DISABLED );
        }
    }

    error = adn4604_active_map( ADN_XPT_MAP0 );
    if (error != MMC_OK) {
        return error;
    }

    return adn4604_update();
}
#endif
