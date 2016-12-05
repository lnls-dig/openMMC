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

/* C Standard includes */
#include "string.h"

/* Project includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "ipmi.h"
#include "port.h"
#include "led.h"
#include "string.h"
#include "task_priorities.h"

const uint32_t amc_led_pincfg[LED_CNT] = {
    [LED_BLUE] = GPIO_LEDBLUE,
    [LED1] = GPIO_LEDRED,
    [LED2] = GPIO_LEDGREEN
};

LEDConfig_t amc_leds_config[LED_CNT] = {
    [LED_BLUE] = {
        .id = LED_BLUE,
        .color = LEDCOLOR_BLUE,
        .act_func = amc_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0,
                .t_toggle = 0,
                .init_status = LEDINIT_ON,
            }
        }
    },

    [LED1] = {
        .id = LED1,
        .color = LEDCOLOR_RED,
        .act_func = amc_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0,
                .t_toggle = 0,
                .init_status = LEDINIT_OFF,
            }
        }
    },

    [LED2] = {
        .id = LED2,
        .color = LEDCOLOR_GREEN,
        .act_func = amc_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 5,
                .t_toggle = 5,
                .init_status = LEDINIT_ON,
            }
        }
    },
};

#ifdef MODULE_RTM
LEDConfig_t rtm_leds_config[LED_CNT] = {
    [LED_BLUE] = {
        .id = LED_BLUE,
        .color = LEDCOLOR_BLUE,
        .act_func = rtm_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0xFF,
                .t_toggle = 0,
                .init_status = LEDINIT_ON,
            }
        }
    },

    [LED1] = {
        .id = LED1,
        .color = LEDCOLOR_RED,
        .act_func = rtm_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0,
                .t_toggle = 0,
                .init_status = LEDINIT_OFF,
            }
        }
    },

    [LED2] = {
        .id = LED2,
        .color = LEDCOLOR_GREEN,
        .act_func = rtm_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 5,
                .t_toggle = 5,
                .init_status = LEDINIT_ON,
            }
        }
    },
};
#endif

void LEDManage( LEDConfig_t *led_cfg );

LEDConfig_t led_config[LEDCONFIG_SIZE][LED_CNT];
QueueHandle_t led_update_queue;

void LED_init( void )
{
    /* AMC LED Pins initialization */
    gpio_init();

    led_update_queue = xQueueCreate( 3, sizeof(LEDUpdate_t) );

    memcpy( &led_config[0], &amc_leds_config, sizeof(amc_leds_config) );
#ifdef MODULE_RTM
    memcpy( &led_config[1], &rtm_leds_config, sizeof(rtm_leds_config) );
#endif

    xTaskCreate( LED_Task, (const char *) "LED Task", 150, (void * ) NULL, tskLED_PRIORITY, ( TaskHandle_t * ) NULL);
}

void LED_Task( void *Parameters )
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);

    LEDUpdate_t cfg;

    uint8_t cycle = 0;

    for ( ;; ) {
        cycle++;

        for ( int i=0; i < LEDCONFIG_SIZE; i++ ) {
            for ( int j=0; j < LED_CNT; j++ ) {
                LEDManage( &led_config[i][j] );
            }
        }

        if ( cycle == 10 ) {
            /* Check for state changes */
            cycle = 0;
            if ( xQueueReceive( led_update_queue, &cfg, 0 ) == pdTRUE ) {
                uint8_t fru,num;

                fru = cfg.fru_id;
                num = cfg.led_num;

                led_config[fru][num].mode = cfg.mode;
                led_config[fru][num].mode_cfg[cfg.mode].active = true;
                /* Only update the settings if its an override */
                if ( cfg.mode != LEDMODE_LOCAL ) {
                    led_config[fru][num].mode_cfg[cfg.mode].init_status = cfg.new_state.init_status;
                    led_config[fru][num].mode_cfg[cfg.mode].t_init = cfg.new_state.t_init;
                    led_config[fru][num].mode_cfg[cfg.mode].t_toggle = cfg.new_state.t_toggle;
                    led_config[fru][num].state = LEDSTATE_INIT;
                    /* Reset the LED internal counter */
                    led_config[fru][num].counter = 0;
                }
            }
        }
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }

}

