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

volatile bool rtm_present = false;
volatile uint8_t rtm_power_level = 0;
extern EventGroupHandle_t rtm_payload_evt;

void RTM_Manage( void * Parameters )
{
    uint8_t ps_old_state = 0xFF;
    uint8_t ps_new_state;
    bool rtm_compatible;
    extern sensor_t * hotswap_rtm_sensor;

    EventBits_t current_evt;

    /* A local copy of rtm_power_level to check if it's changed status */
    uint8_t rtm_pwr_lvl_change = rtm_power_level;

    /* Start with RTM payload disabled */
    rtm_disable_payload_power();

#ifdef BENCH_TEST
    rtm_power_level = 0x01;
    rtm_pwr_lvl_change = 0x00;
#endif

    for ( ;; ) {
        vTaskDelay(100);

        rtm_check_presence( &ps_new_state );

        if ( ps_new_state ^ ps_old_state ) {
            if ( ps_new_state == HOTSWAP_STATE_URTM_PRSENT ) {

                printf("RTM Board detected!\n");

                rtm_present = true;

                /* Create/Read the RTM FRU info before sending the hotswap event */
                fru_init(FRU_RTM);

                /* RTM Present event */
                hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_PRESENT_MASK );
                hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_ABSENT_MASK );
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_PRSENT );

                /* Check the Zone3 compatibility records */
                rtm_compatible = rtm_compatibility_check();
                if ( rtm_compatible ) {
                    printf("RTM Board is compatible! Initializing...\n");
                    /* Send RTM Compatible message */
                    hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_COMPATIBLE );
                    hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_COMPATIBLE_MASK );

                    rtm_hardware_init();
                } else {
                    printf("RTM Board is not compatible.\n");
                    /* Send RTM Incompatible message */
                    hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_INCOMPATIBLE );
                    hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_COMPATIBLE_MASK );
                }

                /* Activate RTM sensors in the SDR table */
                //sdr_activate_sensors(); /* Not implemented yet */

            } else if ( ps_new_state == HOTSWAP_STATE_URTM_ABSENT ) {
                //sdr_disable_sensors(); /* Not implemented yet */

                printf("RTM Board disconnected!\n");

                rtm_present = false;

                hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_ABSENT_MASK );
                hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_URTM_PRESENT_MASK );
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_ABSENT );
            }
            ps_old_state = ps_new_state;
        }

        if ( rtm_pwr_lvl_change ^ rtm_power_level ) {
            rtm_pwr_lvl_change = rtm_power_level;

            if ( rtm_power_level == 0x01 ) {
                hotswap_clear_mask_bit( HOTSWAP_RTM, HOTSWAP_QUIESCED_MASK );

                printf("Enabling RTM Payload power...\n");
                rtm_enable_payload_power();
            } else {
                printf("Disabling RTM Payload power...\n");
                rtm_disable_payload_power();
            }
        }

        current_evt = xEventGroupGetBits( rtm_payload_evt );

        if ( current_evt & PAYLOAD_MESSAGE_QUIESCED ) {
            if ( rtm_quiesce() ) {
                /* Quiesced event */
                printf("RTM Quiesced successfuly!\n");
                hotswap_set_mask_bit( HOTSWAP_RTM, HOTSWAP_QUIESCED_MASK );
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_QUIESCED );
                xEventGroupClearBits( rtm_payload_evt, PAYLOAD_MESSAGE_QUIESCED );
            }
        }
    }
}

void rtm_manage_init( void )
{
    rtm_power_level = 0;

    xTaskCreate( RTM_Manage, "RTM Manage", 100, (void *) NULL, tskRTM_MANAGE_PRIORITY, (TaskHandle_t *) NULL );
}

/* Set Power Level Request handler */

IPMI_HANDLER(ipmi_picmg_set_power_level, NETFN_GRPEXT, IPMI_PICMG_CMD_SET_POWER_LEVEL, ipmi_msg *req, ipmi_msg *rsp )
{
    int len = rsp->data_len = 0;
    uint8_t fru_id = req->data[1];

    if ( fru_id == FRU_RTM ) {
        rtm_power_level = req->data[2];
    }

    rsp->completion_code = IPMI_CC_OK;

    /* Return PICMG Identifier (0x00) */
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
}
