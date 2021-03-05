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
#include "adc_17xx_40xx.h"
#include "payload.h"
#include "ipmi.h"
#include "task_priorities.h"
#include "mcp23016.h"
#include "ad84xx.h"
#include "hotswap.h"
#include "utils.h"
#include "fru.h"
#include "led.h"
#include "board_led.h"


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

const external_gpio_t ext_gpios[15] = {
        [EXT_GPIO_P1V5_VTT_EN] =     { 1, 7 },
        [EXT_GPIO_EN_P1V8] =         { 1, 6 },
        [EXT_GPIO_EN_P1V2] =         { 1, 5 },
        [EXT_GPIO_EN_FMC1_P12V] =    { 1, 4 },
        [EXT_GPIO_EN_FMC2_P12V] =    { 1, 3 },
        [EXT_GPIO_EN_FMC1_PVADJ] =   { 1, 2 },
        [EXT_GPIO_EN_FMC2_PVADJ] =   { 1, 1 },
        [EXT_GPIO_EN_FMC1_P3V3] =    { 1, 0 },
        [EXT_GPIO_EN_FMC2_P3V3] =    { 0, 7 },
        [EXT_GPIO_EN_P1V0] =         { 0, 6 },
        [EXT_GPIO_EN_P3V3] =         { 0, 5 },
        [EXT_GPIO_EN_RTM_PWR] =      { 0, 4 },
        [EXT_GPIO_EN_RTM_MP] =       { 0, 3 },
        [EXT_GPIO_FPGA_I2C_RESET] =  { 0, 2 },
        [EXT_GPIO_PROGRAM_B] =       { 0, 0 }
};


/**
 * @brief Set AFC's DCDC Converters state
 *
 * @param on DCDCs state
 *
 */

uint8_t setDC_DC_ConvertersON(bool on)
{
    uint8_t power_pins[] = {
            EXT_GPIO_EN_P1V0,
            EXT_GPIO_EN_P1V8,
            EXT_GPIO_EN_P3V3,
            EXT_GPIO_EN_FMC1_PVADJ,
            EXT_GPIO_EN_FMC2_PVADJ,
            EXT_GPIO_P1V5_VTT_EN,
            EXT_GPIO_EN_P1V2,
            EXT_GPIO_EN_FMC1_P12V,
            EXT_GPIO_EN_FMC1_P3V3,
            EXT_GPIO_EN_FMC2_P12V,
            EXT_GPIO_EN_FMC2_P3V3
    };

    uint8_t pin;
    if (on) {
        printf("Enable Power\n");

        for (uint8_t i = 0; i < (sizeof(power_pins) / sizeof(power_pins[0])); i++) {
            pin = power_pins[i];
            mcp23016_write_pin( ext_gpios[pin].port_num, ext_gpios[pin].pin_num, true );
            vTaskDelay(10);
        }
    } else {
        printf("Disable Power\n");

        for (uint8_t i = (sizeof(power_pins) / sizeof(power_pins[0])); i > 0; i--) {
            pin = power_pins[i];
            mcp23016_write_pin( ext_gpios[pin].port_num, ext_gpios[pin].pin_num, false );
            vTaskDelay(10);
        }
    }
    return 1;
}

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

uint8_t payload_check_pgood()
{
    /* Threshold set to ~8V */
    const uint16_t PAYLOAD_THRESHOLD = 0x9B2;
    uint16_t dataADC;

    Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

    /* Waiting for A/D conversion complete */
    while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}
    /* Read ADC value */
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC);

    if (dataADC > PAYLOAD_THRESHOLD){
        return 1;
    }
    return 0;
}

uint8_t dcdc_check_pgood()
{
    const uint8_t POWER_GOOD = 0xF;
    const uint32_t pg_pins[] = {
        GPIO_FMC1_PV,
        GPIO_FMC2_PV,
        GPIO_AMC_RTM_PV,
        GPIO_PGOOD_P1V0
    };

    uint8_t pg = 0;
    uint8_t DCDC_flags = 0;

    for (uint8_t pin_num = 0; pin_num < sizeof(pg_pins) / sizeof(pg_pins[0]); pin_num++) {
        pg = gpio_read_pin(PIN_PORT(pg_pins[pin_num]), PIN_NUMBER(pg_pins[pin_num]));
        if (pg){
            DCDC_flags |= pg << pin_num;
        } else {
            DCDC_flags &= ~(1 << pin_num);
        }
    }

    if (DCDC_flags == POWER_GOOD){
        return 1;
    }
    return 0;
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
    bool standalone_mode = false;
    if (get_ipmb_addr() == 0xA2) {
        standalone_mode = true;
    }

    if (!standalone_mode) {
        /* Wait until ENABLE# signal is asserted ( ENABLE == 0) */
        while ( gpio_read_pin( PIN_PORT(GPIO_MMC_ENABLE), PIN_NUMBER(GPIO_MMC_ENABLE) ) == 1 ) {};
    }

    xTaskCreate( vTaskPayload, "Payload", 120, NULL, tskPAYLOAD_PRIORITY, &vTaskPayload_Handle );

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

#ifdef MODULE_ADC
    ADC_CLOCK_SETUP_T ADCSetup;
    Chip_ADC_Init(LPC_ADC, &ADCSetup);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
#endif

#ifdef MCP23016
    if (!dcdc_check_pgood()){

        //set output on all pins
        mcp23016_set_port_dir(0, 0);
        mcp23016_set_port_dir(1, 0);

        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_LOW);
        mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );
        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_INITB), PIN_NUMBER(GPIO_FPGA_INITB), GPIO_LEVEL_LOW);
    }
