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
#include "ipmb.h"
#include "hotswap.h"
#include "sdr.h"
#include "task_priorities.h"
#include "pin_mapping.h"

void RTM_Manage( void * Parameters )
{
    uint8_t ps_old_state = 0xFF;
    uint8_t ps_new_state;

    extern sensor_t * hotswap_rtm_sensor;

    for ( ;; ) {
        vTaskDelay(100);

        ps_new_state = rtm_check_presence();

        if ( ps_new_state ^ ps_old_state ) {
            if ( ps_new_state == RTM_PS_PRESENT ) {
                /* RTM Present */
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_URTM_PRESENT_MASK );

		rtm_hardware_init();

                /* Activate RTM sensors in the SDR table */
		//sdr_activate_sensors(); /* Not implemented yet */

            } else if ( ps_new_state == RTM_PS_ABSENT ) {
                //sdr_disable_sensors(); /* Not implemented yet */
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_URTM_ABSENT_MASK );
            }
            ps_old_state = ps_new_state;
        }

        /* We should not be activating the controlling the RTM LEDs directly, wait for a command to do that */
        if ( rtm_get_hotswap_handle_status() == HOTSWAP_MODULE_HANDLE_CLOSED_MASK ) {
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

    xTaskCreate( RTM_Manage, "RTM Manage", 100, (void *) NULL, tskRTM_MANAGE_PRIORITY, (TaskHandle_t *) NULL );
}
