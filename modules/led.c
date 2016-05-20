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
#include "fru.h"

//extern LED_state_rec_t LEDstate[LED_CNT];

const LED_activity_desc_t LED_Off_Activity = {LED_ACTV_OFF, LED_OFF_STATE, 0, 0};
const LED_activity_desc_t LED_On_Activity = {LED_ACTV_ON, LED_ON_STATE, 0, 0};
const LED_activity_desc_t LED_Short_Blink_Activity = {LED_ACTV_BLINK, LED_OFF_STATE, 9, 1};
const LED_activity_desc_t LED_Long_Blink_Activity = {LED_ACTV_BLINK, LED_OFF_STATE, 1, 9};
const LED_activity_desc_t LED_2Hz_Blink_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 5, 5};
const LED_activity_desc_t LED_3sec_Lamp_Test_Activity = {LED_ACTV_BLINK, LED_ON_STATE, 30, 0};

LED_state_rec_t LEDstate[LED_CNT] = {
    [LED_BLUE] = {
        .local_ptr = &LED_On_Activity,
        .counter = 0,
        .Color = LEDCOLOR_BLUE,
        .pin_cfg = {
            .pin = LEDBLUE_PIN,
            .port = LEDBLUE_PORT,
            .func = LED_PIN_FUNC
        }
    },

    [LED_GREEN] = {
        .local_ptr = &LED_2Hz_Blink_Activity,
        .Color = LEDCOLOR_GREEN,
        .counter = 0,
        .pin_cfg = {
            .pin = LEDGREEN_PIN,
            .port = LEDGREEN_PORT,
            .func = LED_PIN_FUNC
        }

    },

    [LED_RED] = {
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
    /* Task will run every 50ms */
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
    }

}