#endif
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

    for ( ;; ) {
        check_fpga_reset();

        new_state = state;

        current_evt = xEventGroupGetBits( amc_payload_evt );

        if ( current_evt & PAYLOAD_MESSAGE_QUIESCE ) {
            QUIESCED_req = 1;
            xEventGroupClearBits( amc_payload_evt, PAYLOAD_MESSAGE_QUIESCE );
        }

        if ( current_evt & PAYLOAD_MESSAGE_COLD_RST ) {
            state = PAYLOAD_SWITCHING_OFF;
            xEventGroupClearBits( amc_payload_evt, PAYLOAD_MESSAGE_COLD_RST );
        }

        if ( (current_evt & PAYLOAD_MESSAGE_REBOOT) || (current_evt & PAYLOAD_MESSAGE_WARM_RST) ) {
            fpga_soft_reset();
            xEventGroupClearBits( amc_payload_evt, PAYLOAD_MESSAGE_REBOOT );
        }

        PP_good = payload_check_pgood();
        DCDC_good = dcdc_check_pgood();

        switch(state) {

        case PAYLOAD_NO_POWER:
            if (PP_good) {
                new_state = PAYLOAD_POWER_GOOD_WAIT;
            }
            break;

        case PAYLOAD_POWER_GOOD_WAIT:
            /* Turn DDC converters on */
            if ( DCDC_good == 0 ){
                setDC_DC_ConvertersON( true );
            }

            /* Clear hotswap sensor backend power failure bits */
            hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK );
            hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_FAILURE_MASK );

            if ( QUIESCED_req || ( PP_good == 0 ) ) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if ( DCDC_good == 1 ) {
                new_state = PAYLOAD_STATE_FPGA_SETUP;

                gpio_set_pin_state(PIN_PORT(GPIO_FPGA_INITB), PIN_NUMBER(GPIO_FPGA_INITB), GPIO_LEVEL_HIGH);
                mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, true );
                gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_HIGH);
            }
            break;

        case PAYLOAD_STATE_FPGA_SETUP:

            new_state = PAYLOAD_FPGA_ON;
            break;

        case PAYLOAD_FPGA_ON:
            if ( QUIESCED_req == 1 || PP_good == 0 || DCDC_good == 0 ) {
                new_state = PAYLOAD_SWITCHING_OFF;
            }
            break;

        case PAYLOAD_SWITCHING_OFF:
            gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_LOW);
            mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );
            gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_LOW);

            setDC_DC_ConvertersON( false );

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

    memset(hpm_page, 0xFF, sizeof(hpm_page));

    hpm_pg_index = 0;
    hpm_page_addr = 0;

    /* Initialize flash */
    ssp_init( FLASH_SPI, FLASH_SPI_BITRATE, FLASH_SPI_FRAME_SIZE, SSP_MASTER, SSP_INTERRUPT );

    /* Prevent the FPGA from accessing the Flash to configure itself now */
    mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, true );
    mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );
    mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, true );
    mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );

    /* Erase FLASH */
    flash_bulk_erase();

    return IPMI_CC_COMMAND_IN_PROGRESS;
}

uint8_t payload_hpm_upload_block( uint8_t * block, uint16_t size )
{
    /* TODO: Check DONE pin before accessing the SPI bus, since the FPGA may be reading it in order to boot */
    uint8_t remaining_bytes_start;

    if ( sizeof(hpm_page) - hpm_pg_index > size ) {
        /* Our page is not full yet, just append the new data */
        memcpy(&hpm_page[hpm_pg_index], block, size);
        hpm_pg_index += size;

        return IPMI_CC_OK;

    } else {
        /* Complete the remaining bytes on the buffer */
        memcpy(&hpm_page[hpm_pg_index], block, (sizeof(hpm_page) - hpm_pg_index));
        remaining_bytes_start = (sizeof(hpm_page) - hpm_pg_index);

        /* Program the complete page in the Flash */
        flash_program_page( hpm_page_addr, &hpm_page[0], sizeof(hpm_page));

        hpm_page_addr += sizeof(hpm_page);

        /* Empty our buffer and reset the index */
        memset(hpm_page, 0xFF, sizeof(hpm_page));
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
        flash_program_page( hpm_page_addr, &hpm_page[0], (sizeof(hpm_page)-hpm_pg_index));
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
    mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );
    mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, true );

    return IPMI_CC_OK;
}
#endif
