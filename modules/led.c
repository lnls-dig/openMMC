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
const LED_activity_desc_t LED_Short_Blink_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 100, 900};
const LED_activity_desc_t LED_Long_Blink_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 900, 100};
const LED_activity_desc_t LED_2Hz_Blink_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 500, 500};
const LED_activity_desc_t LED_3sec_Lamp_Test_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 3000, 0};

static SemaphoreHandle_t LEDMutex[LED_CNT];

volatile LED_state_rec_t LEDstate[LED_CNT] = {
    {
        .LEDmode = Local_Control,
        .pLocalDesc = &LED_On_Activity,
        .pOvrideDesc = NULL,
        .LocalPscale = 0,
        .OvridePscale = 0,
        .Color = LEDCOLOR_BLUE,
        .pin_cfg = {
            .pin = LEDBLUE_PIN,
            .port = LEDBLUE_PORT,
            .func = LED_PIN_FUNC
        }
    },

    {
        .LEDmode = Local_Control,
        .pLocalDesc = &LED_Off_Activity,
        .pOvrideDesc = NULL,
        .LocalPscale = 0,
        .OvridePscale = 0,
        .Color = LEDCOLOR_RED,
        .pin_cfg = {
            .pin = LEDRED_PIN,
            .port = LEDRED_PORT,
            .func = LED_PIN_FUNC
        }
    },

    {
        .LEDmode = Local_Control,
        .pLocalDesc = &LED_2Hz_Blink_Activity,
        .pOvrideDesc = NULL,
        .LocalPscale = 0,
        .OvridePscale = 0,
        .Color = LEDCOLOR_GREEN,
        .pin_cfg = {
            .pin = LEDGREEN_PIN,
            .port = LEDGREEN_PORT,
            .func = LED_PIN_FUNC
        }
    }
};

void LEDTask ( void * pvParameters )
{
    /* This task will run every 10ms */
    const TickType_t xFrequency = LED_UPDATE_RATE / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;

    volatile int led_id;
    volatile uint8_t* pCurPscale;
    const volatile LED_activity_desc_t* pCurDesc;
    volatile LED_state_rec_t* pLED;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        for (led_id = 0; led_id < LED_CNT; led_id++ ){
            /* Try to take semaphore to access LED configuration struct */
            if ( xSemaphoreTake(LEDMutex[led_id], 0 ) == pdFALSE ) {
                /* If we can't take the semaphore, there's some other task updating the LED struct.
                 * Leave the LED in its current state, we'll update it in the next cycle */
                continue;
            }
            pLED = &LEDstate[led_id];
            if (pLED->LEDmode == Local_Control) {
                /* Point to local activity params */
                pCurDesc = pLED->pLocalDesc;
                pCurPscale = &(pLED->LocalPscale);
            }
            else {
                /* Point to override activity params */
                pCurDesc = pLED->pOvrideDesc;
                pCurPscale = &(pLED->OvridePscale);
            }

            switch ( pCurDesc->action ) {
            case LED_ACTV_ON:
                LED_on(pLED->pin_cfg);
                break;

            case LED_ACTV_OFF:
                LED_off(pLED->pin_cfg);
                break;

            case LED_ACTV_BLINK:
                if ( (*pCurPscale) == 0 ) {
                    /* Prescaler reached bottom. We have to figure out what to do next */
                    if (pCurDesc->initstate == LED_get_state(pLED->pin_cfg)) {
                        /* LED is in initial state */
                        *pCurPscale = (pCurDesc->delay_tog)/LED_UPDATE_RATE;
                    }
                    else {
                        /* LED is in toggled state */
                        *pCurPscale = (pCurDesc->delay_init)/LED_UPDATE_RATE;
                    }
                    /* Invert led state */
                    LED_toggle(pLED->pin_cfg);

                    if ( (*pCurPscale) == 0 ) {
                        /* Loaded a zero prescale value--means the activity descriptor is a single-shot descriptor
                         * that has expired--like a lamp test or a pulse.
                         * if LED is in an override state, revert to the local control state.
                         * if LED is already in local control, revert to a static state which is opposite of the
                         * single-shot state */
                        if (pLED->LEDmode == Local_Control) {
                            if (pLED->pLocalDesc->initstate == LED_ON_STATE) {
                                pLED->pOvrideDesc = &LED_Off_Activity;
                                LED_off(pLED->pin_cfg);
                            } else {
                                pLED->pOvrideDesc = &LED_On_Activity;
                                LED_on(pLED->pin_cfg);
                            }
                        } else {
                            pLED->LEDmode = Local_Control;
                            pCurDesc = pLED->pLocalDesc;
                            pLED->LocalPscale = pCurDesc->delay_init;
                            LED_set_state(pLED->pin_cfg, pCurDesc->initstate);
                        }
                    }
                } else {
                    /* Decrement the current prescaler */
                    (*pCurPscale)--;
                }

                break;

            default:
                break;
            }
            /* Give back the mutex before leaving */
            xSemaphoreGive( LEDMutex[led_id]);
        }
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

void LED_init (void)
{
    for (int i = 0; i<LED_CNT; i++){
        LEDMutex[i] = xSemaphoreCreateMutex();
    }

    xTaskCreate( LEDTask, (const char *) "LED Task", configMINIMAL_STACK_SIZE*2, (void * ) NULL, tskLED_PRIORITY, ( TaskHandle_t * ) NULL);
    /*! @todo Handle task creation error */
}

led_error LED_update( uint8_t led_num, LEDmode_t newLEDmode, LED_activity_desc_t * pLEDact )
{
    volatile LED_state_rec_t* pLED;
    uint8_t all_leds;
    led_error error = led_success;

    if ((led_num >= LED_CNT) && (led_num != 0xFF)) {
        /* Bad argument, we don't control the specified LED */
        /* REQ 3.225 and REQ 3.228 */
        return led_invalid_argument;
    }

    if ((newLEDmode == Override) && (pLEDact == NULL)) {
	/* Invalid override mode */
        return led_invalid_argument;
    }

    /* If the led id is 0xFF, we have to program all available LEDs (lamp test for example) */
    if (led_num == 0xFF) {
	all_leds = LED_CNT;
    } else {
	all_leds = 1;
    }

    for (int led_id = 0; led_id < all_leds; led_id++) {

	if (xSemaphoreTake( LEDMutex[led_id], LED_UPDATE_RATE*2 ) == pdFALSE) {
	    /* We can't return right here, because 1 led may be busy, but the others may still be configured */
            //return led_unspecified_error;
	    error = led_unspecified_error;
	}

	pLED = &LEDstate[led_id];

        if (newLEDmode == Local_Control) {
            /* Update the local control pointer/prescaler */
            if (pLEDact) {
                pLED->pLocalDesc = pLEDact;  /* We can update the local activity descriptor here, or just use the default */
            }
            pLED->LocalPscale = pLED->pLocalDesc->delay_init;
        }
        else {
            /* Update the override pointer/prescaler */
            configASSERT(pLEDact);
            pLED->pOvrideDesc = pLEDact;
            pLED->OvridePscale = pLEDact->delay_init;
        }
        pLED->LEDmode = newLEDmode;

        LED_set_state(pLED->pin_cfg, pLEDact->initstate);
        xSemaphoreGive( LEDMutex[led_id] );

	/* If all LEDs were programmed correctly, the error should still be led_success */
    }
    return error;
}