void LEDManage( LEDConfig_t *led_cfg )
{
    uint8_t mode = led_cfg->mode;
    uint8_t status;
    uint16_t counter_cmp, counter_tog;

    /* Reload counter */
    if ( led_cfg->state == LEDSTATE_INIT ) {
        status = led_cfg->mode_cfg[mode].init_status;
        counter_cmp = led_cfg->mode_cfg[mode].t_init;
        counter_tog = led_cfg->mode_cfg[mode].t_toggle;
    } else {
        status = ~(led_cfg->mode_cfg[mode].init_status);
        counter_cmp = led_cfg->mode_cfg[mode].t_toggle;
        counter_tog = led_cfg->mode_cfg[mode].t_init;
    }

    /* Special case for lamp_test (we must return to the highest priority state */
    if ( (led_cfg->mode == LEDMODE_LAMPTEST) && (led_cfg->counter >= counter_cmp) ) {
        led_cfg->mode_cfg[LEDMODE_LAMPTEST].active = false;
        /* Reset counter */
        led_cfg->counter = 0;

        /* Revert to the previous state */
        if ( led_cfg->mode_cfg[LEDMODE_OVERRIDE].active ) {
            led_cfg->mode = LEDMODE_OVERRIDE;
        } else {
            led_cfg->mode = LEDMODE_LOCAL;
        }
    }

    if ( ( led_cfg->counter >= counter_cmp ) && ( counter_tog != 0 ) ) {
        /* General case for blinking operation - toggle LED */
        led_cfg->state = !(led_cfg->state);
        led_cfg->act_func( led_cfg->id, LEDACT_TOGGLE );
        led_cfg->counter = 0;
    } else {
        /* Stay in the current state and increment counter */
        led_cfg->act_func( led_cfg->id, status );
        led_cfg->counter++;
    }
}

void LEDUpdate( uint8_t fru, uint8_t led_num, uint8_t mode, uint8_t init_status, uint16_t t_init, uint16_t t_toggle )
{
    LEDUpdate_t new_config = {
        .fru_id = fru,
        .led_num = led_num,
        .mode = mode,
        .new_state = {
            .init_status = init_status,
            .t_init = t_init,
            .t_toggle = t_toggle
        }
    };

    if ( new_config.mode == LEDMODE_LOCAL ) {
        led_config[fru][led_num].mode_cfg[LEDMODE_OVERRIDE].active = false;
        led_config[fru][led_num].mode_cfg[LEDMODE_LAMPTEST].active = false;
    }

    if ( xQueueSend( led_update_queue, &new_config, 0 ) != pdTRUE ) {
        /* TODO: Handle error */
    }
}

/* AMC LED acting function */
void amc_led_act( uint8_t id, uint8_t action )
{
    switch( action ) {
    case LEDACT_TURN_ON:
        gpio_set_pin_low( PIN_PORT(amc_led_pincfg[id]), PIN_NUMBER(amc_led_pincfg[id]) );
        break;

    case LEDACT_TURN_OFF:
        gpio_set_pin_high( PIN_PORT(amc_led_pincfg[id]), PIN_NUMBER(amc_led_pincfg[id]) );
        break;

    case LEDACT_TOGGLE:
        gpio_pin_toggle( PIN_PORT(amc_led_pincfg[id]), PIN_NUMBER(amc_led_pincfg[id]) );
        break;

    default:
        break;
    }
}

#ifdef MODULE_RTM

#include "rtm_user.h"

void rtm_led_act( uint8_t id, uint8_t action )
{
    uint8_t curr_state;

    switch( action ) {
    case LEDACT_TURN_ON:
        rtm_ctrl_led( id, 0 );
        break;

    case LEDACT_TURN_OFF:
        rtm_ctrl_led( id, 1 );
        break;

    case LEDACT_TOGGLE:
        curr_state = rtm_read_led( id );
        rtm_ctrl_led( id, !curr_state );
        break;

    default:
        break;
    }
}
#endif


/* IPMI Request handlers */

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

