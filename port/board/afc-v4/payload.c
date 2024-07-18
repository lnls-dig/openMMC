/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2021  Krzysztof Macias <krzysztof.macias@creotech.pl>
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
#include "idt_8v54816.h"
#include "hotswap.h"
#include "utils.h"
#include "fru.h"
#include "led.h"
#include "board_led.h"
#include "board_config.h"
#include "clock_config.h"
#include "eeprom_24xx02.h"
#include "i2c_mapping.h"
#include "pin_mapping.h"

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
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    } else {
        printf("Disable Power\n");

        for (uint8_t i = (sizeof(power_pins) / sizeof(power_pins[0])); i > 0; i--) {
            pin = power_pins[i];
            mcp23016_write_pin( ext_gpios[pin].port_num, ext_gpios[pin].pin_num, false );
            vTaskDelay(pdMS_TO_TICKS(10));
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
    mmc_err err;

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
    mcp23016_write_pin(ext_gpios[EXT_GPIO_EN_RTM_MP].port_num, ext_gpios[EXT_GPIO_EN_RTM_MP].pin_num, true);
#endif

#ifdef MODULE_ADC
    ADC_CLOCK_SETUP_T ADCSetup;
    Chip_ADC_Init(LPC_ADC, &ADCSetup);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
#endif

#ifdef MODULE_MCP23016
    if (!gpio_read_pin(PIN_PORT(GPIO_PGOOD_P1V0), PIN_NUMBER(GPIO_PGOOD_P1V0))){

        /*
         * Configure all GPIOs as outputs
         */
        err = mcp23016_set_port_dir(0, 0);

        if (err != MMC_OK) {
            PRINT_ERR_LINE(err);
        }

        err = mcp23016_set_port_dir(1, 0);

        if (err != MMC_OK) {
            PRINT_ERR_LINE(err);
        }

#ifdef MODULE_DAC_AD84XX
        /* Configure the PVADJ DAC */
        err = mcp23016_write_pin( ext_gpios[EXT_GPIO_DAC_VADJ_RSTn].port_num, ext_gpios[EXT_GPIO_DAC_VADJ_RSTn].pin_num, false );
        if (err != MMC_OK) {
            PRINT_ERR_LINE(err);
        }

        dac_ad84xx_init();

        err = mcp23016_write_pin( ext_gpios[EXT_GPIO_DAC_VADJ_RSTn].port_num, ext_gpios[EXT_GPIO_DAC_VADJ_RSTn].pin_num, true );
        if (err != MMC_OK) {
            PRINT_ERR_LINE(err);
        }

        set_vadj_volt( 0, 2.5 );
        set_vadj_volt( 1, 2.5 );
#endif

        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_LOW);
        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_INITB), PIN_NUMBER(GPIO_FPGA_INITB), GPIO_LEVEL_LOW);

        err = mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );

        if (err != MMC_OK) {
            PRINT_ERR_LINE(err);
        }

        err = mcp23016_write_pin( ext_gpios[EXT_GPIO_FPGA_I2C_RESET].port_num, ext_gpios[EXT_GPIO_FPGA_I2C_RESET].pin_num, true );

        if (err != MMC_OK) {
            PRINT_ERR_LINE(err);
        }
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
    mmc_err err;

    xLastWakeTime = xTaskGetTickCount();

    for ( ;; ) {
        check_fpga_reset();

        new_state = state;

        current_evt = xEventGroupGetBits( amc_payload_evt );
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

        /*
         * When receive a PAYLOAD_MESSAGE_CLOCK_CONFIG message, configure the clock switch
         * and write the new configuration in EEPROM
         */
        if( current_evt & PAYLOAD_MESSAGE_CLOCK_CONFIG ){
            eeprom_24xx02_write(CHIP_ID_RTC_EEPROM, 0x0, clock_config, 16, 10);
            if (PAYLOAD_FPGA_ON) {
                clock_switch_write_reg(clock_config);
            }
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_CLOCK_CONFIG);
        }
        if ( current_evt & PAYLOAD_MESSAGE_COLD_RST ) {
            state = PAYLOAD_RESET;
            xEventGroupClearBits( amc_payload_evt, PAYLOAD_MESSAGE_COLD_RST );
        }

        if ( (current_evt & PAYLOAD_MESSAGE_REBOOT) || (current_evt & PAYLOAD_MESSAGE_WARM_RST) ) {
            fpga_soft_reset();
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_REBOOT | PAYLOAD_MESSAGE_WARM_RST);
        }

        PP_good = payload_check_pgood();
        DCDC_good = gpio_read_pin(PIN_PORT(GPIO_PGOOD_P1V0), PIN_NUMBER(GPIO_PGOOD_P1V0));

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

                err = mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, true );

                if (err != MMC_OK) {
                    PRINT_ERR_LINE(err);
                }

                gpio_set_pin_state(PIN_PORT(GPIO_FPGA_INITB), PIN_NUMBER(GPIO_FPGA_INITB), GPIO_LEVEL_HIGH);
                gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_HIGH);
            }
            break;

        case PAYLOAD_STATE_FPGA_SETUP:
            gpio_set_pin_state(PIN_PORT(GPIO_FMC1_PG_C2M), PIN_NUMBER(GPIO_FMC1_PG_C2M), GPIO_LEVEL_HIGH);
            gpio_set_pin_state(PIN_PORT(GPIO_FMC2_PG_C2M), PIN_NUMBER(GPIO_FMC2_PG_C2M), GPIO_LEVEL_HIGH);


            /*
             * Only change the state if the clock switch configuration
             * succeeds
             */
            if (clock_switch_write_reg(clock_config)) {
                new_state = PAYLOAD_FPGA_ON;
            }
            break;

        case PAYLOAD_FPGA_ON:
            if ( QUIESCED_req == 1 || PP_good == 0 || DCDC_good == 0 ) {
                new_state = PAYLOAD_SWITCHING_OFF;
            }
            break;

        case PAYLOAD_SWITCHING_OFF:
            gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESET), PIN_NUMBER(GPIO_FPGA_RESET), GPIO_LEVEL_LOW);
            gpio_set_pin_state(PIN_PORT(GPIO_FPGA_INITB), PIN_NUMBER(GPIO_FPGA_INITB), GPIO_LEVEL_LOW);
            err = mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );

            gpio_set_pin_state(PIN_PORT(GPIO_FMC1_PG_C2M), PIN_NUMBER(GPIO_FMC1_PG_C2M), GPIO_LEVEL_LOW);
            gpio_set_pin_state(PIN_PORT(GPIO_FMC2_PG_C2M), PIN_NUMBER(GPIO_FMC2_PG_C2M), GPIO_LEVEL_LOW);

            if (err != MMC_OK) {
                PRINT_ERR_LINE(err);
            }

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

uint8_t payload_hpm_prepare_comp( void )
{
    return IPMI_CC_ILLEGAL_COMMAND_DISABLED;
}

uint8_t payload_hpm_upload_block( uint8_t * block, uint16_t size )
{
    return IPMI_CC_ILLEGAL_COMMAND_DISABLED;
}

uint8_t payload_hpm_finish_upload( uint32_t image_size )
{
    return IPMI_CC_ILLEGAL_COMMAND_DISABLED;
}

uint8_t payload_hpm_get_upgrade_status( void )
{
    return IPMI_CC_ILLEGAL_COMMAND_DISABLED;
}

uint8_t payload_hpm_activate_firmware( void )
{
    mmc_err err;

    /* Reset FPGA - Pulse PROGRAM_B pin */
    err = mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, false );

    if (err != MMC_OK) {
        PRINT_ERR_LINE(err);
    }

    err = mcp23016_write_pin( ext_gpios[EXT_GPIO_PROGRAM_B].port_num, ext_gpios[EXT_GPIO_PROGRAM_B].pin_num, true );

    if (err != MMC_OK) {
        PRINT_ERR_LINE(err);
    }

    return IPMI_CC_OK;
}
#endif
