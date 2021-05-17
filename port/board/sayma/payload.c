/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2021  Wojciech Ruclo <wojciech.ruclo@creotech.pl>
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
#include "task_priorities.h"
#include "utils.h"
#include "fru.h"
#include "sdr.h"
#include "led.h"
#include "sensors.h"
#include "board_led.h"

/* payload states
 *   0 - No power
 *
 *   1 - Power Good wait
 *       Since power supply switching
 *       Until detect power good
 *
 *   2 - FPGA on
 *
 *   3 - Power switching off
 *       Power-off sequence
 *
 *   4 - Power QUIESCED
 *       It continues until a power outage on the line 12v
 *
 * 255 - power fail
 */

static void fpga_soft_reset(void)
{
    gpio_set_pin_low(PIN_PORT(GPIO_FPGA_RESETn), PIN_NUMBER(GPIO_FPGA_RESETn));
    asm("NOP");
    gpio_set_pin_high(PIN_PORT(GPIO_FPGA_RESETn), PIN_NUMBER(GPIO_FPGA_RESETn));

    /* Blink RED LED to indicate to the user that the Reset was performed */
    LEDUpdate(FRU_AMC, LED1, LEDMODE_LAMPTEST, LEDINIT_ON, 5, 0);
}

static void check_fpga_reset(void)
{
    static TickType_t edge_time;
    static uint8_t reset_lock;
    static uint8_t last_state = 1;

    TickType_t diff;
    TickType_t cur_time = xTaskGetTickCount();

    uint8_t cur_state = gpio_read_pin(PIN_PORT(GPIO_FRONT_BUTTON), PIN_NUMBER(GPIO_FRONT_BUTTON));

    if ((cur_state == 0) && (last_state == 1)) {
        /* Detects the falling edge of the front panel button */
        edge_time = cur_time;
        reset_lock = 0;
    }

    diff = getTickDifference(cur_time, edge_time);

    if ((diff > pdMS_TO_TICKS(2000)) && (reset_lock == 0) && (cur_state == 0)) {
        fpga_soft_reset();
        /* If the user continues to press the button after the 2s, prevent this action to be repeated */
        reset_lock = 1;
    }

    last_state = cur_state;
}

/**
 * @brief Set Sayma's DCDC Converters state
 *
 * @param on DCDCs state
 */

bool setDC_DC_ConvertersON(bool on)
{
    enum
    {
        ENABLE,
        POWER_GOOD
    };

    uint32_t power_pins[][2] = {
            { GPIO_EN_P0V9, GPIO_P0V95I_PG },
            { GPIO_EN_P5V0, GPIO_PGOOD_P5V0 },
            { GPIO_EN_P0V95, GPIO_P0V95_PG },
            { GPIO_EN_1V8, 0x0 },
            { GPIO_EN_3V3, 0x0 },
            { GPIO_EN_1V5, GPIO_SDRAM_PGOOD },
            { GPIO_EN_1V0, GPIO_LTM_PGOOD },
            { GPIO_EN_1V2, GPIO_P1V2_PG },
            { GPIO_EN_P3V3_CLK, 0x0 },
            { GPIO_EN_P3V3_FMC, GPIO_P3V3_FMC_PGOOD },
            { GPIO_EN_FMC1_P12V, 0x0 },
            { GPIO_EN_RTM_PWR, 0x0 }
    };

    const char *power_pin_names[] = { "P0V95I", "P5V0", "P0V95", "", "", "SDRAM", "LTM", "P1V2", "", "P3V3 FMC", "" };

    uint8_t pin;

    uint32_t pg;
    uint16_t timeoutCounter = 10;

    // ON
    if (on) {

        printf("Enable Power\n");

        for (pin = 0; pin < (sizeof(power_pins) / sizeof(power_pins[0])) - 1; pin++) {

            // Enable power supply
            gpio_set_pin_state(PIN_PORT(power_pins[pin][ENABLE]), PIN_NUMBER(power_pins[pin][ENABLE]), true);

            // Verify Power Good if available
            timeoutCounter = 10;
            pg = power_pins[pin][POWER_GOOD];

            vTaskDelay(10);

            if (pg != 0x0) {

                while (timeoutCounter > 0) {
                    if (gpio_read_pin(PIN_PORT(pg), PIN_NUMBER(pg)) == 1)
                        break;
                    vTaskDelay(10);
                    timeoutCounter--;
                };

                if (timeoutCounter == 0) {
                    printf("%s POWER GOOD error.\n", power_pin_names[pin]);
                    return false;
                }
            }
        }
    }

    // OFF
    else {

        printf("Disable Power\n");

        for (pin = (sizeof(power_pins) / sizeof(power_pins[0])); pin > 0; pin--) {
            gpio_set_pin_state(PIN_PORT(power_pins[pin - 1][ENABLE]), PIN_NUMBER(power_pins[pin - 1][ENABLE]), false);
            vTaskDelay(10);
        }
    }
    return true;
}

