/*
 * payload.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Piotr Miedzik <P.Miedzik@gsi.de>
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
 */

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "port.h"
#include "payload.h"
#include "sdr.h"
#include "board_version.h"
#include "pin_mapping.h"
#include "ipmi.h"
#include "task_priorities.h"
#include "adn4604.h"
#include "led.h"
#include "ad84xx.h"
#include "hotswap.h"
#include "flash_spi.h"

/* payload states
 *   0 - no power
 *   1 - power switching on
 *       Power Up sequence
 *
 *   2 - power good wait
 *       Since power supply switching
 *       Until detect power good
 *
 *   3 - power good
 *       Here you can configure devices such as clock crossbar and others
 *       We have to reset pin state program b
 *
 *   4 - fpga booting
 *       Since DCDC converters initialization
 *       Until FPGA DONE signal
 *       about 30 sec
 *
 *   5 - fpga working
 *
 *   6 - power switching off
 *       Power-off sequence
 *
 *   7 - power QUIESCED
 *       It continues until a power outage on the line 12v
 *       or for 30 seconds (???)
 *
 * 255 - power fail
 */

void setDC_DC_ConvertersON( bool on )
{
    bool _on = on;

    /* @todo: check vadj relationship */
    bool _on_fmc1 = false | on;
    bool _on_fmc2 = false | on;

    gpio_set_pin_state( GPIO_EN_FMC1_PVADJ_PORT, GPIO_EN_FMC1_PVADJ_PIN, _on_fmc1);
    gpio_set_pin_state( GPIO_EM_FMC1_P12V_PORT, GPIO_EM_FMC1_P12V_PIN, _on_fmc1);
    gpio_set_pin_state( GPIO_EN_FMC1_P3V3_PORT, GPIO_EN_FMC1_P3V3_PIN, _on_fmc1);

    gpio_set_pin_state( GPIO_EN_FMC2_PVADJ_PORT, GPIO_EN_FMC2_PVADJ_PIN, _on_fmc2);
    gpio_set_pin_state( GPIO_EM_FMC2_P12V_PORT, GPIO_EM_FMC2_P12V_PIN, _on_fmc2);
    gpio_set_pin_state( GPIO_EN_FMC2_P3V3_PORT, GPIO_EN_FMC2_P3V3_PIN, _on_fmc2);


    gpio_set_pin_state( GPIO_EN_P1V0_PORT, GPIO_EN_P1V0_PIN, _on);
    gpio_set_pin_state( GPIO_EN_P1V8_PORT, GPIO_EN_P1V8_PIN, _on); // <- this one causes problems if not switched off before power loss
    gpio_set_pin_state( GPIO_EN_P1V2_PORT, GPIO_EN_P1V2_PIN, _on);
    gpio_set_pin_state( GPIO_EN_1V5_VTT_PORT, GPIO_EN_1V5_VTT_PIN, _on);
    gpio_set_pin_state( GPIO_EN_P3V3_PORT, GPIO_EN_P3V3_PIN, _on);
}

void initializeDCDC( void )
{
    setDC_DC_ConvertersON(false);
    gpio_set_pin_dir( GPIO_EN_P1V2_PORT, GPIO_EN_P1V2_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_P1V8_PORT, GPIO_EN_P1V8_PIN, OUTPUT);

    gpio_set_pin_dir( GPIO_EN_FMC2_P3V3_PORT, GPIO_EN_FMC2_P3V3_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_FMC2_PVADJ_PORT, GPIO_EN_FMC2_PVADJ_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EM_FMC2_P12V_PORT, GPIO_EM_FMC2_P12V_PIN, OUTPUT);

    gpio_set_pin_dir( GPIO_EM_FMC1_P12V_PORT, GPIO_EM_FMC1_P12V_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_FMC1_P3V3_PORT, GPIO_EN_FMC1_P3V3_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_FMC1_PVADJ_PORT,  GPIO_EN_FMC1_PVADJ_PIN, OUTPUT);

    gpio_set_pin_dir( GPIO_EN_P3V3_PORT, GPIO_EN_P3V3_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_1V5_VTT_PORT, GPIO_EN_1V5_VTT_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_P1V0_PORT, GPIO_EN_P1V0_PIN, OUTPUT);

    /* RTM TEST */
    gpio_set_pin_dir( 1, 30, OUTPUT);

    gpio_set_pin_dir( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, OUTPUT);
}


