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

#ifndef LED_H_
#define LED_H_

#include "ipmi.h"

/* LEDs Pin and port definitions */
#define LED_OFF_STATE		(1)
#define LED_ON_STATE		(0)

/* PICMG-defined color codes for set/get LED state commands */
#define LEDCOLOR_BLUE		(1)
#define LEDCOLOR_RED		(2)
#define LEDCOLOR_GREEN		(3)
#define LEDCOLOR_AMBER		(4)
#define LEDCOLOR_ORANGE		(5)
#define LEDCOLOR_WHITE		(6)
#define LEDCOLOR_NOCHANGE	(0xE)
#define LEDCOLOR_DEFAULT	(0xF)

typedef enum {
    LED_BLUE,
    LED_GREEN,
    LED_RED,
    LED_CNT
} LED_id;

typedef struct {
    uint8_t port;
    uint8_t pin;
    uint8_t func;
} LEDPincfg_t;

typedef enum {
    LED_ACTV_ON=0,
    LED_ACTV_OFF,
    LED_ACTV_BLINK
} LEDactivity_t;

typedef struct {
    LEDactivity_t action;
    uint8_t initstate;	   // initial state, either LEDOFF or LEDON
    uint32_t delay_init;        // period in initial state in 10ms units
    uint32_t delay_tog;        // period in opposite state in 10ms units, 0=skip
} LED_activity_desc_t;

typedef struct {
    LED_activity_desc_t cur_cfg;
    LED_activity_desc_t last_cfg;
    LED_activity_desc_t const * local_ptr;
    uint32_t counter;
    uint8_t Color;
    LEDPincfg_t pin_cfg;
    QueueHandle_t queue;
} LED_state_rec_t;

typedef enum {
    led_success = 0,
    led_unspecified_error,
    led_invalid_argument
} led_error;

extern const LED_activity_desc_t LED_Off_Activity;
extern const LED_activity_desc_t LED_On_Activity;
extern const LED_activity_desc_t LED_Short_Blink_Activity;
extern const LED_activity_desc_t LED_Long_Blink_Activity;
extern const LED_activity_desc_t LED_3sec_Lamp_Test_Activity;

#define LED_on(cfg)		       gpio_clr_pin(cfg.port, cfg.pin)
#define LED_off(cfg)		       gpio_set_pin(cfg.port, cfg.pin)
#define LED_toggle(cfg)		       gpio_pin_toggle(cfg.port, cfg.pin)
#define LED_is_off(cfg)		       gpio_read_pin(cfg.port, cfg.pin)
#define LED_is_on(cfg)		       !gpio_read_pin(cfg.port, cfg.pin)
#define LED_get_state(cfg)	       gpio_read_pin(cfg.port, cfg.pin)
#define LED_set_state(cfg, state)      gpio_set_pin_state(cfg.port, cfg.pin, state)

void LED_init(void);
led_error LED_update( uint8_t led_num, const LED_activity_desc_t * pLEDact );
void ipmi_picmg_set_led(ipmi_msg *req, ipmi_msg *rsp);

#endif /* LED_H_ */
