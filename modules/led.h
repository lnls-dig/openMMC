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

#ifdef MODULE_RTM
#define LEDCONFIG_SIZE 2
#else
#define LEDCONFIG_SIZE 1
#endif

#include "port.h"

enum LED_ID {
    LED_BLUE = 0,
    LED1, /* Red */
    LED2,  /* Green */
    LED_CNT
};

enum LEDColor {
    LEDCOLOR_BLUE = 1,
    LEDCOLOR_RED,
    LEDCOLOR_GREEN,
    LEDCOLOR_AMBER,
    LEDCOLOR_ORANGE,
    LEDCOLOR_WHITE
};

enum LEDInit_status {
    LEDINIT_ON = 0x00,
    LEDINIT_OFF = 0xFF
};

enum LEDState {
    LEDSTATE_INIT = 0x00,
    LEDSTATE_TOGGLED = 0xFF
};

enum LEDAct {
    LEDACT_TURN_ON = 0x00,
    LEDACT_TOGGLE,
    LEDACT_TURN_OFF = 0xFF
};

enum LEDMode {
    LEDMODE_LOCAL,
    LEDMODE_OVERRIDE,
    LEDMODE_LAMPTEST,
    LEDMODE_CNT
};

/* LED Acting Function signature */
typedef void (* led_act_func_t)( uint8_t id, uint8_t action );

typedef struct {
    bool active;
    uint8_t init_status;
    uint16_t t_init;
    uint16_t t_toggle;
} LEDState_t;

typedef struct LEDConfig {
    uint8_t id;
    uint8_t color;
    led_act_func_t act_func;
    uint8_t state;
    uint8_t mode;
    LEDState_t mode_cfg[LEDMODE_CNT];
    uint16_t counter;
} LEDConfig_t;

typedef struct LEDUpdate {
    uint8_t fru_id;
    uint8_t led_num;
    uint8_t mode;
    LEDState_t new_state;
} LEDUpdate_t;


void LED_init( void );
void LED_Task( void *Parameters );
void LEDUpdate( uint8_t fru, uint8_t led_num, uint8_t mode, uint8_t init_status, uint16_t t_init, uint16_t t_toggle );
void amc_led_act( uint8_t id, uint8_t action );
#ifdef MODULE_RTM
void rtm_led_act( uint8_t id, uint8_t action );
#endif

#endif /* LED_H_ */