QueueHandle_t queue_payload_handle = 0;

void payload_send_message(uint8_t msg)
{
    if (queue_payload_handle) {
        xQueueSend(queue_payload_handle, &msg, (TickType_t) 0);
    }
}

TaskHandle_t vTaskPayload_Handle;

void payload_init( void )
{
    xTaskCreate(vTaskPayload, "Payload", 450, NULL, tskPAYLOAD_PRIORITY, &vTaskPayload_Handle);
    queue_payload_handle = xQueueCreate(5, sizeof(uint8_t));

    initializeDCDC();

#ifdef MODULE_DAC_AD84XX
    dac_vadj_init();
    dac_vadj_config( 0, 25 );
    dac_vadj_config( 1, 25 );
#endif

    if (afc_board_info.board_version == BOARD_VERSION_AFC_V3_1) {
        /* Flash CS Mux */
        /* 0 = FPGA reads bitstream from Program memory
         * 1 = FPGA reads bitstream from User memory
         */
        gpio_set_pin_dir(0, 19, OUTPUT);
        gpio_set_pin_state(0, 19, LOW);

        /* Init_B */
        /* TODO: Check Init_b pin for error on initialization, then use it as output control */

        gpio_set_pin_dir(0, 20, OUTPUT);
        gpio_set_pin_state(0, 20, HIGH);
    }
    /* Enable RTM */
    gpio_set_pin_state( 1, 30, true);
}

