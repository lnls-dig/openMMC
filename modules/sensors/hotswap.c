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


extern const sensor_t const sensor_array[NUM_SDR];

void EINT3_IRQHandler( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t hotswap_state = 0xFF;
    static TickType_t last_time;
    TickType_t current_time = xTaskGetTickCountFromISR();

    /* Simple debouncing routine */
    /* If the last interruption happened in the last 200ms, this one is only a bounce, ignore it and wait for the next interruption */

    if (getTickDifference(current_time, last_time) < DEBOUNCE_TIME){
        /*! @todo Clear all the active interrupts tied to EXT3, not only the hot swap */
        Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 << HOT_SWAP_HANDLE_PIN));
        return;
    }

    /* Checks if the interrupt occurred in Port2 */
    if (Chip_GPIOINT_IsIntPending(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT)) {
        /*! @bug If any other GPIO interruption is enable in Port 2, it'll trigger the hotswap messaging */
        if ( gpio_read_pin(HOT_SWAP_HANDLE_PORT, HOT_SWAP_HANDLE_PIN) == 0 ) {
            hotswap_state = HOT_SWAP_STATE_HANDLE_CLOSED;
        } else {
            hotswap_state = HOT_SWAP_STATE_HANDLE_OPENED;
        }

        xTaskNotifyFromISR( vTaskHotSwap_Handle, hotswap_state, eSetValueWithOverwrite, &xHigherPriorityTaskWoken );

        Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 << HOT_SWAP_HANDLE_PIN));
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void hotswap_init( void )
{
    /* Enable Clock for GPIO Block */
    Chip_GPIOINT_Init(LPC_GPIOINT);

    /* Enable Rising and Falling edge interruption on Hot Swap pin */
    Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 << HOT_SWAP_HANDLE_PIN));
    Chip_GPIOINT_SetIntRising(LPC_GPIOINT, HOT_SWAP_HANDLE_PORT, (1 <<HOT_SWAP_HANDLE_PIN));

    /* Configure the IRQ */
    NVIC_SetPriority( EINT3_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
    NVIC_EnableIRQ( EINT3_IRQn );

    /* Create Hot Swap task */
    xTaskCreate( vTaskHotSwap, "Hot Swap", configMINIMAL_STACK_SIZE*2, (void *) NULL, tskHOTSWAP_PRIORITY, &vTaskHotSwap_Handle);
}

void vTaskHotSwap( void *Parameters )
{
    ipmi_msg pmsg;
    uint8_t data_len = 0;
    SDR_type_01h_t *pSDR = NULL;
    sensor_data_entry_t * pDATA;
    uint8_t new_flag;
    uint8_t i;

    for ( ;; ) {
        new_flag = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        /* Find hotswap sensor in SDR table */
        for ( i = 0; i < NUM_SDR; i++ ) {

            /* Check if the handle pointer is not NULL */
            if (sensor_array[i].task_handle == NULL) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if ( *(sensor_array[i].task_handle) != xTaskGetCurrentTaskHandle() ) {
                continue;
            }

            pSDR = (SDR_type_01h_t *) sensor_array[i].sdr;
            pDATA = sensor_array[i].data;

            data_len = 0;
            pmsg.dest_LUN = 0;
            pmsg.netfn = NETFN_SE;
            pmsg.cmd = IPMI_PLATFORM_EVENT_CMD;
            pmsg.data[data_len++] = 0x04;
            pmsg.data[data_len++] = 0xf2;
            pmsg.data[data_len++] = pSDR->sensornum;
            pmsg.data[data_len++] = 0x6f;
            pmsg.data[data_len++] = (new_flag >> 1); // hot swap state
            pmsg.data_len = data_len;

            if (ipmb_send_request( &pmsg ) == ipmb_error_success) {
                /* Update the SDR */
                pDATA->comparator_status = (pDATA->comparator_status & 0xFC) | new_flag;
            } else {
                xTaskNotifyGive(xTaskGetCurrentTaskHandle());
            }
            break;
        }
    }
}
