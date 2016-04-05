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
#include "pca9554.h"
#include "sdr.h"
#include "task_priorities.h"
#include "pin_mapping.h"

void rtm_enable_payload_power( void )
{
    gpio_set_pin_state(GPIO_EN_RTM_PWR_PORT, GPIO_EN_RTM_PWR_PIN, 1 );
}

void rtm_disable_payload_power( void )
{
    gpio_set_pin_state(GPIO_EN_RTM_PWR_PORT, GPIO_EN_RTM_PWR_PIN, 0 );
}

uint8_t rtm_get_hotswap_handle_status( void )
{
    if ( pca9554_read_pin( RTM_GPIO_HOTSWAP_HANDLE ) ) {
        return HOTSWAP_MODULE_HANDLE_OPEN_MASK;
    } else {
        return HOTSWAP_MODULE_HANDLE_CLOSED_MASK;
    }
}

void rtm_enable_i2c( void )
{
    /* Enable I2C communication with RTM */
    gpio_set_pin_dir( GPIO_RTM_PS_PORT, GPIO_RTM_PS_PIN, OUTPUT );
    gpio_set_pin_dir( GPIO_EN_RTM_I2C_PORT, GPIO_EN_RTM_I2C_PIN, OUTPUT );
    gpio_set_pin_state( GPIO_EN_RTM_I2C_PORT, GPIO_EN_RTM_I2C_PIN, HIGH );
}

void rtm_disable_i2c( void )
{
    gpio_set_pin_dir( GPIO_RTM_PS_PORT, GPIO_RTM_PS_PIN, INPUT );
    gpio_set_pin_dir( GPIO_EN_RTM_I2C_PORT, GPIO_EN_RTM_I2C_PIN, INPUT );
}

void RTM_Manage( void * Parameters )
{
    uint8_t ps_old_state = 0xFF;
    uint8_t ps_new_state;

    extern sensor_t * hotswap_rtm_sensor;

    for ( ;; ) {
        vTaskDelay(200);

        rtm_disable_i2c();
        ps_new_state = gpio_read_pin( GPIO_RTM_PS_PORT, GPIO_RTM_PS_PIN );

        if ( ps_new_state ^ ps_old_state ) {
            if (ps_new_state == RTM_PS_PRESENT) {
                /* RTM Present */
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_URTM_PRESENT_MASK );

                rtm_enable_i2c();
                pca9554_set_port_dir( 0x1F );
                /* Turn on Blue LED and off Red and Green */
                pca9554_write_pin( RTM_GPIO_LED_BLUE, 0 );
                pca9554_write_pin( RTM_GPIO_LED_RED, 1 );
                pca9554_write_pin( RTM_GPIO_LED_GREEN, 1 );

                /* Include RTM sensors in the SDR table */
                rtm_insert_sdr_entries();

            } else if ( ps_new_state == RTM_PS_ABSENT ) {
                //rtm_remove_sdr_entries();   //Not implemented yet
                hotswap_send_event( hotswap_rtm_sensor, HOTSWAP_URTM_ABSENT_MASK );
            }
            ps_old_state = ps_new_state;
        }

        /* Remove this, we should not be activating the RTM directly, wait for a command to do that */
        rtm_enable_i2c();
        if ( rtm_get_hotswap_handle_status() == HOTSWAP_MODULE_HANDLE_CLOSED_MASK ) {
            rtm_enable_payload_power();
            pca9554_write_pin( RTM_GPIO_LED_BLUE, 1 );
            pca9554_write_pin( RTM_GPIO_LED_GREEN, 0 );
        } else {
            rtm_disable_payload_power();
            pca9554_write_pin( RTM_GPIO_LED_BLUE, 0 );
            pca9554_write_pin( RTM_GPIO_LED_GREEN, 1 );
        }
    }
}

void rtm_manage_init( void )
{
    gpio_set_pin_dir( GPIO_EN_RTM_PWR_PORT, GPIO_EN_RTM_PWR_PIN, OUTPUT );
    gpio_set_pin_dir( GPIO_RTM_PS_PORT, GPIO_RTM_PS_PIN, INPUT );
    gpio_set_pin_dir( GPIO_EN_RTM_I2C_PORT, GPIO_EN_RTM_I2C_PIN, INPUT );

    xTaskCreate( RTM_Manage, "RTM Manage", 80, (void *) NULL, tskRTM_MANAGE_PRIORITY, (TaskHandle_t *) NULL );
}
