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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "event_groups.h"

/* Project includes */
#include "port.h"
#include "rtm.h"
#include "rtm_user.h"
#include "ipmi.h"
#include "hotswap.h"
#include "sdr.h"
#include "task_priorities.h"
#include "fru.h"
#include "hotswap.h"
#include "payload.h"
#include "uart_debug.h"
#include "led.h"
#include "board_led.h"

volatile bool rtm_present;
extern EventGroupHandle_t rtm_payload_evt;

void RTM_Manage( void * Parameters )
{
    uint8_t ps_old_state = 0xFF;
    uint8_t ps_new_state;
    bool rtm_compatible = false;
    extern sensor_t * hotswap_rtm_sensor;
    EventBits_t current_evt;
    uint8_t rtm_hs_state;
    uint8_t payload_ready_new_state = 0x0;
    uint8_t payload_ready_old_state = 0x0;

    bool start = true;

    /* Defaults to not present */
    rtm_present = false;

    /* Start with RTM payload disabled */
    // rtm_disable_payload_power();

    for ( ;; ) {
        vTaskDelay(500);

        rtm_check_presence( &ps_new_state );

        if ( ps_new_state ^ ps_old_state ) {

            if ( ps_new_state == HOTSWAP_STATE_URTM_PRSENT ) {

                printf("[RTM] Rear Board detected!\n");

                rtm_present = true;

                /* Initialize basic hardware to enable communication */
                rtm_hardware_init();

                /* Create/Read the RTM FRU info before sending the hotswap event */
                fru_init(FRU_RTM);

                /* RTM Present event */
                hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_PRESENT_MASK );
                hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_ABSENT_MASK );
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_PRSENT );

                /* Check the Zone3 compatibility records */
                rtm_compatible = rtm_compatibility_check();
                if ( rtm_compatible ) {

                    printf("[RTM] Rear Board is compatible!\n");

                    /* Send RTM Compatible message */
                    hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_COMPATIBLE );
                    hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_COMPATIBLE_MASK );

                } else {

                    printf("[RTM] Rear Board is not compatible!\n");

                    /* Send RTM Incompatible message */
                    hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_INCOMPATIBLE );
                    hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_COMPATIBLE_MASK );
                }

                /* Activate RTM sensors in the SDR table */
                //sdr_activate_sensors(); /* Not implemented yet */

            } else if ( ps_new_state == HOTSWAP_STATE_URTM_ABSENT ) {

                /* Disable RTM sensors */
                //sdr_disable_sensors(); /* Not implemented yet */

                printf("[RTM] Rear Board disconnected!\n");

                /* Close hardware communication */
                rtm_hardware_close();

                rtm_present = false;

                /* RTM Absent event */
                hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_ABSENT_MASK );
                hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_PRESENT_MASK );
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_ABSENT );
            }

            ps_old_state = ps_new_state;
        }

        if (payload_ready_new_state ^ payload_ready_old_state) {
            if (payload_ready_new_state == 0x1 && rtm_present && start && rtm_compatible) {

                /* Perform hotswap first read */
                while (!rtm_get_hotswap_handle_status(&rtm_hs_state));

                /* Override RTM state so that if the handle is closed when the MMC is starting,
                 * the LED and payload power remains in the correct state */
                if (rtm_hs_state == 0) {
                    LEDUpdate(FRU_RTM, LED_BLUE, LEDMODE_OVERRIDE, LEDINIT_OFF, 0, 0);
                    payload_send_message(FRU_RTM, PAYLOAD_MESSAGE_RTM_ENABLE);
                } else {
                    LEDUpdate(FRU_RTM, LED_BLUE, LEDMODE_OVERRIDE, LEDINIT_ON, 0, 0);
                    payload_send_message(FRU_RTM, PAYLOAD_MESSAGE_QUIESCE);
                }

            }
            payload_ready_old_state = payload_ready_new_state;
            start = false;
        }

        /* Check enable/disable events */
        current_evt = xEventGroupGetBits( rtm_payload_evt );

        if ( current_evt & PAYLOAD_MESSAGE_QUIESCE ) {
            if ( rtm_quiesce() ) {
                rtm_disable_payload_power();
                /* Quiesced event */
                printf("[RTM] Quiesced RTM successfully!\n");
                hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_QUIESCED_MASK );
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_QUIESCED );
                payload_ready_new_state = 0;
            } else {
                printf("[RTM] RTM failed to quiesce!\n");
            }
            xEventGroupClearBits( rtm_payload_evt, PAYLOAD_MESSAGE_QUIESCE );

        } else if ( current_evt & PAYLOAD_MESSAGE_RTM_ENABLE ) {
            if (rtm_compatible) {
                hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_QUIESCED_MASK );
                printf("[RTM] Enabling RTM Payload power...\n");
                rtm_enable_payload_power();
            } else {
                printf("[RTM] Impossible to enable payload power to an incompatible RTM board!\n");
            }
            xEventGroupClearBits( rtm_payload_evt, PAYLOAD_MESSAGE_RTM_ENABLE );
        } else if (current_evt & PAYLOAD_MESSAGE_RTM_READY) {
            payload_ready_new_state = 1;
            xEventGroupClearBits(rtm_payload_evt, PAYLOAD_MESSAGE_RTM_READY);
        }
    }
}

void rtm_manage_init( void )
{
    xTaskCreate( RTM_Manage, "RTM Manage", 150, (void *) NULL, tskRTM_MANAGE_PRIORITY, (TaskHandle_t *) NULL );
}

/* Set Power Level Request handler */

IPMI_HANDLER(ipmi_picmg_set_power_level, NETFN_GRPEXT, IPMI_PICMG_CMD_SET_POWER_LEVEL, ipmi_msg *req, ipmi_msg *rsp )
{
    int len = rsp->data_len = 0;
    uint8_t fru_id = req->data[1];
    uint8_t power_level = req->data[2];

    /*
     * Power Level:
     * 00h = Power off
     * 01h - 14h = Select power level, if available
     * 0xFF = Do not change power level
     */
    if ( fru_id == FRU_RTM && power_level != 0xFF ) {
        if (power_level == 0x00) {
            payload_send_message(FRU_RTM, PAYLOAD_MESSAGE_QUIESCE);
        } else {
            payload_send_message(FRU_RTM, PAYLOAD_MESSAGE_RTM_ENABLE);
        }
    }

    rsp->completion_code = IPMI_CC_OK;

    /* Return PICMG Identifier (0x00) */
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
}
