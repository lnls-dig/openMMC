/*
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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

/*!
 * @file led.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief Module to control all boards LEDs
 * @todo Implement color selecting feature
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* C Standard includes */
#include "stdio.h"
#include "string.h"

/* Project includes */
#include "chip.h"
#include "port.h"
#include "led.h"
#include "pin_mapping.h"
#include "task_priorities.h"

const LED_activity_desc_t LED_Off_Activity = {LED_ACTV_OFF, LED_OFF_STATE, 0, 0};
const LED_activity_desc_t LED_On_Activity = {LED_ACTV_ON, LED_ON_STATE, 0, 0};
const LED_activity_desc_t LED_Short_Blink_Activity = {LED_ACTV_BLINK, LED_OFF_STATE, 9, 1};
const LED_activity_desc_t LED_Long_Blink_Activity = {LED_ACTV_BLINK, LED_OFF_STATE, 1, 9};
const LED_activity_desc_t LED_2Hz_Blink_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 5, 5};
const LED_activity_desc_t LED_3sec_Lamp_Test_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 30, 0};

LED_state_rec_t LEDstate[LED_CNT] = {
    {
        .local_ptr = &LED_On_Activity,
        .counter = 0,
        .Color = LEDCOLOR_BLUE,
        .pin_cfg = {
            .pin = LEDBLUE_PIN,
            .port = LEDBLUE_PORT,
            .func = LED_PIN_FUNC
        }
    },

    {
        .local_ptr = &LED_2Hz_Blink_Activity,
        .Color = LEDCOLOR_GREEN,
        .counter = 0,
        .pin_cfg = {
                    .pin = LEDGREEN_PIN,
                    .port = LEDGREEN_PORT,
                    .func = LED_PIN_FUNC
                }

    },

    {
        .local_ptr = &LED_Off_Activity,
        .Color = LEDCOLOR_RED,
        .counter = 0,
        .pin_cfg = {
                    .pin = LEDRED_PIN,
                    .port = LEDRED_PORT,
                    .func = LED_PIN_FUNC
                }
    }
};

/* 1 LED cycle is 10 periods of 100ms */
#define LED_CYCLE_COUNTER 10

QueueHandle_t led_update_queue;

void LEDTask( void * Parameters )
{
    uint8_t cycle = 0;
    LED_activity_desc_t new_cfg;
    LED_state_rec_t* pLED;
    uint8_t led_id;
    /* Task will run every 100ms */
    TickType_t xFrequency = 100 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        for (led_id = 0; led_id < LED_CNT; led_id++) {
            pLED = &LEDstate[led_id];
            /* Update the led configuration only after perfoming a full cycle on each action */
            if (cycle == 0) {
                if (xQueueReceive( pLED->queue, &new_cfg, 0 ) == pdTRUE) {
                    /* Save the last config */
		    memcpy(&(pLED->last_cfg), &(pLED->cur_cfg), sizeof(LED_activity_desc_t));

                    /* Update the config struct */
		    memcpy(&(pLED->cur_cfg), &new_cfg, sizeof(LED_activity_desc_t));
		    LED_set_state(pLED->pin_cfg, pLED->cur_cfg.initstate);
                    pLED->counter = pLED->cur_cfg.delay_init;
                }
            }

            switch (pLED->cur_cfg.action) {
            case LED_ACTV_ON:
                LED_on(pLED->pin_cfg);
                break;

            case LED_ACTV_OFF:
                LED_off(pLED->pin_cfg);
                break;

            case LED_ACTV_BLINK:
                if (pLED->counter == 0) {
                    if (pLED->cur_cfg.initstate == LED_get_state(pLED->pin_cfg)) {
                        /* LED is in initial state */
                        pLED->counter = pLED->cur_cfg.delay_tog;
                    } else {
                        /* LED is in toggled state */
                        pLED->counter = pLED->cur_cfg.delay_init;
                    }

                    LED_toggle(pLED->pin_cfg);

                    if ( pLED->counter == 0 ) {
                        /* Loaded a zero prescale value--means the activity descriptor is a single-shot descriptor
                         * that has expired--like a lamp test or a pulse.
                         * Revert the LED to the last know state and stay in toggled state until next cycle
                         */
                        pLED->counter = cycle;
			memcpy(&(pLED->cur_cfg), &(pLED->last_cfg), sizeof(LED_activity_desc_t));
			//pLED->cur_cfg = pLED->last_cfg;
                    }
                } else {
                    (pLED->counter)--;
                }
                break;
            }
        }
        if (cycle == 0) {
            /* Reload the cycle counter */
            cycle = LED_CYCLE_COUNTER;
        } else {
            cycle--;
        }
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        //vTaskDelay(90);
    }

}

void LED_init(void)
{
    LED_state_rec_t* pLED;
    for (int i = 0; i<LED_CNT; i++){
	pLED = &LEDstate[i];
	pLED->queue = xQueueCreate( 2, sizeof(LED_activity_desc_t));
	xQueueSend( pLED->queue, pLED->local_ptr, 0 );
    }

    xTaskCreate( LEDTask, (const char *) "LED Task", configMINIMAL_STACK_SIZE, (void * ) NULL, tskLED_PRIORITY, ( TaskHandle_t * ) NULL);
    /*! @todo Handle task creation error */
}

led_error LED_update( uint8_t led_num, const LED_activity_desc_t * pLEDact )
{
    volatile LED_state_rec_t* pLED;
    uint8_t all_leds;
    LED_activity_desc_t new_cfg;

    if ((led_num >= LED_CNT) && (led_num != 0xFF)) {
        /* Bad argument, we don't control the specified LED */
        /* REQ 3.225 and REQ 3.228 */
        return led_invalid_argument;
    }

    /* If the led id is 0xFF, we have to program all available LEDs (lamp test for example) */
    if (led_num == 0xFF) {
        all_leds = LED_CNT;
    } else {
        all_leds = 1;
    }

    for (int i = 0; i < all_leds; i++) {

        pLED = &LEDstate[led_num];

        if (pLEDact == NULL) {
            /* Update the local control pointer/prescaler */
	    new_cfg = *(pLED->local_ptr);
        } else {
            new_cfg = *(pLEDact);
        }
	/* Send the new config to the LED Task */
        if (xQueueSend(pLED->queue, &new_cfg, 0) != pdTRUE) {
	    /* TODO: Handle error */
	}
    }
    return led_success;
}
