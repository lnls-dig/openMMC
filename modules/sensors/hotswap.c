/*
 *   hotswap.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015
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

#define HOTSWAP_POLL
//#define HOTSWAP_INT

#ifdef HOTSWAP_INT
void EINT3_IRQHandler( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t hotswap_state = 0xFF;
    static TickType_t last_time;
    TickType_t current_time = xTaskGetTickCountFromISR();

    /* Simple debouncing routine */
    /* If the last interruption happened in the last 200ms, this one is only a bounce, ignore it and wait for the next interruption */

    if (getTickDifference(current_time, last_time) < DEBOUNCE_TIME) {
        return;
    }

    /* Checks if the interrupt occurred in Port2 */
    if (!Chip_GPIOINT_IsIntPending(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT)) {
        /* Clear interrupt pending bit */
        Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 << HOT_SWAP_HANDLE_PIN));
        return;
    }

    if ((Chip_GPIOINT_GetStatusRising(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT) >> HOT_SWAP_HANDLE_PIN) & 1) {
        hotswap_state = HOT_SWAP_STATE_HANDLE_OPENED;
    } else if ((Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT) >> HOT_SWAP_HANDLE_PIN) & 1) {
        hotswap_state = HOT_SWAP_STATE_HANDLE_CLOSED;
    }

    /* Notify the Hotswap task about the handle event */
    xTaskNotifyFromISR( vTaskHotSwap_Handle, hotswap_state, eSetValueWithOverwrite, &xHigherPriorityTaskWoken );

    /* Clear interrupt pending bit */
    Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 << HOT_SWAP_HANDLE_PIN));

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
#endif


static uint8_t hotswap_get_handle_status( void )
{
    if (gpio_read_pin(HOT_SWAP_HANDLE_PORT, HOT_SWAP_HANDLE_PIN)) {
        return HOTSWAP_MODULE_HANDLE_OPEN_MASK;
    } else {
        return HOTSWAP_MODULE_HANDLE_CLOSED_MASK;
    }
}

static SDR_type_02h_t * hotswap_pSDR;
static sensor_t * hotswap_sensor;

void hotswap_init( void )
{
#ifdef HOTSWAP_INT
    /* Enable Clock for GPIO Block */
    Chip_GPIOINT_Init(LPC_GPIOINT);

    /* Enable Rising and Falling edge interruption on Hot Swap pin */
    Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 << HOT_SWAP_HANDLE_PIN));
    Chip_GPIOINT_SetIntRising(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 << HOT_SWAP_HANDLE_PIN));

    /* Configure the IRQ */
    NVIC_SetPriority( EINT3_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
    NVIC_EnableIRQ( EINT3_IRQn );
#endif
    /* Create Hot Swap task */
    xTaskCreate( vTaskHotSwap, "Hot Swap", 200, (void *) NULL, tskHOTSWAP_PRIORITY, &vTaskHotSwap_Handle);

    for ( uint8_t i = 0; i < NUM_SDR; i++ ) {

        /* Check if the handle pointer is not NULL */
        if (sensor_array[i].task_handle == NULL) {
            continue;
        }

        /* Check if this task should update the selected SDR */
        if ( *(sensor_array[i].task_handle) != vTaskHotSwap_Handle ) {
            continue;
        }

	hotswap_sensor = &sensor_array[i];
        hotswap_pSDR = (SDR_type_02h_t *) sensor_array[i].sdr;

        hotswap_sensor->readout_value = hotswap_get_handle_status();
    }
}

void vTaskHotSwap( void *Parameters )
{
    ipmi_msg pmsg;
    uint8_t data_len = 0;
    uint8_t evt_msg;

#ifdef HOTSWAP_INT
    uint8_t new_flag;
    uint8_t init_state;

    /* Enable first event */
    if ( gpio_read_pin(HOT_SWAP_HANDLE_PORT, HOT_SWAP_HANDLE_PIN) == 0 ) {
        init_state = HOT_SWAP_STATE_HANDLE_CLOSED;
        LED_update( LED_BLUE, &LED_Off_Activity );
    } else {
        init_state = HOT_SWAP_STATE_HANDLE_OPENED;
        LED_update( LED_BLUE, &LED_On_Activity );
    }

    xTaskNotify(xTaskGetCurrentTaskHandle(), init_state, eSetValueWithOverwrite);
#endif

#ifdef HOTSWAP_POLL
    /* Init old_state with a different value, so that the uC always send its state on startup */
    static uint8_t old_state = 0xFF;
    static uint8_t new_state;
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

#endif

    for ( ;; ) {
#ifdef HOTSWAP_INT
        new_flag = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        evt_msg = new_flag >> 1;

        if ( ipmi_event_send(hotswap_sensor, ASSERTION_EVENT, &evt_msg, sizeof(evt_msg)) == ipmb_error_success) {
            /* Update the SDR */
            hotswap_sensor->readout_value = (hotswap_sensor->readout_value & 0xFC) | new_flag;
        } else {
            /* If the message fails to be sent, unblock itself to try again */
            xTaskNotifyGive(xTaskGetCurrentTaskHandle());
        }

#endif
#ifdef HOTSWAP_POLL
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        new_state = hotswap_get_handle_status();

        if( new_state == old_state ) {
            continue;
        }

        evt_msg = new_state >> 1;

        if ( ipmi_event_send(hotswap_sensor, ASSERTION_EVENT, &evt_msg, sizeof(evt_msg)) == ipmb_error_success) {
            /* Update the SDR */
            hotswap_sensor->readout_value = new_state;
            old_state = new_state;
        }
#endif
    }
}
