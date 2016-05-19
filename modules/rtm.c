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

/* Project includes */
#include "rtm.h"
#include "port.h"
#include "rtm_user.h"
#include "ipmi.h"
#include "hotswap.h"
#include "sdr.h"
#include "task_priorities.h"
#include "pin_mapping.h"
#include "fru.h"

volatile uint8_t rtm_power_level = 0;

void RTM_Manage( void * Parameters )
{
    uint8_t ps_old_state = 0xFF;
    uint8_t ps_new_state;
    Bool rtm_compatible;
    extern sensor_t * hotswap_rtm_sensor;

    for ( ;; ) {
        vTaskDelay(100);

        rtm_check_presence( &ps_new_state );

        if ( ps_new_state ^ ps_old_state ) {
            if ( ps_new_state == HOTSWAP_STATE_URTM_PRSENT ) {

                /* RTM Present */
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_PRSENT );

                /* Check the Zone3 compatibility records */
                rtm_compatible = rtm_compatibility_check();
                if ( rtm_compatible ) {
                    /* Send RTM Compatible message */
                    hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_COMPATIBLE );
                    rtm_hardware_init();
                } else {
                    /* Send RTM Incompatible message */
                    hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_INCOMPATIBLE );
                }

                /* Activate RTM sensors in the SDR table */
                //sdr_activate_sensors(); /* Not implemented yet */

            } else if ( ps_new_state == HOTSWAP_STATE_URTM_ABSENT ) {
                //sdr_disable_sensors(); /* Not implemented yet */
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_STATE_URTM_ABSENT );
            }
            ps_old_state = ps_new_state;
        }

        if ( rtm_power_level == 0x01 ) {
            rtm_enable_payload_power();
        } else {
            rtm_disable_payload_power();
        }
    }
}

void rtm_manage_init( void )
{
    gpio_set_pin_dir( GPIO_EN_RTM_PWR_PORT, GPIO_EN_RTM_PWR_PIN, OUTPUT );
    gpio_set_pin_dir( GPIO_RTM_PS_PORT, GPIO_RTM_PS_PIN, INPUT );
    gpio_set_pin_dir( GPIO_EN_RTM_I2C_PORT, GPIO_EN_RTM_I2C_PIN, INPUT );

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