void LED_init(void)
{
    /* Init LED Pin */
    gpio_init();

    /* Set pins as output */
    gpio_set_pin_dir( LEDBLUE_PORT, LEDBLUE_PIN, OUTPUT);
    gpio_set_pin_dir( LEDGREEN_PORT, LEDGREEN_PIN, OUTPUT);
    gpio_set_pin_dir( LEDRED_PORT, LEDRED_PIN, OUTPUT);

    LED_state_rec_t* pLED;
    for (int i = 0; i<LED_CNT; i++){
        pLED = &LEDstate[i];
        pLED->queue = xQueueCreate( 2, sizeof(LED_activity_desc_t));
        xQueueSend( pLED->queue, pLED->local_ptr, 0 );
    }

    xTaskCreate( LEDTask, (const char *) "LED Task", 80, (void * ) NULL, tskLED_PRIORITY, ( TaskHandle_t * ) NULL);
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

/*!
 * @brief Handler for "Set FRU LED State"" request. Check IPMI 2.0
 * table 3-31 for more information.
 *
 * @param[in] req Pointer to request struct to be handled and answered. Contains
 * which LED should be set, how it should be set and other commands.
 *
 * @param[out] rsp Pointer to response struct to be modified with the message
 *
 * @return void
 */

IPMI_HANDLER(ipmi_picmg_set_fru_led_state, NETFN_GRPEXT, IPMI_PICMG_CMD_SET_FRU_LED_STATE, ipmi_msg *req, ipmi_msg *rsp )
{
    uint8_t fru_id = req->data[1];

    led_error error;
    const LED_activity_desc_t * pLEDact;
    LED_activity_desc_t LEDact;
    pLEDact = &LEDact;
    /* We use this pointer assignment, so we can also set it to NULL if we need */

    if ( fru_id == FRU_RTM )  {
	/* RTM LEDs control are not implemented yet, just return an OK */
	rsp->completion_code = IPMI_CC_OK;
	rsp->data_len = 0;
	rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
	return;
    }

    switch (req->data[3]) {
    case 0x00:
        /* OFF override */
        pLEDact = &LED_Off_Activity;
        break;
    case 0xFF:
        /* ON override */
        pLEDact = &LED_On_Activity;
        break;
    case 0xFB:
        /* Lamp Test */
        /*! @todo Put the lamp test as a higher priority action, not a type of override */
        LEDact.action = LED_ACTV_BLINK;
        LEDact.initstate = LED_ON_STATE;
        /* On duration in 100ms units */
        LEDact.delay_init = req->data[4] * 100;
        /* Set the toggle delay to 0, so we know its a "single-shot" descriptor, so the LED module should revert to its override/local_control state later */
        LEDact.delay_tog = 0;
        break;
    case 0xFC:
        /* Local state */
        pLEDact = NULL;
        break;
    case 0xFD:
    case 0xFE:
        /* Reserved */
        break;
    default:
        /* Blink Override */
        LEDact.action = LED_ACTV_BLINK;
        LEDact.initstate = LED_ON_STATE;
        /* On duration in 10ms units */
        LEDact.delay_init = req->data[4] / 10;
        /* Off duration in 10ms units*/
        LEDact.delay_tog = req->data[3] / 10;
        break;
    }

    error = LED_update( req->data[2], pLEDact );

    switch (error) {
    case led_success:
        rsp->completion_code = IPMI_CC_OK;
        break;
    case led_invalid_argument:
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
        break;
    case led_unspecified_error:
        rsp->completion_code = IPMI_CC_UNSPECIFIED_ERROR;
        break;
    }
    rsp->data_len = 0;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}

IPMI_HANDLER(ipmi_picmg_get_fru_led_properties, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_FRU_LED_PROPERTIES, ipmi_msg *req, ipmi_msg *rsp )
{
    uint8_t len = rsp->data_len = 0;

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    /* FRU can control the BLUE LED, LED 1 (RED) and LED 2 (GREEN) */
    rsp->data[len++] = 0x03;
    /* Application specific LED count */
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_picmg_get_fru_led_state, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_FRU_LED_STATE, ipmi_msg *req, ipmi_msg *rsp )
{
    uint8_t len = rsp->data_len = 0;
    uint8_t led_id = req->data[2];

    if (led_id > LED_CNT) {
	rsp->data_len = len;
	rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
	return;
    }
    rsp->data[len++] = IPMI_PICMG_GRP_EXT;

    /* LED State:
     * [7:4] Reserved,
     * [3] LED has an unmet hardware restriction,
     * [2] Lamp Test enabled
     * [2] Override state enabled
     * [2] Local control enabled */
    rsp->data[len++] = 0x01; // Reading not yet implemented

    /* Local Control LED function */
    switch (LEDstate[led_id].cur_cfg.action) {
    case LED_ACTV_OFF:
	rsp->data[len++] = 0x00;
	rsp->data[len++] = 0x00;
	break;
    case LED_ACTV_ON:
	rsp->data[len++] = 0xFF;
	rsp->data[len++] = 0x00;
	break;
    case LED_ACTV_BLINK:
	rsp->data[len++] = LEDstate[led_id].cur_cfg.delay_init;
	rsp->data[len++] = LEDstate[led_id].cur_cfg.delay_tog;
	break;
    }

    /*  Local Control Color */
    rsp->data[len++] = LEDstate[led_id].Color;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}


IPMI_HANDLER(ipmi_picmg_get_led_color_capabilities, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_LED_COLOR_CAPABILITIES, ipmi_msg *req, ipmi_msg *rsp )
{
    uint8_t len = rsp->data_len = 0;
    uint8_t led_id = req->data[2];

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;

    /* LED Color Capabilities */
    rsp->data[len++] = LEDstate[led_id].Color;
    /* Default LED Color in Local Control State */
    rsp->data[len++] = LEDstate[led_id].Color;
    /* Default LED Color in Override State */
    rsp->data[len++] = LEDstate[led_id].Color;
    /* LED Flags:
     * [7:2] Reserved,
     * [1] LED has a hardware restriction,
     * [0] LED won't work without payload power */
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}