EventGroupHandle_t amc_payload_evt = NULL;
#ifdef MODULE_RTM
EventGroupHandle_t rtm_payload_evt = NULL;
#endif

void payload_send_message(uint8_t fru_id, EventBits_t msg)
{
    if ((fru_id == FRU_AMC) && amc_payload_evt) {
        xEventGroupSetBits(amc_payload_evt, msg);
#ifdef MODULE_RTM
    } else if ((fru_id == FRU_RTM) && rtm_payload_evt) {
        xEventGroupSetBits(rtm_payload_evt, msg);
#endif
    }
}

TaskHandle_t vTaskPayload_Handle;

void payload_init(void)
{
    if (!bench_test) {
        /* Wait until ENABLE# signal is asserted ( ENABLE == 0) */
        while ( gpio_read_pin( PIN_PORT(GPIO_MMC_ENABLE), PIN_NUMBER(GPIO_MMC_ENABLE) ) == 1) {
        };
    }

    xTaskCreate(vTaskPayload, "Payload", 240, NULL, tskPAYLOAD_PRIORITY, &vTaskPayload_Handle);

    amc_payload_evt = xEventGroupCreate();
#ifdef MODULE_RTM
    rtm_payload_evt = xEventGroupCreate();
#endif

    gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESETn), PIN_NUMBER(GPIO_FPGA_RESETn), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_LOW);
}

void vTaskPayload(void *pvParameters)
{
    uint8_t state = PAYLOAD_NO_POWER;
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

    for (;;) {

        check_fpga_reset();

        new_state = state;

        current_evt = xEventGroupGetBits(amc_payload_evt);

        if (current_evt & PAYLOAD_MESSAGE_PPGOOD) {
            PP_good = 1;
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_PPGOOD);
        }

        if (current_evt & PAYLOAD_MESSAGE_PPGOODn) {
            PP_good = 0;
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_PPGOODn);
        }

        if (current_evt & PAYLOAD_MESSAGE_DCDC_PGOOD) {
            DCDC_good = 1;
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_DCDC_PGOOD);
        }

        if (current_evt & PAYLOAD_MESSAGE_DCDC_PGOODn) {
            DCDC_good = 0;
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_DCDC_PGOODn);
        }

        if (current_evt & PAYLOAD_MESSAGE_COLD_RST) {
            state = PAYLOAD_SWITCHING_OFF;
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_COLD_RST);
        }

        if (current_evt & PAYLOAD_MESSAGE_WARM_RST) {
            fpga_soft_reset();
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_WARM_RST);
        }

        if (current_evt & PAYLOAD_MESSAGE_REBOOT) {
            fpga_soft_reset();
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_REBOOT);
        }

        if (current_evt & PAYLOAD_MESSAGE_QUIESCE) {
            QUIESCED_req = 1;
            xEventGroupClearBits(amc_payload_evt, PAYLOAD_MESSAGE_QUIESCE);
        }

        PP_good = gpio_read_pin(PIN_PORT(GPIO_P12V0_OK), PIN_NUMBER(GPIO_P12V0_OK));

        switch (state) {

        case PAYLOAD_NO_POWER:

            if (PP_good) {
                new_state = PAYLOAD_POWER_GOOD_WAIT;
            }
            break;

        case PAYLOAD_POWER_GOOD_WAIT:

            /* Turn DDC converters on */
            DCDC_good = setDC_DC_ConvertersON(true);

            /* Clear hotswap sensor backend power failure bits */
            hotswap_clear_mask_bit(HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK);
            hotswap_clear_mask_bit(HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_FAILURE_MASK);

            if (QUIESCED_req == 1 || PP_good == 0 || DCDC_good == 0) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (DCDC_good == 1) {
                new_state = PAYLOAD_FPGA_ON;

#ifdef MODULE_RTM
                /* Send payload ready message to RTM */
                payload_send_message(FRU_RTM, PAYLOAD_MESSAGE_RTM_READY);
#endif

                vTaskDelay(50);
                gpio_set_pin_state(PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_HIGH);
                gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESETn), PIN_NUMBER(GPIO_FPGA_RESETn), GPIO_LEVEL_HIGH);
            }
            break;

        case PAYLOAD_FPGA_ON:

            if (QUIESCED_req == 1 || PP_good == 0 || DCDC_good == 0) {
                new_state = PAYLOAD_SWITCHING_OFF;
            }
            break;

        case PAYLOAD_SWITCHING_OFF:

            gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESETn), PIN_NUMBER(GPIO_FPGA_RESETn), GPIO_LEVEL_LOW);
            gpio_set_pin_state( PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), GPIO_LEVEL_LOW );

            /* Turn DDC converters off */
            setDC_DC_ConvertersON(false);

            /* Respond to quiesce event if any */
            if (QUIESCED_req) {
                hotswap_set_mask_bit(HOTSWAP_AMC, HOTSWAP_QUIESCED_MASK);
                hotswap_send_event(hotswap_amc_sensor, HOTSWAP_STATE_QUIESCED);
                hotswap_clear_mask_bit(HOTSWAP_AMC, HOTSWAP_QUIESCED_MASK);
                QUIESCED_req = 0;
            }

            /* Respond to power good error event if any */
            if (DCDC_good == 0) {
                new_state = PAYLOAD_ERROR;
            } else {
                new_state = PAYLOAD_QUIESCED;
            }

            /* Reset the power good flags to avoid the state machine to start over without a new read from the sensors */
            DCDC_good = 0;
            break;

        case PAYLOAD_QUIESCED:
            /* Wait until power goes down to restart the cycle */
            if (PP_good == 0) {
                new_state = PAYLOAD_NO_POWER;
            }
            break;

        case PAYLOAD_ERROR:

            printf("[PAYLOAD ERROR] Turned off.\n");

            /* Turn on RED LED to indicate to the user that the payload error was detected */
            LEDUpdate(FRU_AMC, LED1, LEDMODE_OVERRIDE, LEDINIT_ON, 0, 0);
            break;

        default:
            break;
        }

        state = new_state;
        vTaskDelayUntil(&xLastWakeTime, PAYLOAD_BASE_DELAY);
    }
}