void vTaskPayload(void *pvParmeters)
{
    payload_state state = PAYLOAD_NO_POWER;
    payload_state new_state = PAYLOAD_STATE_NO_CHANGE;

    uint8_t P12V_good = 0;
    uint8_t P1V0_good = 0;
    uint8_t FPGA_boot_DONE = 0;
    uint8_t QUIESCED_req = 0;

    uint8_t evt_msg;

    uint8_t current_message;

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);

    for ( ;; ) {

        /* Initialize one of the FMC's DCDC so we can measure when the Payload Power is present */
        gpio_set_pin_state( GPIO_EM_FMC1_P12V_PORT, GPIO_EM_FMC1_P12V_PIN, HIGH);

        new_state = state;

        /* Read all messages from the queue */
        while(xQueueReceive(queue_payload_handle, &current_message, (TickType_t) 0 )) {
            switch (current_message) {
            case PAYLOAD_MESSAGE_P12GOOD:
                P12V_good = 1;
                break;
            case PAYLOAD_MESSAGE_P12GOODn:
                P12V_good = 0;
                break;
            case PAYLOAD_MESSAGE_PGOOD:
                P1V0_good = 1;
                break;
            case PAYLOAD_MESSAGE_PGOODn:
                P1V0_good = 0;
                break;
            case PAYLOAD_MESSAGE_QUIESCED:
                QUIESCED_req = 1;
                break;
            }
        }

        FPGA_boot_DONE = gpio_read_pin( GPIO_DONE_B_PORT, GPIO_DONE_B_PIN);
        P1V0_good = gpio_read_pin( GPIO_PGOOD_P1V0_PORT,GPIO_PGOOD_P1V0_PIN);

        switch(state) {
        case PAYLOAD_NO_POWER:
            if (P12V_good == 1) {
                new_state = PAYLOAD_SWITCHING_ON;
            }
            QUIESCED_req = 0;
            break;

        case PAYLOAD_SWITCHING_ON:
            setDC_DC_ConvertersON(true);
            new_state = PAYLOAD_POWER_GOOD_WAIT;
            break;

        case PAYLOAD_POWER_GOOD_WAIT:
            if (QUIESCED_req) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (P1V0_good == 1) {
                new_state = PAYLOAD_STATE_FPGA_SETUP;
            }
            break;

        case PAYLOAD_STATE_FPGA_SETUP:
#ifdef MODULE_CLOCK_SWITCH
            adn4604_setup();
#endif
            new_state = PAYLOAD_FPGA_BOOTING;
            break;

        case PAYLOAD_FPGA_BOOTING:
            if (QUIESCED_req == 1) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (FPGA_boot_DONE) {
                new_state = PAYLOAD_FPGA_WORKING;
            } else if (P12V_good == 0) {
                QUIESCED_req = 0;
                new_state = PAYLOAD_NO_POWER;
            }
            break;

        case PAYLOAD_FPGA_WORKING:
            if (QUIESCED_req == 1) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (P12V_good == 0) {
                QUIESCED_req = 0;
                new_state = PAYLOAD_NO_POWER;
            }
            break;

        case PAYLOAD_SWITCHING_OFF:
            setDC_DC_ConvertersON(false);
            sensor_array[HOT_SWAP_SENSOR].readout_value = HOTSWAP_QUIESCED_MASK;

            evt_msg = HOTSWAP_QUIESCED_MASK >> 1;

            if ( ipmi_event_send(&sensor_array[HOT_SWAP_SENSOR], ASSERTION_EVENT, &evt_msg, sizeof(evt_msg)) == ipmb_error_success) {
                QUIESCED_req = 0;
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

IPMI_HANDLER(ipmi_picmg_cmd_fru_control, NETFN_GRPEXT, IPMI_PICMG_CMD_FRU_CONTROL, ipmi_msg *req, ipmi_msg *rsp)
{
    payload_send_message(PAYLOAD_MESSAGE_QUIESCED);
    rsp->completion_code = IPMI_CC_OK;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}

/* HPM Functions */
#ifdef MODULE_HPM

#include "string.h"

uint8_t hpm_page[256];
uint8_t hpm_pg_index;
uint32_t hpm_page_addr;

uint8_t payload_hpm_prepare_comp( void )
{
    /* Initialize variables */
    memset(hpm_page, 0, sizeof(hpm_page));
    hpm_pg_index = 0;
    hpm_page_addr = 0;

    /* TODO: Check DONE pin before accessing the SPI bus, since the FPGA may be reading it in order to boot */

    /* Initialize flash */
    ssp_init( FLASH_SPI, FLASH_SPI_BITRATE, FLASH_SPI_FRAME_SIZE, SSP_MASTER, SSP_INTERRUPT );

    /* Prevent the FPGA from accessing the Flash to configure itself now */
    gpio_set_pin_dir( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, OUTPUT);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, LOW);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, LOW);

    /* Erase FLASH */
    flash_bulk_erase();

    return 1;
}

uint32_t payload_hpm_upload_block( uint8_t * block, uint16_t size )
{
    /* TODO: Check DONE pin before accessing the SPI bus, since the FPGA may be reading it in order to boot */
    uint8_t remaining_bytes_start;

    if ( sizeof(hpm_page) - hpm_pg_index > size) {
        /* Our page is not full yet, just append the new data */
        memcpy(&hpm_page[hpm_pg_index], block, size);
        hpm_pg_index += size;

    } else {
        /* Complete the remaining bytes on the buffer */
        memcpy(&hpm_page[hpm_pg_index], block, (sizeof(hpm_page) - hpm_pg_index));
        remaining_bytes_start = (sizeof(hpm_page) - hpm_pg_index);

        /* Program the complete page in the Flash */
        flash_program_page( hpm_page_addr, &hpm_page[0], sizeof(hpm_page));

        hpm_page_addr += sizeof(hpm_page);

        /* Empty our buffer and reset the index */
        memset(hpm_page, 0, sizeof(hpm_page));
        hpm_pg_index = 0;

        /* Save the trailing bytes */
        memcpy(&hpm_page[hpm_pg_index], block+remaining_bytes_start, size-remaining_bytes_start);

        hpm_pg_index = size-remaining_bytes_start;

    }
    /* Return the offset address */
    return hpm_page_addr + hpm_pg_index;
}

uint8_t payload_hpm_finish_upload( uint32_t image_size )
{
    /* Reset FPGA - Pulse PROGRAM_B pin */
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, LOW);
    asm("nop");
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);

    LED_activity_desc_t LEDact;
    const LED_activity_desc_t * pLEDact;
    pLEDact = &LEDact;
    pLEDact = &LED_On_Activity;
    LED_update( LED_RED, pLEDact );

    return 1;
}
#endif
