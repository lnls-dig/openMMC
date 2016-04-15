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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "port.h"
#include "sdr.h"
#include "pin_mapping.h"
#include "hotswap.h"
#include "task_priorities.h"
#include "ipmi.h"
#include "led.h"
#include "utils.h"
#include "rtm.h"

static uint8_t hotswap_get_handle_status( void )
{
    if (gpio_read_pin(HOT_SWAP_HANDLE_PORT, HOT_SWAP_HANDLE_PIN)) {
        return HOTSWAP_MODULE_HANDLE_OPEN_MASK;
    } else {
        return HOTSWAP_MODULE_HANDLE_CLOSED_MASK;
    }
}

SDR_type_02h_t * hotswap_amc_pSDR;
sensor_t * hotswap_amc_sensor;

SDR_type_02h_t * hotswap_rtm_pSDR;
sensor_t * hotswap_rtm_sensor;

void hotswap_init( void )
{
    /* Create Hot Swap task */
    xTaskCreate( vTaskHotSwap, "Hot Swap", 150, (void *) NULL, tskHOTSWAP_PRIORITY, &vTaskHotSwap_Handle);

    SDR_type_02h_t * hotswap_pSDR;
    sensor_t * hotswap_sensor;

    /* Iterate through the SDR Table to find all the Hotswap entries */
    for ( hotswap_sensor = sdr_head; hotswap_sensor != NULL; hotswap_sensor = hotswap_sensor->next) {

	if ( hotswap_sensor->task_handle == NULL ) {
	    continue;
	}

	/* Check if this task should update the selected SDR */
	if ( *(hotswap_sensor->task_handle) != vTaskHotSwap_Handle ) {
	    continue;
	}

        hotswap_pSDR = (SDR_type_02h_t *) hotswap_sensor->sdr;

	if ( hotswap_pSDR->entityID == 0xC1 ) {
	    hotswap_amc_sensor = hotswap_sensor;
	    hotswap_amc_pSDR = hotswap_pSDR;
	} else if ( hotswap_pSDR->entityID == 0xC0 ) {
#ifdef MODULE_RTM
	    hotswap_rtm_sensor = hotswap_sensor;
	    hotswap_rtm_pSDR = hotswap_pSDR;
#endif
	}

    }
}

void vTaskHotSwap( void *Parameters )
{
    /* Init old_state with a different value, so that the uC always send its state on startup */
    static uint8_t old_state_amc = 0xFF;
    static uint8_t new_state_amc;
    static uint8_t old_state_rtm = 0xFF;
    static uint8_t new_state_rtm;

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 50;

    /* Override Blue LED state so that if the handle is closed when the MMC is starting, the LED remains in the correct state */
    if ( gpio_read_pin(HOT_SWAP_HANDLE_PORT, HOT_SWAP_HANDLE_PIN) == 0 ) {
        LED_update( LED_BLUE, &LED_Off_Activity );
    } else {
        LED_update( LED_BLUE, &LED_On_Activity );
    }

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for ( ;; ) {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        new_state_amc = hotswap_get_handle_status();

        if ( new_state_amc ^ old_state_amc ) {
            if ( hotswap_send_event( hotswap_amc_sensor, new_state_amc ) == ipmb_error_success ) {
                hotswap_set_mask_bit( HOTSWAP_AMC, new_state_amc );
                old_state_amc = new_state_amc;
            }
        }

#ifdef MODULE_RTM
        new_state_rtm = rtm_get_hotswap_handle_status();

        if ( new_state_rtm ^ old_state_rtm ) {
            if ( hotswap_send_event( hotswap_rtm_sensor, new_state_rtm ) == ipmb_error_success ) {
                hotswap_set_mask_bit( HOTSWAP_RTM, new_state_rtm );
                old_state_rtm = new_state_rtm;
            }
        }
#endif
    }
}

ipmb_error hotswap_send_event( sensor_t *sensor, uint8_t evt )
{
    uint8_t evt_msg;

    evt_msg = evt >> 1;
    return ipmi_event_send( sensor, ASSERTION_EVENT, &evt_msg, sizeof( evt_msg ) );
}

void hotswap_clear_mask_bit( uint8_t fru, uint8_t mask )
{
    if ( fru == HOTSWAP_AMC ) {
        hotswap_amc_sensor->readout_value &= ~mask;
    } else if ( fru == HOTSWAP_RTM ) {
        hotswap_rtm_sensor->readout_value &= ~mask;
    }
}

void hotswap_set_mask_bit( uint8_t fru, uint8_t mask )
{
    if ( fru == HOTSWAP_AMC ) {
        hotswap_amc_sensor->readout_value |= mask;
    } else if ( fru == HOTSWAP_RTM ) {
        hotswap_rtm_sensor->readout_value |= mask;
    }
}