IPMI_HANDLER(ipmi_picmg_set_fru_led_state, NETFN_GRPEXT, IPMI_PICMG_CMD_SET_FRU_LED_STATE, ipmi_msg *req, ipmi_msg *rsp)
{
    uint8_t fru_id = req->data[1];
    uint8_t led_num = req->data[2];
    uint8_t function = req->data[3];

    switch ( function ) {
    case 0x00:
        /* OFF override */
        LEDUpdate( fru_id, led_num, LEDMODE_OVERRIDE, LEDINIT_OFF, 0, 0 );
        break;
    case 0xFF:
        /* ON override */
        LEDUpdate( fru_id, led_num, LEDMODE_OVERRIDE, LEDINIT_ON, 0, 0 );
        break;
    case 0xFB:
        /* Lamp Test */
        LEDUpdate( fru_id, led_num, LEDMODE_LAMPTEST, LEDINIT_ON, req->data[4], 0 );
        break;
    case 0xFC:
        /* Restore to Local Control state */
        LEDUpdate( fru_id, led_num, LEDMODE_LOCAL, 0, 0, 0 );
        break;
    case 0xFD:
    case 0xFE:
        /* Reserved */
        break;
    default:
        /* Blink Override */
        LEDUpdate( fru_id, led_num, LEDMODE_OVERRIDE, LEDINIT_ON, req->data[4]/10, req->data[3]/10 );
        break;
    }

    rsp->completion_code = IPMI_CC_OK;
    rsp->data_len = 0;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}

IPMI_HANDLER(ipmi_picmg_get_fru_led_properties, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_FRU_LED_PROPERTIES, ipmi_msg *req, ipmi_msg *rsp )
{
    uint8_t len = rsp->data_len = 0;
    uint8_t fru = req->data[1];

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    /* FRU can control the BLUE LED, LED 1 (RED) and LED 2 (GREEN) */
    rsp->data[len++] = sizeof(led_config[fru])/sizeof(led_config[fru][0]);
    /* Application specific LED count */
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_picmg_get_fru_led_state, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_FRU_LED_STATE, ipmi_msg *req, ipmi_msg *rsp )
{
    uint8_t len = rsp->data_len = 0;
    uint8_t fru = req->data[1];
    uint8_t id = req->data[2];
    LEDConfig_t *cfg = &led_config[fru][id];

    if ( id > ( sizeof( led_config[fru] )/sizeof( led_config[fru][0] )-1 ) ) {
        rsp->data_len = len;
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
        return;
    }

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;

    /* LED State:
     * [7:4] Reserved,
     * [3] LED has an unmet hardware restriction,
     * [2] Lamp Test enabled
     * [1] Override state enabled
     * [0] Local control enabled */
    rsp->data[len++] = ( (cfg->mode_cfg[LEDMODE_LAMPTEST].active) << 2 ) |
        ( ( cfg->mode_cfg[LEDMODE_OVERRIDE].active ) << 1 ) |
        ( ( cfg->mode_cfg[LEDMODE_LOCAL].active ) << 0 );

    /* Local Control LED function */
    rsp->data[len++] = cfg->mode_cfg[LEDMODE_LOCAL].t_init;
    rsp->data[len++] = 0x00;
    rsp->data[len++] = cfg->color;

    if ( cfg->mode_cfg[LEDMODE_OVERRIDE].active ) {
        if ( cfg->mode_cfg[LEDMODE_OVERRIDE].init_status == LEDINIT_OFF ) {
            rsp->data[len++] = cfg->mode_cfg[LEDMODE_OVERRIDE].t_init;
            rsp->data[len++] = cfg->mode_cfg[LEDMODE_OVERRIDE].t_toggle;
        } else {
            rsp->data[len++] = cfg->mode_cfg[LEDMODE_OVERRIDE].t_toggle;
            rsp->data[len++] = cfg->mode_cfg[LEDMODE_OVERRIDE].t_init;
        }
        rsp->data[len++] = cfg->color;
    }

    if ( cfg->mode_cfg[LEDMODE_LAMPTEST].active ) {
        rsp->data[len++] = cfg->mode_cfg[LEDMODE_LAMPTEST].t_init;
    }

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_picmg_get_led_color_capabilities, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_LED_COLOR_CAPABILITIES, ipmi_msg *req, ipmi_msg *rsp )
{
    uint8_t len = rsp->data_len = 0;
    uint8_t fru = req->data[1];
    uint8_t led_id = req->data[2];

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;

    /* LED Color Capabilities */
    rsp->data[len++] = led_config[fru][led_id].color;
    /* Default LED Color in Local Control State */
    rsp->data[len++] = led_config[fru][led_id].color;
    /* Default LED Color in Override State */
    rsp->data[len++] = led_config[fru][led_id].color;
    /* LED Flags:
     * [7:2] Reserved,
     * [1] LED has a hardware restriction,
     * [0] LED won't work without payload power */
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}