void vTaskPowerGood(void *Parameters)
{
    /* Initialize old_state with a different value, so that the uC always send its state on startup */
    static uint8_t new_state_pg = 0, old_state_pg = 1;

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 50;

    /* Initialize the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    uint8_t pin_state;
    uint8_t pg_ok = 0b1111111;

    uint32_t pg_pins[] = {
        GPIO_P0V95I_PG,
        GPIO_PGOOD_P5V0,
        GPIO_P0V95_PG,
        GPIO_SDRAM_PGOOD,
        GPIO_LTM_PGOOD,
        GPIO_P1V2_PG,
        GPIO_P3V3_FMC_PGOOD
    };
    uint8_t PG_NUM = 7;

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        uint8_t i;
        for (i = 0; i < PG_NUM; i++) {
            pin_state = gpio_read_pin(PIN_PORT(pg_pins[i]), PIN_NUMBER(pg_pins[i]));
            if (pin_state) {
                new_state_pg |= pin_state << i;
            }
            else {
                new_state_pg &= ~(1 << i);
            }
        }

        if (new_state_pg ^ old_state_pg) {
            if (new_state_pg == pg_ok) {
                payload_send_message(FRU_AMC, PAYLOAD_MESSAGE_DCDC_PGOOD);
            }
            else {
                payload_send_message(FRU_AMC, PAYLOAD_MESSAGE_DCDC_PGOODn);
            }
            old_state_pg = new_state_pg;
        }
    }
}

/* HPM Functions */
#ifdef MODULE_HPM

#include "flash_spi.h"
#include "string.h"

uint8_t hpm_page[256];
uint8_t hpm_pg_index;
uint32_t hpm_page_addr;

uint8_t payload_hpm_prepare_comp( void )
{
    /* Initialize variables */
    memset(hpm_page, 0xFF, sizeof(hpm_page));
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
    /* Check if the last page was already programmed */
    if (!hpm_pg_index) {
        /* Program the complete page in the Flash */
        flash_program_page( hpm_page_addr, &hpm_page[0], (sizeof(hpm_page)-hpm_pg_index));
        hpm_pg_index = 0;
        hpm_page_addr = 0;

        return IPMI_CC_COMMAND_IN_PROGRESS;
    }

    return IPMI_CC_OK;
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
#endif

